

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"

#include "RecoEgamma/EgammaHLTProducers/interface/EgammaHLTPixelMatchParamObjects.h"

#include "DataFormats/SiPixelDetId/interface/PXBDetId.h" 
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h" 
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "FWCore/Framework/interface/ESHandle.h"
class EgammaHLTPixelMatchVarProducer : public edm::global::EDProducer<> {
public:

  explicit EgammaHLTPixelMatchVarProducer(const edm::ParameterSet&);
  ~EgammaHLTPixelMatchVarProducer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::StreamID sid, edm::Event&, const edm::EventSetup&) const override;
  std::array<float,4> calS2(const reco::ElectronSeed& seed,int charge)const;

private: 
  // ----------member data ---------------------------
  
  const edm::EDGetTokenT<reco::RecoEcalCandidateCollection> recoEcalCandidateToken_;
  const edm::EDGetTokenT<reco::ElectronSeedCollection> pixelSeedsToken_;

  egPM::Param<reco::ElectronSeed> dPhi1Para_;
  egPM::Param<reco::ElectronSeed> dPhi2Para_;
  egPM::Param<reco::ElectronSeed> dRZ2Para_;
  
  int productsToWrite_;
  
};

EgammaHLTPixelMatchVarProducer::EgammaHLTPixelMatchVarProducer(const edm::ParameterSet& config) : 
  recoEcalCandidateToken_(consumes<reco::RecoEcalCandidateCollection>(config.getParameter<edm::InputTag>("recoEcalCandidateProducer"))),
  pixelSeedsToken_(consumes<reco::ElectronSeedCollection>(config.getParameter<edm::InputTag>("pixelSeedsProducer"))),
  dPhi1Para_(config.getParameter<edm::ParameterSet>("dPhi1SParams")),
  dPhi2Para_(config.getParameter<edm::ParameterSet>("dPhi2SParams")),
  dRZ2Para_(config.getParameter<edm::ParameterSet>("dRZ2SParams")),
  productsToWrite_(config.getParameter<int>("productsToWrite"))
  
{
  //register your products  
  produces < reco::RecoEcalCandidateIsolationMap >("s2");
  if(productsToWrite_>=1){
    produces < reco::RecoEcalCandidateIsolationMap >("dPhi1BestS2");
    produces < reco::RecoEcalCandidateIsolationMap >("dPhi2BestS2");
    produces < reco::RecoEcalCandidateIsolationMap >("dzBestS2");
  }
  if(productsToWrite_>=2){
    produces < reco::RecoEcalCandidateIsolationMap >("dPhi1");
    produces < reco::RecoEcalCandidateIsolationMap >("dPhi2");
    produces < reco::RecoEcalCandidateIsolationMap >("dz");
    
    produces < reco::RecoEcalCandidateIsolationMap >("dPhi1SubDet");
    produces < reco::RecoEcalCandidateIsolationMap >("dPhi2SubDet");
    produces < reco::RecoEcalCandidateIsolationMap >("dzSubDet");
    
    produces < reco::RecoEcalCandidateIsolationMap >("nrClus");
    produces < reco::RecoEcalCandidateIsolationMap >("seedClusEFrac");
    
    produces < reco::RecoEcalCandidateIsolationMap >("seedHit1DetId1");
    produces < reco::RecoEcalCandidateIsolationMap >("seedHit1DetId2");
    produces < reco::RecoEcalCandidateIsolationMap >("seedHit2DetId1");
    produces < reco::RecoEcalCandidateIsolationMap >("seedHit2DetId2");
  }

}

EgammaHLTPixelMatchVarProducer::~EgammaHLTPixelMatchVarProducer()
{}

void EgammaHLTPixelMatchVarProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {

  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>(("recoEcalCandidateProducer"), edm::InputTag("hltL1SeededRecoEcalCandidate"));
  desc.add<edm::InputTag>(("pixelSeedsProducer"), edm::InputTag("electronPixelSeeds"));
  
  edm::ParameterSetDescription varParamDesc;
  edm::ParameterSetDescription binParamDesc;
  
  auto binDescCases = 
    "AbsEtaClus" >> 
    (edm::ParameterDescription<double>("xMin",0.0,true) and
     edm::ParameterDescription<double>("xMax",3.0,true) and
     edm::ParameterDescription<int>("yMin",0,true) and
     edm::ParameterDescription<int>("yMax",99999,true) and
     edm::ParameterDescription<std::string>("funcType","pol0",true) and
     edm::ParameterDescription<std::vector<double>>("funcParams",{0.},true)) or
    "AbsEtaClusPhi" >>
    (edm::ParameterDescription<double>("xMin",0.0,true) and
     edm::ParameterDescription<double>("xMax",3.0,true) and
     edm::ParameterDescription<int>("yMin",0,true) and
     edm::ParameterDescription<int>("yMax",99999,true) and
     edm::ParameterDescription<std::string>("funcType","pol0",true) and
     edm::ParameterDescription<std::vector<double>>("funcParams",{0.},true)) or 
     "AbsEtaClusEt" >>
    (edm::ParameterDescription<double>("xMin",0.0,true) and
     edm::ParameterDescription<double>("xMax",3.0,true) and
     edm::ParameterDescription<int>("yMin",0,true) and
     edm::ParameterDescription<int>("yMax",99999,true) and
     edm::ParameterDescription<std::string>("funcType","pol0",true) and
     edm::ParameterDescription<std::vector<double>>("funcParams",{0.},true));
  
  binParamDesc.ifValue(edm::ParameterDescription<std::string>("binType","AbsEtaClus",true), std::move(binDescCases));
  
  
  varParamDesc.addVPSet("bins",binParamDesc);
  desc.add("dPhi1SParams",varParamDesc);
  desc.add("dPhi2SParams",varParamDesc);
  desc.add("dRZ2SParams",varParamDesc);
  desc.add<int>("productsToWrite",0);
  descriptions.add(("hltEgammaHLTPixelMatchVarProducer"), desc);  
}

