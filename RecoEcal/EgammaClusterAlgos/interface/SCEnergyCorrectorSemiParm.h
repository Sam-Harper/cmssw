//--------------------------------------------------------------------------------------------------
// $Id $
//
// SCEnergyCorrectorSemiParm
//
// Helper Class for applying regression-based energy corrections with optimized BDT implementation
//
// Original Author: J.Bendavid
//
// Refactored, modernised and extended to HGCAL by S. Harper (RAL/CERN) 
// with input from S. Bhattacharya (DESY)
//--------------------------------------------------------------------------------------------------

#ifndef RecoEcal_EgammaClusterAlgos_SCEnergyCorrectorSemiParm_h
#define RecoEcal_EgammaClusterAlgos_SCEnergyCorrectorSemiParm_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "CondFormats/EgammaObjects/interface/GBRForestD.h"
#include "RecoEgamma/EgammaTools/interface/EgammaBDTOutputTransformer.h"
#include "RecoEgamma/EgammaTools/interface/HGCalShowerShapeHelper.h"

class SCEnergyCorrectorSemiParm {
public:
  SCEnergyCorrectorSemiParm();
  
  void setTokens(const edm::ParameterSet &iConfig, edm::ConsumesCollector &cc);
  void setEventSetup(const edm::EventSetup &es);
  void setEvent(const edm::Event &e);

  std::pair<double, double> getCorrections(const reco::SuperCluster &sc) const;
  void modifyObject(reco::SuperCluster &sc)const;

  std::vector<float> getRegData(const reco::SuperCluster &sc)const;
 
protected:
  class RegParam{
  public:
    RegParam(std::string meanKey="",float meanLow=0,float meanHigh=0,
	     std::string sigmaKey="",float sigmaLow=0,float sigmaHigh=0):
      meanKey_(std::move(meanKey)),sigmaKey_(std::move(sigmaKey)),
      meanOutTrans_(meanLow,meanHigh),sigmaOutTrans_(sigmaLow,sigmaHigh),
      meanForest_(nullptr),sigmaForest_(nullptr){}
       
    void setForests(const edm::EventSetup& setup);
      
    double mean(const std::vector<float>& data)const;
    double sigma(const std::vector<float>& data)const;

  private:
    std::string meanKey_;
    std::string sigmaKey_;
    EgammaBDTOutputTransformer meanOutTrans_;
    EgammaBDTOutputTransformer sigmaOutTrans_;
    const GBRForestD* meanForest_; //not owned
    const GBRForestD* sigmaForest_; //not owned
  };

  //barrel = always ecal barrel, endcap may be ECAL or HGCAL
  RegParam regParamBarrel_;
  RegParam regParamEndcap_;

  edm::ESHandle<CaloTopology> caloTopo_;
  edm::ESHandle<CaloGeometry> caloGeom_;

  edm::EDGetTokenT<EcalRecHitCollection> tokenEBRecHits_;
  edm::EDGetTokenT<EcalRecHitCollection> tokenEERecHits_;
  edm::EDGetTokenT<reco::PFRecHitCollection> tokenHgcalEERecHits_;
  edm::EDGetTokenT<reco::PFRecHitCollection> tokenHgcalHEBRecHits_;
  edm::EDGetTokenT<reco::PFRecHitCollection> tokenHgcalHEFRecHits_;
  edm::EDGetTokenT<reco::VertexCollection> tokenVertices_;

  edm::Handle<EcalRecHitCollection> recHitsEB_;
  edm::Handle<EcalRecHitCollection> recHitsEE_;
  edm::Handle<reco::PFRecHitCollection> recHitsHgcalEE_;
  edm::Handle<reco::PFRecHitCollection> recHitsHgcalHEB_;
  edm::Handle<reco::PFRecHitCollection> recHitsHgcalHEF_;
  edm::Handle<reco::VertexCollection> vertices_;

  edm::InputTag ecalHitsEBInputTag_;
  edm::InputTag ecalHitsEEInputTag_;
  edm::InputTag hgcalHitsEEInputTag_;
  edm::InputTag hgcalHitsHEBInputTag_;
  edm::InputTag hgcalHitsHEFInputTag_;
  
  edm::InputTag vertexInputTag_;

  //returns barrel for ecal barrel, otherwise returns endcap
  const RegParam& getRegParam(const DetId& detId)const{
    return detId.det()==DetId::Ecal && detId.subdetId()==EcalBarrel ?
      regParamBarrel_ : regParamEndcap_;
  }

private:
  float getInputEnergy(const reco::SuperCluster& sc)const;
  std::vector<float> getRegDataECALV1(const reco::SuperCluster& sc)const;
  std::vector<float> getRegDataECALHLTV1(const reco::SuperCluster& sc)const;
  std::vector<float> getRegDataHGCALV1(const reco::SuperCluster& sc)const;
  std::vector<float> getRegDataHGCALHLTV1(const reco::SuperCluster& sc)const;

  bool isHLT_;
  bool isPhaseII_;
  bool applySigmaIetaIphiBug_;  //there was a bug in sigmaIetaIphi for the 74X application
  int nHitsAboveThresholdEB_;
  int nHitsAboveThresholdEE_;
  int nHitsAboveThresholdHG_;
  float hitsEnergyThreshold_;
  float hgcalCylinderR_;
  HGCalShowerShapeHelper hgcalShowerShapes_;
};
#endif
