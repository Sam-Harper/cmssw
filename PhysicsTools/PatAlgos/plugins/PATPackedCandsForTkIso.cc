#include <string>


#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/Association.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/Association.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

//The aim of this class is emulate the PATPackedCandidate producer to produce only
//packed candidates from charged PF candidates
//This means AOD can have

namespace pat {
  class PATPackedCandsForTkIso : public edm::global::EDProducer<> {
  public:
    explicit PATPackedCandsForTkIso(const edm::ParameterSet& iConfig);
    ~PATPackedCandsForTkIso()=default;
    
    virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
    static pat::PackedCandidate::LostInnerHits getLostInnerHitsStatus(const reco::TrackBase& trk);
    static pat::PackedCandidate::PVAssociationQuality convertAODToMiniAODVertAssoQualFlag(const size_t qual);

  private:
    void addPackedCandidate(const edm::Event& iEvent,reco::TrackRef trkRef,reco::PFCandidateRef pfCand,
			    const reco::Track& usedTrk,
			    std::vector<pat::PackedCandidate>& packedCands)const;
      
    bool passTrkCuts(const reco::TrackBase& trk)const;
    static const reco::Track& getUsedTrk(reco::TrackRef& trk,const std::vector<reco::PFCandidateRef>& pfEles);
    static reco::VertexRef getVertex(const reco::TrackBase& trk,const reco::PFCandidateRef pfCand,
			      const edm::Handle<edm::Association<reco::VertexCollection> > vertAsso,
			      const edm::Handle<reco::VertexCollection> vertices);
    static reco::PFCandidateRef getPFCand(reco::TrackRef trk,const edm::Handle<reco::PFCandidateCollection> pfCands);
    static reco::Candidate::PolarLorentzVector getP4(const reco::TrackBase& trk,const reco::PFCandidateRef cand);

    template<typename T>
    static edm::Handle<T> getHandle(const edm::Event& event,const edm::EDGetTokenT<T>& token){
      edm::Handle<T> handle;
      event.getByToken(token,handle);
      return handle;
    }
  private:

    const edm::EDGetTokenT<reco::PFCandidateCollection> pfCandsToken_;
    const edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
    const edm::EDGetTokenT<reco::VertexCollection> verticesToken_;  
    const edm::EDGetTokenT<edm::Association<reco::VertexCollection> > vertAssoToken_;
    const edm::EDGetTokenT<edm::ValueMap<int> > vertAssoQualToken_;
         
    const double minPt_;
    const int minHits_;
    const int minPixelHits_;
  }; 
}

pat::PATPackedCandsForTkIso::PATPackedCandsForTkIso(const edm::ParameterSet& iConfig) :
  pfCandsToken_(consumes<reco::PFCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCands"))),
  tracksToken_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("tracks"))),
  verticesToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
  vertAssoToken_(consumes<edm::Association<reco::VertexCollection> >(iConfig.getParameter<edm::InputTag>("vertAsso"))),
  vertAssoQualToken_(consumes<edm::ValueMap<int> >(iConfig.getParameter<edm::InputTag>("vertAssoQual"))),
  minPt_(iConfig.getParameter<double>("minPt")),
  minHits_(iConfig.getParameter<int>("minHits")),
  minPixelHits_(iConfig.getParameter<int>("minPixelHits"))
{
  produces<std::vector<pat::PackedCandidate> > ();
}


