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


#include <iostream>
#include <vector>
#include <memory>

class RecoEcalCandidateFromPatTrigObjProducer : public edm::global::EDProducer<> {
public:
  RecoEcalCandidateFromPatTrigObjProducer(const edm::ParameterSet& ps);
  ~RecoEcalCandidateFromPatTrigObjProducer()override{};

  void produce(edm::StreamID sid, edm::Event& evt, const edm::EventSetup& es) const override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  const edm::EDGetTokenT<std::vector<pat::TriggerObjectStandAlone> > trigObjsToken_;
  const edm::EDPutTokenT<reco::RecoEcalCandidateCollection> recoEcalCandidateToken_;
  const std::string collectionToMatch_;
};

RecoEcalCandidateFromPatTrigObjProducer::RecoEcalCandidateFromPatTrigObjProducer(const edm::ParameterSet& config)
    : trigObjsToken_(
      consumes<std::vector<pat::TriggerObjectStandAlone>>(config.getParameter<edm::InputTag>("trigObjs"))
    ),
    recoEcalCandidateToken_(
      produces<reco::RecoEcalCandidateCollection>(config.getParameter<std::string>("recoEcalCandidateCollection")) 
    ),
    collectionToMatch_(config.getParameter<std::string>("collectionToMatch"))
  {}

void RecoEcalCandidateFromPatTrigObjProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>(("trigObjs"), edm::InputTag(""));
  desc.add<std::string>(("recoEcalCandidateCollection"), "");
  desc.add<std::string>(("collectionToMatch"), "");
  descriptions.add(("hltRecoEcalCandidateFromPatTrigObjProducer"), desc);
}

void RecoEcalCandidateFromPatTrigObjProducer::produce(edm::StreamID sid,
                                                      edm::Event& iEvent,
                                                      const edm::EventSetup&) const {
  
  auto outColl = std::make_unique<reco::RecoEcalCandidateCollection>();

  const auto& trigObjs = iEvent.get(trigObjsToken_);
  for (const auto& trigObj : trigObjs) {    
    if (trigObj.collection() != collectionToMatch_) continue;
    reco::RecoEcalCandidate newCandidate(0, trigObj.p4(), trigObj.vertex());
    outColl->push_back(newCandidate);
  }
  iEvent.put(recoEcalCandidateToken_,std::move(outColl));
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(RecoEcalCandidateFromPatTrigObjProducer);