void EgammaHLTPixelMatchVarProducer::produce(edm::StreamID sid, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  
  // Get the HLT filtered objects
  edm::Handle<reco::RecoEcalCandidateCollection> recoEcalCandHandle;
  iEvent.getByToken(recoEcalCandidateToken_,recoEcalCandHandle);


  edm::Handle<reco::ElectronSeedCollection> pixelSeedsHandle;
  iEvent.getByToken(pixelSeedsToken_,pixelSeedsHandle);

  if(!recoEcalCandHandle.isValid() || !pixelSeedsHandle.isValid()) return;

  edm::ESHandle<TrackerTopology> trackerTopoHandle;
  iSetup.get<TrackerTopologyRcd>().get(trackerTopoHandle);
  
  

  auto dPhi1BestS2Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto dPhi2BestS2Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto dzBestS2Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto s2Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  
  auto dPhi1Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto dPhi2Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto dzMap = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);  
  auto dPhi1SubDetMap = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto dPhi2SubDetMap = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto dzSubDetMap = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto nrClusMap = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto seedClusEFracMap = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle); 
  auto seed1DetId1Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto seed1DetId2Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto seed2DetId1Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);
  auto seed2DetId2Map = std::make_unique<reco::RecoEcalCandidateIsolationMap>(recoEcalCandHandle);

  for(unsigned int candNr = 0; candNr<recoEcalCandHandle->size(); candNr++) {

    reco::RecoEcalCandidateRef candRef(recoEcalCandHandle,candNr);
    reco::SuperClusterRef candSCRef = candRef->superCluster();

    float bestDPhi1=std::numeric_limits<float>::max();
    float bestDPhi2=bestDPhi1;
    float bestDZ=bestDPhi1;
    float bestDPhi1SubDet=-1;
    float bestDPhi2SubDet=bestDPhi1SubDet;
    float bestDZSubDet=bestDPhi1SubDet;
    int bestSeed1DetId=0;
    int bestSeed2DetId=0;
    
    std::array<float,4> bestS2{{std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max()}};
    for(auto & seed : *pixelSeedsHandle){
      edm::RefToBase<reco::CaloCluster> pixelClusterRef = seed.caloCluster() ;
      reco::SuperClusterRef pixelSCRef = pixelClusterRef.castTo<reco::SuperClusterRef>() ;
      if(&(*candSCRef) ==  &(*pixelSCRef)){
	
	std::array<float,4> s2Data = calS2(seed,-1);
	std::array<float,4> s2DataPos = calS2(seed,+1);
	if(s2Data[0]<bestS2[0]) bestS2=s2Data;
	if(s2DataPos[0]<bestS2[0]) bestS2=s2DataPos;
	
	if(productsToWrite_>=2){
	  
	  auto getBestVal=[](float val1,float val2){return std::abs(val1)<std::abs(val2) ? val1 : val2;};
	  float seedDPhi1 = getBestVal(seed.dPhi1(),seed.dPhi1Pos());
	  float seedDPhi2 = getBestVal(seed.dPhi2(),seed.dPhi2Pos());
	  float seedDRZ2 = getBestVal(seed.dRz2(),seed.dRz2Pos());
	  float seedSubDet = seed.subDet1() + seed.subDet2()*10;
	  
	  auto recHitIt = seed.recHits().first;
	  int seed1DetId = recHitIt!=seed.recHits().second ? recHitIt->geographicalId().rawId() : 0;
	  ++recHitIt;
	  int seed2DetId = recHitIt!=seed.recHits().second ? recHitIt->geographicalId().rawId() : 0;
	  
	  auto setBestVal=[](float newVal,float newSubDet,int detId,float& bestVal,float& bestSubDet,int& bestDetId)
	    {if(std::abs(newVal)<std::abs(bestVal)){bestVal=newVal;bestSubDet=newSubDet;bestDetId=detId;}};
	  
	  setBestVal(seedDPhi1,seedSubDet,seed1DetId,bestDPhi1,bestDPhi1SubDet,bestSeed1DetId);
	  setBestVal(seedDPhi2,seedSubDet,seed2DetId,bestDPhi2,bestDPhi2SubDet,bestSeed2DetId);
	  setBestVal(seedDRZ2,seedSubDet,seed2DetId,bestDZ,bestDZSubDet,bestSeed2DetId);
	}
      }
    }

   
    s2Map->insert(candRef,bestS2[0]);
    if(productsToWrite_>=1){
      dPhi1BestS2Map->insert(candRef,bestS2[1]);
      dPhi2BestS2Map->insert(candRef,bestS2[2]);
      dzBestS2Map->insert(candRef,bestS2[3]);
    }
    if(productsToWrite_>=2){
       nrClusMap->insert(candRef,candSCRef->clustersSize());
       float seedClusEFrac = candSCRef->rawEnergy()>0 ? candSCRef->seed()->energy() / candSCRef->rawEnergy() : 0.;
       seedClusEFracMap->insert(candRef,seedClusEFrac);
       
       
       
       dPhi1Map->insert(candRef,bestDPhi1);
       dPhi2Map->insert(candRef,bestDPhi2);
       dzMap->insert(candRef,bestDZ);
       
       dPhi1SubDetMap->insert(candRef,bestDPhi1SubDet);
       dPhi2SubDetMap->insert(candRef,bestDPhi2SubDet);
       dzSubDetMap->insert(candRef,bestDZSubDet);
       
       auto splitDetId=[](int detId,bool upper){ 
	 if(upper) detId=detId>>16;
	 detId&=0xFFFF;
	 return detId;
       };

       // DetId seed1DetId(bestSeed1DetId);
       // if(seed1DetId.subdetId()==PixelSubdetector::PixelBarrel){
       // 	 int layer = trackerTopoHandle->pxbLayer(seed1DetId);
       // 	 PXBDetId seed1PXDetId(seed1DetId);
       // 	 std::cout <<" seed 1 bpix "<<seed1PXDetId<<" layer "<<layer<<std::endl;
       // }else if(seed1DetId.subdetId()==PixelSubdetector::PixelEndcap){
       // 	 int disk = trackerTopoHandle->pxfDisk(seed1DetId);
       // 	 PXFDetId seed1PXDetId(seed1DetId);
       // 	 std::cout <<" seed 1 fpix "<<seed1PXDetId<<" disk "<<disk<<std::endl;
       // }
       seed1DetId1Map->insert(candRef,splitDetId(bestSeed1DetId,false));
       seed1DetId2Map->insert(candRef,splitDetId(bestSeed1DetId,true));
       seed2DetId1Map->insert(candRef,splitDetId(bestSeed2DetId,false));
       seed2DetId2Map->insert(candRef,splitDetId(bestSeed2DetId,true));

       int test = splitDetId(bestSeed1DetId,false) | splitDetId(bestSeed1DetId,true)<<16;
       if(test!=bestSeed1DetId){
	 std::cout <<" miss match "<<test<<" "<<bestSeed1DetId<<std::endl;
       }

    }
    
  }

  iEvent.put(std::move(s2Map),"s2");
  if(productsToWrite_>=1){
    iEvent.put(std::move(dPhi1BestS2Map),"dPhi1BestS2");
    iEvent.put(std::move(dPhi2BestS2Map),"dPhi2BestS2");
    iEvent.put(std::move(dzBestS2Map),"dzBestS2");
  }
  if(productsToWrite_>=2){
    iEvent.put(std::move(dPhi1Map),"dPhi1");
    iEvent.put(std::move(dPhi2Map),"dPhi2");
    iEvent.put(std::move(dzMap),"dz");
    iEvent.put(std::move(dPhi1SubDetMap),"dPhi1SubDet");
    iEvent.put(std::move(dPhi2SubDetMap),"dPhi2SubDet");
    iEvent.put(std::move(dzSubDetMap),"dzSubDet");
    iEvent.put(std::move(nrClusMap),"nrClus");
    iEvent.put(std::move(seedClusEFracMap),"seedClusEFrac");
    iEvent.put(std::move(seed1DetId1Map),"seedHit1DetId1");
    iEvent.put(std::move(seed1DetId2Map),"seedHit1DetId2");
    iEvent.put(std::move(seed2DetId1Map),"seedHit2DetId1");
    iEvent.put(std::move(seed2DetId2Map),"seedHit2DetId2");
    
  }
}

std::array<float,4> EgammaHLTPixelMatchVarProducer::calS2(const reco::ElectronSeed& seed,int charge)const
{
  const float dPhi1Const = dPhi1Para_(seed);	
  const float dPhi2Const = dPhi2Para_(seed);
  const float dRZ2Const = dRZ2Para_(seed);
  
  float dPhi1 = (charge <0 ? seed.dPhi1() : seed.dPhi1Pos())/dPhi1Const;
  float dPhi2 = (charge <0 ? seed.dPhi2() : seed.dPhi2Pos())/dPhi2Const;
  float dRz2 = (charge <0 ? seed.dRz2() : seed.dRz2Pos())/dRZ2Const;
  
  float s2 = dPhi1*dPhi1+dPhi2*dPhi2+dRz2*dRz2;
  return std::array<float,4>{{s2,dPhi1,dPhi2,dRz2}}; 
}


DEFINE_FWK_MODULE(EgammaHLTPixelMatchVarProducer);
