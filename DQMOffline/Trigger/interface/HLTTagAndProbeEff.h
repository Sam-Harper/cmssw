#ifndef DQMOffline_Trigger_HLTTagAndProbeEff_h
#define DQMOffline_Trigger_HLTTagAndProbeEff_h


#include "FWCore/Common/interface/TriggerNames.h"

#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"


#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include <vector>
#include <string>

//functions we wish to add that are not direct member functions
namespace{
  template<typename ObjType> 
    float scEtaFunc(const ObjType& obj)
  {
    return obj.superCluster()->eta();
  }
}

namespace{
  bool passTrig(const float objEta,float objPhi, const trigger::TriggerEvent& trigEvt,
		const std::string & filterNames){
    constexpr float kMaxDR2 = 0.1*0.1;
    
    edm::InputTag filterTag(filterName,"","HLTX"); 
    trigger::size_type filterIndex = trigEvt.filterIndex(filterTag); 
    if(filterIndex<trigEvent.sizeFilters()){ //check that filter is in triggerEvent
      const trigger::Keys& trigKeys = trigEvt.filterKeys(filterIndex); 
      const trigger::TriggerObjectCollection & trigObjColl(trigEvt.getObjects());
      for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){ 
	const trigger::TriggerObject& trigObj = trigObjColl[*keyIt];
	if(reco::deltaR2(trigObj.eta(),trigObj.phi(),objEta,objPhi)<kMaxDR) return true;
      }
    }
    return false;
  }
  bool passTrig(const float objEta,float objPhi, const trigger::TriggerEvent& trigEvt,
		const std::vector<std::string>& filterNames){
    for(auto& filterName : filterNames){
      if(!passTrig(objEta,objPhi,trigEvt,filterName)) return false;
    }
    return true;
  }
}

//our base class for our histograms
//takes an object, edm::Event,edm::EventSetup and fills the histogram
//with the predetermined variable (or varaibles) 
template <typename ObjType,typename ValType> 
class HLTDQM1DHist : public HLTDQMHist {
public:
  HLTDQMHist()=default;
  virtual ~HLTDQMHist()=default;
  virtual void fill(const ObjType& objType,const edm::Event& event,const edm::EventSetup& setup)=0;
};


//this class is a specific implimentation of a HLTDQMHist
//it has the value with which to fill the histogram 
//and the histogram itself
//we do not own the histogram
template <typename ObjType,typename ValType> 
class HLTDQM1DHist : public HLTDQMHist {
public:
  void fill(const ObjType& objType,const edm::Event& event,const edm::EventSetup& setup)override{
    hist_->Fill(var_(objType));
  }
private:
  std::function<ValType(const ObjType&)> var_;
  TH1* hist_; //we do not own this
};


template <typename ObjType> 
class HLTDQMHistColl {
public:
  
  explicit HLTDQMHistColl(const edm::ParameterSet& config){std::cout <<"placeholder"<<std::endl;}
  void bookHists(DQMStore::IBooker& iBooker,const edm::ParameterSet& config,const std::string& baseName);
  void fillHists(const ObjType& objType,const edm::Event& event,const edm::EventSetup& setup);
private:

  std::vector<std::unique_ptr<HLTDQMHist> > hists_;
  std::string filterName_;
  std::string baseHistName_;
  edm::ParameterSet histsConfig_;		 
};

template <typename ObjType>
void HLTDQMHistColl<ObjType>::bookHists(DQMStore::IBooker& iBooker,
					const edm::ParameterSet& config,
					const std::string& baseName)
{
  auto histConfigs = config.getParameter<std::vector<edm::ParameterSet> >("hists");
  for(auto& histConfig : histConfigs){
    auto binLowEdges = histConfig.getParameters<std::vector<double> >("binLowEdges");
    auto nameSuffex = histConfig.getParameter<std::string>("nameSuffex");
    auto me = iBooker.book1D((baseName+nameSuffex).c_str(),(baseName+nameSuffex).c_str(),
			     binLowEdges.size()-1,&binLowEdges[0]);
    std::unique_ptr<HLTDQMHist> hist;
    auto vsVar = histConfig.getParameter<std::string>("vsVar");
    switch(vsVar){
    case "et":hist = std::make_unique_ptr<HLTDQM1DHist<ObjType,float> >(me->getTH1(),ObjType::et);
      break;
    case "pt" : hist = std::make_unique_ptr<HLTDQM1DHist<ObjType,float> >(me->getTH1(),ObjType::pt);
      break;
    case "eta": hist = std::make_unique_ptr<HLTDQM1DHist<ObjType,float> >(me->getTH1(),ObjType::eta);
      break;
    case "scEta": hist = std::make_unique_ptr<HLTDQM1DHist<ObjType,float> >(me->getTH1(),scEtaFunc);
      break;
    default:
      throw cms::Exception("ConfigError") <<" vsVar "<<vsVar<<" not recognised"<<std::endl;
    }
       
    hists_.emplace_back(std::move(hist));
  }
}

