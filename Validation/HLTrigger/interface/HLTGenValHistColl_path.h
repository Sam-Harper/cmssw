#ifndef DQMOnline_Trigger_HLTGenValHistColl_path_h
#define DQMOnline_Trigger_HLTGenValHistColl_path_h

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

#include "DQMOffline/Trigger/interface/FunctionDefs.h"
#include "DQMOffline/Trigger/interface/UtilFuncs.h"

#include "Validation/HLTrigger/interface/HLTGenValHistColl_filter.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "Validation/HLTrigger/interface/HLTGenValObject.h"

using namespace reco;

class HLTGenValHistColl_path {
public:
  typedef dqm::legacy::MonitorElement MonitorElement;
  typedef dqm::legacy::DQMStore DQMStore;

  explicit HLTGenValHistColl_path(std::string objType, std::string triggerPath, HLTConfigProvider& hltConfig);

  static edm::ParameterSetDescription makePSetDescription();
  static edm::ParameterSetDescription makePSetDescriptionHistConfigs();

  void bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs);
  void fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent);

  // this is public as it is accessed by HLTGenValSource. Could be replaced by a getter.
  std::string triggerPath_;

private:

  std::vector<HLTGenValHistColl_filter> collection_filter_;
  std::string objType_;
  std::vector<std::string> filters_;
  HLTConfigProvider hltConfig_;
};

HLTGenValHistColl_path::HLTGenValHistColl_path(std::string objType, std::string triggerPath, HLTConfigProvider& hltConfig)
    : triggerPath_(triggerPath), objType_(objType), hltConfig_(hltConfig) {

  collection_filter_.emplace_back(HLTGenValHistColl_filter(objType_, "beforeAnyFilter"));

  filters_ = hltConfig_.saveTagsModules(triggerPath_);
  std::set<std::string>::iterator ilabel;
  for (auto & filter : filters_) {
    collection_filter_.emplace_back(HLTGenValHistColl_filter(objType_, filter));
  }

}

edm::ParameterSetDescription HLTGenValHistColl_path::makePSetDescription() {
  // TODO
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  return desc;
}

edm::ParameterSetDescription HLTGenValHistColl_path::makePSetDescriptionHistConfigs() {
  // TODO
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  return desc;
}

void HLTGenValHistColl_path::bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs) {
  for (auto& collection_filter : collection_filter_) collection_filter.bookHists(iBooker, histConfigs);
}

void HLTGenValHistColl_path::fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent) {
  for (auto& collection_filter : collection_filter_) collection_filter.fillHists(obj, triggerEvent);
}

#endif
