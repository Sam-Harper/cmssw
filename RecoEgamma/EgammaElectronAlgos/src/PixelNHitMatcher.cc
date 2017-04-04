#include "RecoEgamma/EgammaElectronAlgos/interface/PixelNHitMatcher.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/TrajectorySeed/interface/TrajectorySeed.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/PerpendicularBoundPlaneBuilder.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"

#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"

#include "RecoEgamma/EgammaElectronAlgos/interface/FTSFromVertexToPointFactory.h"
#include "RecoEgamma/EgammaElectronAlgos/interface/ElectronUtilities.h"

PixelNHitMatcher::PixelNHitMatcher(const edm::ParameterSet& pset):
  cacheIDMagField_(0)
{
  useRecoVertex_ = pset.getParameter<bool>("useRecoVertex");
  const auto cutsPSets=pset.getParameter<std::vector<edm::ParameterSet> >("matchingCuts");
  for(const auto & cutPSet : cutsPSets){
    matchingCuts_.push_back(MatchingCuts(cutPSet));
  }
  nrHitsRequired_=matchingCuts_.size();
}

void PixelNHitMatcher::fillDescriptions(edm::ConfigurationDescriptions& description)
{
  edm::ParameterSetDescription desc;
  desc.add<bool>("useRecoVertex",false);

  edm::ParameterSetDescription cutsDesc;
  cutsDesc.add<double>("dPhiMax",0.04);
  cutsDesc.add<double>("dZMax",0.04);
  cutsDesc.add<double>("dRIMax",0.04);
  cutsDesc.add<double>("dRFMax",0.04);
  desc.addVPSet("matchingCuts",cutsDesc);
  description.add("pixelNHitMatch",desc);
}

void PixelNHitMatcher::doEventSetup(const edm::EventSetup& iSetup)
{
  if (cacheIDMagField_!=iSetup.get<IdealMagneticFieldRecord>().cacheIdentifier()) {
    iSetup.get<IdealMagneticFieldRecord>().get(magField_);
    cacheIDMagField_=iSetup.get<IdealMagneticFieldRecord>().cacheIdentifier();
    forwardPropagator_=std::make_unique<PropagatorWithMaterial>(alongMomentum,kElectronMass_,&*(magField_));
    backwardPropagator_=std::make_unique<PropagatorWithMaterial>(oppositeToMomentum,kElectronMass_,&*(magField_));
  }
}

std::vector<SeedWithInfo>
PixelNHitMatcher::compatibleSeeds(const TrajectorySeedCollection& seeds, const GlobalPoint& candPos,
				  const GlobalPoint & vprim, const float energy)
{
  if(!forwardPropagator_ || backwardPropagator_ || !magField_.isValid()){
    throw cms::Exception("LogicError") <<__FUNCTION__<<" can not make pixel seeds as event setup has not properly been called";
  }

  clearCache();
  

  std::vector<SeedWithInfo> matchedSeeds;
  for(const auto& seed : seeds) {
    std::vector<HitInfo> matchedHitsNeg = processSeed(seed,candPos,vprim,energy,-1);
    std::vector<HitInfo> matchedHitsPos = processSeed(seed,candPos,vprim,energy,+1);
    if(matchedHitsNeg.size()==nrHitsRequired_ ||
       matchedHitsPos.size()==nrHitsRequired_){
      //do the result
      std::cout <<"do something "<<std::endl;
    }
  }
  return matchedSeeds;
}


