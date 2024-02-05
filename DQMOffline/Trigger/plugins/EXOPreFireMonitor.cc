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

#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"


#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "CondFormats/L1TObjects/interface/L1TUtmTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1TUtmTriggerMenuRcd.h"

#include "DQMOffline/Trigger/plugins/TriggerDQMBase.h"

template<typename ObjType,typename CollType=std::vector<ObjType> > 
class EXOPreFireMonitor : public DQMEDAnalyzer {
public:
  typedef dqm::reco::MonitorElement MonitorElement;
  typedef dqm::reco::DQMStore DQMStore;
  
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
  

  EXOPreFireMonitor(const edm::ParameterSet&);
  ~EXOPreFireMonitor() override = default;
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

  bool unprefireable(const edm::Event& iEvent);
  bool hasL1AInBXNoBefore(const edm::Event& iEvent,int bx);
  std::string objName(){return "Object";}
  
  const std::string folderName_;
  const std::string baseName_;
  GenericTriggerEventFlag sampleTrigRequirements_;
  StringCutObjectSelector<ObjType, true> objSel_;
  const edm::EDGetTokenT<edm::ValueMap<bool> > objSelVMToken_;
  const edm::EDGetTokenT<BXVector<GlobalAlgBlk> > l1DecisionToken_;
  const edm::ESGetToken<L1TUtmTriggerMenu, L1TUtmTriggerMenuRcd> l1MenuToken_;
  unsigned int l1FirstBunchInTrainIndx_;
  unsigned int l1FirstBunchBeforeTrainIndx_;
  

  edm::EDGetTokenT<CollType> objsToken_;
  EffME objPt_;
  EffME objPtEta_;
  
};

template<typename ObjType,typename CollType> 
EXOPreFireMonitor<ObjType,CollType>::EXOPreFireMonitor(const edm::ParameterSet& iConfig)
    : folderName_(iConfig.getParameter<std::string>("folderName")),
      baseName_(iConfig.getParameter<std::string>("baseName")), 
      sampleTrigRequirements_(iConfig.getParameter<edm::ParameterSet>("sampleTrigRequirements"), consumesCollector()),
      objSel_(iConfig.getParameter<std::string>("objectSelection")),
      objSelVMToken_(consumesOpt<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("objectSelectionVM"))),
      l1DecisionToken_(consumes<BXVector<GlobalAlgBlk>>(iConfig.getParameter<edm::InputTag>("l1Decision"))),
      l1MenuToken_{esConsumes<edm::Transition::BeginRun>()},
      l1FirstBunchInTrainIndx_(512),
      l1FirstBunchBeforeTrainIndx_(512),
      objsToken_(consumes<CollType>(iConfig.getParameter<edm::InputTag>("objs")))
{}

namespace{
  int getSeedIndx(const std::map<std::string, L1TUtmAlgorithm>& algoMap,const std::string& name){
    const auto& algoIt = algoMap.find(name);
    if(algoIt!=algoMap.end()){
      return algoIt->second.getIndex();
    }else{
      return algoMap.size();
    }
  }
}

template<typename ObjType,typename CollType> 
void EXOPreFireMonitor<ObjType,CollType>::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& iRun, edm::EventSetup const& iSetup) {

  if (sampleTrigRequirements_.on()) {
     sampleTrigRequirements_.initRun(iRun, iSetup);
  }
  std::string currentFolder = folderName_;
  ibooker.setCurrentFolder(currentFolder);

  const auto& l1Menu = iSetup.getData(l1MenuToken_);
  const auto& l1MenuMap = l1Menu.getAlgorithmMap();

  l1FirstBunchInTrainIndx_ = getSeedIndx(l1MenuMap,"L1_FirstBunchInTrain");
  l1FirstBunchBeforeTrainIndx_ = getSeedIndx(l1MenuMap,"L1_FirstBunchBeforeTrain");


  std::cout <<"L1FirstBunchInTrainIdx " <<l1FirstBunchInTrainIndx_<<std::endl;
  std::cout <<"L1FirstBunchBeforeTrainIdx " <<l1FirstBunchBeforeTrainIndx_<<std::endl;
  objPt_.book1D(ibooker,baseName_+"_vsPt","Leading "+objName()+" Pt;p_{T} [GeV];Entries",6,0.,3000.);
  objPtEta_.book2D(ibooker,baseName_+"_vsPtEta","Leading "+objName()+" Pt vs #eta;#eta;pt_{T} [GeV];Entries",10.,-5.,5.,6.,0.,3000.);
  
}

template<typename ObjType,typename CollType> 
bool EXOPreFireMonitor<ObjType,CollType>::unprefireable(const edm::Event& iEvent){
  const auto& l1DecisionBXVector = iEvent.get(l1DecisionToken_);
  const auto& l1Decision = l1DecisionBXVector.at(0,0);

  return l1Decision.getAlgoDecisionInitial(l1FirstBunchInTrainIndx_);

}

template<typename ObjType,typename CollType> 
bool EXOPreFireMonitor<ObjType,CollType>::hasL1AInBXNoBefore(const edm::Event& iEvent,int bx){
  const auto& l1DecisionBXVector = iEvent.get(l1DecisionToken_);
  const auto& l1Decision = l1DecisionBXVector.at(bx,0);
  const auto& algoDecisionFinal = l1Decision.getAlgoDecisionFinal();

  for(size_t bitNr=0;bitNr<algoDecisionFinal.size();bitNr++){
    if(bitNr==l1FirstBunchBeforeTrainIndx_) continue;
    if(algoDecisionFinal[bitNr]) return true;
  } 

  return false;
}

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

template<typename ObjType,typename CollType> 
void EXOPreFireMonitor<ObjType,CollType>::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // Filter out events if Trigger Filtering is requested
  if (sampleTrigRequirements_.on() && !sampleTrigRequirements_.accept(iEvent, iSetup)) {
     return;
  }
  if (!unprefireable(iEvent)){
    return;
  }

  bool l1aBXM1 = hasL1AInBXNoBefore(iEvent,-1);
  
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
  auto leadPtObj = getLeadingPtObj(selected);
  if(leadPtObj!=nullptr){
    objPt_.fill(!l1aBXM1,leadPtObj->pt());
    objPtEta_.fill(!l1aBXM1,leadPtObj->eta(),leadPtObj->pt());
  }
  

}

template<typename ObjType,typename CollType> 
void EXOPreFireMonitor<ObjType,CollType>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  //desc.add<std::string>("folderName", "HLT/EXOPreFire");
  desc.setAllowAnything();
  descriptions.addWithDefaultLabel(desc);
}


using EXOJetPreFireMonitor = EXOPreFireMonitor<reco::PFJet>;
using EXOElePreFireMonitor = EXOPreFireMonitor<reco::GsfElectron>;
using EXOPhoPreFireMonitor = EXOPreFireMonitor<reco::Photon>;
using EXOPFMETPreFireMonitor = EXOPreFireMonitor<reco::PFMET>;
using EXOMuonPreFireMonitor = EXOPreFireMonitor<reco::Muon>;
					  
 
DEFINE_FWK_MODULE(EXOJetPreFireMonitor);
DEFINE_FWK_MODULE(EXOElePreFireMonitor);
DEFINE_FWK_MODULE(EXOPhoPreFireMonitor);
DEFINE_FWK_MODULE(EXOPFMETPreFireMonitor);
DEFINE_FWK_MODULE(EXOMuonPreFireMonitor);