void pat::PATPackedCandsForTkIso::produce(edm::StreamID,edm::Event& iEvent,const edm::EventSetup& iSetup)const
{
  auto pfCandsHandle = getHandle(iEvent,pfCandsToken_);
  auto tracksHandle = getHandle(iEvent,tracksToken_);
  // auto verticesHandle = getHandle(iEvent,verticesToken_);
  // auto vertAssoHandle = getHandle(iEvent,vertAssoToken_);
  // auto vertAssoQualHandle = getHandle(iEvent,vertAssoQualToken_);
  
  //  const reco::VertexRefProd vertsProdRef(verticesHandle);
    
  std::unique_ptr<std::vector<pat::PackedCandidate> > packedCands(new std::vector<pat::PackedCandidate>);
  
  //because for electrons we use the GsfTrack, we need to know 
  //which tracks match to PF candidates that are electrons
  //we cant do this of GedGsfElectrons as the ctf track matching
  //requirement is different
  std::vector<reco::PFCandidateRef> pfEles;
  for(size_t candNr=0;candNr<pfCandsHandle->size();candNr++){
    reco::PFCandidateRef pfCand(pfCandsHandle,candNr);
    // if(std::abs(pfCand->pt()-1.26256)<0.0001) {
    //   std::cout <<"pfCandFound "<<pfCand->pt()<<" "<<pfCand->eta()<<" "<<pfCand->phi()<<" "<<pfCand->trackRef().isNonnull()<<" "<<pfCand->gsfTrackRef().isNonnull()<<" "<<pfCand->pdgId();
    //   if(pfCand->trackRef().isNonnull()) std::cout <<" "<<pfCand->trackRef()->pt()<<" "<<pfCand->trackRef()->eta()<<" "<<pfCand->trackRef()->phi();
    //   std::cout <<std::endl;
    // }
    // if(std::abs(pfCand->pdgId())==11) std::cout <<"pfCandEle "<<pfCand->pt()<<" "<<pfCand->eta()<<" "<<pfCand->phi()<<" "<<pfCand->trackRef().isNonnull()<<" "<<pfCand->gsfTrackRef().isNonnull()<<std::endl;
    if(std::abs(pfCand->pdgId())==11) pfEles.push_back(pfCand);
    
  }

  //we have to manually add the electrons which dont have tracks but do have gsf tracks
  //it is possible now we double count but there is no away around it other than 
  //a) rejecting all pf electrons (not going to happen)
  //b) doing a geometric match between gsf tracks and ctf tracks  ( a pain)
  for(auto pfEle : pfEles){
    if(pfEle->trackRef().isNull() && pfEle->gsfTrackRef().isNonnull()){
      if(passTrkCuts(*pfEle->gsfTrackRef())){
	addPackedCandidate(iEvent,pfEle->trackRef(),pfEle,*pfEle->gsfTrackRef(),*packedCands);
      }
    }
  }
  //so in filling the candidates, we have to watch for electrons and gsf vs ctf tracks
  //gsf track : track properties stored in packed candidate
  //ctf track : vertex association & high purityflag stored in packed candidate
  //ctf track : matches the packed candidate
  //gsf track : default vertex dz match (only relavent if the association had failed)
  for(size_t trkNr=0;trkNr<tracksHandle->size();trkNr++){
    edm::Ref<reco::TrackCollection> trkRef(tracksHandle,trkNr);
    const reco::Track& usedTrk = getUsedTrk(trkRef,pfEles); //will be the GsfTrack if it exists 

    // if(std::abs(trkRef->pt()-1.2)<0.1) std::cout <<"trk found "<<trkRef->pt()<<" "<<trkRef->eta()<<" "<<trkRef->phi()<<" used track "<<usedTrk.pt()<<" "<<usedTrk.eta()<<" "<<usedTrk.phi()<<" "<< trkRef->hitPattern().numberOfValidHits() << " "<<trkRef->hitPattern().numberOfValidPixelHits() <<" trk pt err "<<trkRef->ptError()<<std::endl;

    if(passTrkCuts(usedTrk)){
      const reco::PFCandidateRef pfCand= getPFCand(trkRef,pfCandsHandle);

      //  if(pfCand.isNonnull() &&  std::abs(pfCand->pdgId())==11) std::cout <<"pfCandEle "<<pfCand->pt()<<" "<<pfCand->eta()<<" "<<pfCand->phi()<<" trk "<<trkRef->pt()<<" "<<trkRef->eta()<<" "<<trkRef->phi()<<" used trk "<<usedTrk.pt()<<" "<<usedTrk.eta()<<" "<<usedTrk.phi()<<std::endl;

      //  std::cout<<" trk "<<trkRef->pt()<<" "<<trkRef->eta()<<" "<<trkRef->phi()<<" used trk "<<usedTrk.pt()<<" "<<usedTrk.eta()<<" "<<usedTrk.phi();
      //if(pfCand.isNonnull()) std::cout <<" pfCand "<<pfCand->pdgId()<<" "<<pfCand->pt()<<" "<<pfCand->eta()<<" "<<pfCand->phi()<<" "<<pfCand->charge()<<" gsftrk "<<pfCand->gsfTrackRef().isNonnull();
      //  std::cout<<std::endl;
      addPackedCandidate(iEvent,trkRef,pfCand,usedTrk,*packedCands);
    }
  }
  

  iEvent.put(std::move(packedCands));
}

