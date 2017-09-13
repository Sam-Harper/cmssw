#ifndef RECOEGAMMA_EGAMMAISOLATIONALGOS_ELETKISOLTKSELECTOR_H
#define RECOEGAMMA_EGAMMAISOLATIONALGOS_ELETKISOLTKSELECTOR_H

#include "DataFormats/TrackReco/interface/TrackBase.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

class EleTkIsolTkSelector {
private:
  struct TrkCuts {
    float minPt;
    float minDR2;
    float maxDR2;
    float minDEta;
    float maxDZ;
    float minHits;
    float minPixelHits;
    float maxDPtPt;
    std::vector<reco::TrackBase::TrackQuality> allowedQualities;
    std::vector<reco::TrackBase::TrackAlgorithm> algosToReject;
    explicit TrkCuts(const edm::ParameterSet& para);
    static edm::ParameterSetDescription pSetDescript();
  };
  
public:
  explicit EleTkIsolTkSelector(const edm::ParameterSet& para);
  EleTkIsolTkSelector(const EleTkIsolTkSelector&)=default;
  ~EleTkIsolTkSelector()=default;
  EleTkIsolTkSelector& operator=(const EleTkIsolTkSelector&)=default;
  
  static edm::ParameterSetDescription pSetDescript();
  
  bool operator()(const reco::TrackBase& trk,
		  const double trkPt,
		  const double eleEta,const double elePhi,
		  const double eleVZ)const;
private:

  static bool passTrkSel(const reco::TrackBase& trk,
			 const double trkPt,
			 const TrkCuts& cuts,
			 const double eleEta,const double elePhi,
			 const double eleVZ);
  //no qualities specified, accept all, ORed
  static bool passQual(const reco::TrackBase& trk,
		       const std::vector<reco::TrackBase::TrackQuality>& quals);
  static bool passAlgo(const reco::TrackBase& trk,
		       const std::vector<reco::TrackBase::TrackAlgorithm>& algosToRej);

private:
  
  
  TrkCuts barrelCuts_,endcapCuts_;
};

#endif
