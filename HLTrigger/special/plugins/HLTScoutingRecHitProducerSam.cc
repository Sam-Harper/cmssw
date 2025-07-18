#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/Math/interface/libminifloat.h"
#include "DataFormats/ParticleFlowReco/interface/PFLayer.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"
#include "DataFormats/Scouting/interface/Run3ScoutingEBRecHit.h"
#include "DataFormats/Scouting/interface/Run3ScoutingEBRecHitCompact.h"
#include "DataFormats/Scouting/interface/Run3ScoutingEERecHit.h"
#include "DataFormats/Scouting/interface/Run3ScoutingHBHERecHit.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

class HLTScoutingRecHitProducerSam : public edm::global::EDProducer<> {
public:
  explicit HLTScoutingRecHitProducerSam(const edm::ParameterSet&);
  ~HLTScoutingRecHitProducerSam() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::StreamID, edm::Event& iEvent, edm::EventSetup const&) const final;

  const edm::EDGetTokenT<reco::PFRecHitCollection> recoPFRecHitsTokenECAL_;
  const edm::EDGetTokenT<reco::PFRecHitCollection> recoPFRecHitsTokenECALCleaned_;
  const edm::EDGetTokenT<reco::PFRecHitCollection> recoPFRecHitsTokenHBHE_;
  const double minEnergyEB_;
  const double minEnergyEE_;
  const double minEnergyHBHE_;
  const int mantissaPrecision_;
};

HLTScoutingRecHitProducerSam::HLTScoutingRecHitProducerSam(const edm::ParameterSet& iConfig)
    : recoPFRecHitsTokenECAL_(consumes(iConfig.getParameter<edm::InputTag>("pfRecHitsECAL"))),
      recoPFRecHitsTokenECALCleaned_(consumes(iConfig.getParameter<edm::InputTag>("pfRecHitsECALCleaned"))),      
      recoPFRecHitsTokenHBHE_(consumes(iConfig.getParameter<edm::InputTag>("pfRecHitsHBHE"))),
      minEnergyEB_(iConfig.getParameter<double>("minEnergyEB")),
      minEnergyEE_(iConfig.getParameter<double>("minEnergyEE")),
      minEnergyHBHE_(iConfig.getParameter<double>("minEnergyHBHE")),
      mantissaPrecision_(iConfig.getParameter<int>("mantissaPrecision")) {
  produces<Run3ScoutingEBRecHitCollection>("EB");
  produces<Run3ScoutingEBRecHitCompactCollection>("EB");
  produces<Run3ScoutingEERecHitCollection>("EE");
  produces<Run3ScoutingEBRecHitCollection>("EBCleaned");
  produces<Run3ScoutingEERecHitCollection>("EECleaned");

  produces<Run3ScoutingHBHERecHitCollection>("HBHE");
  produces<std::vector<uint16_t>>("EBEnergies");
  produces<std::vector<uint16_t>>("EBEnergiesZeroPadded");
  produces<std::vector<float>>("EBEnergies");
  produces<std::vector<float>>("EBEnergiesZeroPadded");
  produces<std::vector<uint32_t>>("EBDetIds");
  produces<std::vector<uint16_t>>("EBDetIdsCompact");
  produces<std::vector<uint32_t>>("EBFlags");
  produces<std::vector<uint32_t>>("EBFlagsZeroPadded");
  produces<std::vector<char>>("EBIEtas");
  produces<std::vector<uint16_t>>("EBIPhis");
  produces<std::vector<uint16_t>>("EBTimes");
  produces<std::vector<uint16_t>>("EBTimesZeroPadded");
  produces<std::vector<float>>("EBTimes");
  produces<std::vector<float>>("EBTimesZeroPadded");
  produces<std::vector<uint64_t>>("EBMask");
  produces<std::vector<uint16_t>>("EEEnergies");
  produces<std::vector<uint16_t>>("HBHEEnergies");
  produces<std::vector<float>>("EEEnergiesFloat");
}

namespace {
 
  template <typename T>
  std::unique_ptr<T> makeVectorReserved(size_t size) {
    auto vec = std::make_unique<T>();
    vec->reserve(size);
    return vec;
  }
}

