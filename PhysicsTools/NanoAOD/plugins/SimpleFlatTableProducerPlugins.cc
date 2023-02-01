#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"

#include "DataFormats/Candidate/interface/Candidate.h"
typedef SimpleFlatTableProducer<reco::Candidate> SimpleCandidateFlatTableProducer;

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
typedef EventSingletonSimpleFlatTableProducer<GenEventInfoProduct> SimpleGenEventFlatTableProducer;

#include "SimDataFormats/HTXS/interface/HiggsTemplateCrossSections.h"
typedef EventSingletonSimpleFlatTableProducer<HTXS::HiggsClassification> SimpleHTXSFlatTableProducer;

#include "DataFormats/Math/interface/Point3D.h"
typedef EventSingletonSimpleFlatTableProducer<math::XYZPointF> SimpleXYZPointFlatTableProducer;

#include "DataFormats/L1Trigger/interface/EGamma.h"
typedef BXVectorSimpleFlatTableProducer<l1t::EGamma> SimpleTriggerL1EGFlatTableProducer;

#include "DataFormats/L1Trigger/interface/Jet.h"
typedef BXVectorSimpleFlatTableProducer<l1t::Jet> SimpleTriggerL1JetFlatTableProducer;

#include "DataFormats/L1Trigger/interface/Tau.h"
typedef BXVectorSimpleFlatTableProducer<l1t::Tau> SimpleTriggerL1TauFlatTableProducer;

#include "DataFormats/L1Trigger/interface/Muon.h"
typedef BXVectorSimpleFlatTableProducer<l1t::Muon> SimpleTriggerL1MuonFlatTableProducer;

#include "DataFormats/L1Trigger/interface/EtSum.h"
typedef BXVectorSimpleFlatTableProducer<l1t::EtSum> SimpleTriggerL1EtSumFlatTableProducer;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(SimpleCandidateFlatTableProducer);
DEFINE_FWK_MODULE(SimpleGenEventFlatTableProducer);
DEFINE_FWK_MODULE(SimpleHTXSFlatTableProducer);
DEFINE_FWK_MODULE(SimpleXYZPointFlatTableProducer);
DEFINE_FWK_MODULE(SimpleTriggerL1EGFlatTableProducer);
DEFINE_FWK_MODULE(SimpleTriggerL1JetFlatTableProducer);
DEFINE_FWK_MODULE(SimpleTriggerL1MuonFlatTableProducer);
DEFINE_FWK_MODULE(SimpleTriggerL1TauFlatTableProducer);
DEFINE_FWK_MODULE(SimpleTriggerL1EtSumFlatTableProducer);
