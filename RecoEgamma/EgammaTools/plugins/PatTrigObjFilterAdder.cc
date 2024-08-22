#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include <iostream>
#include <vector>
#include <memory>

namespace {

  bool objMatch(const pat::TriggerObjectStandAlone& trigObj, const trigger::TriggerObject& trigObjToMatch) {
    return reco::deltaR2(trigObj.eta(), trigObj.phi(), trigObjToMatch.eta(), trigObjToMatch.phi()) < 0.001;
  }

  bool filterMatch(const pat::TriggerObjectStandAlone& trigObj, const trigger::TriggerEvent& trigEvent,const std::string& filter) {    
    edm::InputTag filterTag(filter,"",trigEvent.usedProcessName());
    trigger::size_type filterIndex = trigEvent.filterIndex(filterTag); 
    if(filterIndex<trigEvent.sizeFilters()){ //check that filter is in triggerEvent
      const trigger::Keys& trigKeys = trigEvent.filterKeys(filterIndex); 
      const trigger::TriggerObjectCollection & trigObjColl(trigEvent.getObjects());
      for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){ 
        if(objMatch(trigObj,trigObjColl[*keyIt])){
          return true;
        }
      }
    }
    return false;
  }
}

class PatTrigObjFilterAdder : public edm::global::EDProducer<> {
public:
  PatTrigObjFilterAdder(const edm::ParameterSet& ps);
  ~PatTrigObjFilterAdder()override{};

  void produce(edm::StreamID sid, edm::Event& evt, const edm::EventSetup& es) const override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  const edm::EDGetTokenT<std::vector<pat::TriggerObjectStandAlone> > trigObjsToken_;
  const edm::EDPutTokenT<std::vector<pat::TriggerObjectStandAlone> > trigObjsPutToken_;
  const edm::EDGetTokenT<trigger::TriggerEvent> trigEvtToken_;
  const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
  const std::vector<std::string> filtersToAdd_;

};

PatTrigObjFilterAdder::PatTrigObjFilterAdder(const edm::ParameterSet& config)
    : trigObjsToken_(
      consumes<std::vector<pat::TriggerObjectStandAlone>>(config.getParameter<edm::InputTag>("trigObjs"))
    ),trigObjsPutToken_(
      produces<std::vector<pat::TriggerObjectStandAlone>>()
    ),trigEvtToken_(
      consumes<trigger::TriggerEvent>(config.getParameter<edm::InputTag>("trigEvt"))
    ),trigResultsToken_(
      consumes<edm::TriggerResults>(config.getParameter<edm::InputTag>("trigResults"))
    ),
    filtersToAdd_(config.getParameter<std::vector<std::string>>("filters"))
  {}

void PatTrigObjFilterAdder::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>(("trigObjs"), edm::InputTag(""));
  desc.add<edm::InputTag>(("trigEvt"), edm::InputTag(""));
  desc.add<edm::InputTag>(("trigResults"), edm::InputTag(""));
  desc.add<std::vector<std::string>>(("filters"), {});  
  descriptions.add(("hltPatTrigObjFilterAdder"), desc);
}

void PatTrigObjFilterAdder::produce(edm::StreamID sid,
                                    edm::Event& iEvent,
                                    const edm::EventSetup&) const {
  
  

  
  auto trigObjs = std::make_unique<std::vector<pat::TriggerObjectStandAlone>>(iEvent.get(trigObjsToken_));

  const auto& trigEvent = iEvent.get(trigEvtToken_);
  const auto& trigResults = iEvent.get(trigResultsToken_);
  for(auto& trigObj : *trigObjs) {    
    trigObj.unpackFilterLabels(iEvent,trigResults);
    for(const auto& filter : filtersToAdd_){
      if(filterMatch(trigObj,trigEvent,filter)){
        trigObj.addFilterLabel(filter);
      }
    }
  }
  iEvent.put(trigObjsPutToken_,std::move(trigObjs));

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PatTrigObjFilterAdder);
