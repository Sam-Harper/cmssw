#include <string>
#include <vector>

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/Registry.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

#include "CommonTools/TriggerUtils/interface/GenericTriggerEventFlag.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DQMOffline/Trigger/interface/UtilFuncs.h"
/****************************************************************************

Demonstration on how to fill a histogram in the DQM
Actual modules tend to be more sophisticated and use classes to handle the 
monitor elements

Note, in DQM an item being produced is called a monitoring element, these are
typically 1D or 2D histograms


This module shows
1) using the GenericTriggerEventFlag to apply trigger and HV requirements to
   the sample
2) the booking of a 1D histogram to monitor a quantity (in this case the bx 
   of passing events)
3) the booking of histograms to monitor an efficiency to a given HLT filter, in this case 
   we demonstrate using a simple class to handle the fact that two histograms (tot,pass)
   need to be booked
4) the application of selection to objects either by StringCutObjectSelector
   or a precomputed valuemap of bools 

****************************************************************************/




namespace {
  template<typename T> const T* getLeadingPtObj(const std::vector<T*>& objs){
    float bestPt=0;
    const T* bestObj = nullptr;
    for(const auto& obj: objs){
      if(obj->pt()>bestPt) {
	bestPt = obj->pt();
	bestObj = obj;
      }
    }
    return bestObj;
  }
}

template<typename ObjType,typename CollType=std::vector<ObjType> > 
class ExampleTriggerDQMSource : public DQMEDAnalyzer {
public:
  //usually I dont like typedef but here its DQM convention as the ME/DQMStore classes have changed in the past
  typedef dqm::reco::MonitorElement MonitorElement;
  typedef dqm::reco::DQMStore DQMStore;
  
  //this is simply a little helper to allow us to book efficiency histograms easier
  //it automatically books both a pass and tot histogram and allows us to fill them easily
  class EffME {
  public:
    EffME(){}
    ~EffME() {}

    template <typename... Args>
    void fill(const bool pass, Args... args){
      if(pass) numer_ ->Fill(args...);
      denom_->Fill(args...);
    }

    template<typename... Args>
   
    void book1D(DQMStore::IBooker& ibooker,const std::string& name,const std::string& title,Args... args){
      numer_ = ibooker.book1D(name+"_pass",title+" (pass)",args...);
      denom_ = ibooker.book1D(name+"_tot",title+" (all)",args...);
      
    }

    template<typename... Args>
    void book2D(DQMStore::IBooker& ibooker,const std::string& name,const std::string& title,Args... args){
      numer_ = ibooker.book2D(name+"_pass",title+" (pass)",args...);
      denom_ = ibooker.book2D(name+"_tot",title+" (all)",args...);
    }
  private:
    MonitorElement* numer_ = nullptr;
    MonitorElement* denom_ = nullptr;
    
  };
  

  ExampleTriggerDQMSource(const edm::ParameterSet&);
  ~ExampleTriggerDQMSource() override = default;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

protected:
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;  
  void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) override;

private:
  template <typename T> edm::EDGetTokenT<T> consumesOpt(const edm::InputTag& tag){
    if(tag.label().empty()){
      return edm::EDGetTokenT<T>();
    }else{
      return consumes<T>(tag);
    }
  }
  std::string objName(){return "Object";}
  
  const std::string folderName_;
  const std::string baseName_;
  GenericTriggerEventFlag sampleTrigRequirements_;  
  const std::string filterName_;
  const std::string processName_;
  edm::EDGetTokenT<trigger::TriggerEvent> trigEvtToken_;
  StringCutObjectSelector<ObjType, true> objSel_;  
  const edm::EDGetTokenT<edm::ValueMap<bool> > objSelVMToken_; 
  edm::EDGetTokenT<CollType> objsToken_;
  MonitorElement* bxME_;
  EffME objPtEff_;
  EffME objPtEtaEff_;
  
};




template<typename ObjType,typename CollType> 
ExampleTriggerDQMSource<ObjType,CollType>::ExampleTriggerDQMSource(const edm::ParameterSet& iConfig)
    : folderName_(iConfig.getParameter<std::string>("folderName")),
      baseName_(iConfig.getParameter<std::string>("baseName")), 
      sampleTrigRequirements_(iConfig.getParameter<edm::ParameterSet>("sampleTrigRequirements"), consumesCollector()),
      filterName_(iConfig.getParameter<std::string>("filterName")),
      processName_(iConfig.getParameter<edm::InputTag>("trigEvt").process()),
      trigEvtToken_(consumes<trigger::TriggerEvent>(iConfig.getParameter<edm::InputTag>("trigEvt"))),  
      objSel_(iConfig.getParameter<std::string>("objectSelection")), 
      objSelVMToken_(consumesOpt<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("objectSelectionVM"))),
      objsToken_(consumes<CollType>(iConfig.getParameter<edm::InputTag>("objs")))
{}

