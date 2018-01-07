#include "DataFormats/RecoCandidate/interface/RecoGenericCandidate.h"

using namespace reco;

RecoGenericCandidate::~RecoGenericCandidate() { }

RecoGenericCandidate * RecoGenericCandidate::clone() const { 
  return new RecoGenericCandidate( * this ); 
}
