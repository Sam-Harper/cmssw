#ifndef RECOEGAMMA_EGAMMAISOLATIONALGOS_ELETKISOLFROMCANDS_H
#define RECOEGAMMA_EGAMMAISOLATIONALGOS_ELETKISOLFROMCANDS_H

#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EleTkIsolTkSelector.h"
 
class EleTkIsolFromCands {

public:
  explicit EleTkIsolFromCands(const edm::ParameterSet& para);
  EleTkIsolFromCands(const EleTkIsolFromCands&)=default;
  ~EleTkIsolFromCands()=default;
  EleTkIsolFromCands& operator=(const EleTkIsolFromCands&)=default;

  static edm::ParameterSetDescription pSetDescript();

  std::pair<int,double> calIsol(const reco::TrackBase& trk,const pat::PackedCandidateCollection& cands,const edm::View<reco::GsfElectron>& eles)const;
  
  std::pair<int,double> calIsol(const double eleEta,const double elePhi,const double eleVZ,
				const pat::PackedCandidateCollection& cands,
				const edm::View<reco::GsfElectron>& eles)const;
 
  double calIsolPt(const reco::TrackBase& trk,const pat::PackedCandidateCollection& cands,
		   const edm::View<reco::GsfElectron>& eles)const{
    return calIsol(trk,cands,eles).second;
  }
  
  double calIsolPt(const double eleEta,const double elePhi,const double eleVZ,
		   const pat::PackedCandidateCollection& cands,
		   const edm::View<reco::GsfElectron>& eles)const{
    return calIsol(eleEta,elePhi,eleVZ,cands,eles).second;
  }

  
private:
  //for PF electron candidates the "trk pt" is not the track pt
  //its the trk-calo gsfele combination energy * trk sin(theta)
  //so the idea is to match with the gsf electron and get the orginal
  //gsftrack's pt
  double getTrkPt(const reco::TrackBase& trk,
		  const edm::View<reco::GsfElectron>& eles)const;

private:
  EleTkIsolTkSelector tkSelector_;
};


#endif
