#ifndef DQMOnline_Trigger_HLTGenValHistColl_filter_h
#define DQMOnline_Trigger_HLTGenValHistColl_filter_h

//********************************************************************************
//
// Description:
//   This contains a collection of HLTGenvalHists used to measure the efficiency of a
//   specified path. It is resonsible for booking and filling the histograms.
//
//   Currently, only one hist is booked, before the filter is applied. This will be expanded later.
//
// Author : Finn Labe, UHH, Oct. 2021
// (Heavily borrowed from Sam Harpers HLTDQMFilterEffHists)
//
//***********************************************************************************

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "Validation/HLTrigger/interface/HLTGenValHist.h"
#include "DQMOffline/Trigger/interface/FunctionDefs.h"
#include "DQMOffline/Trigger/interface/UtilFuncs.h"

#include "Validation/HLTrigger/interface/HLTGenValObject.h"

#include "DataFormats/Math/interface/deltaR.h"

#include <utility>

using namespace reco;

class HLTGenValHistColl_filter {
public:
  typedef dqm::legacy::MonitorElement MonitorElement;
  typedef dqm::legacy::DQMStore DQMStore;

  explicit HLTGenValHistColl_filter(std::string objType, std::string path);

  static edm::ParameterSetDescription makePSetDescription();
  static edm::ParameterSetDescription makePSetDescriptionHistConfigs();

  void bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs);
  void fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent);

private:
  void book1D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig);

  std::vector<std::unique_ptr<HLTGenValHist> > hists_;
  std::string objType_;
  std::string filter_;
};

HLTGenValHistColl_filter::HLTGenValHistColl_filter(std::string objType, std::string filter)
    : objType_(objType),
      filter_(filter) {}

edm::ParameterSetDescription HLTGenValHistColl_filter::makePSetDescription() {
  // TODO
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  return desc;
}

edm::ParameterSetDescription HLTGenValHistColl_filter::makePSetDescriptionHistConfigs() {
  // TODO
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  return desc;
}

void HLTGenValHistColl_filter::bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs) {
  for (auto& histConfig : histConfigs) book1D(iBooker, histConfig);
}

// actual booker function for 1D hists
void HLTGenValHistColl_filter::book1D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig) {
  auto binLowEdgesDouble = histConfig.getParameter<std::vector<double> >("binLowEdges");
  std::vector<float> binLowEdges;
  binLowEdges.reserve(binLowEdgesDouble.size());
  for (double lowEdge : binLowEdgesDouble) binLowEdges.push_back(lowEdge);
  auto vsVar = histConfig.getParameter<std::string>("vsVar");

  std::string histname, histtitle;

  if(filter_ == "beforeAnyFilter") {
    histname = objType_ + "_vs" + vsVar + "_before";
    histtitle = objType_ + " vs " + vsVar +" before";
  } else {
    histname = objType_ + "_" + filter_ + "_vs" + vsVar + "_after";
    histtitle = objType_ + "_" + filter_ + "_vs" + vsVar + "_after";
  }

  auto me = iBooker.book1D(histname.c_str(), histtitle.c_str(), binLowEdges.size() - 1, &binLowEdges[0]);

  std::unique_ptr<HLTGenValHist> hist;
  auto vsVarFunc = hltdqm::getUnaryFuncFloat<HLTGenValObject>(vsVar);
  if (!vsVarFunc) {
    throw cms::Exception("ConfigError") << " vsVar " << vsVar << " is giving null ptr (likely empty) in " << __FILE__
                                        << "," << __LINE__ << std::endl;
  }
  hist = std::make_unique<HLTGenValHist1D>(me->getTH1(), vsVar, vsVarFunc);
  hists_.emplace_back(std::move(hist));
}

// histogram filling routine
void HLTGenValHistColl_filter::fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent) {

  // this handles the "before" step
  // there may be a better way how to do this
  if(filter_ == "beforeAnyFilter") {
    for (auto& hist : hists_) hist->fill(obj);
  } else {
    // main filling code

    // get gilters
    edm::InputTag filterTag(filter_, "", "HLT"); // TODO replace the third argument hre
    size_t filterIndex = triggerEvent->filterIndex(filterTag);

    // get trigger objects of filter in question
    trigger::TriggerObjectCollection allTriggerObjects = triggerEvent->getObjects();
    trigger::TriggerObjectCollection selectedObjects;
    if (filterIndex < triggerEvent->sizeFilters()) {
      const auto& keys = triggerEvent->filterKeys(filterIndex);
      for (unsigned short key : keys) {
        trigger::TriggerObject foundObject = allTriggerObjects[key];
        selectedObjects.push_back(foundObject);
      }
    }

    // do a deltaR matching
    double mindR2 = 999;
    for (const auto & filterobj : selectedObjects) {
      double dR = deltaR2(obj, filterobj);
      if(dR < mindR2) mindR2 = dR;
    }
    double dR2limit = 0.1; // TODO put this working point as a variable somewhere

    // filling hist if GEN particle is matched to some filter object
    if(mindR2 < dR2limit) for (auto& hist : hists_) hist->fill(obj);

  }
}

#endif
