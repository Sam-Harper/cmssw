#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"

#include "RecoEgamma/EgammaElectronAlgos/interface/PixelNHitMatcher.h"

class ElectronNSeedProducer : public edm::stream::EDProducer<> {
public:
  
  
  explicit ElectronNSeedProducer( const edm::ParameterSet & ) ;
  virtual ~ElectronNSeedProducer()=default;  
  
  virtual void produce( edm::Event &, const edm::EventSetup & ) override final;
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:

  PixelNHitMatcher matcher_;
  
  std::vector<edm::EDGetTokenT<reco::SuperClusterCollection> > superClustersTokens_;
  edm::EDGetTokenT<TrajectorySeedCollection> initialSeedsToken_ ;
  edm::EDGetTokenT<std::vector<reco::Vertex> > verticesToken_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_ ;
  
};

namespace {
  template<typename T> 
  edm::Handle<T> getHandle(const edm::Event& event,const edm::EDGetTokenT<T>& token)
  {
    edm::Handle<T> handle;
    event.getByToken(token,handle);
    return handle;
  }

  template<typename T>
  GlobalPoint convertToGP(const T& orgPoint){
    return GlobalPoint(orgPoint.x(),orgPoint.y(),orgPoint.z());
  }
}

ElectronNSeedProducer::ElectronNSeedProducer( const edm::ParameterSet& pset):
  matcher_(pset),
  initialSeedsToken_(consumes<TrajectorySeedCollection>(pset.getParameter<edm::InputTag>("initialSeeds"))),
  verticesToken_(consumes<std::vector<reco::Vertex> >(pset.getParameter<edm::InputTag>("vertices"))),
  beamSpotToken_(consumes<reco::BeamSpot>(pset.getParameter<edm::InputTag>("beamSpot")))
{
  const auto superClusTags = pset.getParameter<std::vector<edm::InputTag> >("superClusters");
  for(const auto& scTag : superClusTags){
    superClustersTokens_.emplace_back(consumes<reco::SuperClusterCollection>(scTag));
  }
  produces<reco::ElectronSeedCollection>() ;
}

void ElectronNSeedProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("initialSeeds",edm::InputTag());
  desc.add<edm::InputTag>("vertices",edm::InputTag());
  desc.add<edm::InputTag>("beamSpot",edm::InputTag());
  desc.add<std::vector<edm::InputTag> >("superClusters");
  
  PixelNHitMatcher::fillDescriptions(descriptions);
}

void ElectronNSeedProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  matcher_.doEventSetup(iSetup);

  auto initialSeedsHandle = getHandle(iEvent,initialSeedsToken_);

  auto beamSpotHandle = getHandle(iEvent,beamSpotToken_);
  GlobalPoint primVtxPos = convertToGP(beamSpotHandle->position());

  for(const auto& superClustersToken : superClustersTokens_){
    auto superClustersHandle = getHandle(iEvent,superClustersToken);
    for(const auto& superClus : *superClustersHandle){
      std::vector<SeedWithInfo> matchedSeeds = 
	matcher_.compatibleSeeds(*initialSeedsHandle,convertToGP(superClus.position()),
				 primVtxPos,superClus.energy());
      

    }

  }
}
  
DEFINE_FWK_MODULE(ElectronNSeedProducer);
