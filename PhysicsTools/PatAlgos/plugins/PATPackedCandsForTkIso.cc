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
    
    static pat::PackedCandidate::PVAssociationQuality convertAODToMiniAODVertAssoQualFlag(const size_t qual);

  private:
    bool passTrkCuts(const reco::TrackBase& trk)const;
    static const reco::Track& getUsedTrk(reco::TrackRef& trk,std::vector<const reco::PFCandidate*> pfEles);
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
  auto verticesHandle = getHandle(iEvent,verticesToken_);
  auto vertAssoHandle = getHandle(iEvent,vertAssoToken_);
  auto vertAssoQualHandle = getHandle(iEvent,vertAssoQualToken_);
  
  const reco::VertexRefProd vertsProdRef(verticesHandle);
    
  std::unique_ptr<std::vector<pat::PackedCandidate> > packedCands(new std::vector<pat::PackedCandidate>);
  
  //because for electrons we use the GsfTrack, we need to know 
  //which tracks match to PF candidates that are electrons
  //we cant do this of GedGsfElectrons as the ctf track matching
  //requirement is different
  std::vector<const reco::PFCandidate*> pfEles;
  for(auto& pfCand : *pfCandsHandle){
    if(std::abs(pfCand.pdgId())==11) pfEles.push_back(&pfCand);
  }

  //so in filling the candidates, we have to watch for electrons and gsf vs ctf tracks
  //gsf track : track properties stored in packed candidate
  //ctf track : vertex association & high purityflag stored in packed candidate
  //ctf track : matches the packed candidate
  //gsf track : default vertex dz match (only relavent if the association had failed)
  

  for(size_t trkNr=0;trkNr<tracksHandle->size();trkNr++){
    edm::Ref<reco::TrackCollection> trkRef(tracksHandle,trkNr);
    const reco::Track& usedTrk = getUsedTrk(trkRef,pfEles); //will be the GsfTrack if it exists 
    if(passTrkCuts(usedTrk)){
      const reco::PFCandidateRef pfCand= getPFCand(trkRef,pfCandsHandle);

      auto p4 = getP4(usedTrk,pfCand);
      const reco::VertexRef vertRef = getVertex(usedTrk,pfCand,vertAssoHandle,verticesHandle);
      int vertAssoQual=(*vertAssoQualHandle)[pfCand];
      
      packedCands->push_back(pat::PackedCandidate(p4,usedTrk.vertex(),usedTrk.phi(),
						  211*usedTrk.charge(),vertsProdRef,vertRef.key()));
      pat::PackedCandidate& packedCand = packedCands->back();
      packedCand.setTrackProperties(usedTrk);
      //      packedCand.setLostInnerHits(lostHits);
      packedCand.setAssociationQuality(convertAODToMiniAODVertAssoQualFlag(vertAssoQual));
      packedCand.setTrackHighPurity(trkRef->quality(reco::TrackBase::highPurity));
      if(vertRef->trackWeight(trkRef) > 0.5 && vertAssoQual == 7) {
	packedCand.setAssociationQuality(pat::PackedCandidate::UsedInFitTight);
      }
      if(pfCand.isNonnull() && pfCand->muonRef().isNonnull()){
	packedCand.setMuonID(pfCand->muonRef()->isStandAloneMuon(),pfCand->muonRef()->isGlobalMuon());
      }  
    }
  }
  iEvent.put(std::move(packedCands));
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
  return trk.pt()>minPt_ &&
    trk.numberOfValidHits() >= minHits_ && 
    trk.hitPattern().numberOfValidPixelHits() >= minPixelHits_;
 
}

const reco::Track& pat::PATPackedCandsForTkIso::
getUsedTrk(reco::TrackRef& trk,std::vector<const reco::PFCandidate*> pfEles)
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
  reco::VertexRef vertexRef  = (*vertAsso)[pfCand];
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