//all DQMAnalysers supply  a bookHistograms function which as you have guessed books the histograms
//it acts as the beginRun function so other per run setup needs to happen here as well
template<typename ObjType,typename CollType> 
void ExampleTriggerDQMSource<ObjType,CollType>::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& iRun, edm::EventSetup const& iSetup) {

  //this initialises our trigger and HV requirements
  if (sampleTrigRequirements_.on()) {
     sampleTrigRequirements_.initRun(iRun, iSetup);
  }

  //now setup the booker and book the histograms
  std::string currentFolder = folderName_;
  ibooker.setCurrentFolder(currentFolder);
  //now we book the bx histogram
  //note it is usually best to set the bins in the python config
  bxME_ =  ibooker.book1D(baseName_+"_bx","Selected BX;bx;Entries",4000,0,4000);
  //here we book our efficency histograms using our helper class, again better to use the python confg to set bins
  objPtEff_.book1D(ibooker,baseName_+"_vsPt","Leading "+objName()+" Pt;p_{T} [GeV];Entries",20,0.,100.);
  objPtEtaEff_.book2D(ibooker,baseName_+"_vsPtEta","Leading "+objName()+" Pt vs #eta;#eta;pt_{T} [GeV];Entries",30.,-3.,3.,20,0.,100.);
  
}


template<typename ObjType,typename CollType> 
void ExampleTriggerDQMSource<ObjType,CollType>::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // Filter out events if Trigger Filtering is requested
  if (sampleTrigRequirements_.on() && !sampleTrigRequirements_.accept(iEvent, iSetup)) {
     return;
  }
  
  const auto& trigEvt = iEvent.get(trigEvtToken_);
  auto objCollHandle = iEvent.getHandle(objsToken_);
  auto selVMHandle = objSelVMToken_.isUninitialized() ? edm::Handle<edm::ValueMap<bool>>() : iEvent.getHandle(objSelVMToken_);
  std::vector<const ObjType*> selected;
  for (size_t objNr = 0; objNr < objCollHandle->size(); objNr++) {
    edm::Ref<CollType> objRef(objCollHandle, objNr);

    if(objSel_(*objRef) &&
       (objSelVMToken_.isUninitialized()==true || (*selVMHandle)[objRef] == true)
       ){      
      selected.push_back(&*objRef);
    }
  }
  const auto leadPtObj = getLeadingPtObj(selected);
  if(leadPtObj!=nullptr){

    bool pass = hltdqm::passTrig(leadPtObj->eta(),leadPtObj->phi(),trigEvt,filterName_,processName_);
    bxME_->Fill(iEvent.eventAuxiliary().bunchCrossing());
    objPtEff_.fill(pass,leadPtObj->pt());
    objPtEtaEff_.fill(pass,leadPtObj->eta(),leadPtObj->pt());
  }
  

}

template<typename ObjType,typename CollType> 
void ExampleTriggerDQMSource<ObjType,CollType>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("folderName", "HLT/Example");
  desc.add<std::string>("baseName","example");
  desc.add<std::string>("filterName","hltEle32WPTightGsfTrackIso");
  desc.add<edm::InputTag>("trigEvt",edm::InputTag("hltTriggerSummaryAOD::HLT"));
  desc.add<std::string>("objectSelection","");
  desc.add<edm::InputTag>("objectSelectionVM",edm::InputTag());
  desc.add<edm::InputTag>("objs",edm::InputTag());
  edm::ParameterSetDescription genericTriggerEventPSet;
  GenericTriggerEventFlag::fillPSetDescription(genericTriggerEventPSet);
  desc.add<edm::ParameterSetDescription>("sampleTrigRequirements", genericTriggerEventPSet);
  
  descriptions.addWithDefaultLabel(desc);
}


template<>
std::string ExampleTriggerDQMSource<reco::PFJet>::objName(){return "Jet";}
template<>
std::string ExampleTriggerDQMSource<reco::GsfElectron>::objName(){return "Electron";}
template<>
std::string ExampleTriggerDQMSource<reco::Photon>::objName(){return "Photon";}
template<>
std::string ExampleTriggerDQMSource<reco::Muon>::objName(){return "Muon";}

using JetExampleTriggerDQMSource = ExampleTriggerDQMSource<reco::PFJet>;
using EleExampleTriggerDQMSource = ExampleTriggerDQMSource<reco::GsfElectron>;
using PhoExampleTriggerDQMSource = ExampleTriggerDQMSource<reco::Photon>;
using MuonExampleTriggerDQMSource = ExampleTriggerDQMSource<reco::Muon>;
					  
DEFINE_FWK_MODULE(JetExampleTriggerDQMSource);
DEFINE_FWK_MODULE(EleExampleTriggerDQMSource);
DEFINE_FWK_MODULE(PhoExampleTriggerDQMSource);
DEFINE_FWK_MODULE(MuonExampleTriggerDQMSource);
