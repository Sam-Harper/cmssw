#ifndef HLTGenLevelEff_h
#define HLTGenLevelEff_h

//***************************************************************************
//
// Description:
//   This class produces histograms which can be used to make efficiencies for specified HLT filters
//   These histograms can be binned in various variables
//
//   In a nutshell:
//   1) reads collection of GEN and RECO level objects (before and after a filter)
//   2) performs a matching of RECO object to GEN object (or should this be event-level?)
//   3) measures efficiency of certain filter
//
//
// Author: Finn Labe, 2021
// Shamelessly stolen from Sam: https://github.com/cms-sw/cmssw/blob/master/DQMOffline/Trigger/python/EgammaMonitoring_cff.py
//
//***************************************************************************

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "DQMOffline/Trigger/interface/UtilFuncs.h"
#include "Validation/HLTrigger/interface/HLTGenLevelHists.h"

#include "CommonTools/TriggerUtils/interface/GenericTriggerEventFlag.h"

#include <vector>
#include <string>

namespace {
  template <typename T>
  edm::Handle<T> getHandle(const edm::Event& event, const edm::EDGetTokenT<T>& token) {
    edm::Handle<T> handle;
    event.getByToken(token, handle);
    return handle;
  }
}  // namespace

// class definition
template <typename ObjType, typename ObjCollType>
class HLTGenLevelEff {
public:
  typedef dqm::legacy::DQMStore DQMStore;

  explicit HLTGenLevelEff(const edm::ParameterSet& pset, edm::ConsumesCollector&& cc);

  static edm::ParameterSetDescription makePSetDescription();

  void beginRun(const edm::Run& run, const edm::EventSetup& setup);
  void bookHists(DQMStore::IBooker& iBooker);
  void fill(const edm::Event& event, const edm::EventSetup& setup);

private:
  std::vector<edm::Ref<ObjCollType> > getPassingRefs(const edm::Handle<ObjCollType>& objCollHandle,
                                                     const trigger::TriggerEvent& trigEvt,
                                                     const std::vector<std::string>& filterNames,
                                                     const edm::Handle<edm::ValueMap<bool> >& vidHandle);

private:
  edm::EDGetTokenT<ObjCollType> objToken_;
  edm::EDGetTokenT<trigger::TriggerEvent> trigEvtToken_;
  edm::EDGetTokenT<edm::ValueMap<bool> > VIDToken_;

  std::string hltProcess_;

  std::vector<std::string> filters_;

  std::vector<edm::ParameterSet> histConfigs_;
  std::vector<HLTGenLevelHists<ObjType> > filterHists_;

  GenericTriggerEventFlag sampleTrigRequirements_;
};

// constructor
template <typename ObjType, typename ObjCollType>
HLTGenLevelEff<ObjType, ObjCollType>::HLTGenLevelEff(
    const edm::ParameterSet& pset, edm::ConsumesCollector&& cc)
    : sampleTrigRequirements_(pset.getParameter<edm::ParameterSet>("sampleTrigRequirements"), cc) {

  edm::InputTag trigEvtTag = pset.getParameter<edm::InputTag>("trigEvent");

  objToken_ = cc.consumes<ObjCollType>(pset.getParameter<edm::InputTag>("coll"));
  trigEvtToken_ = cc.consumes<trigger::TriggerEvent>(trigEvtTag);
  VIDToken_ = cc.consumes<edm::ValueMap<bool> >(pset.getParameter<edm::InputTag>("VIDCuts"));

  hltProcess_ = trigEvtTag.process();

  filters_ = pset.getParameter<std::vector<std::string> >("filters");

  histConfigs_ = pset.getParameter<std::vector<edm::ParameterSet> >("histConfigs");
  const auto& filterConfigs = pset.getParameter<std::vector<edm::ParameterSet> >("filterConfigs");

  std::string baseHistName = pset.getParameter<std::string>("baseHistName");

  for (auto& config : filterConfigs) {
    filterHists_.emplace_back(HLTGenLevelHists<ObjType>(config, baseHistName, hltProcess_));
  }
}

