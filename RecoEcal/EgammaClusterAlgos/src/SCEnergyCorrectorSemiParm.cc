#include "RecoEcal/EgammaClusterAlgos/interface/SCEnergyCorrectorSemiParm.h"

#include "CondFormats/DataRecord/interface/GBRDWrapperRcd.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalTools.h"

#include "FWCore/Utilities/interface/isFinite.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include <vdt/vdtMath.h>

using namespace reco;

namespace{
  std::pair<edm::Ptr<reco::CaloCluster>,float> getMaxDRNonSeedCluster(const reco::SuperCluster& sc){
    float maxDR2 = 0.;
    edm::Ptr<reco::CaloCluster> maxDRClus;
    const edm::Ptr<reco::CaloCluster> & seedClus = sc.seed();
 
    for (const auto& clus : sc.clusters()){
      if (clus == seedClus){
	continue;
      }
    
      // find cluster with max dR
      const double dr2 = reco::deltaR2(*clus, *seedClus);
      if (dr2 > maxDR2) {
        maxDR2 = dr2;
      }
    }
    return {maxDRClus,std::sqrt(maxDR2)};
  }
  template<typename T>
  int countRecHits(const T& recHitHandle,float threshold){
    int count=0;
    if(recHitHandle.isValid()){
      for(const auto& recHit : *recHitHandle){
	if(recHit.energy()>threshold){
	  count++;
	}
      }
    }
    return count;
  }
}

SCEnergyCorrectorSemiParm::SCEnergyCorrectorSemiParm()
  : caloTopo_(nullptr),caloGeom_(nullptr),
    isHLT_(false),isPhaseII_(false),applySigmaIetaIphiBug_(false),
    nHitsAboveThresholdEB_(0),nHitsAboveThresholdEE_(0),nHitsAboveThresholdHG_(0),
    hitsEnergyThreshold_(-1.),hgcalCylinderR_(0.) {}
      

void SCEnergyCorrectorSemiParm::setTokens(const edm::ParameterSet &iConfig, edm::ConsumesCollector &cc) {
  isHLT_ = iConfig.getParameter<bool>("isHLT");
  isPhaseII_ = iConfig.getParameter<bool>("isPhaseII");
  applySigmaIetaIphiBug_ = iConfig.getParameter<bool>("applySigmaIetaIphiBug");
  tokenEBRecHits_ = cc.consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ecalRecHitsEB"));
  if(not isPhaseII_){
    tokenEERecHits_ = cc.consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ecalRecHitsEE"));
  }else{
    tokenHgcalEERecHits_ = cc.consumes<reco::PFRecHitCollection>(iConfig.getParameter<edm::InputTag>("hgcalRecHitsEE"));
    tokenHgcalHEBRecHits_ = cc.consumes<reco::PFRecHitCollection>(iConfig.getParameter<edm::InputTag>("hgcalRecHitsHEB"));
    tokenHgcalHEFRecHits_ = cc.consumes<reco::PFRecHitCollection>(iConfig.getParameter<edm::InputTag>("hgcalRecHitsHEF"));
    hgcalCylinderR_ = iConfig.getParameter<double>("hgcalCylinderR");
  }

  regParamBarrel_ = RegParam(iConfig.getParameter<std::string>("regressionKeyEB"),
			     iConfig.getParameter<double>("regressionMinEB"),
			     iConfig.getParameter<double>("regressionMaxEB"),
			     iConfig.getParameter<std::string>("uncertaintyKeyEB"),
			     iConfig.getParameter<double>("uncertaintyMinEB"),
			     iConfig.getParameter<double>("uncertaintyMaxEB"));
  regParamEndcap_ = RegParam(iConfig.getParameter<std::string>("regressionKeyEE"),
			     iConfig.getParameter<double>("regressionMinEE"),
			     iConfig.getParameter<double>("regressionMaxEE"),
			     iConfig.getParameter<std::string>("uncertaintyKeyEE"),
			     iConfig.getParameter<double>("uncertaintyMinEE"),
			     iConfig.getParameter<double>("uncertaintyMaxEE"));

  if (not isHLT_) {
    tokenVertices_ = cc.consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"));
  } else {
    hitsEnergyThreshold_ = iConfig.getParameter<double>("eThreshold");
  }
}

void SCEnergyCorrectorSemiParm::setEventSetup(const edm::EventSetup &es) {
  es.get<CaloTopologyRecord>().get(caloTopo_);
  es.get<CaloGeometryRecord>().get(caloGeom_);

  regParamBarrel_.setForests(es);
  regParamEndcap_.setForests(es);
  
  if(isPhaseII_){
    hgcalShowerShapes_.initPerSetup(es);
  }
}