template <typename ObjType>
void HLTDQMHistColl<ObjType>::fillHists(const ObjType& objType,
					const edm::Event& event,
					const edm::EventSetup& setup)
{
  for(auto& hist : hists_){
    hist->fill(objType,event,setup); 
  }
}

template<typename ObjType>
class RangeCuts {
  RangeCuts(const edm::ParameterSet& config){
    auto varName = config.getParameter<std::string>("rangeVar");
    switch(varName){
    case "et": varFunc_ = &ObjType::et;
      break;
    case "pt": varFunc_ = &ObjType::pt;
      break;
    case "eta": varFunc_ = &ObjType::eta:
      break;
    case "scEta": varFunc_ = &scEtaFunc;
      break;
    case: "": //empty string, allow it to auto pass
      break;
    default:
      throw cms::Exception("ConfigError") <<" rangeVar "<<varName<<" not recognised"<<std::endl;
    }
    auto ranges = config.getParameter<std::vector<std::string> >("allowedRanges");
    for(auto range: ranges){
      std::vector<std::string> splitRange;
      boost::split(splitRange,range,boost::is_any_of(":"));
      if(splitRange.size()!=2) throw cms::Exception("ConfigError") <<"range "<<range<<" is not of format X:Y"<<std::endl;
      allowedRanges_.push_back({std::stof(splitRange[0]),std::stof(splitRange[1])});
    }
  }
  bool operator(const ObjType& obj)const{
    if(!varFunc_) return true; //auto pass if we dont specify a variable function
    else{ 
      float varVal = varFunc_(obj);
      for(auto& range : allowedRanges){
	if(varVal>=range.first && varVal<range.second) return true;
      }
      return false;
    }
  }
  private:
  std::function<float(const ObjType&)> varFunc_;
  std::vector<std::pair<float,float> > allowedRanges_;
};

template<typename ObjType>
class RangeCutsColl {
public:
  explicit RangeCutsColl(edm::ParameterSet& config){
    const auto cutsConfig config.getParameter<vector<edm::ParameterSet> >("cuts");
    for(const auto & cutConfig : cutsConfig) rangeCuts_.emplace_back(RangeCuts(cutConfig));
  }
  //if no cuts are defined, it returns true
  bool operator(const ObjType& obj)const{
    for(auto& cut : rangeCuts_){
      if(!cut(obj)) return false;
    }
    return true;
  }
private:
  std::vector<RangeCuts<ObjType> rangeCuts_;
};

template <typename ObjType,typename ObjCollType> 
class HLTTagAndProbeEff {
public:
    
  explicit HLTTagAndProbeEff(const edm::ParameterSet& pset,edm::ConsumesCollector && cc);
 
  void bookMEs();
  void fill(const edm::Event& event,const edm::EventSetup& setup);
  
private:
  edm::EDGetTokenT<ObjCollType> objToken_;
  edm::EDGetTokenT<TriggerEvent> trigEvtToken_;
  edm::EDGetTokenT<TriggerResults> trigResultsToken_;
  
  std::string hltProcess_;

  std::string tagTrigger_;
	    
  std::vector<std::string> tagFilters_;
  std::string tagVIDCuts_;
  RangeCutsColl tagRangeCuts_;

  std::vector<std::string> probeFilter_;
  std::string probeVIDCuts_;
  RangeCutsColl<ObjType> probeRangeCuts_;

  float minMass_;
  float maxMass_;
  bool requireOpSign_;

  std::vector<HLTDQMHistColl<ObjType> > histColls_;
    
};

