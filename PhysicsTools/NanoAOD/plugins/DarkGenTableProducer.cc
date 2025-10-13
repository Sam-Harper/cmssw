#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"



#include <vector>
#include <iostream>
#include <algorithm>

class DarkGenTableProducer : public edm::global::EDProducer<> {
public:
  DarkGenTableProducer(edm::ParameterSet const& params)
      : genPartTag_(consumes<reco::GenParticleCollection>(params.getParameter<edm::InputTag>("src"))),
        label_(params.getParameter<std::string>("label"))
      {
    produces<nanoaod::FlatTable>();
  }

  ~DarkGenTableProducer() override {}

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    auto darkGenTable = std::make_unique<nanoaod::FlatTable>(1, label_, true);    
    const auto& genParts = iEvent.get(genPartTag_);
    const auto [mediator, darkQuark1, darkQuark2] = getDarkParticles(genParts);


    addParticle(mediator, "Mediator_", *darkGenTable);
    addParticle(darkQuark1, "DarkQuark1_", *darkGenTable);
    addParticle(darkQuark2, "DarkQuark2_", *darkGenTable);


    iEvent.put(std::move(darkGenTable));
  }


  void addParticle(const reco::GenParticle* part, const std::string& prefix, nanoaod::FlatTable& out) const {
    if (part == nullptr) {
      out.addColumnValue<int>(prefix + "pdgId", 0, "PDG ID", nanoaod::FlatTable::IntColumn);
      out.addColumnValue<float>(prefix + "mass", 0, "Mass", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "pt", 0, "Pt", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "eta", 0, "Eta", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "phi", 0, "Phi", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<int>(prefix + "status", 0, "Status", nanoaod::FlatTable::IntColumn);
    } else {
      out.addColumnValue<int>(prefix + "pdgId", part->pdgId(), "PDG ID", nanoaod::FlatTable::IntColumn);
      out.addColumnValue<float>(prefix + "mass", part->mass(), "Mass", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "pt", part->pt(), "Pt", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "eta", part->eta(), "Eta", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "phi", part->phi(), "Phi", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<int>(prefix + "status", part->status(), "Status", nanoaod::FlatTable::IntColumn);
    }
  }


  // Returns (mother, da1, da2). Null refs mean "not found".
  const reco::GenParticle*
  getDarkMediator(const reco::GenParticleCollection& genparts) const {  
  
    for (size_t index = 0; index < genparts.size(); ++index) {
        const reco::GenParticle& genpart = genparts[index];
        if (std::abs(genpart.pdgId()) > 100000 ) {
          return getLastCopy(&genpart);
        }
    }
    return nullptr;
  }

  const reco::GenParticle* 
  getLastCopy(const reco::GenParticle* part) const {
    const reco::GenParticle* lastCopy = part;
    bool foundNext = true;
    while (foundNext) {
        foundNext = false;
        for (size_t i = 0; i < lastCopy->numberOfDaughters(); ++i) {
            const auto& dau = lastCopy->daughterRef(i);
            if (dau->pdgId() == lastCopy->pdgId()) {
                lastCopy = &*dau;
                foundNext = true;
                break;
            }
        }
    }
    return lastCopy;
  }

  std::tuple<const reco::GenParticle*, const reco::GenParticle*, const reco::GenParticle*>
  getDarkParticles(const reco::GenParticleCollection& genparts)const {
    auto mediator = getDarkMediator(genparts);
    if (mediator == nullptr) {
        return {nullptr, nullptr, nullptr}; 
    }else{
        const reco::GenParticle* darkQuark1 = nullptr;
        const reco::GenParticle* darkQuark2 = nullptr;
        for (size_t i = 0; i < mediator->numberOfDaughters(); ++i) {
            const auto& dau = mediator->daughterRef(i);
            if (std::abs(dau->pdgId()) > 0) {
                if (darkQuark1 == nullptr) {
                    darkQuark1 = &*dau;
                } else if (darkQuark2 == nullptr) {
                    darkQuark2 = &*dau;
                }
            }
        }
        return {mediator, darkQuark1, darkQuark2};
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src", edm::InputTag("genParticles"))->setComment("tag for the input gen particles");
    desc.add<std::string>("label", "DarkGen")->setComment("label for the output table");
    descriptions.add("darkGenTable", desc);
  }

protected:
  const edm::EDGetTokenT<reco::GenParticleCollection> genPartTag_;
  const std::string label_;
  
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(DarkGenTableProducer);