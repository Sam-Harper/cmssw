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
        label_(params.getParameter<std::string>("label")),
        genPartLabel_(params.getParameter<std::string>("genPartLabel"))
      {
    produces<nanoaod::FlatTable>();
    produces<nanoaod::FlatTable>("genPartTable");
      }

  ~DarkGenTableProducer() override {}

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    auto darkGenTable = std::make_unique<nanoaod::FlatTable>(1, label_, true);    
    
    const auto& genParts = iEvent.get(genPartTag_);
    auto genPartTable = std::make_unique<nanoaod::FlatTable>(genParts.size(), genPartLabel_, false, true);
    const auto [mediator, darkQuark1, darkQuark2] = getDarkParticles(genParts);


    addParticle(mediator, genParts, "Mediator_", *darkGenTable);
    addParticle(darkQuark1, genParts, "DarkQuark1_", *darkGenTable);
    addParticle(darkQuark2, genParts, "DarkQuark2_", *darkGenTable);


    size_t darkQuark1Index = darkQuark1 ? std::distance(&genParts[0], darkQuark1) : 0;
    size_t darkQuark2Index = darkQuark2 ? std::distance(&genParts[0], darkQuark2) : 0;

    std::vector<size_t> darkQ1DauIndices;
    std::vector<size_t> darkQ2DauIndices;

    getAllDaughterIndices(darkQuark1Index, genParts, darkQ1DauIndices);
    getAllDaughterIndices(darkQuark2Index, genParts, darkQ2DauIndices);

    std::vector<size_t> darkMo(genParts.size(), -1);
    for (size_t idx : darkQ1DauIndices) {
        darkMo[idx] = darkQuark1Index;
    }
    for (size_t idx : darkQ2DauIndices) {
        darkMo[idx] = darkQuark2Index;
    }
    genPartTable->addColumn<int>("darkMo", darkMo, "Index of the dark quark mother in genParticles collection, -1 if none", nanoaod::FlatTable::IntColumn);

    iEvent.put(std::move(darkGenTable));
    iEvent.put(std::move(genPartTable), "genPartTable");
  }


  void addParticle(const reco::GenParticle* part, const reco::GenParticleCollection& genParts, const std::string& prefix, nanoaod::FlatTable& out) const {
    if (part == nullptr) {
      out.addColumnValue<int>(prefix + "pdgId", 0, "PDG ID", nanoaod::FlatTable::IntColumn);
      out.addColumnValue<float>(prefix + "mass", 0, "Mass", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "pt", 0, "Pt", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "eta", 0, "Eta", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "phi", 0, "Phi", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<int>(prefix + "status", 0, "Status", nanoaod::FlatTable::IntColumn);
      out.addColumnValue<int>(prefix + "idx", -1, "Index in genParticles collection", nanoaod::FlatTable::IntColumn);
    } else {
      out.addColumnValue<int>(prefix + "pdgId", part->pdgId(), "PDG ID", nanoaod::FlatTable::IntColumn);
      out.addColumnValue<float>(prefix + "mass", part->mass(), "Mass", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "pt", part->pt(), "Pt", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "eta", part->eta(), "Eta", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<float>(prefix + "phi", part->phi(), "Phi", nanoaod::FlatTable::FloatColumn);
      out.addColumnValue<int>(prefix + "status", part->status(), "Status", nanoaod::FlatTable::IntColumn);
      auto index = std::distance(&genParts[0], part);
      out.addColumnValue<int>(prefix + "idx", index, "Index in genParticles collection", nanoaod::FlatTable::IntColumn);      
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
  size_t 
  getLastCopyIndex(const size_t partIndex,const reco::GenParticleCollection& genparts) const {
    const reco::GenParticle* lastCopy = &genparts[partIndex];
    size_t lastCopyIndex = partIndex;
    bool foundNext = true;
    while (foundNext) {
        foundNext = false;
        for (size_t i = 0; i < lastCopy->numberOfDaughters(); ++i) {
            const auto& dau = lastCopy->daughterRef(i);
            if (dau->pdgId() == lastCopy->pdgId()) {
                lastCopyIndex = dau.key();
                lastCopy = &*dau;
                foundNext = true;
                break;
            }
        }
    }
    return lastCopyIndex;
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

  void getAllDaughters(const reco::GenParticle* part, std::vector<const reco::GenParticle*>& daughters) const {
    for (size_t i = 0; i < part->numberOfDaughters(); ++i) {
        const auto& dau = part->daughterRef(i);
        daughters.push_back(&*dau);
        getAllDaughters(&*dau, daughters);
    }
  }

  void getAllDaughterIndices(const size_t partIndex, const reco::GenParticleCollection& genparts, std::vector<size_t>& daughterIndices) const {
    const reco::GenParticle* part = &genparts[partIndex];
    for (size_t i = 0; i < part->numberOfDaughters(); ++i) {
        const auto& dau = part->daughterRef(i);
        daughterIndices.push_back(dau.key());
        getAllDaughterIndices(dau.key(), genparts, daughterIndices);
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src", edm::InputTag("genParticles"))->setComment("tag for the input gen particles");
    desc.add<std::string>("label", "DarkGen")->setComment("label for the output table");
    desc.add<std::string>("genPartLabel", "GenPart")->setComment("label for the gen particle table");
    descriptions.add("darkGenTable", desc);
  }

protected:
  const edm::EDGetTokenT<reco::GenParticleCollection> genPartTag_;
  const std::string label_;
  const std::string genPartLabel_;
  
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(DarkGenTableProducer);