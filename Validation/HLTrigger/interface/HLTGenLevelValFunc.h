#ifndef DQMOFFLINE_TRIGGER_HLTGENLEVELVALFUNCS
#define DQMOFFLINE_TRIGGER_HLTGENLEVELVALFUNCS

// Author: Finn Labe

// Description: A collection of functions which assist and automate the creation
//             of useful monitor elements for Eg DQM
//

#include "DQMOffline/Trigger/interface/EgHLTMonElemManager.h"
#include "DQMOffline/Trigger/interface/EgHLTOffEle.h"
#include "DQMOffline/Trigger/interface/EgHLTOffPho.h"
#include "DQMOffline/Trigger/interface/EgHLTMonElemWithCut.h"
#include "DQMOffline/Trigger/interface/EgHLTMonElemMgrEBEE.h"
#include "DQMOffline/Trigger/interface/EgHLTMonElemMgrHEP.h"
#include "DQMOffline/Trigger/interface/EgHLTDQMCut.h"
#include "DQMOffline/Trigger/interface/EgHLTMonElemContainer.h"
#include "DQMOffline/Trigger/interface/EgHLTBinData.h"
#include "DQMOffline/Trigger/interface/EgHLTTrigTools.h"
#include "DQMOffline/Trigger/interface/EgHLTMonElemWithCutEBEE.h"

#include <boost/algorithm/string.hpp>

namespace HLTGenLevelVal {
  using dqm::legacy::DQMStore;
  using dqm::legacy::MonitorElement;

  struct CutMasks;

  class HLTGenLevelValFuncs {
  private:
    DQMStore::IBooker& iBooker;

  public:
    HLTGenLevelValFuncs(DQMStore::IBooker& i) : iBooker(i){};
    ~HLTGenLevelValFuncs() = default;
    ;
    DQMStore::IBooker& getIB() { return iBooker; };

  public:
    void initHists(std::vector<MonElemManagerBase<OffEle>*>& histVec, // initializing hists
                         const std::string& filterName,
                         const std::string& baseName,
                         const BinData& bins);

    template <class T, typename varType>
    void addHist(std::vector<MonElemManagerBase<T>*>& histVec, // adding a histogram to a vector
                    const std::string& name,
                    const std::string& title,
                    const BinData::Data1D& binData,
                    varType (T::*varFunc)() const) {
      histVec.push_back(new MonElemMgrEBEE<T, varType>(iBooker, name, title, binData.nr, binData.min, binData.max, varFunc));
    }

  };  // end of class
}  // namespace HLTGenLevelVal

#endif
