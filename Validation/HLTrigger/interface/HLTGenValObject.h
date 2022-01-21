#ifndef DQMOnline_Trigger_HLTGenValObject_h
#define DQMOnline_Trigger_HLTGenValObject_h

/****************************************************************************
* \class HLTGenValObject
*
* This class is a "wrapper" for the Generator level validation code, which unifies various object types.
* It handles the different type of objects the code needs to run on: GenParticles, GenJets and event-level pt sums
*
* \author Finn Labe - University of Hamburg - Nov. 2021
*
****************************************************************************/

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include <vector>

class HLTGenValObject {
public:

  // empty constructor
  HLTGenValObject() {}

  // constructor from GenParticle
  HLTGenValObject(const reco::GenParticle &p)
      : p4Polar_(p.p4()), p4Cartesian_(p.p4()) {}

  // constructor from GenJet
  HLTGenValObject(const reco::GenJet &p)
      : p4Polar_(p.p4()), p4Cartesian_(p.p4()) {}

  // constructor from LorentzVector (for energy sums)
  HLTGenValObject(const reco::Candidate::PolarLorentzVector& p)
      : p4Polar_(p), p4Cartesian_(p) {}

  // destructor
  ~HLTGenValObject() {}

  // object functions, for usage of HLTGenValObjects by other modules
  double pt() const { return p4Polar_.pt(); }
  double eta() const { return p4Polar_.eta(); }
  double phi() const { return p4Polar_.phi(); }
  double et() const { return (pt() <= 0) ? 0 : p4Cartesian_.Et(); }

private:
  // containing information in two "shapes"
  math::PtEtaPhiMLorentzVector p4Polar_;
  math::XYZTLorentzVector p4Cartesian_;
};

#endif
