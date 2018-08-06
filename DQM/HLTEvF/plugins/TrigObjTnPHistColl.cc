#include "TrigObjTnPHistColl.h"

TrigObjTnPHistColl::TrigObjTnPHistColl(const edm::ParameterSet& config,edm::ConsumesCollector&& cc):
  tagCuts_(config.getParameter<std::vector<edm::ParameterSet>>("tagCuts")),
  probeCuts_(config.getParameter<std::vector<edm::ParameterSet>>("probeCuts")),
  tagFilters_(config.getParameter<edm::ParameterSet>("tagFilters")),
  collName_(config.getParameter<std::string>("collName")),
  folderName_(config.getParameter<std::string>("folderName")),
  histBins_(config.getParameter<edm::ParameterSet>("histBins")),
  evtTrigSel_(config.getParameter<edm::ParameterSet>("evtTrigSel"),cc)
{
  for(auto probeFilter : config.getParameter<std::vector<std::string> >("probeFilters")){
    probeHists_.emplace_back(ProbeData(std::move(probeFilter)));
  }
}

edm::ParameterSetDescription TrigObjTnPHistColl::makePSetDescription()
{
  edm::ParameterSetDescription desc;
  desc.addVPSet("tagCuts",VarRangeCut<trigger::TriggerObject>::makePSetDescription(),std::vector<edm::ParameterSet>());
  desc.addVPSet("probeCuts",VarRangeCut<trigger::TriggerObject>::makePSetDescription(),std::vector<edm::ParameterSet>());
  desc.add<edm::ParameterSetDescription>("tagFilters",FilterSelector::makePSetDescription());
  desc.add<std::string>("collName","stdTag");
  desc.add<std::string>("folderName","HLT/EGM/TrigObjTnP");
  desc.add<edm::ParameterSetDescription>("histBins",HistColl::Bins::makePSetDescription());
  desc.add<std::vector<std::string>>("probeFilters",std::vector<std::string>());  

  edm::ParameterSetDescription trigEvtFlagDesc;
  trigEvtFlagDesc.add<bool>("andOr",false);
  trigEvtFlagDesc.add<unsigned int>("verbosityLevel",1);
  trigEvtFlagDesc.add<bool>("andOrDcs", false);  
  trigEvtFlagDesc.add<edm::InputTag>("dcsInputTag", edm::InputTag("scalersRawToDigi") );
  trigEvtFlagDesc.add<std::vector<int> >("dcsPartitions",{24,25,26,27,28,29});
  trigEvtFlagDesc.add<bool>("errorReplyDcs", true);
  trigEvtFlagDesc.add<std::string>("dbLabel","");
  trigEvtFlagDesc.add<bool>("andOrHlt", true); //true = OR, false = and
  trigEvtFlagDesc.add<edm::InputTag>("hltInputTag", edm::InputTag("TriggerResults::HLT") );
  trigEvtFlagDesc.add<std::vector<std::string> >("hltPaths",{});
  trigEvtFlagDesc.add<std::string>("hltDBKey","");
  trigEvtFlagDesc.add<bool>("errorReplyHlt",false);
  desc.add<edm::ParameterSetDescription>("evtTrigSel",trigEvtFlagDesc);
  
  return desc;
}

void TrigObjTnPHistColl::bookHists(DQMStore::IBooker& iBooker)
{
  iBooker.setCurrentFolder(folderName_);
  for(auto& probe : probeHists_){
    probe.bookHists(collName_,iBooker,histBins_);
  }
}

void TrigObjTnPHistColl::fill(const trigger::TriggerEvent& trigEvt,
			      const edm::Event& event,const edm::EventSetup& setup)
{
  if(evtTrigSel_.accept(event,setup)==false) return;

  auto tagTrigKeys = tagFilters_.getPassingKeys(trigEvt);
  for(auto& tagKey : tagTrigKeys){
    const trigger::TriggerObject& tagObj = trigEvt.getObjects()[tagKey];
    if(tagCuts_(tagObj)){
      for(auto& probeColl : probeHists_) probeColl.fill(tagKey,trigEvt,probeCuts_);
    }
  }
}

