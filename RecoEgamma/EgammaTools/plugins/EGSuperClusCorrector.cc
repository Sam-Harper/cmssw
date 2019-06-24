#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "RecoEgamma/EgammaTools/interface/SCEnergyCorrectorSemiParm.h"

class EGSuperClusCorrector : public edm::stream::EDProducer<> {
 public:  
  struct CollData {
    edm::EDGetTokenT<reco::SuperClusterCollection> token;
    std::string outputLabel;
    CollData(const edm::ParameterSet& config,edm::ConsumesCollector&& cc):
      token(cc.consumes<reco::SuperClusterCollection>(config.getParameter<edm::InputTag>("coll"))),
      outputLabel(config.getParameter<std::string>("outputLabel"))
    {}
    static edm::ParameterSetDescription makePSetDescription(){
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("coll",edm::InputTag());
      desc.add<std::string>("outputLabel","");
      return desc;
    }
  };
  
  explicit EGSuperClusCorrector(const edm::ParameterSet&);
  ~EGSuperClusCorrector() override{}

  void produce(edm::Event&, const edm::EventSetup&) override;
  //static void fillDescriptions(edm::ConfigurationDescriptions& descriptions); 
  
private:  
  SCEnergyCorrectorSemiParm corrector_;
  std::vector<CollData> scColls_;
};

EGSuperClusCorrector::EGSuperClusCorrector(const edm::ParameterSet& config)
{
  const auto& psets = config.getParameter<std::vector<edm::ParameterSet> >("colls");
  for(const auto& pset : psets){
    scColls_.push_back(CollData(pset,consumesCollector()));
  }
  
  for(const auto& scColl : scColls_){
    produces<reco::SuperClusterCollection>(scColl.outputLabel);
  }
  auto cc = consumesCollector();
  corrector_.setTokens(config.getParameter<edm::ParameterSet>("regressionCfg"),cc);
}

// void EGSuperClusCorrector::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
// {
//   edm::ParameterSetDescription desc;
//   desc.addVPSet("colls",CollData::makePSetDescription());
//   descriptions.add("egSuperClusCorrector",desc);
// }

void EGSuperClusCorrector::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  corrector_.setEvent(iEvent);
  corrector_.setEventSetup(iSetup);
  for(auto& coll : scColls_){
    auto outSCs = std::make_unique<reco::SuperClusterCollection>();
    const auto& inSCs = *iEvent.getHandle(coll.token);
    for(const auto& inSC : inSCs){
      outSCs->push_back(inSC);
      corrector_.modifyObject(outSCs->back());
    }
    iEvent.put(std::move(outSCs),coll.outputLabel);
  }
  
}

DEFINE_FWK_MODULE(EGSuperClusCorrector);
