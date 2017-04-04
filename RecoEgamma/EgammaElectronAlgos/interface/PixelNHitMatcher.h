#ifndef RecoEgamma_EgammaElectronAlgos_PixelNHitMatch_h
#define RecoEgamma_EgammaElectronAlgos_PixelNHitMatch_h





#include "FWCore/Framework/interface/ESHandle.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/MaterialEffects/interface/PropagatorWithMaterial.h"

#include <unordered_map>

namespace edm{
  class EventSetup;
  class ConfigurationDescriptions;
  class ParameterSet;
}

class FreeTrajectoryState;
class TrackingRecHit;

class SeedWithInfo {
  int dummy_;
};

//stolen from PixelHitMatcher
//decide if its evil or not later
//actually I think the answer is, yes, yes its evil
//maybe replace with less evil?
namespace std{
  template<>
    struct hash<std::pair<int,GlobalPoint> > {
    std::size_t operator()(const std::pair<int,GlobalPoint>& g) const {
      auto h1 = std::hash<unsigned long long>()((unsigned long long)g.first);
      unsigned long long k; memcpy(&k, &g.second,sizeof(k));
      auto h2 = std::hash<unsigned long long>()(k);
      return h1 ^ (h2 << 1);
      }
  };
  template<>
  struct equal_to<std::pair<int,GlobalPoint> > : public std::binary_function<std::pair<int,GlobalPoint>,std::pair<int,GlobalPoint>,bool> {
    bool operator()(const std::pair<int,GlobalPoint>& a, 
		    const std::pair<int,GlobalPoint>& b)  const {
      return (a.first == b.first) & (a.second == b.second);
    }
  };
}

class PixelNHitMatcher {
public:
  class HitInfo {
  public:
    HitInfo():detId_(0),
	      dRZ_(std::numeric_limits<float>::max()),
	      dPhi_(std::numeric_limits<float>::max()),
	      hit_(nullptr){}
	      
    HitInfo(const GlobalPoint& vtxPos,
	    const TrajectoryStateOnSurface& trajState,
	    const TrackingRecHit& hit);
    ~HitInfo()=default;
    
    int subdetId()const{return detId_.subdetId();}
    DetId detId()const{return detId_;}
    float dRZ()const{return dRZ_;}
    float dPhi()const{return dPhi_;}
    const GlobalPoint& pos()const{return pos_;}
    const TrackingRecHit* hit()const{return hit_;}
  private:
    DetId detId_;
    GlobalPoint pos_;
    float dRZ_;
    float dPhi_;    
    const TrackingRecHit* hit_; //we do not own this
  };

  class MatchingCuts {
  public:
    explicit MatchingCuts(const edm::ParameterSet& pset);
    bool operator()(const HitInfo& hit)const;
  private:
    float dPhiMax_;
    float dZMax_;
    float dRIMax_;
    float dRFMax_; 
  };

public:  
  explicit PixelNHitMatcher(const edm::ParameterSet& pset);
  ~PixelNHitMatcher()=default;

  static void fillDescriptions(edm::ConfigurationDescriptions& description);
  

  void doEventSetup(const edm::EventSetup& iSetup);
  
  std::vector<SeedWithInfo>
  compatibleSeeds(const TrajectorySeedCollection& seeds, const GlobalPoint& candPos,
		  const GlobalPoint & vprim, const float energy);
  
private:
  
  std::vector<HitInfo> processSeed(const TrajectorySeed& seed, const GlobalPoint& candPos,
				   const GlobalPoint & vprim, const float energy, const int charge );

  static float getZVtxFromExtrapolation(const GlobalPoint& primeVtxPos,const GlobalPoint& hitPos,
					const GlobalPoint& candPos);
  
  bool passTrajPreSel(const GlobalPoint& hitPos,const GlobalPoint& candPos)const;
  
  PixelNHitMatcher::HitInfo matchFirstHit(const TrajectorySeed& seed,
					  const TrajectoryStateOnSurface& trajState,
					  const GlobalPoint& vtxPos,
					  const PropagatorWithMaterial& propagator);

  PixelNHitMatcher::HitInfo match2ndToNthHit(const TrajectorySeed& seed,
					     const FreeTrajectoryState& trajState,
					     const size_t hitNr,	
					     const GlobalPoint& prevHitPos,
					     const GlobalPoint& vtxPos,
					     const PropagatorWithMaterial& propagator);
  
  const TrajectoryStateOnSurface& getTrajStateFromVtx(const TrackingRecHit& hit,const TrajectoryStateOnSurface& initialState,const PropagatorWithMaterial& propagator);
  const TrajectoryStateOnSurface& getTrajStateFromPoint(const TrackingRecHit& hit,const FreeTrajectoryState& initialState,const GlobalPoint& point,const PropagatorWithMaterial& propagator);

  void clearCache();

  bool passesMatchSel(const HitInfo& hit,const size_t hitNr)const;
  
private:
  static constexpr float kElectronMass_ = 0.000511;
  static constexpr float kPhiCut_ = std::cos(2.5);
  std::unique_ptr<PropagatorWithMaterial> forwardPropagator_;
  std::unique_ptr<PropagatorWithMaterial> backwardPropagator_;
  unsigned long long cacheIDMagField_;
  edm::ESHandle<MagneticField> magField_;

  size_t nrHitsRequired_;
  bool useRecoVertex_;
  std::vector<MatchingCuts> matchingCuts_;

  std::unordered_map<int,TrajectoryStateOnSurface> trajStateFromVtxPosChargeCache_;
  std::unordered_map<int,TrajectoryStateOnSurface> trajStateFromVtxNegChargeCache_;

  std::unordered_map<std::pair<int,GlobalPoint>,TrajectoryStateOnSurface> trajStateFromPointPosChargeCache_;
  std::unordered_map<std::pair<int,GlobalPoint>,TrajectoryStateOnSurface> trajStateFromPointNegChargeCache_;

};

#endif