//trigger::Keys is likely a vector containing 0-3 short ints (probably 10 max),
// passing by value makes this much  easier code wise (otherwise would have to 
//create a dummy empty vector) and shouldnt be too much of a performance hit
const trigger::Keys TrigObjTnPHistColl::getKeys(const trigger::TriggerEvent& trigEvt,const std::string& filterName)
{
  edm::InputTag filterTag(filterName,"",trigEvt.usedProcessName());
  trigger::size_type filterIndex = trigEvt.filterIndex(filterTag); 
  if(filterIndex<trigEvt.sizeFilters()) return trigEvt.filterKeys(filterIndex);
  else return trigger::Keys();
}

TrigObjTnPHistColl::TrigObjCuts::TrigObjCuts(const edm::ParameterSet& config)
{
  

}

TrigObjTnPHistColl::TrigObjCuts::CutData::CutData(const edm::ParameterSet& config)
{
  

}

TrigObjTnPHistColl::FilterSelector::FilterSelector(const edm::ParameterSet& config):
  isAND_(config.getParameter<bool>("isAND"))
{
  auto filterSetConfigs = config.getParameter<std::vector<edm::ParameterSet>>("filterSets");
  for(auto& filterSetConfig : filterSetConfigs) filterSets_.emplace_back(FilterSet(filterSetConfig));
}

edm::ParameterSetDescription TrigObjTnPHistColl::FilterSelector::makePSetDescription()
{
  edm::ParameterSetDescription desc;
  desc.addVPSet("filterSets",FilterSet::makePSetDescription(),std::vector<edm::ParameterSet>());
  desc.add<bool>("isAND",false);
  return desc;
}


const trigger::Keys TrigObjTnPHistColl::FilterSelector::getPassingKeys(const trigger::TriggerEvent& trigEvt)const
{
  trigger::Keys passingKeys;
  bool isFirstFilterSet = true;
  for(const auto& filterSet : filterSets_){
    auto keysOfFilterSet = filterSet.getPassingKeys(trigEvt);
    if(isFirstFilterSet) passingKeys = keysOfFilterSet;
    else mergeTrigKeys(passingKeys,keysOfFilterSet,isAND_);
    isFirstFilterSet = false;
  }
  cleanTrigKeys(passingKeys);
  return passingKeys;
}

void TrigObjTnPHistColl::FilterSelector::mergeTrigKeys(trigger::Keys& keys,const trigger::Keys& keysToMerge,bool isAND)
{
  if(isAND){
    for(auto& key : keys) {
      if(std::count(keysToMerge.begin(),keysToMerge.end(),key)==0){
	key=std::numeric_limits<trigger::size_type>::max();
      }
    }
  }else{
    for(const auto key : keysToMerge){
      keys.push_back(key);
    }
  }  
}

void TrigObjTnPHistColl::FilterSelector::cleanTrigKeys(trigger::Keys& keys)
{
  std::sort(keys.begin(),keys.end());
  std::unique(keys.begin(),keys.end());
  while(!keys.empty() && keys.back()==std::numeric_limits<trigger::size_type>::max()){
    keys.pop_back();
  }
}

TrigObjTnPHistColl::FilterSelector::FilterSet::FilterSet(const edm::ParameterSet& config):
  filters_(config.getParameter<std::vector<std::string>>("filters")),
  isAND_(config.getParameter<bool>("isAND"))
{
  
}

edm::ParameterSetDescription TrigObjTnPHistColl::FilterSelector::FilterSet::makePSetDescription()
{
  edm::ParameterSetDescription desc;
  desc.add<std::vector<std::string> >("filters",std::vector<std::string>());
  desc.add<bool>("isAND",true);
  return desc;
}

const trigger::Keys TrigObjTnPHistColl::FilterSelector::FilterSet::getPassingKeys(const trigger::TriggerEvent& trigEvt)const
{
  trigger::Keys passingKeys;
  bool firstFilter = true;
  for(const auto& filterName : filters_){
    const trigger::Keys& trigKeys = getKeys(trigEvt,filterName);
    if(firstFilter) {
      passingKeys = trigKeys;
      firstFilter = false;
    }else mergeTrigKeys(passingKeys,trigKeys,isAND_);
  }
  cleanTrigKeys(passingKeys);
  
  return passingKeys;
}

