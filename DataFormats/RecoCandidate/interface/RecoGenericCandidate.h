#ifndef RecoCandidate_RecoGenericCandidate_h
#define RecoCandidate_RecoGenericCandidate_h
/** \class reco::RecoGenericCandidate
 *
 * Reco Candidates with nothing special 
 * 
 * RecoCandidate is virtual otherwise this class wouldnt exist
 *
 * 
 *
 *
 */
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"

namespace reco {

  class RecoGenericCandidate : public RecoCandidate {
  public:
    /// default constructor
    RecoGenericCandidate() : RecoCandidate() { }
    /// constructor from values
    RecoGenericCandidate( Charge q , const LorentzVector & p4, const Point & vtx = Point( 0, 0, 0 ),
		       int pdgId = 0, int status = 0 ) :
      RecoCandidate( q, p4, vtx, pdgId, status ) { }
    /// constructor from values
    RecoGenericCandidate( Charge q , const PolarLorentzVector & p4, const Point & vtx = Point( 0, 0, 0 ),
		       int pdgId = 0, int status = 0 ) :
      RecoCandidate( q, p4, vtx, pdgId, status ) { }
    /// destructor
    ~RecoGenericCandidate() override;
    /// returns a clone of the candidate
    RecoGenericCandidate * clone() const override;

  private:
    /// check overlap with another candidate
    /// in this case we never ever overlap
    bool overlap( const Candidate & ) const override {return false;}
  };
  
}

#endif