template <typename ObjType, typename ObjCollType>
edm::ParameterSetDescription HLTGenLevelEff<ObjType, ObjCollType>::makePSetDescription() {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD", "", "HLT"));
  desc.add<edm::InputTag>("coll", edm::InputTag());
  desc.add<edm::InputTag>("VIDCuts", edm::InputTag());
  desc.add<std::vector<std::string> >("filters", std::vector<std::string>());
  desc.addVPSet("histConfigs",
                HLTGenLevelHists<ObjType>::makePSetDescriptionHistConfigs(),
                std::vector<edm::ParameterSet>());
  desc.addVPSet("filterConfigs",
                HLTGenLevelHists<ObjType>::makePSetDescription(),
                std::vector<edm::ParameterSet>());
  desc.add<std::string>("baseHistName");

  // TODO what is all this stuff?? Maybe delete it? Maybe not? Who knows...
  edm::ParameterSetDescription trigEvtFlagDesc;
  trigEvtFlagDesc.add<bool>("andOr", false);
  trigEvtFlagDesc.add<unsigned int>("verbosityLevel", 1);
  trigEvtFlagDesc.add<bool>("andOrDcs", false);
  trigEvtFlagDesc.add<edm::InputTag>("dcsInputTag", edm::InputTag("scalersRawToDigi"));
  trigEvtFlagDesc.add<std::vector<int> >("dcsPartitions", {24, 25, 26, 27, 28, 29});
  trigEvtFlagDesc.add<bool>("errorReplyDcs", true);
  trigEvtFlagDesc.add<std::string>("dbLabel", "");
  trigEvtFlagDesc.add<bool>("andOrHlt", true);  //true = OR, false = and
  trigEvtFlagDesc.add<edm::InputTag>("hltInputTag", edm::InputTag("TriggerResults::HLT"));
  trigEvtFlagDesc.add<std::vector<std::string> >("hltPaths", {});
  trigEvtFlagDesc.add<std::string>("hltDBKey", "");
  trigEvtFlagDesc.add<bool>("errorReplyHlt", false);
  desc.add<edm::ParameterSetDescription>("sampleTrigRequirements", trigEvtFlagDesc);

  return desc;
}

// calling the booker that books the books.
template <typename ObjType, typename ObjCollType>
void HLTGenLevelEff<ObjType, ObjCollType>::bookHists(DQMStore::IBooker& iBooker) {
  for (auto& filterHist : filterHists_)
    filterHist.bookHists(iBooker, histConfigs_);
}

template <typename ObjType, typename ObjCollType>
void HLTGenLevelEff<ObjType, ObjCollType>::beginRun(const edm::Run& run,
                                                                                    const edm::EventSetup& setup) {
  if (sampleTrigRequirements_.on())
    sampleTrigRequirements_.initRun(run, setup);
}

// fill method, calling the fill method which calls the actual fills. yes really.
template <typename ObjType, typename ObjCollType>
void HLTGenLevelEff<ObjType, ObjCollType>::fill(const edm::Event& event,
                                                                                const edm::EventSetup& setup) {
  auto collHandle = getHandle(event, objToken_);
  auto trigEvtHandle = getHandle(event, trigEvtToken_);
  auto VIDHandle = getHandle(event, VIDToken_);

  //we need the object collection and trigger info at the minimum
  if (!collHandle.isValid() || !trigEvtHandle.isValid())
    return;

  //if GenericTriggerEventFlag is "off", it'll return true regardless
  //if so if its off, we auto pass which is the behaviour we wish to have
  //if its null, we auto fail (because that shouldnt happen)
  // yeah I have no idea what this means. lets keep it in.
  if (sampleTrigRequirements_.accept(event, setup) == false)
    return;

  // Oh looks like I *do* need getPassingRefs...
  std::vector<edm::Ref<ObjCollType> > refs =
      getPassingRefs(collHandle, *trigEvtHandle, filters_, VIDHandle);

  for (auto& ref : refs) {
    for (auto& filterHist : filterHists_) {
      filterHist.fillHists(*ref, event, setup, *trigEvtHandle);
    }
  }
}

// what does this do exactly?
template <typename ObjType, typename ObjCollType>
std::vector<edm::Ref<ObjCollType> > HLTGenLevelEff<ObjType, ObjCollType>::getPassingRefs(
    const edm::Handle<ObjCollType>& objCollHandle,
    const trigger::TriggerEvent& trigEvt,
    const std::vector<std::string>& filterNames,
    const edm::Handle<edm::ValueMap<bool> >& vidHandle) {
  std::vector<edm::Ref<ObjCollType> > passingRefs;
  for (size_t objNr = 0; objNr < objCollHandle->size(); objNr++) {
    edm::Ref<ObjCollType> ref(objCollHandle, objNr);
    if (hltdqm::passTrig(ref->eta(), ref->phi(), trigEvt, filterNames, true /* ??? */, hltProcess_) &&
        (vidHandle.isValid() == false || (*vidHandle)[ref] == true)) {
      passingRefs.push_back(ref);
    }
  }
  return passingRefs;
}

#endif