void HLTScoutingRecHitProducerSam::produce(edm::StreamID, edm::Event& iEvent, edm::EventSetup const&) const {
  // ECAL
  auto const& recoPFRecHitsECAL = iEvent.get(recoPFRecHitsTokenECAL_);

  auto run3ScoutEBRecHits = std::make_unique<Run3ScoutingEBRecHitCollection>();
  run3ScoutEBRecHits->reserve(recoPFRecHitsECAL.size());

  auto run3ScoutEERecHits = std::make_unique<Run3ScoutingEERecHitCollection>();
  run3ScoutEERecHits->reserve(recoPFRecHitsECAL.size());

  auto run3ScoutEBRecHitsCompact = makeVectorReserved<std::vector<Run3ScoutingEBRecHitCompact>>(recoPFRecHitsECAL.size());

  auto energiesEBZP = std::make_unique<std::vector<uint16_t>>(EBDetId::kSizeForDenseIndexing,0);
  auto energiesEB = makeVectorReserved<std::vector<uint16_t>>(recoPFRecHitsECAL.size());
  auto energiesEBFloat =makeVectorReserved<std::vector<float>>(recoPFRecHitsECAL.size());
  auto energiesEBFloatZP = std::make_unique<std::vector<float>>(EBDetId::kSizeForDenseIndexing, 0.0f);
  auto timesEBZP = std::make_unique<std::vector<uint16_t>>(EBDetId::kSizeForDenseIndexing,0);
  auto timesEB = makeVectorReserved<std::vector<uint16_t>>(recoPFRecHitsECAL.size());
  auto timesEBFloat = makeVectorReserved<std::vector<float>>(recoPFRecHitsECAL.size());
  auto timesEBFloatZP = std::make_unique<std::vector<float>>(EBDetId::kSizeForDenseIndexing, 0.0f);
  auto flagsEB = makeVectorReserved<std::vector<uint32_t>>(recoPFRecHitsECAL.size());
  auto flagsEBZP = std::make_unique<std::vector<uint32_t>>(EBDetId::kSizeForDenseIndexing, 0);
  
  auto detIdsEB = makeVectorReserved<std::vector<uint32_t>>(recoPFRecHitsECAL.size());
  auto detIdsCompactEB = makeVectorReserved<std::vector<uint16_t>>(recoPFRecHitsECAL.size());
  auto iEtasEB = makeVectorReserved<std::vector<char>>(recoPFRecHitsECAL.size());
  auto iPhisEB = makeVectorReserved<std::vector<uint16_t>>(recoPFRecHitsECAL.size());
  
  auto maskEB= std::make_unique<std::vector<uint64_t>>((EBDetId::kSizeForDenseIndexing+63) / 64,0);
  
  for (auto const& rh : recoPFRecHitsECAL) {
    if (rh.layer() == PFLayer::ECAL_BARREL) {
      if (rh.energy() < minEnergyEB_) {
        continue;
      }
      uint16_t energyEB = MiniFloatConverter::float32to16(rh.energy());
      uint16_t timeEB = MiniFloatConverter::float32to16(rh.time());

      run3ScoutEBRecHits->emplace_back(
          MiniFloatConverter::reduceMantissaToNbitsRounding(rh.energy(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(rh.time(), mantissaPrecision_),
          rh.detId(),
          rh.flags());

      run3ScoutEBRecHitsCompact->emplace_back(
          energyEB,
          timeEB,
          EBDetId(rh.detId()).denseIndex(),
          rh.flags());
      
      float energyEBFloat = MiniFloatConverter::reduceMantissaToNbitsRounding(rh.energy(), mantissaPrecision_);
      energiesEBZP->at(EBDetId(rh.detId()).denseIndex()) = energyEB;
      energiesEB->push_back(energyEB);
      detIdsEB->push_back(rh.detId());
      detIdsCompactEB->push_back(EBDetId(rh.detId()).denseIndex());
      iEtasEB->push_back(EBDetId(rh.detId()).ieta());
      iPhisEB->push_back(EBDetId(rh.detId()).iphi());
      energiesEBFloatZP->at(EBDetId(rh.detId()).denseIndex()) = energyEBFloat;
      energiesEBFloat->push_back(MiniFloatConverter::reduceMantissaToNbitsRounding(rh.energy(), mantissaPrecision_));
      
      float timeEBFloat = MiniFloatConverter::reduceMantissaToNbitsRounding(rh.time(), mantissaPrecision_);
      timesEBZP->at(EBDetId(rh.detId()).denseIndex());
      timesEB->push_back(timeEB);
      timesEBFloatZP->at(EBDetId(rh.detId()).denseIndex()) = timeEBFloat;
      timesEBFloat->push_back(timeEBFloat);
      flagsEB->push_back(rh.flags());
      flagsEBZP->at(EBDetId(rh.detId()).denseIndex()) = rh.flags();

    } else if (rh.layer() == PFLayer::ECAL_ENDCAP) {
      if (rh.energy() < minEnergyEE_) {
        continue;
      }

      run3ScoutEERecHits->emplace_back(
          MiniFloatConverter::reduceMantissaToNbitsRounding(rh.energy(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(rh.time(), mantissaPrecision_),
          rh.detId());
        /*
        
      uint16_t energyEE = MiniFloatConverter::float32to16(rh.energy());
      energiesEE->at(EEDetId(rh.detId()).denseIndex()) = energyEE;
      energiesEEFloat->at(EEDetId(rh.detId()).denseIndex()) = MiniFloatConverter::reduceMantissaToNbitsRounding(rh.energy(), mantissaPrecision_);
    */
    } else {
      edm::LogWarning("HLTScoutingRecHitProducerSam")
          << "Skipping PFRecHit because of unexpected PFLayer value (" << rh.layer() << ").";
    }
  }
  for (size_t i = 0; i < energiesEBZP->size(); ++i) {
    if (energiesEBZP->at(i) != 0) {      
      maskEB->at(i / 64) |= (1ULL << (i % 64));
    }
  }  


  iEvent.put(std::move(run3ScoutEBRecHits), "EB");
  iEvent.put(std::move(run3ScoutEBRecHitsCompact), "EB");
  iEvent.put(std::move(run3ScoutEERecHits), "EE");
  iEvent.put(std::move(energiesEB), "EBEnergies");
  iEvent.put(std::move(energiesEBZP), "EBEnergiesZeroPadded");
  iEvent.put(std::move(energiesEBFloat), "EBEnergies");
  iEvent.put(std::move(energiesEBFloatZP), "EBEnergiesZeroPadded");
  iEvent.put(std::move(timesEB), "EBTimes");
  iEvent.put(std::move(timesEBZP), "EBTimesZeroPadded");
  iEvent.put(std::move(timesEBFloat), "EBTimes");
  iEvent.put(std::move(timesEBFloatZP), "EBTimesZeroPadded");
  iEvent.put(std::move(detIdsEB), "EBDetIds");
  iEvent.put(std::move(detIdsCompactEB), "EBDetIdsCompact");
  iEvent.put(std::move(iEtasEB), "EBIEtas");
  iEvent.put(std::move(iPhisEB), "EBIPhis");
  iEvent.put(std::move(flagsEB), "EBFlags");
  iEvent.put(std::move(flagsEBZP), "EBFlagsZeroPadded");
  iEvent.put(std::move(maskEB), "EBMask");

  auto const& recoPFRecHitsECALCleaned = iEvent.get(recoPFRecHitsTokenECALCleaned_);


  auto run3ScoutEBRecHitsCleaned = makeVectorReserved<Run3ScoutingEBRecHitCollection>(recoPFRecHitsECALCleaned.size());
  auto run3ScoutEERecHitsCleaned = makeVectorReserved<Run3ScoutingEERecHitCollection>( recoPFRecHitsECALCleaned.size());
  
   for (auto const& rh : recoPFRecHitsECALCleaned) {
    if (rh.layer() == PFLayer::ECAL_BARREL) {
      if (rh.energy() < minEnergyEB_) {
        continue;
      }
      run3ScoutEBRecHitsCleaned->emplace_back(
          MiniFloatConverter::reduceMantissaToNbitsRounding(rh.energy(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(rh.time(), mantissaPrecision_),
          rh.detId(),
          rh.flags());
    } else if (rh.layer() == PFLayer::ECAL_ENDCAP) {
      if (rh.energy() < minEnergyEE_) {
        continue;
      }
      run3ScoutEERecHitsCleaned->emplace_back(
          MiniFloatConverter::reduceMantissaToNbitsRounding(rh.energy(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(rh.time(), mantissaPrecision_),
          rh.detId());
      }
    }
  iEvent.put(std::move(run3ScoutEBRecHitsCleaned), "EBCleaned");
  iEvent.put(std::move(run3ScoutEERecHitsCleaned), "EECleaned");
  
  // HBHE
  auto const& recoPFRecHitsHBHE = iEvent.get(recoPFRecHitsTokenHBHE_);

  auto run3ScoutHBHERecHits = std::make_unique<Run3ScoutingHBHERecHitCollection>();
  run3ScoutHBHERecHits->reserve(recoPFRecHitsHBHE.size());

  for (auto const& rh : recoPFRecHitsHBHE) {
    if (rh.energy() < minEnergyHBHE_) {
      continue;
    }

    run3ScoutHBHERecHits->emplace_back(
        MiniFloatConverter::reduceMantissaToNbitsRounding(rh.energy(), mantissaPrecision_),
        rh.detId());
  }

  iEvent.put(std::move(run3ScoutHBHERecHits), "HBHE");
}

void HLTScoutingRecHitProducerSam::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("pfRecHitsECAL", edm::InputTag("hltParticleFlowRecHitECALUnseeded"));
  desc.add<edm::InputTag>("pfRecHitsECALCleaned", edm::InputTag("hltParticleFlowRecHitECALUnseeded", "Cleaned"));
  desc.add<edm::InputTag>("pfRecHitsHBHE", edm::InputTag("hltParticleFlowRecHitHBHE"));
  desc.add<double>("minEnergyEB", -1)->setComment("Minimum energy of the EcalBarrel PFRecHit in GeV");
  desc.add<double>("minEnergyEE", -1)->setComment("Minimum energy of the EcalEndcap PFRecHit in GeV");
  desc.add<double>("minEnergyHBHE", -1)->setComment("Minimum energy of the HBHE PFRecHit in GeV");
  desc.add<int>("mantissaPrecision", 10)->setComment("default of 10 corresponds to float16, change to 23 for float32");
  descriptions.add("HLTScoutingRecHitProducerSam", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HLTScoutingRecHitProducerSam);
