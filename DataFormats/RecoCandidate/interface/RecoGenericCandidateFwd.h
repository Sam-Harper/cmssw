#ifndef RecoCandidate_RecoGenericCandidateFwd_h
#define RecoCandidate_RecoGenericCandidateFwd_h
#include <vector>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"

namespace reco {
  class RecoGenericCandidate;

  /// collectin of RecoGenericCandidate objects
  typedef std::vector<RecoGenericCandidate> RecoGenericCandidateCollection;

  /// reference to an object in a collection of RecoGenericCandidate objects
  typedef edm::Ref<RecoGenericCandidateCollection> RecoGenericCandidateRef;

  /// reference to a collection of RecoGenericCandidate objects
  typedef edm::RefProd<RecoGenericCandidateCollection> RecoGenericCandidateRefProd;

  /// vector of objects in the same collection of RecoGenericCandidate objects
  typedef edm::RefVector<RecoGenericCandidateCollection> RecoGenericCandidateRefVector;

  /// iterator over a vector of reference to RecoGenericCandidate objects
  typedef RecoGenericCandidateRefVector::iterator recoGenericCandidate_iterator;  
}

#endif
