#include "Validation/HLTrigger/interface/HLTGenLevelValSource.h"
#include "DQMOffline/Trigger/interface/HLTGenLevelValMon.h"

// todo most likely replace these!
#include "DQMOffline/Trigger/interface/EgHLTDebugFuncs.h"
#include "DQMOffline/Trigger/interface/EgHLTDQMCut.h"
#include "DQMOffline/Trigger/interface/EgHLTTrigTools.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include "FWCore/Framework/interface/Run.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include <boost/algorithm/string.hpp>

//#include "DQMOffline/Trigger/interface/EgHLTCutCodes.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
using namespace HLTGenLevelVal;

HLTGenValSource::HLTGenValSource(const edm::ParameterSet& iConfig) : nrEventsProcessed_(0) {
  binData_.setup(iConfig.getParameter<edm::ParameterSet>("binData"));
  cutMasks_.setup(iConfig.getParameter<edm::ParameterSet>("cutMasks"));
  HLTFilterNames_ = iConfig.getParameter<std::vector<std::string> >("HLTFilterNames");

  hltTag_ = iConfig.getParameter<std::string>("hltTag");

  dirName_ = iConfig.getParameter<std::string>("DQMDirName");
  subdirName_ = iConfig.getParameter<std::string>("subDQMDirName");
}

EgHLTOfflineSource::~EgHLTOfflineSource() default;

void EgHLTOfflineSource::bookHistograms(DQMStore::IBooker& iBooker, edm::Run const& run, edm::EventSetup const& c) {
  iBooker.setCurrentFolder(dirName_);

  //the one monitor element the source fills directly
  nrEventsProcessedMonElem_ = iBooker.bookInt("nrEventsProcessed");

  //if the HLTConfig changes during the job, the results are "un predictable" but in practice should be fine
  //the HLTConfig is used for working out which triggers are active, working out which filternames correspond to paths and L1 seeds
  //assuming those dont change for E/g it *should* be fine
  HLTConfigProvider hltConfig;
  bool changed = false;
  hltConfig.init(run, c, hltTag_, changed);

  std::vector<std::string> hltFiltersUsed = HLTFilterNames_;
  trigCodes.reset(TrigCodes::makeCodes(hltFiltersUsed)); // this may crash

  MonElemFuncs monElemFuncs(iBooker, *trigCodes);

  //now book ME's
  iBooker.setCurrentFolder(dirName_ + "/" + subdirName_);
  //each trigger path will generate object distributions, BUT not trigger efficiencies...
  for (auto const& HLTFilterName : HLTFilterNames_) {
    iBooker.setCurrentFolder(dirName_ + "/" + subdirName_ + "/" + eleHLTFilterName);
    addEleTrigPath(monElemFuncs, eleHLTFilterName);
  }

  //efficiencies of one trigger path relative to another
  monElemFuncs.initTightLooseTrigHists(eleMonElems_, eleTightLooseTrigNames_, binData_, "gsfEle");
  //new EgHLTDQMVarCut<OffEle>(cutMasks_.stdEle,&OffEle::cutCode));
  //monElemFuncs.initTightLooseTrigHistsTrigCuts(eleMonElems_,eleTightLooseTrigNames_,binData_);

  iBooker.setCurrentFolder(dirName_);
}

void EgHLTOfflineSource::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  const double weight = 1.;  //we have the ability to weight but its disabled for now - maybe use this for prescales?
  nrEventsProcessed_++;
  nrEventsProcessedMonElem_->Fill(nrEventsProcessed_);

  for (auto& filterMonHist : filterMonHists_) {
    filterMonHist->fill(offEvt_, weight);
  }
}

void EgHLTOfflineSource::addEleTrigPath(MonElemFuncs& monElemFuncs, const std::string& name) {
  auto* filterMon = new EleHLTFilterMon(monElemFuncs, name, trigCodes->getCode(name.c_str()), binData_, cutMasks_, dohep_);
  eleFilterMonHists_.push_back(filterMon);
  std::sort(eleFilterMonHists_.begin(), eleFilterMonHists_.end(), [](auto const& x, auto const& y) { return *x < *y; });
  //takes a minor efficiency hit at initalisation to ensure that the vector is always sorted
}