//checks if the hits of the seed match within requested selection
//matched hits are required to be consecutive, as soon as hit isnt matched,
//the function returns, it doesnt allow skipping hits
std::vector<PixelNHitMatcher::HitInfo>
PixelNHitMatcher::processSeed(const TrajectorySeed& seed, const GlobalPoint& candPos,
			      const GlobalPoint & vprim, const float energy, const int charge )
{
  
  if(seed.nHits()!=nrHitsRequired_){
    throw cms::Exception("Configuration") <<"PixelNHitMatcher is being fed seeds with "<<seed.nHits()<<" but requires "<<nrHitsRequired_<<" for a match, it is inconsistantly configured";
  }

  
  FreeTrajectoryState trajStateFromVtx = FTSFromVertexToPointFactory::get(*magField_, candPos, vprim, energy, charge);
  PerpendicularBoundPlaneBuilder bpb;
  TrajectoryStateOnSurface initialTrajState(trajStateFromVtx,*bpb(trajStateFromVtx.position(), 
								  trajStateFromVtx.momentum()));
 
  std::vector<HitInfo> matchedHits;
  HitInfo firstHit = matchFirstHit(seed,initialTrajState,vprim,*backwardPropagator_);
  if(passesMatchSel(firstHit,0)){
    matchedHits.push_back(firstHit);
    
    //now we can figure out the z vertex
    double zVertex = useRecoVertex_ ? vprim.z() : getZVtxFromExtrapolation(vprim,firstHit.pos(),candPos);
    GlobalPoint vertex(vprim.x(),vprim.y(),zVertex);
    
    //FIXME: rename this variable
    FreeTrajectoryState fts2 = FTSFromVertexToPointFactory::get(*magField_, firstHit.pos(), 
								vertex, energy, charge) ;
    
    GlobalPoint prevHitPos = firstHit.pos();
    for(size_t hitNr=1;hitNr<nrHitsRequired_;hitNr++){
      HitInfo hit = match2ndToNthHit(seed,fts2,hitNr,prevHitPos,vertex,*forwardPropagator_);
      if(passesMatchSel(hit,hitNr)){
	matchedHits.push_back(hit);
	prevHitPos = hit.pos();
      }else break;
    }
  }
  return matchedHits;
}

// compute the z vertex from the candidate position and the found pixel hit
float PixelNHitMatcher::getZVtxFromExtrapolation(const GlobalPoint& primeVtxPos,const GlobalPoint& hitPos,
						 const GlobalPoint& candPos)
{
  auto sq = [](float x){return x*x;};
  auto calRDiff = [sq](const GlobalPoint& p1,const GlobalPoint& p2){
    return std::sqrt(sq(p2.x()-p1.x()) + sq(p2.y()-p1.y()));
  };
  const double r1Diff = calRDiff(primeVtxPos,hitPos);
  const double r2Diff = calRDiff(hitPos,candPos);
  return hitPos.z() - r1Diff*(candPos.z()-hitPos.z())/r2Diff;
}

bool PixelNHitMatcher::passTrajPreSel(const GlobalPoint& hitPos,const GlobalPoint& candPos)const
{
  float dt = hitPos.x()*candPos.x()+hitPos.y()*candPos.y();
  if (dt<0) return false;
  if (dt<kPhiCut_*(candPos.perp()*hitPos.perp())) return false;
  return true;
}

const TrajectoryStateOnSurface& PixelNHitMatcher::getTrajStateFromVtx(const TrackingRecHit& hit,const TrajectoryStateOnSurface& initialState,const PropagatorWithMaterial& propagator)
{
  auto& trajStateFromVtxCache = initialState.charge()==1 ? trajStateFromVtxPosChargeCache_ :
                                                           trajStateFromVtxNegChargeCache_;

  auto key = hit.det()->gdetIndex();
  auto res = trajStateFromVtxCache.find(key);
  if(res!=trajStateFromVtxCache.end()) return res->second;
  else{ //doesnt exist, need to make it
    //FIXME: check for efficiency
    auto val = trajStateFromVtxCache.emplace(key,propagator.propagate(initialState,hit.det()->surface()));
    return val.first->second;
  }
}