template <typename ObjType,typename ObjCollType> 
HLTTagAndProbeEff<ObjType,ObjCollType>::HLTTagAndProbeEff(const edm::ParameterSet& pset,edm::ConsumesCollector && cc)
{

  edm::InputTag trigEvtTag = pset.getParameter<edm::InputTag>("trigEvent");

  objToken_ = cc.consumes<ObjCollType>(pset.getParameter<edm::InputTag>("objColl"));
  trigEvtToken_ = cc.consumes<ObjCollType>(trigEvtTag);
  trigResultsToken_ = cc.consumes<ObjCollType>(pset.getParameter<edm::InputTag>("trigResults"));

  hltProcess_ = trigEvtTag.process();

  tagTrigger_ = pset.getParameter<std::string>("tagTrigger");
  tagFilters_ = pset.getParameter<std::vector<std::string> >("tagFilters");
  tagVIDCuts_ = pset.getParameter<std::string>("tagVIDCuts");
  tagRangeCuts_ = pset.getParameter<edm::ParameterSet>("tagRangeCuts");

  probeFilters_ = pset.getParameter<std::vector<std::string> >("probeFilters");
  probeVIDCuts_ = pset.getParameter<std::string>("probeVIDCuts");
  probeRangeCuts_ = pset.getParameter<edm::ParameterSet>("probeRangeCuts");
  
  minMass_ = pset.getParameter<double>("minMass");
  maxMass_ = pset.getParameter<double>("maxMass");
  requireOpSign = pset.getParameter<bool>("requireOpSign");
  
  
}

template <typename ObjType,typename ObjCollType> 
std::vector<edm::Ref<ObjType> > HLTTagAndProbeEff<ObjType,ObjCollType>::
getPassingRefs(edm::Handle<ObjCollType>& objCollHandle,
	       edm::Handle<bool>& vidHandle,
	       const trigger::TriggerEvent& trigEvt,
	       const std::vector<std::string> filterNames,
	       const RangeCutsColl<ObjType>& rangeCuts)
{
  std::vector<edm::Ref<ObjType> > passingRefs;
  for(size_t objNr=0;objNr<objCollHandle->size();objNr++){
    edm::Ref<ObjType> ref(objCollHandle,objNr);
    
    if(rangeCuts(*ref) && 
       passTrig(*ref,trigEvt,filterNames) && 
       (!vidHandle->isValid() || (*vidHandle)[ref]==true)){
      passingRefs.push_back(ref);
      
    }
  }
  return passingRefs;
}

template <typename ObjType> 
void HLTTagAndProbeEff<ObjType>::fill(const edm::Event& event,const edm::EventSetup& setup)
{
  auto objCollHandle = getHandle(event,objToken_); 
  auto trigEvtHandle = getHandle(event,trigEvtToken_);
  auto trigResultsHandle = getHandle(event,trigResultsToken_);
  auto tagVIDHandle = getHandle(event,tagVIDToken_);
  auto probeVIDHandle = getHandle(event,probeVIDToken_);
  
  //we need the object collection and trigger info at the minimum
  if(!objCollHandle.isValid() || !trigEvtHandle.isValid() || !trigResultsHandle.isValid()) return;
  
  const edm::TriggerNames& trigNames = event.triggerNames(*trigResultsHandle);

  std::vector<edm::Ref<ObjType> > tagRefs = getPassingRefs(objCollHandle,*trigEvtHandle,
							   tagFilters_,
							   tagVIDHandle,tagRangeCuts_);
  std::vector<edm::Ref<ObjType> > probeRefs = getPassingRefs(objCollHandle,*trigEvtHandle,
							     probeFilters_,
							     probeVIDHandle,probeRangeCuts_);
  
  for(auto& tagRef : tagsRefs){
    for(auto& probeRef : probeRefs){
      if(tagRef==probeRef) continue; //otherwise its the same object...
      float mass = (tagRef->p4()+probeRef->p4()).mag();
      
      if( ( mass>minMass_ || minMass_<0 ) && 
	  ( mass<maxMass_ || maxMass_<0 ) && 
	  ( !requireOpSign || tagRef->charge()!=probeRef->charge()) ){

	for(auto& histColl : histColls_){
	  histColl.fill(probeRef,event,setup,*trigSumHandle);
	}
	      
      }//end of t&p pair cuts
    }//end of probe loop
  }//end of tag loop
      
}

#endif
