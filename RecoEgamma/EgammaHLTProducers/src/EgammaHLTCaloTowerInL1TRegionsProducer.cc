// makes CaloTowerCandidates from CaloTowers
// original author: M. Sani (UCSD)

#include <cmath>
#include "DataFormats/RecoCandidate/interface/RecoCaloTowerCandidate.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoEgamma/EgammaHLTProducers/interface/EgammaHLTCaloTowerInL1TRegionsProducer.h"

#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

// Math
#include "Math/GenVector/VectorUtil.h"
#include <cmath>

using namespace edm;
using namespace reco;
using namespace std;
//using namespace l1extra ;

EgammaHLTCaloTowerInL1TRegionsProducer::EgammaHLTCaloTowerInL1TRegionsProducer( const ParameterSet & p ) : towers_ (consumes<CaloTowerCollection>(p.getParameter<InputTag> ("towerCollection"))),
										   cone_ (p.getParameter<double> ("useTowersInCone")),
										   l1EGseeds_ (consumes<l1t::EGammaBxCollection>(p.getParameter< edm::InputTag > ("L1IsoCand"))),
										   
										   EtThreshold_ (p.getParameter<double> ("EtMin")),
										   EThreshold_ (p.getParameter<double> ("EMin")) {
  
  produces<CaloTowerCollection>();
}

void EgammaHLTCaloTowerInL1TRegionsProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {

  edm::ParameterSetDescription desc;
  
  desc.add<edm::InputTag>(("towerCollection"), edm::InputTag("hltRecoEcalCandidate"));
  desc.add<edm::InputTag>(("L1IsoCand"), edm::InputTag("hltTowerMakerForAll"));
  desc.add<edm::InputTag>(("L1NonIsoCand"), edm::InputTag("fixedGridRhoFastjetAllCalo")); //L1NonIsoCand is redundant and will be deleted in step2 of L1S2 migration
  desc.add<double>(("useTowersInCone"), 0.8); 
  desc.add<double>(("EtMin"), 1.0); 
  desc.add<double>(("EMin"), 1.0); 
  descriptions.add(("hltCaloTowerForEgamma"), desc);  
}


void EgammaHLTCaloTowerInL1TRegionsProducer::produce(edm::StreamID, edm::Event & evt, edm::EventSetup const &) const
{
  edm::Handle<CaloTowerCollection> caloTowers;
  evt.getByToken(towers_, caloTowers);

  edm::Handle<l1t::EGammaBxCollection> egColl;
  evt.getByToken(l1EGseeds_, egColl); 
  std::auto_ptr<CaloTowerCollection> cands(new CaloTowerCollection);
  cands->reserve(caloTowers->size());

  for (unsigned idx = 0; idx < caloTowers->size(); idx++) {
    const CaloTower* cal = &((*caloTowers) [idx]);
    if (cal->et() >= EtThreshold_ && cal->energy() >= EThreshold_) {
      math::PtEtaPhiELorentzVector p(cal->et(), cal->eta(), cal->phi(), cal->energy());
      for (auto egIt= egColl->begin(0); egIt != egColl->end(0); ++egIt){
	double delta  = ROOT::Math::VectorUtil::DeltaR(egIt->p4().Vect(), p);
	if(delta < cone_) {
	  cands->push_back(*cal);
	  break;
	}
      }
    }
  }

  evt.put( cands );  
}
