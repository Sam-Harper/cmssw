


#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "CondFormats/GBRForest/interface/GBRForestD.h"
#include "CondFormats/DataRecord/interface/GBRDWrapperRcd.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "RecoEgamma/EgammaTools/interface/EgammaBDTOutputTransformer.h"

namespace {
  reco::RecoEcalCandidate createCorrectedEcalCand(const reco::RecoEcalCandidate& cand,double corrEnergy){
    const auto p4 = cand.p4()*corrEnergy/cand.p4().E();
    reco::RecoEcalCandidate newCand(cand.charge(),p4,cand.vertex(),cand.pdgId(),cand.status());
    newCand.setSuperCluster(cand.superCluster());
    return newCand;    				      
  }
}

class EgammaHLTEnergyRegressionCorrector : public edm::stream::EDProducer<> {
public:

  EgammaHLTEnergyRegressionCorrector(const edm::ParameterSet& iConfig);
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

  template <edm::Transition tr = edm::Transition::Event>
  void setTokens(const edm::ParameterSet& iConfig);

  void setEventSetup(const edm::EventSetup& es);
  void setEvent(const edm::Event& e);

std::vector<float> getFeatures(const reco::RecoEcalCandidateRef& candRef,const edm::Event& iEvent) const;
std::pair<double, double> getCorrections(const reco::RecoEcalCandidateRef& candRef,const edm::Event& iEvent) const; 
private:
  class RegParam {
  public:
    RegParam(std::string meanKey = "",
             float meanLow = 0,
             float meanHigh = 0,
             std::string sigmaKey = "",
             float sigmaLow = 0,
             float sigmaHigh = 0)
        : meanKey_(std::move(meanKey)),
          sigmaKey_(std::move(sigmaKey)),
          meanOutTrans_(meanLow, meanHigh),
          sigmaOutTrans_(sigmaLow, sigmaHigh) {}
    RegParam(edm::ConsumesCollector cc,
             std::string meanKey = "",
             float meanLow = 0,
             float meanHigh = 0,
             std::string sigmaKey = "",
             float sigmaLow = 0,
             float sigmaHigh = 0)
        : RegParam(meanKey, meanLow, meanHigh, sigmaKey, sigmaLow, sigmaHigh) {
      setTokens(cc);
    }
    template <edm::Transition esTransition = edm::Transition::BeginLuminosityBlock>
    void setTokens(edm::ConsumesCollector cc);
    void setForests(const edm::EventSetup& setup);

    double mean(const std::vector<float>& data) const;
    double sigma(const std::vector<float>& data) const;

  private:
    std::string meanKey_;
    std::string sigmaKey_;
    EgammaBDTOutputTransformer meanOutTrans_;
    EgammaBDTOutputTransformer sigmaOutTrans_;
    const GBRForestD* meanForest_;
    const GBRForestD* sigmaForest_;
    edm::ESGetToken<GBRForestD, GBRDWrapperRcd> meanForestToken_;
    edm::ESGetToken<GBRForestD, GBRDWrapperRcd> sigmaForestToken_;
  };

  //returns barrel for ecal barrel, otherwise returns endcap
  const RegParam& getRegParam(const DetId& detId) const {
    return detId.det() == DetId::Ecal && detId.subdetId() == EcalBarrel ? regParamBarrel_ : regParamEndcap_;
  }

  RegParam regParamBarrel_;
  RegParam regParamEndcap_;
  edm::EDGetTokenT<reco::RecoEcalCandidateCollection> src_;
  std::vector<std::pair<std::string,edm::EDGetTokenT<reco::RecoEcalCandidateIsolationMap> > > features_;

};


EgammaHLTEnergyRegressionCorrector::EgammaHLTEnergyRegressionCorrector(const edm::ParameterSet& iConfig){
  setTokens(iConfig);
  produces<reco::RecoEcalCandidateCollection>(); 
}

template <edm::Transition esTransition>
void EgammaHLTEnergyRegressionCorrector::RegParam::setTokens(edm::ConsumesCollector cc) {
  meanForestToken_ = cc.esConsumes<GBRForestD, GBRDWrapperRcd, esTransition>(edm::ESInputTag("", meanKey_));
  sigmaForestToken_ = cc.esConsumes<GBRForestD, GBRDWrapperRcd, esTransition>(edm::ESInputTag("", sigmaKey_));
}

template <edm::Transition esTransition>
void EgammaHLTEnergyRegressionCorrector::setTokens(const edm::ParameterSet& iConfig) {

  regParamBarrel_ = RegParam(iConfig.getParameter<std::string>("regressionKeyEB"),
                             iConfig.getParameter<double>("regressionMinEB"),
                             iConfig.getParameter<double>("regressionMaxEB"),
                             iConfig.getParameter<std::string>("uncertaintyKeyEB"),
                             iConfig.getParameter<double>("uncertaintyMinEB"),
                             iConfig.getParameter<double>("uncertaintyMaxEB"));
  regParamBarrel_.setTokens<esTransition>(consumesCollector());
  regParamEndcap_ = RegParam(iConfig.getParameter<std::string>("regressionKeyEE"),
                             iConfig.getParameter<double>("regressionMinEE"),
                             iConfig.getParameter<double>("regressionMaxEE"),
                             iConfig.getParameter<std::string>("uncertaintyKeyEE"),
                             iConfig.getParameter<double>("uncertaintyMinEE"),
                             iConfig.getParameter<double>("uncertaintyMaxEE"));
  regParamEndcap_.setTokens<esTransition>(consumesCollector());

  const auto& featurePSets = iConfig.getParameter<std::vector<edm::ParameterSet>>("features");
  for(const auto& featurePSet : featurePSets){
    features_.emplace_back(std::make_pair(featurePSet.getParameter<std::string>("name"),
					  consumes<reco::RecoEcalCandidateIsolationMap>(featurePSet.getParameter<edm::InputTag>("src"))
					  ));
  }
  src_ = consumes<reco::RecoEcalCandidateCollection>(iConfig.getParameter<edm::InputTag>("src"));
  
}

