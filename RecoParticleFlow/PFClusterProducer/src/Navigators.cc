#include "FWCore/Framework/interface/MakerMacros.h"

#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitNavigatorBase.h"
#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitDualNavigator.h"
#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitCaloNavigator.h"
#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitCaloNavigatorWithTime.h"
#include "RecoParticleFlow/PFClusterProducer/interface/PFECALHashNavigator.h"

#include "Geometry/CaloTopology/interface/EcalBarrelHardcodedTopology.h"
#include "Geometry/CaloTopology/interface/EcalEndcapHardcodedTopology.h"
#include "Geometry/CaloTopology/interface/EcalPreshowerHardcodedTopology.h"

class PFRecHitEcalBarrelNavigatorWithTime : public PFRecHitCaloNavigatorWithTime<EBDetId,EcalBarrelTopology> {
public:
  PFRecHitEcalBarrelNavigatorWithTime(const edm::ParameterSet& iConfig):
    PFRecHitCaloNavigatorWithTime(iConfig)
    {

    }

  void beginEvent(const edm::EventSetup& iSetup) {
    edm::ESHandle<CaloGeometry> geoHandle;
    iSetup.get<CaloGeometryRecord>().get(geoHandle);
    topology_.reset( new EcalBarrelTopology(geoHandle) );
  }
};

class PFRecHitEcalEndcapNavigatorWithTime : public PFRecHitCaloNavigatorWithTime<EEDetId,EcalEndcapTopology> {
 public:
  PFRecHitEcalEndcapNavigatorWithTime(const edm::ParameterSet& iConfig):
    PFRecHitCaloNavigatorWithTime(iConfig)
    {

    }

  void beginEvent(const edm::EventSetup& iSetup) {
    edm::ESHandle<CaloGeometry> geoHandle;
    iSetup.get<CaloGeometryRecord>().get(geoHandle);
    topology_.reset( new EcalEndcapTopology(geoHandle) );
  }
};

class PFRecHitEcalBarrelNavigator : public PFRecHitCaloNavigator<EBDetId,EcalBarrelTopology> {
 public:
  PFRecHitEcalBarrelNavigator(const edm::ParameterSet& iConfig) {

  }

  void beginEvent(const edm::EventSetup& iSetup) {
    edm::ESHandle<CaloGeometry> geoHandle;
    iSetup.get<CaloGeometryRecord>().get(geoHandle);
    topology_.reset( new EcalBarrelTopology(geoHandle) );
  }
};

class PFRecHitEBHardCodedNavigator : public PFRecHitCaloNavigator<EBDetId,EcalBarrelHardcodedTopology> {
 public:
  PFRecHitEBHardCodedNavigator(const edm::ParameterSet& iConfig)
  { 
    topology_.reset(new EcalBarrelHardcodedTopology);
  }

  void beginEvent(const edm::EventSetup& iSetup) {}
};

class PFRecHitEEHardCodedNavigator : public PFRecHitCaloNavigator<EEDetId,EcalEndcapHardcodedTopology> {
 public:
  PFRecHitEEHardCodedNavigator(const edm::ParameterSet& iConfig)
  {
    topology_.reset(new EcalEndcapHardcodedTopology);
  }

  void beginEvent(const edm::EventSetup& iSetup) {}
};

class PFRecHitEcalEndcapNavigator : public PFRecHitCaloNavigator<EEDetId,EcalEndcapTopology> {
 public:
  PFRecHitEcalEndcapNavigator(const edm::ParameterSet& iConfig) {

  }

  void beginEvent(const edm::EventSetup& iSetup) {
    edm::ESHandle<CaloGeometry> geoHandle;
    iSetup.get<CaloGeometryRecord>().get(geoHandle);
    topology_.reset( new EcalEndcapTopology(geoHandle) );
  }
};

class PFRecHitESHardCodedNavigator : public PFRecHitCaloNavigator<ESDetId,EcalPreshowerHardcodedTopology> {
 public:
  PFRecHitESHardCodedNavigator(const edm::ParameterSet& iConfig) {
    topology_.reset(new EcalPreshowerHardcodedTopology);
  }


  void beginEvent(const edm::EventSetup& iSetup) {}
};