void SCEnergyCorrectorSemiParm::setEvent(const edm::Event &event) {
  event.getByToken(tokenEBRecHits_, recHitsEB_);
  if(!isPhaseII_){
    event.getByToken(tokenEERecHits_, recHitsEE_);
  }else{
    event.getByToken(tokenHgcalEERecHits_, recHitsHgcalEE_);
    event.getByToken(tokenHgcalHEBRecHits_, recHitsHgcalHEB_);
    event.getByToken(tokenHgcalHEFRecHits_, recHitsHgcalHEF_);
    hgcalShowerShapes_.initPerEvent(*recHitsHgcalEE_);
  }
  if(isHLT_ || isPhaseII_){
    nHitsAboveThresholdEB_ = countRecHits(recHitsEB_,hitsEnergyThreshold_);
    nHitsAboveThresholdEE_ = countRecHits(recHitsEE_,hitsEnergyThreshold_);
    nHitsAboveThresholdHG_ = countRecHits(recHitsHgcalEE_,hitsEnergyThreshold_)+
      countRecHits(recHitsHgcalHEB_,hitsEnergyThreshold_)+
      countRecHits(recHitsHgcalHEF_,hitsEnergyThreshold_);
  }
  if (!isHLT_){
    event.getByToken(tokenVertices_, vertices_);
  }
}

std::pair<double, double> SCEnergyCorrectorSemiParm::getCorrections(const reco::SuperCluster &sc) const {
  std::pair<double, double> corrEnergyAndRes = {-1,-1};

  const auto regData = getRegData(sc);
  if(regData.empty()){
    //supercluster has no valid regression, return default values
    return corrEnergyAndRes;
  }
  const auto& regParam = getRegParam(sc.seed()->seed());

  double mean = regParam.mean(regData);
  double sigma = regParam.sigma(regData);
    
  const double energyCorr = mean * getInputEnergy(sc);
  const double resolutionEst = sigma * energyCorr;
 
  corrEnergyAndRes.first = energyCorr;
  corrEnergyAndRes.second = resolutionEst;

  return corrEnergyAndRes;
}

void SCEnergyCorrectorSemiParm::modifyObject(reco::SuperCluster &sc)const {
  std::pair<double, double> cor = getCorrections(sc);
  if (cor.first < 0)
    return;
  sc.setEnergy(cor.first);
  sc.setCorrectedEnergy(cor.first);
  if (cor.second >=0) {
    sc.setCorrectedEnergyUncertainty(cor.second);
  }
}

std::vector<float> SCEnergyCorrectorSemiParm::getRegData(const reco::SuperCluster &sc)const
{
  switch(sc.seed()->seed().det()){
  case DetId::Ecal:
    return isHLT_ ? getRegDataECALHLTV1(sc) : getRegDataECALV1(sc);
  case DetId::HGCalEE:
    return isHLT_ ? getRegDataHGCALHLTV1(sc) : getRegDataHGCALV1(sc);
  default:
    return std::vector<float>();
  }
}

void SCEnergyCorrectorSemiParm::RegParam::setForests(const edm::EventSetup& setup)
{
  edm::ESHandle<GBRForestD> meanHandle;
  edm::ESHandle<GBRForestD> sigmaHandle;
  setup.get<GBRDWrapperRcd>().get(meanKey_, meanHandle);
  setup.get<GBRDWrapperRcd>().get(sigmaKey_, sigmaHandle);
  meanForest_ = meanHandle.product();
  sigmaForest_ = sigmaHandle.product();
}

double SCEnergyCorrectorSemiParm::RegParam::mean(const std::vector<float>& data)const
{
  return meanForest_ ?  meanOutTrans_(meanForest_->GetResponse(data.data())) : -1;
}

double SCEnergyCorrectorSemiParm::RegParam::sigma(const std::vector<float>& data)const
{
  return sigmaForest_ ?  sigmaOutTrans_(sigmaForest_->GetResponse(data.data())) : -1;
}

float SCEnergyCorrectorSemiParm::getInputEnergy(const reco::SuperCluster& sc)const
{
  const DetId& seedId = sc.seed()->seed();
  //preshower is only used for HLT Ecal Endcap
  //for reco, the mustache is mainly used to calibrate the ECAL hence no preshower
  if(isHLT_ && seedId.det()==DetId::Ecal && seedId.subdetId()==EcalEndcap){
    return sc.rawEnergy() + sc.preshowerEnergy();
  }else{
    return sc.rawEnergy();
  } 
}

