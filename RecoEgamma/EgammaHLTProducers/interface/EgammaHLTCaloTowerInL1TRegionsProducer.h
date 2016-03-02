#ifndef RecoEgamma_EgammaHLTProducers_EgammaHLTCaloTowerInL1TRegionsProducer_h
#define RecoEgamma_EgammaHLTProducers_EgammaHLTCaloTowerInL1TRegionsProducer_h

/** \class EgammaHLTCaloTowerInL1TRegionsProducer
 *
 * Framework module that produces a collection
 * of calo towers in the region of interest for Egamma HLT reconnstruction,
 * \author M. Sani (UCSD)
 *
 */

#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"

#include <string>

namespace edm {
  class ConfigurationDescriptions;
}


class EgammaHLTCaloTowerInL1TRegionsProducer : public edm::global::EDProducer<> {
 public:

  EgammaHLTCaloTowerInL1TRegionsProducer( const edm::ParameterSet & );
  ~EgammaHLTCaloTowerInL1TRegionsProducer() {};
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::StreamID, edm::Event &, edm::EventSetup const &) const override final;

  const edm::EDGetTokenT<CaloTowerCollection> towers_;
  const double cone_;
  const edm::EDGetTokenT<l1t::EGammaBxCollection> l1EGseeds_;
  const double EtThreshold_;
  const double EThreshold_;
};

#endif
