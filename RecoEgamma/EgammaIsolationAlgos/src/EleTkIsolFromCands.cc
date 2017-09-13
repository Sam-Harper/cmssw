#include "RecoEgamma/EgammaIsolationAlgos/interface/EleTkIsolFromCands.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"


EleTkIsolFromCands::EleTkIsolFromCands(const edm::ParameterSet& para):
  tkSelector_(para)
{
  
}

edm::ParameterSetDescription EleTkIsolFromCands::pSetDescript()
{
  return EleTkIsolTkSelector::pSetDescript();
}

std::pair<int,double> 
EleTkIsolFromCands::calIsol(const reco::TrackBase& eleTrk,
			    const pat::PackedCandidateCollection& cands,
			    const edm::View<reco::GsfElectron>& eles)const
{
  return calIsol(eleTrk.eta(),eleTrk.phi(),eleTrk.vz(),cands,eles);
}

std::pair<int,double> 
EleTkIsolFromCands::calIsol(const double eleEta,const double elePhi,
			    const double eleVZ,
			    const pat::PackedCandidateCollection& cands,
			    const edm::View<reco::GsfElectron>& eles)const
{
  double ptSum=0.;
  int nrTrks=0;
  
  for(auto& cand  : cands){
    if(cand.charge()!=0){
      const reco::Track& trk = cand.pseudoTrack(); 
      double trkPt = std::abs(cand.pdgId())!=11 ? trk.pt() : getTrkPt(trk,eles);
      if(tkSelector_(trk,trkPt,eleEta,elePhi,eleVZ)){	
	ptSum+=trkPt;
	nrTrks++;
      }
    }
  }
  return {nrTrks,ptSum};	
}

//so the working theory here is that the track we have is the electrons gsf track
//if so, lets get the pt of the gsf track before E/p combinations
//if no match found to a gsf ele with a gsftrack, return the pt of the input track
double EleTkIsolFromCands::
getTrkPt(const reco::TrackBase& trk,
	 const edm::View<reco::GsfElectron>& eles)const
{
  //note, the trk.eta(),trk.phi() should be identical to the gsf track eta,phi
  //although this may not be the case due to roundings after packing
  auto match=[](const reco::TrackBase& trk,const reco::GsfElectron& ele){
    return std::abs(trk.eta()-ele.gsfTrack()->eta())<0.001 &&
    reco::deltaPhi(trk.phi(),ele.gsfTrack()->phi())<0.001;// && 
  };
  for(auto& ele : eles){
    if(ele.gsfTrack().isNonnull()){
      if(match(trk,ele)){
	return ele.gsfTrack()->pt();
      }
    }
  }
  return trk.pt();

}
