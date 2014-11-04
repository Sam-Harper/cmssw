#include "RecoEgamma/EgammaIsolationAlgos/interface/PfBlockBasedIsolation.h"
#include <cmath>
#include "DataFormats/Math/interface/deltaR.h"



#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"

//--------------------------------------------------------------------------------------------------

PfBlockBasedIsolation::PfBlockBasedIsolation() {
  // Default Constructor.
}




//--------------------------------------------------------------------------------------------------
PfBlockBasedIsolation::~PfBlockBasedIsolation()
{

}


void  PfBlockBasedIsolation::setup ( const edm::ParameterSet& conf ) {

  coneSize_             = conf.getParameter<double>("coneSize");  

}


std::vector<reco::PFCandidateRef>  PfBlockBasedIsolation::calculate(math::XYZTLorentzVectorD p4, const reco::PFCandidateRef pfEGCand, const edm::Handle<reco::PFCandidateCollection> pfCandidateHandle) {
  
  std::vector<reco::PFCandidateRef> myVec;
  
  math::XYZVector candidateMomentum(p4.px(),p4.py(),p4.pz());
  math::XYZVector candidateDirection=candidateMomentum.Unit();

  const reco::PFCandidate::ElementsInBlocks& theElementsInpfEGcand = (*pfEGCand).elementsInBlocks();
  reco::PFCandidate::ElementsInBlocks::const_iterator ieg = theElementsInpfEGcand.begin();
  const reco::PFBlockRef egblock = ieg->first;


  unsigned nObj = pfCandidateHandle->size();
  for(unsigned int lCand=0; lCand < nObj; lCand++) {

    reco::PFCandidateRef pfCandRef(reco::PFCandidateRef(pfCandidateHandle,lCand));

    float dR = 0.0;
    if( coneSize_ < 10.0 ) {
      dR = deltaR(candidateDirection.Eta(), candidateDirection.Phi(),  pfCandRef->eta(),   pfCandRef->phi());         
      if ( dR> coneSize_ ) continue;
    }

    const reco::PFCandidate::ElementsInBlocks& theElementsInPFcand = pfCandRef->elementsInBlocks();

    bool elementFound=false;
    for (reco::PFCandidate::ElementsInBlocks::const_iterator ipf = theElementsInPFcand.begin(); ipf<theElementsInPFcand.end(); ++ipf) {
 
      if ( ipf->first == egblock && !elementFound ) {
	for (ieg = theElementsInpfEGcand.begin(); ieg<theElementsInpfEGcand.end(); ++ieg) {
	  if ( ipf->second == ieg->second && !elementFound  ) {
	    if(correctElementTypeToMatch(*pfCandRef,*ipf)){
	      elementFound=true;
	      myVec.push_back(pfCandRef);    
	    }//end correct type check
	  }//end match of element
	}//end loop over all EG elements
      }//end check that PFBlock is same as e/gamma
    }//end loop over all elements of PF candidate

  }//end loop over all pf candidates

  return myVec;
  
}


bool PfBlockBasedIsolation::correctElementTypeToMatch(const reco::PFCandidate& cand,const reco::PFCandidate::ElementInBlock& elemInBlock)
{
  
  reco::PFCandidate::ParticleType pfType = cand.particleId();
  const reco::PFBlockElement* elem = elemInBlock.second<elemInBlock.first->elements().size() ? &elemInBlock.first->elements()[elemInBlock.second] : nullptr;
  
  reco::PFBlockElement::Type elemType = elem ? elem->type() : reco::PFBlockElement::NONE;
  
  //PF photons must be matched by SC / ECAL elements to the electron, not sufficient to share a track
  if(pfType==reco::PFCandidate::gamma){
    if(elemType==reco::PFBlockElement::ECAL || elemType==reco::PFBlockElement::SC) return true;
    else return false;
  }else return true;
}

