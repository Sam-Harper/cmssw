#ifndef RECOEGAMMA_EGAMMAISOLATIONALGOS_ELETKISOLFROMTKS_H
#define RECOEGAMMA_EGAMMAISOLATIONALGOS_ELETKISOLFROMTKS_H

#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h" 
#include "DataFormats/TrackReco/interface/Track.h" 

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EleTkIsolTkSelector.h"
 
class EleTkIsolFromTks {

public:
  explicit EleTkIsolFromTks(const edm::ParameterSet& para);
  EleTkIsolFromTks(const EleTkIsolFromTks&)=default;
  ~EleTkIsolFromTks()=default;
  EleTkIsolFromTks& operator=(const EleTkIsolFromTks&)=default;

  static edm::ParameterSetDescription pSetDescript();

  std::pair<int,double> calIsol(const reco::TrackBase& trk,const reco::TrackCollection& trks)const;
  
  std::pair<int,double> calIsol(const double eleEta,const double elePhi,const double eleVZ,
				const reco::TrackCollection& trks)const;
  
  double calIsolPt(const reco::TrackBase& trk,const reco::TrackCollection& trks)const{
    return calIsol(trk,trks).second;
  }
  
  double calIsolPt(const double eleEta,const double elePhi,const double eleVZ,
		   const reco::TrackCollection& trks)const{
    return calIsol(eleEta,elePhi,eleVZ,trks).second;
  }

private:
  EleTkIsolTkSelector tkSelector_;
};


#endif