void TrigObjTnPHistColl::HistColl::bookHists(DQMStore::IBooker& iBooker,
					     const std::string& name,const std::string& title,
					     const TrigObjTnPHistColl::HistColl::Bins& bins )
{
  ptHist_ = iBooker.book2D((name+"_pt").c_str(),(title+";p_{T} [GeV];mass [GeV]").c_str(),
			   bins.pt.size()-1,bins.pt.data(),
			   bins.mass.size()-1,bins.mass.data());
  etaHist_ = iBooker.book2D((name+"_eta").c_str(),(title+";#eta;mass [GeV]").c_str(),
			    bins.eta.size()-1,bins.eta.data(),
			    bins.mass.size()-1,bins.mass.data());
  phiHist_ = iBooker.book2D((name+"_phi").c_str(),(title+";#phi [rad];mass [GeV]").c_str(),
			    bins.phi.size()-1,bins.phi.data(),
			    bins.mass.size()-1,bins.mass.data());
}

void TrigObjTnPHistColl::HistColl::fill(const trigger::TriggerObject& probe,float mass)
{
  ptHist_->Fill(probe.pt(),mass);
  etaHist_->Fill(probe.eta(),mass);
  phiHist_->Fill(probe.phi(),mass);
}

TrigObjTnPHistColl::HistColl::Bins::Bins(const edm::ParameterSet& config)
{
  auto getVecFloat = [&config](const std::string& name){
    auto vecDouble = config.getParameter<std::vector<double> >(name);
    std::vector<float> vecFloat;
    for(double x : vecDouble) vecFloat.push_back(x);
    return vecFloat;
  };
  pt = getVecFloat("pt");
  eta = getVecFloat("eta");
  phi = getVecFloat("phi");
  mass = getVecFloat("mass");
}  

edm::ParameterSetDescription TrigObjTnPHistColl::HistColl::Bins::makePSetDescription()
{
  edm::ParameterSetDescription desc;
  desc.add<std::vector<double> >("pt",{10,20,30,60,100});
  desc.add<std::vector<double> >("eta",{-2.5,-1.5,0,1.5,2.5});
  desc.add<std::vector<double> >("phi",{-3.14,-1.57,0,-1.57,3.14});
  std::vector<double> massBins;
  for(float mass = 60;mass<=120;mass+=1) massBins.push_back(mass);
  desc.add<std::vector<double> >("mass",massBins);
  return desc;
}

void TrigObjTnPHistColl::ProbeData::bookHists(const std::string& tagName,
					      DQMStore::IBooker& iBooker,
					      const TrigObjTnPHistColl::HistColl::Bins& bins)
{
  hists_.bookHists(iBooker,tagName+"_"+probeFilter_,tagName+"_"+probeFilter_,bins);
}

void TrigObjTnPHistColl::ProbeData::fill(const trigger::size_type tagKey,const trigger::TriggerEvent& trigEvt,const VarRangeCutColl<trigger::TriggerObject>& probeCuts)
{
  auto probeKeys = getKeys(trigEvt,probeFilter_);
  for(auto probeKey : probeKeys){
    const trigger::TriggerObject& probe = trigEvt.getObjects()[probeKey];
    if(tagKey != probeKey && probeCuts(probe) ){
      const trigger::TriggerObject& tag = trigEvt.getObjects()[tagKey];
      auto massFunc = [](float pt1,float eta1,float phi1,float pt2,float eta2,float phi2){
	return std::sqrt( 2*pt1*pt2*( std::cosh(eta1-eta2) - std::cos(phi1-phi2) ) );
      };
      float mass = massFunc(tag.pt(),tag.eta(),tag.phi(),probe.pt(),probe.eta(),probe.phi());
      hists_.fill(probe,mass);
    }
  }
}