void pat::PATPackedCandsForTkIso::
addPackedCandidate(const edm::Event& iEvent,reco::TrackRef trkRef,reco::PFCandidateRef pfCand,
		   const reco::Track& usedTrk,
		   std::vector<pat::PackedCandidate>& packedCands)const
{
  //first we need to detect if the pfCand is a photon and if so, set it to a null ref
  //this is because tracks associated with photons are "lost" and therefore picked up via lost tracks
  //where there is no candidate
  if(pfCand.isNonnull() && pfCand->pdgId()==22) pfCand=reco::PFCandidateRef(nullptr,0);

  auto p4 = getP4(usedTrk,pfCand);
  if(p4.pt() > minPt_){
    
    auto verticesHandle = getHandle(iEvent,verticesToken_);
    auto vertAssoHandle = getHandle(iEvent,vertAssoToken_);
    auto vertAssoQualHandle = getHandle(iEvent,vertAssoQualToken_);
    
    const reco::VertexRefProd vertsProdRef(verticesHandle);
    
    const reco::VertexRef vertRef = getVertex(usedTrk,pfCand,vertAssoHandle,verticesHandle);
    int vertAssoQual=pfCand.isNonnull() ? (*vertAssoQualHandle)[pfCand] : 0;
    
    int pdgId = pfCand.isNonnull() ? pfCand->pdgId() : 211* usedTrk.charge();
 
 
    
    packedCands.push_back(pat::PackedCandidate(p4,usedTrk.vertex(),usedTrk.phi(),
					       pdgId,vertsProdRef,vertRef.key()));
    pat::PackedCandidate& packedCand = packedCands.back();
    packedCand.setTrackProperties(usedTrk);
    packedCand.setLostInnerHits(getLostInnerHitsStatus(usedTrk));
    packedCand.setTrackHighPurity(trkRef.isNonnull() && trkRef->quality(reco::TrackBase::highPurity));
    packedCand.setAssociationQuality(convertAODToMiniAODVertAssoQualFlag(vertAssoQual));
    if(vertRef->trackWeight(trkRef) > 0.5 && (vertAssoQual == 7 || pfCand.isNull()) ) {
      packedCand.setAssociationQuality(pat::PackedCandidate::UsedInFitTight);
    }
    if(pfCand.isNonnull() && pfCand->muonRef().isNonnull()){
      packedCand.setMuonID(pfCand->muonRef()->isStandAloneMuon(),pfCand->muonRef()->isGlobalMuon());
    }
  }
}  

pat::PackedCandidate::LostInnerHits
pat::PATPackedCandsForTkIso::getLostInnerHitsStatus(const reco::TrackBase& trk)
{
  int nrLostHits =  trk.hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS);
  
  if(nrLostHits==0){
    if(trk.hitPattern().hasValidHitInFirstPixelBarrel()) return pat::PackedCandidate::validHitInFirstPixelBarrelLayer;
    else return pat::PackedCandidate::noLostInnerHits;
  }else if(nrLostHits==1){
    return pat::PackedCandidate::oneLostInnerHit;
  }else{
    return pat::PackedCandidate::moreLostInnerHits;
  }
}
 