std::vector<float> SCEnergyCorrectorSemiParm::getRegDataECALV1(const reco::SuperCluster& sc)const
{
  std::vector<float> eval(30,0.); 

  const reco::CaloCluster &seedCluster = *(sc.seed());
  const bool iseb = seedCluster.hitsAndFractions()[0].first.subdetId() == EcalBarrel;
  const EcalRecHitCollection *recHits = iseb ? recHitsEB_.product() : recHitsEE_.product();

  const CaloTopology *topo = caloTopo_.product();
  
  const double raw_energy = sc.rawEnergy();
  const int numberOfClusters = sc.clusters().size();

  std::vector<float> localCovariances = EcalClusterTools::localCovariances(seedCluster, recHits, topo);

  const float eLeft = EcalClusterTools::eLeft(seedCluster, recHits, topo);
  const float eRight = EcalClusterTools::eRight(seedCluster, recHits, topo);
  const float eTop = EcalClusterTools::eTop(seedCluster, recHits, topo);
  const float eBottom = EcalClusterTools::eBottom(seedCluster, recHits, topo);
  
  float sigmaIetaIeta = sqrt(localCovariances[0]);
  float sigmaIetaIphi = std::numeric_limits<float>::max();
  float sigmaIphiIphi = std::numeric_limits<float>::max();
  
  if (!edm::isNotFinite(localCovariances[2]))
    sigmaIphiIphi = sqrt(localCovariances[2]);
  
  // extra shower shapes
  const float see_by_spp =
    sigmaIetaIeta * (applySigmaIetaIphiBug_ ? std::numeric_limits<float>::max() : sigmaIphiIphi);
  if (see_by_spp > 0) {
    sigmaIetaIphi = localCovariances[1] / see_by_spp;
  } else if (localCovariances[1] > 0) {
    sigmaIetaIphi = 1.f;
  } else {
    sigmaIetaIphi = -1.f;
  }
  
  // calculate sub-cluster variables
  std::vector<float> clusterRawEnergy;
  clusterRawEnergy.resize(std::max(3, numberOfClusters), 0);
  std::vector<float> clusterDEtaToSeed;
  clusterDEtaToSeed.resize(std::max(3, numberOfClusters), 0);
  std::vector<float> clusterDPhiToSeed;
  clusterDPhiToSeed.resize(std::max(3, numberOfClusters), 0);
  float clusterMaxDR = 999.;
  float clusterMaxDRDPhi = 999.;
  float clusterMaxDRDEta = 999.;
  float clusterMaxDRRawEnergy = 0.;
  
  size_t iclus = 0;
  float maxDR = 0;
  edm::Ptr<reco::CaloCluster> pclus;
  const edm::Ptr<reco::CaloCluster> &theseed = sc.seed();
  // loop over all clusters that aren't the seed
  auto clusend = sc.clustersEnd();
  for (auto clus = sc.clustersBegin(); clus != clusend; ++clus) {
    pclus = *clus;
    
    if (theseed == pclus)
      continue;
    clusterRawEnergy[iclus] = pclus->energy();
    clusterDPhiToSeed[iclus] = reco::deltaPhi(pclus->phi(), theseed->phi());
    clusterDEtaToSeed[iclus] = pclus->eta() - theseed->eta();
    
    // find cluster with max dR
    const auto the_dr = reco::deltaR(*pclus, *theseed);
    if (the_dr > maxDR) {
      maxDR = the_dr;
      clusterMaxDR = maxDR;
      clusterMaxDRDPhi = clusterDPhiToSeed[iclus];
      clusterMaxDRDEta = clusterDEtaToSeed[iclus];
      clusterMaxDRRawEnergy = clusterRawEnergy[iclus];
    }
    ++iclus;
  }
  
  eval[0] = vertices_->size();
  eval[1] = raw_energy;
  eval[2] = sc.etaWidth();
  eval[3] = sc.phiWidth();
  eval[4] = EcalClusterTools::e3x3(seedCluster, recHits, topo) / raw_energy;
  eval[5] = seedCluster.energy() / raw_energy;
  eval[6] = EcalClusterTools::eMax(seedCluster, recHits) / raw_energy;
  eval[7] = EcalClusterTools::e2nd(seedCluster, recHits) / raw_energy;
  eval[8] = (eLeft + eRight != 0.f ? (eLeft - eRight) / (eLeft + eRight) : 0.f);
  eval[9] = (eTop + eBottom != 0.f ? (eTop - eBottom) / (eTop + eBottom) : 0.f);
  eval[10] = sigmaIetaIeta;
  eval[11] = sigmaIetaIphi;
  eval[12] = sigmaIphiIphi;
  eval[13] = std::max(0, numberOfClusters - 1);
  eval[14] = clusterMaxDR;
  eval[15] = clusterMaxDRDPhi;
  eval[16] = clusterMaxDRDEta;
  eval[17] = clusterMaxDRRawEnergy / raw_energy;
  eval[18] = clusterRawEnergy[0] / raw_energy;
  eval[19] = clusterRawEnergy[1] / raw_energy;
  eval[20] = clusterRawEnergy[2] / raw_energy;
  eval[21] = clusterDPhiToSeed[0];
  eval[22] = clusterDPhiToSeed[1];
  eval[23] = clusterDPhiToSeed[2];
  eval[24] = clusterDEtaToSeed[0];
  eval[25] = clusterDEtaToSeed[1];
  eval[26] = clusterDEtaToSeed[2];
  if (iseb) {
    EBDetId ebseedid(seedCluster.seed());
    eval[27] = ebseedid.ieta();
    eval[28] = ebseedid.iphi();
  } else {
    EEDetId eeseedid(seedCluster.seed());
    eval[27] = eeseedid.ix();
    eval[28] = eeseedid.iy();
    //seed cluster eta is only needed for the 106X Ultra Legacy regressions
    //and was not used in the 74X regression however as its just an extra varaible
    //at the end, its harmless to add for the 74X regression
    eval[29] = seedCluster.eta();
  }
  return eval;
}
 