class PFRecHitPreshowerNavigator : public PFRecHitCaloNavigator<ESDetId,EcalPreshowerTopology> {
 public:
  PFRecHitPreshowerNavigator(const edm::ParameterSet& iConfig) {

  }


  void beginEvent(const edm::EventSetup& iSetup) {
    edm::ESHandle<CaloGeometry> geoHandle;
    iSetup.get<CaloGeometryRecord>().get(geoHandle);
    topology_.reset( new EcalPreshowerTopology(geoHandle) );
  }
};


class PFRecHitHCALNavigator : public PFRecHitCaloNavigator<HcalDetId,HcalTopology,false> {
 public:
  PFRecHitHCALNavigator(const edm::ParameterSet& iConfig) {

  }


  void beginEvent(const edm::EventSetup& iSetup) {    
      edm::ESHandle<HcalTopology> hcalTopology;
      iSetup.get<IdealGeometryRecord>().get( hcalTopology );
      topology_.release();
      topology_.reset(hcalTopology.product());
  }
};
class PFRecHitHCALNavigatorWithTime : public PFRecHitCaloNavigatorWithTime<HcalDetId,HcalTopology,false> {
 public:
  PFRecHitHCALNavigatorWithTime(const edm::ParameterSet& iConfig):
    PFRecHitCaloNavigatorWithTime(iConfig)
  {
    
  }


  void beginEvent(const edm::EventSetup& iSetup) {    
      edm::ESHandle<HcalTopology> hcalTopology;
      iSetup.get<IdealGeometryRecord>().get( hcalTopology );
      topology_.release();
      topology_.reset(hcalTopology.product());
  }
};


class PFRecHitCaloTowerNavigator : public PFRecHitCaloNavigator<CaloTowerDetId,CaloTowerTopology> {
 public:
  PFRecHitCaloTowerNavigator(const edm::ParameterSet& iConfig) {

  }


  void beginEvent(const edm::EventSetup& iSetup) {
    topology_.reset( new CaloTowerTopology() );
  }
};

typedef PFRecHitDualNavigator<PFLayer::ECAL_BARREL,
			      PFRecHitEcalBarrelNavigator,
			      PFLayer::ECAL_ENDCAP,
			    PFRecHitEcalEndcapNavigator> PFRecHitECALNavigator;


typedef PFRecHitDualNavigator<PFLayer::ECAL_BARREL,
			      PFRecHitEBHardCodedNavigator,
			      PFLayer::ECAL_ENDCAP,
			      PFRecHitEEHardCodedNavigator> PFRecHitECALHardCodedNavigator;

typedef  PFRecHitDualNavigator<PFLayer::ECAL_BARREL,
			       PFRecHitEcalBarrelNavigatorWithTime,
			       PFLayer::ECAL_ENDCAP,
	   PFRecHitEcalEndcapNavigatorWithTime> PFRecHitECALNavigatorWithTime;


EDM_REGISTER_PLUGINFACTORY(PFRecHitNavigationFactory, "PFRecHitNavigationFactory");

DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitEcalBarrelNavigator, "PFRecHitEcalBarrelNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitEcalEndcapNavigator, "PFRecHitEcalEndcapNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitEBHardCodedNavigator, "PFRecHitEBHardCodedNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitEEHardCodedNavigator, "PFRecHitEEHardCodedNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitEcalBarrelNavigatorWithTime, "PFRecHitEcalBarrelNavigatorWithTime");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitEcalEndcapNavigatorWithTime, "PFRecHitEcalEndcapNavigatorWithTime");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFECALHashNavigator, "PFECALHashNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitECALNavigator, "PFRecHitECALNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitECALHardCodedNavigator, "PFRecHitECALHardCodedNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitECALNavigatorWithTime, "PFRecHitECALNavigatorWithTime");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitCaloTowerNavigator, "PFRecHitCaloTowerNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitPreshowerNavigator, "PFRecHitPreshowerNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitESHardCodedNavigator, "PFRecHitESHardCodedNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitHCALNavigator, "PFRecHitHCALNavigator");
DEFINE_EDM_PLUGIN(PFRecHitNavigationFactory, PFRecHitHCALNavigatorWithTime, "PFRecHitHCALNavigatorWithTime");

