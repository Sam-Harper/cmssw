#include "PhysicsTools/SelectorUtils/interface/CutApplicatorWithEventContentBase.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "RecoEgamma/ElectronIdentification/interface/EBEECutValues.h"

#include "RecoEgamma/EgammaIsolationAlgos/interface/ElectronTkIsolationCorr.h"

class GsfEleTrkPtIsoNoJetCoreCut : public CutApplicatorWithEventContentBase {
public:
  GsfEleTrkPtIsoNoJetCoreCut(const edm::ParameterSet& c);
  
  result_type operator()(const reco::GsfElectronPtr&) const override final;
  
  void setConsumes(edm::ConsumesCollector&) override final;
  void getEventContent(const edm::EventBase&) override final;

  double value(const reco::CandidatePtr& cand) const override final;

  CandidateType candidateType() const override final { 
    return ELECTRON; 
  }

private:
 
  EBEECutValues slopeTerm_;
  EBEECutValues slopeStart_;
  EBEECutValues constTerm_;
  
  
  edm::Handle<reco::TrackCollection> trksHandle_;
  edm::Handle<reco::BeamSpot> beamSpotHandle_;
  
};

DEFINE_EDM_PLUGIN(CutApplicatorFactory,
		  GsfEleTrkPtIsoNoJetCoreCut,
		  "GsfEleTrkPtIsoNoJetCoreCut");

GsfEleTrkPtIsoNoJetCoreCut::GsfEleTrkPtIsoNoJetCoreCut(const edm::ParameterSet& params) :
  CutApplicatorWithEventContentBase(params),
  slopeTerm_(params,"slopeTerm"),
  slopeStart_(params,"slopeStart"),
  constTerm_(params,"constTerm")
{
  contentTags_.emplace("tracks",params.getParameter<edm::InputTag>("tracks"));
  contentTags_.emplace("beamSpot",params.getParameter<edm::InputTag>("beamSpot"));

}

void GsfEleTrkPtIsoNoJetCoreCut::setConsumes(edm::ConsumesCollector& cc) {
  contentTokens_.emplace("tracks",cc.consumes<reco::TrackCollection>(contentTags_["tracks"]));
  contentTokens_.emplace("beamSpot",cc.consumes<reco::BeamSpot>(contentTags_["beamSpot"]));
}

void GsfEleTrkPtIsoNoJetCoreCut::getEventContent(const edm::EventBase& ev) {  
  ev.getByLabel(contentTags_["tracks"],trksHandle_);
  ev.getByLabel(contentTags_["beamSpot"],beamSpotHandle_);
}


CutApplicatorBase::result_type 
GsfEleTrkPtIsoNoJetCoreCut::
operator()(const reco::GsfElectronPtr& cand) const{  
  
  const float isolTrkPt = value(cand);
  
  const float et = cand->et();
  const float cutValue = et > slopeStart_(cand)  ? slopeTerm_(cand)*(et-slopeStart_(cand)) + constTerm_(cand) : constTerm_(cand);
  return isolTrkPt < cutValue;
}

double GsfEleTrkPtIsoNoJetCoreCut::
value(const reco::CandidatePtr& cand) const {
  
  reco::GsfElectronPtr ele(cand);  
  if(trksHandle_.isValid() && beamSpotHandle_.isValid()){
    ElectronTkIsolationCorr isolCorr(0.3,&*trksHandle_,&*beamSpotHandle_);
    return isolCorr.getCorrectedTrkIso(&*ele);
  }else return ele->dr03TkSumPt();
}