std::vector<float> SCEnergyCorrectorSemiParm::getRegDataECALHLTV1(const reco::SuperCluster& sc)const
{
  std::vector<float> eval(7,0.);
  auto maxDRNonSeedClus = getMaxDRNonSeedCluster(sc);
  //ECAL regressions train with maxDR being 999 if there is no cluster other than seed clus
  const float clusterMaxDR =  maxDRNonSeedClus.first.isNonnull() ? maxDRNonSeedClus.second : 999.;
  
  const reco::CaloCluster &seedCluster = *(sc.seed());
  const bool iseb = seedCluster.hitsAndFractions()[0].first.subdetId() == EcalBarrel;
  const EcalRecHitCollection *recHits = iseb ? recHitsEB_.product() : recHitsEE_.product();
  
  eval[0] = nHitsAboveThresholdEB_+nHitsAboveThresholdEE_;
  eval[1] = sc.eta();
  eval[2] = sc.phiWidth();
  eval[3] = EcalClusterTools::e3x3(seedCluster, recHits, caloTopo_.product()) / sc.rawEnergy();
  eval[4] = std::max(0, static_cast<int>(sc.clusters().size()) - 1);
  eval[5] = clusterMaxDR;
  eval[6] = sc.rawEnergy();
  
  return eval;
}

std::vector<float> SCEnergyCorrectorSemiParm::getRegDataHGCALV1(const reco::SuperCluster& sc)const {
  std::vector<float> eval(17,0.);

  auto pcaWidths = hgcalShowerShapes_.getPCAWidths(hgcalCylinderR_);

  eval[0] = sc.rawEnergy();
  eval[1] = sc.eta();
  eval[2] = sc.etaWidth();
  eval[3] = sc.phiWidth();
  eval[4] = sc.clusters().size();
  eval[5] = sc.hitsAndFractions().size();
  eval[6] = sc.eta()-sc.seed()->eta();
  eval[7] = reco::deltaPhi(sc.phi(),sc.seed()->phi());
  // eval[8] = eMax/sc.rawEnergy();
  // eval[9] = e2nd/sc.rawEnergy();
  eval[10] = std::sqrt(pcaWidths.sigma2uu);
  eval[11] = std::sqrt(pcaWidths.sigma2vv);
  eval[12] = std::sqrt(pcaWidths.sigma2ww);
  eval[13] = hgcalShowerShapes_.getRvar(hgcalCylinderR_, sc.energy());
  eval[14] = sc.seed()->energy()/sc.rawEnergy();
  eval[15] = nHitsAboveThresholdEB_ + nHitsAboveThresholdHG_;
  
  return eval;
}

std::vector<float> SCEnergyCorrectorSemiParm::getRegDataHGCALHLTV1(const reco::SuperCluster& sc)const
{
  std::vector<float> eval(7,0.);
  const float clusterMaxDR = getMaxDRNonSeedCluster(sc).second;
  

  //hgcalShowerShapes_.initPerObject(sc.hitsAndFractions());
 
  eval[0] = nHitsAboveThresholdEB_ + nHitsAboveThresholdHG_;
  eval[1] = sc.eta();
  eval[2] = sc.phiWidth();
  eval[3] = hgcalShowerShapes_.getRvar(hgcalCylinderR_, sc.energy());
  eval[4] = std::max(0, static_cast<int>(sc.clusters().size()) - 1);
  eval[5] = clusterMaxDR;
  eval[6] = sc.rawEnergy();
  
  return eval;
}