const TrajectoryStateOnSurface& PixelNHitMatcher::getTrajStateFromPoint(const TrackingRecHit& hit,const FreeTrajectoryState& initialState,const GlobalPoint& point,const PropagatorWithMaterial& propagator)
{
  
  auto& trajStateFromPointCache = initialState.charge()==1 ? trajStateFromPointPosChargeCache_ :
                                                             trajStateFromPointNegChargeCache_;

  auto key = std::make_pair(hit.det()->gdetIndex(),point);
  auto res = trajStateFromPointCache.find(key);
  if(res!=trajStateFromPointCache.end()) return res->second;
  else{ //doesnt exist, need to make it
    //FIXME: check for efficiency
    auto val = trajStateFromPointCache.emplace(key,propagator.propagate(initialState,hit.det()->surface()));
    return val.first->second;
  }
}

PixelNHitMatcher::HitInfo PixelNHitMatcher::matchFirstHit(const TrajectorySeed& seed,const TrajectoryStateOnSurface& trajState,const GlobalPoint& vtxPos,const PropagatorWithMaterial& propagator)
{
  const TrajectorySeed::range& hits = seed.recHits();
  auto hitIt = hits.first;

  if(hitIt->isValid()){
    const TrajectoryStateOnSurface& trajStateFromVtx = getTrajStateFromVtx(*hitIt,trajState,propagator);
    
    if(trajState.isValid()) return HitInfo(vtxPos,trajStateFromVtx,*hitIt);  
  }
  return HitInfo();
}

PixelNHitMatcher::HitInfo PixelNHitMatcher::match2ndToNthHit(const TrajectorySeed& seed,
							     const FreeTrajectoryState& initialState,
							     const size_t hitNr,
							     const GlobalPoint& prevHitPos,
							     const GlobalPoint& vtxPos,
							     const PropagatorWithMaterial& propagator)
{
  const TrajectorySeed::range& hits = seed.recHits();
  auto hitIt = hits.first+hitNr;
  
  if(hitIt->isValid()){
    const TrajectoryStateOnSurface& trajState = getTrajStateFromPoint(*hitIt,initialState,prevHitPos,propagator);
    
    if(trajState.isValid()) return HitInfo(vtxPos,trajState,*hitIt);  
  }
  return HitInfo();
  
}

void PixelNHitMatcher::clearCache()
{
  trajStateFromVtxPosChargeCache_.clear();
  trajStateFromVtxNegChargeCache_.clear();
  trajStateFromPointPosChargeCache_.clear();
  trajStateFromPointPosChargeCache_.clear();
}

bool PixelNHitMatcher::passesMatchSel(const PixelNHitMatcher::HitInfo& hit,const size_t hitNr)const
{
  if(hitNr<matchingCuts_.size()){
    return matchingCuts_[hitNr](hit);
  }else{
    throw cms::Exception("LogicError") <<" Error, attempting to apply selection to hit "<<hitNr<<" but only cuts for "<<matchingCuts_.size()<<" defined";
  }
  
}

PixelNHitMatcher::HitInfo::HitInfo(const GlobalPoint& vtxPos,
				   const TrajectoryStateOnSurface& trajState,
				   const TrackingRecHit& hit):
  detId_(hit.geographicalId()),
  pos_(hit.globalPosition()),
  hit_(&hit)
{
  EleRelPointPair pointPair(pos_,trajState.globalParameters().position(),vtxPos);
  dRZ_ = detId_.subdetId()==PixelSubdetector::PixelBarrel ? pointPair.dZ() : pointPair.dPerp();
  dPhi_ = pointPair.dPhi();
}

PixelNHitMatcher::MatchingCuts::MatchingCuts(const edm::ParameterSet& pset):
  dPhiMax_(pset.getParameter<double>("dPhiMax")),
  dZMax_(pset.getParameter<double>("dZMax")),
  dRIMax_(pset.getParameter<double>("dRIMax")),
  dRFMax_(pset.getParameter<double>("dRFMax"))
{
  
}

bool PixelNHitMatcher::MatchingCuts::operator()(const PixelNHitMatcher::HitInfo& hit)const
{
  if(hit.dPhi() > dPhiMax_) return false;
  float dZOrRMax = hit.subdetId()==PixelSubdetector::PixelBarrel ? dZMax_ : dRFMax_;
  if(hit.dRZ() > dZOrRMax) return false;
  
  return true;
}
