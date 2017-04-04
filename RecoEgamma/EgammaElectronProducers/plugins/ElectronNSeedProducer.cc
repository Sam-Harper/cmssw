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
  edm::ParameterSetDescription cutsDesc;
  cutsDesc.add<double>("dPhiMax",0.04);
  cutsDesc.add<double>("dZMax",0.04);
  cutsDesc.add<double>("dRIMax",0.04);
  cutsDesc.add<double>("dRFMax",0.04);
  desc.add<bool>("useRecoVertex",false);
  desc.addVPSet("matchingCuts",cutsDesc);


  descriptions.add("electronNSeedProducer",desc);
  PixelNHitMatcher::fillDescriptions(descriptions);
}

void ElectronNSeedProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  matcher_.doEventSetup(iSetup);

  auto eleSeeds = std::make_unique<reco::ElectronSeedCollection>();
  
  auto initialSeedsHandle = getHandle(iEvent,initialSeedsToken_);

  auto beamSpotHandle = getHandle(iEvent,beamSpotToken_);
  GlobalPoint primVtxPos = convertToGP(beamSpotHandle->position());

  for(const auto& superClustersToken : superClustersTokens_){
    auto superClustersHandle = getHandle(iEvent,superClustersToken);
    //    for(const auto& superClus : *superClustersHandle){
    for(size_t scNr=0;scNr<superClustersHandle->size();scNr++){
      reco::SuperClusterRef superClusRef(superClustersHandle,scNr);
      const std::vector<PixelNHitMatcher::SeedWithInfo> matchedSeeds = 
	matcher_.compatibleSeeds(*initialSeedsHandle,convertToGP(superClusRef->position()),
				 primVtxPos,superClusRef->energy());
      
      for(auto& matchedSeed : matchedSeeds){
	reco::ElectronSeed eleSeed(matchedSeed.seed()); 
	reco::ElectronSeed::CaloClusterRef caloClusRef(superClusRef);
	eleSeed.setCaloCluster(caloClusRef,0x3,matchedSeed.detId(0),matchedSeed.detId(1),0.,0.);
	eleSeed.setPosAttributes(matchedSeed.dRZPos(1),matchedSeed.dPhiPos(1),
				 matchedSeed.dRZPos(0),matchedSeed.dPhiPos(0));
	eleSeed.setNegAttributes(matchedSeed.dRZNeg(1),matchedSeed.dPhiNeg(1),
				 matchedSeed.dRZNeg(0),matchedSeed.dPhiNeg(0));
	eleSeeds->emplace_back(eleSeed);
      }
    }
    
  }
  iEvent.put(std::move(eleSeeds));
}
  
DEFINE_FWK_MODULE(ElectronNSeedProducer);
