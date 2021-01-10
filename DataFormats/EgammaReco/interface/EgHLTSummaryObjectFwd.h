#ifndef DataFormats_EgammaReco_EgHLTSummaryObjectFwd_h
#define DataFormats_EgammaReco_EgHLTSummaryObjectFwd_h

#include <vector>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/RefProd.h"

namespace reco {
  class EgHLTSummaryObject;

  typedef std::vector<EgHLTSummaryObject> EgHLTSummaryObjectCollection;
  typedef edm::Ref<EgHLTSummaryObjectCollection> EgHLTSummaryObjectRef;
  typedef edm::RefProd<EgHLTSummaryObjectCollection> EgHLTSummaryObjectRefProd;
  typedef edm::RefVector<EgHLTSummaryObjectCollection> EgHLTSummaryObjectRefVector;
  typedef EgHLTSummaryObjectRefVector::iterator EgHLTSummaryObjectIterator;
}  // namespace reco

#endif
