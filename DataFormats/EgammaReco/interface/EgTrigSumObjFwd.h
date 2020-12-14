#ifndef DataFormats_EgammaReco_EgTrigSumObjFwd_h
#define DataFormats_EgammaReco_EgTrigSumObjFwd_h

#include <vector>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/RefProd.h"

namespace reco {
  class EgTrigSumObj;

  typedef std::vector<EgTrigSumObj> EgTrigSumObjCollection;
  typedef edm::Ref<EgTrigSumObjCollection> EgTrigSumObjRef;
  typedef edm::RefProd<EgTrigSumObjCollection> EgTrigSumObjRefProd;
  typedef edm::RefVector<EgTrigSumObjCollection> EgTrigSumObjRefVector;
  typedef EgTrigSumObjRefVector::iterator egTrigSumObj_iterator;
}  // namespace reco

#endif