pat::PackedCandidate::PVAssociationQuality 
pat::PATPackedCandsForTkIso::convertAODToMiniAODVertAssoQualFlag(const size_t qual)
{  
  //  static constexpr std::array<int,8> aodToMiniAODMap  = {1,0,1,1,4,4,5,6};
  static constexpr std::array<pat::PackedCandidate::PVAssociationQuality,8> aodToMiniAODMap{ {
        pat::PackedCandidate::OtherDeltaZ,
	pat::PackedCandidate::NotReconstructedPrimary,
	pat::PackedCandidate::OtherDeltaZ,
	pat::PackedCandidate::OtherDeltaZ,
	pat::PackedCandidate::CompatibilityBTag,
	pat::PackedCandidate::CompatibilityBTag,
	pat::PackedCandidate::CompatibilityDz,
	pat::PackedCandidate::UsedInFitLoose 
  } };
 
  if(qual<aodToMiniAODMap.size()) return aodToMiniAODMap[qual];
  else{
    edm::LogWarning("PATPackedCandsForTkIso") <<" passed quality value "<<qual<<" is larger than "<<aodToMiniAODMap.size()<<". This means that the PrimaryVertexAssignment::Quality enum has been updated but this function which translates it to pat::PackedCandidate::PVAssociationQuality has not. The vertex association quality of the pat::PackedCandidates being produced is therefore incorrect. This needs to be fixed. Location of the error "<<__FUNCTION__<<"() (in "<<__FILE__<<", line "<<__LINE__<<")"<<std::endl;
    return pat::PackedCandidate::NotReconstructedPrimary;
  }
    
}


bool pat::PATPackedCandsForTkIso::passTrkCuts(const reco::TrackBase& trk)const
{
  return trk.numberOfValidHits() >= minHits_ && 
    trk.hitPattern().numberOfValidPixelHits() >= minPixelHits_;
 
}

const reco::Track& pat::PATPackedCandsForTkIso::
getUsedTrk(reco::TrackRef& trk,const std::vector<reco::PFCandidateRef>& pfEles)
{
  for(auto pfEle : pfEles){
    if(trk==pfEle->trackRef()){
      //I'm going to cycle through the other electrons just incase they also 
      //match the std track and have a gsf track
      if(pfEle->gsfTrackRef().isNonnull()) return *pfEle->gsfTrackRef();
    }
  }
  return *trk;
}

//this is a copy of how PATPackedCandidateProducer does it
reco::VertexRef 
pat::PATPackedCandsForTkIso::getVertex(const reco::TrackBase& trk,const reco::PFCandidateRef pfCand,
				       const edm::Handle<edm::Association<reco::VertexCollection> > vertAsso,
				       const edm::Handle<reco::VertexCollection> vertices)
{
  reco::VertexRef vertexRef(vertices.id());
  if(pfCand.isNonnull()) {
    vertexRef = (*vertAsso)[pfCand];
    
    if(vertexRef.isNull()){
      //okay minor different with PATPackedCandidateProducer, they use 1e99, I use max value
      float minDZ=std::numeric_limits<float>::max();
      for(size_t vertNr=0;vertNr<vertices->size();vertNr++){
	const reco::Vertex& vert = (*vertices)[vertNr];
	float dz=std::abs(trk.dz(vert.position()));
	if(dz<minDZ){ 
	  minDZ = dz;
	  vertexRef = reco::VertexRef(vertices,vertNr);
	}
      }
    }
  }else{ //we do it PATLostTracks way (yes this is does make a small difference in the resulting dz sometimes)
    if(!vertices->empty()) vertexRef = reco::VertexRef(vertices, 0);
  }
  return vertexRef;
}

reco::PFCandidateRef
pat::PATPackedCandsForTkIso::getPFCand(reco::TrackRef trk,const edm::Handle<reco::PFCandidateCollection> pfCands)
{
  for(size_t pfCandNr=0;pfCandNr<pfCands->size();pfCandNr++){
    auto pfCand = (*pfCands)[pfCandNr];
    if(pfCand.trackRef().isNonnull() && trk==pfCand.trackRef()) return reco::PFCandidateRef(pfCands,pfCandNr);
  }
  return reco::PFCandidateRef(pfCands.id());
}

reco::Candidate::PolarLorentzVector
pat::PATPackedCandsForTkIso::getP4(const reco::TrackBase& trk,const reco::PFCandidateRef pfCand)
{
  if(pfCand.isNull()) return  reco::Candidate::PolarLorentzVector(trk.pt(),trk.eta(),trk.phi(),0.13957018);
  else return pfCand->polarP4();
}


using pat::PATPackedCandsForTkIso;
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATPackedCandsForTkIso);