void EgammaHLTEnergyRegressionCorrector::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("hltEgammaCandidates"));
  desc.add<double>("regressionMinEB", 0.2);
  desc.add<double>("regressionMaxEB", 2.0);
  desc.add<double>("regressionMinEE", 0.2);
  desc.add<double>("regressionMaxEE", 2.0);
  desc.add<double>("uncertaintyMinEB", 0.0002);
  desc.add<double>("uncertaintyMaxEB", 0.5);
  desc.add<double>("uncertaintyMinEE", 0.0002);
  desc.add<double>("uncertaintyMaxEE", 0.5);
  desc.add<std::string>("regressionKeyEB", "pfscecal_EBCorrection_offline_v2");
  desc.add<std::string>("regressionKeyEE", "pfscecal_EECorrection_offline_v2");
  desc.add<std::string>("uncertaintyKeyEB", "pfscecal_EBUncertainty_offline_v2");
  desc.add<std::string>("uncertaintyKeyEE", "pfscecal_EEUncertainty_offline_v2");
 
  edm::ParameterSetDescription featuresValidator;
  featuresValidator.add<std::string>("name");
  featuresValidator.add<edm::InputTag>("src");

  std::vector<edm::ParameterSet> featuresDefault;
  edm::ParameterSet featureDefault;
  featureDefault.addParameter<std::string>("name","sigmaIEtaIEta");
  featureDefault.addParameter<edm::InputTag>("src",edm::InputTag("hltEgammaShowerShape"));
  featuresDefault.emplace_back(featureDefault);

  desc.addVPSet("features",featuresValidator,featuresDefault);
  descriptions.add("hltEgammaEnergyRegressionCorrector",desc);
}

void EgammaHLTEnergyRegressionCorrector::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) 
{
  regParamBarrel_.setForests(iSetup);
  regParamEndcap_.setForests(iSetup);
  auto outCands = std::make_unique<reco::RecoEcalCandidateCollection>();
  
  auto recoEcalCandHandle = iEvent.getHandle(src_);
  if(recoEcalCandHandle.isValid()){
    for(size_t candNr=0;candNr<recoEcalCandHandle->size();candNr++){
      reco::RecoEcalCandidateRef candRef(recoEcalCandHandle,candNr);
      const auto corrs = getCorrections(candRef,iEvent);
      outCands->push_back(createCorrectedEcalCand(*candRef,corrs.first));      
    }
  }
  iEvent.put(std::move(outCands));

}


std::pair<double, double> EgammaHLTEnergyRegressionCorrector::getCorrections(const reco::RecoEcalCandidateRef& candRef,const edm::Event& iEvent) const{
  std::pair<double, double> corrEnergyAndRes = {-1, -1};

  const auto features = getFeatures(candRef,iEvent);
  if (features.empty()) {
    return corrEnergyAndRes;
  }
  DetId seedId = candRef->superCluster()->seed()->seed();
  const auto& regParam = getRegParam(seedId);

  double mean = regParam.mean(features);
  double sigma = regParam.sigma(features);

  double energyCorr = mean * (candRef->superCluster()->rawEnergy() + candRef->superCluster()->preshowerEnergy());
  double resolutionEst = sigma * energyCorr;

  corrEnergyAndRes.first = energyCorr;
  corrEnergyAndRes.second = resolutionEst;

  return corrEnergyAndRes;
}

void EgammaHLTEnergyRegressionCorrector::RegParam::setForests(const edm::EventSetup& setup) {
  meanForest_ = &setup.getData(meanForestToken_);
  sigmaForest_ = &setup.getData(sigmaForestToken_);
}

double EgammaHLTEnergyRegressionCorrector::RegParam::mean(const std::vector<float>& data) const {
  return meanForest_ ? meanOutTrans_(meanForest_->GetResponse(data.data())) : -1;
}

double EgammaHLTEnergyRegressionCorrector::RegParam::sigma(const std::vector<float>& data) const {
  return sigmaForest_ ? sigmaOutTrans_(sigmaForest_->GetResponse(data.data())) : -1;
}


 std::vector<float> EgammaHLTEnergyRegressionCorrector::getFeatures(const reco::RecoEcalCandidateRef& candRef,const edm::Event& iEvent) const {

  std::vector<float> candFeatures(features_.size());
  std::transform(features_.begin(),features_.end(),candFeatures.begin(),
		 [&iEvent,&candRef](const auto& prod){return iEvent.get(prod.second)[candRef];});

  return candFeatures;
}


DEFINE_FWK_MODULE(EgammaHLTEnergyRegressionCorrector);
