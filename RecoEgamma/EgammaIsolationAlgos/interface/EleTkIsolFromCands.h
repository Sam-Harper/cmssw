#ifndef RECOEGAMMA_EGAMMAISOLATIONALGOS_ELETKISOLFROMCANDS_H
#define RECOEGAMMA_EGAMMAISOLATIONALGOS_ELETKISOLFROMCANDS_H

#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

//author S. Harper (RAL)
//this class does a simple calculation of the track isolation for a track with eta,
//phi and z vtx (typically the GsfTrack of the electron). It uses
//PFPackedCandidates as proxies for the tracks. It now has been upgraded to 
//take general tracks as input also
//
//Note: due to rounding and space saving issues, the isolation calculated on tracks and 
//PFPackedCandidates will differ slightly even if the same cuts are used. These differences
//are thought to be inconsquencial but as such its important to remake the PFPackedCandidates
//in RECO/AOD if you want to be consistant with miniAOD
//
//The track version is more for variables that are stored in the electron rather than 
//objects which are recomputed on the fly for AOD/miniAOD

//new for 9X: 
//  1) its now possible to use generalTracks as input
//  2) adapted to 9X PF packed candidate improvements
//  2a) the PF packed candidates now store the pt of the track in addition to the pt of the 
//      candidate. This means there is no need to pass in the electron collection any more
//      to try and undo the electron e/p combination when the candidate is an electron
//  2b) we now not only store the GsfTrack of the electron but also the general track of the electron
//      there are three input collections now:
//         packedPFCandidates : all PF candidates (with the track for ele candidates being the gsftrack)
//         lostTracks : all tracks which were not made into a PFCandidate but passed some preselection
//         lostTracks::eleTracks : KF electron tracks
//      as such to avoid double counting the GSF and KF versions of an electron track, we now need to 
//      specify if the electron PF candidates are to be rejected in the sum over that collection or not.
//      Note in all this, I'm not concerned about the electron in questions track, that will be rejected,
//      I'm concerned about near by fake electrons which have been recoed by PF


class EleTkIsolFromCands {
public:
  enum class Mode{
    ACCEPTALL=0,
    REJECTELES,
    ONLYELES,
  };

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

  TrkCuts barrelCuts_,endcapCuts_;

public:
  explicit EleTkIsolFromCands(const edm::ParameterSet& para);
  EleTkIsolFromCands(const EleTkIsolFromCands&)=default;
  ~EleTkIsolFromCands()=default;
  EleTkIsolFromCands& operator=(const EleTkIsolFromCands&)=default;

  static edm::ParameterSetDescription pSetDescript();

  std::pair<int,double> calIsol(const reco::TrackBase& trk,const pat::PackedCandidateCollection& cands,const Mode)const;
  std::pair<int,double> calIsol(const double eleEta,const double elePhi,const double eleVZ,
				const pat::PackedCandidateCollection& cands,const Mode)const;

  
  std::pair<int,double> calIsol(const reco::TrackBase& trk,const reco::TrackCollection& tracks)const;
  std::pair<int,double> calIsol(const double eleEta,const double elePhi,const double eleVZ,
				const reco::TrackCollection& tracks)const;
  
  //little helper function for the four calIsol functions for it to directly return the pt
  template<typename ...Args> 
  double calIsolPt(Args && ...args){return calIsol(std::forward<Args>(args)...).second;}


  // double calIsolPt(const reco::TrackBase& trk,const pat::PackedCandidateCollection& cands)const{
  //   return calIsol(trk,cand).second;
  // }
  // double calIsolPt(const double eleEta,const double elePhi,const double eleVZ,
  // 		   const pat::PackedCandidateCollection& cands)const{
  //   return calIsol(eleEta,elePhi,eleVZ,cands).second;
  // }
  // double calIsolPt(const reco::TrackBase& trk,const reco::TrackCollection& tracks)const{
  //   return calIsol(trk,tracks).second;
  // }
  // double calIsolPt(const double eleEta,const double elePhi,const double eleVZ,
  // 		   const reco::TrackCollection& tracks)const{
  //   return calIsol(eleEta,elePhi,eleVZ,tracks).second;
  // }
  
  


  static bool passTrkSel(const reco::TrackBase& trk,
			 const double trkPt,
			 const TrkCuts& cuts,
			 const double eleEta,const double elePhi,
			 const double eleVZ);
  
private:
  //no qualities specified, accept all, ORed
  static bool passQual(const reco::TrackBase& trk,
		       const std::vector<reco::TrackBase::TrackQuality>& quals);
  static bool passAlgo(const reco::TrackBase& trk,
		       const std::vector<reco::TrackBase::TrackAlgorithm>& algosToRej);
  static bool passMode(const pat::PackedCandidate& cand,const EleTkIsolFromCands::Mode mode);
};


#endif
