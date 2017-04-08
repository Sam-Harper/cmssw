#ifndef DataFormats_EgammaReco_ElectronNHitSeed_h
#define DataFormats_EgammaReco_ElectronNHitSeed_h

//********************************************************************
//
// A verson of reco::ElectronSeed which can have N hits 
//
// Noticed that h/e values never seem to used anywhere and they are a 
// mild pain to propagate in the new framework so they were removed
// Likewise, the hit mask (we always use all the hits in the seed)
//
// author: S. Harper (RAL), 2017
//
//*********************************************************************


#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeed.h"
#include "DataFormats/TrajectoryState/interface/TrackCharge.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/Ref.h"

#include <vector>
#include <limits>

namespace reco
{
  
  class ElectronNHitSeed : public TrajectorySeed {
  public :
    struct PMVars {
      float dRZPos;
      float dRZNeg;
      float dPhiPos;
      float dPhiNeg;
      int layerOrDisk;
      int detId;

      void setDPhi(float pos,float neg){dPhiPos=pos;dPhiNeg=neg;}
      void setDRZ(float pos,float neg){dRZPos=pos;dRZNeg=neg;}
      void setDet(int iDetId,int iLayerOrDisk){detId=iDetId;layerOrDisk=iLayerOrDisk;}
    };
    
    
    typedef edm::OwnVector<TrackingRecHit> RecHitContainer ;
    typedef edm::RefToBase<CaloCluster> CaloClusterRef ;
    typedef edm::Ref<TrackCollection> CtfTrackRef ;
    static std::string const & name()
    {
      static std::string const name_("ElectronNHitSeed") ;
      return name_;
    }
    
    //! Construction of base attributes
    ElectronNHitSeed() ;
    ElectronNHitSeed( const TrajectorySeed & ) ;
    ElectronNHitSeed( PTrajectoryStateOnDet & pts, RecHitContainer & rh,  PropagationDirection & dir ) ;
    ElectronNHitSeed * clone() const { return new ElectronNHitSeed(*this) ; }
    virtual ~ElectronNHitSeed()=default;

    //! Set additional info
    void setCtfTrack( const CtfTrackRef & ) ;
    void setCaloCluster( const CaloClusterRef& clus){caloCluster_=clus;isEcalDriven_=true;}
    void addHitInfo(const PMVars& hitVars){hitInfo_.push_back(hitVars);}
    //! Accessors
    const CtfTrackRef& ctfTrack() const { return ctfTrack_ ; }
    const CaloClusterRef& caloCluster() const { return caloCluster_ ; }
   
    //! Utility
    TrackCharge getCharge() const { return startingState().parameters().charge() ; }

    bool isEcalDriven() const { return isEcalDriven_ ; }
    bool isTrackerDriven() const { return isTrackerDriven_ ; }

  private:

    CtfTrackRef ctfTrack_ ;
    CaloClusterRef caloCluster_ ;
    std::vector<PMVars> hitInfo_;
    
    bool isEcalDriven_ ;
    bool isTrackerDriven_ ;

  };
}

#endif
