#ifndef DQMOnline_Trigger_HLTGenValHistColl_path_h
#define DQMOnline_Trigger_HLTGenValHistColl_path_h

//********************************************************************************
//
// Description:
//   This contains a collection of HLTGenValHistColl_filter used to measure the efficiencies of all
//   filters in a specified path. The actual booking and filling happens in the respective HLTGenValHistColl_filters.
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

// class containing a collection of HLTGenValHistColl_filters for a specific path
// at object creation time, the object type (used for systematically naming the histogram),
// triggerPath, hltConfig and dR2limit (for deltaR matching) need to be specified
// functions for initial booking of hists, and filling of hists for a single object, are available
class HLTGenValHistColl_path {
public:
  typedef dqm::legacy::MonitorElement MonitorElement;
  typedef dqm::legacy::DQMStore DQMStore;

  explicit HLTGenValHistColl_path(std::string objType, std::string triggerPath, HLTConfigProvider& hltConfig, double dR2limit);

  void bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs, std::vector<edm::ParameterSet>& histConfigs2D);
  void fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent);

  // this is public as it is accessed by HLTGenValSource. Could be replaced by a getter.
  std::string triggerPath_;

private:
  std::vector<HLTGenValHistColl_filter> collection_filter_;
  std::vector<std::string> filters_;
  HLTConfigProvider hltConfig_;
};

HLTGenValHistColl_path::HLTGenValHistColl_path(std::string objType, std::string triggerPath, HLTConfigProvider& hltConfig, double dR2limit)
    : triggerPath_(triggerPath), hltConfig_(hltConfig) {

  // as we want a "before" hist before any filter is applied, this dummy is added to the collection
  collection_filter_.emplace_back(HLTGenValHistColl_filter(objType, "beforeAnyFilter", "HLT", dR2limit));

  // getting all filters from path
  filters_ = hltConfig_.saveTagsModules(triggerPath_);
  for (auto & filter : filters_) {
    std::cout << filter << std::endl; // debug to see which filter we found
    collection_filter_.emplace_back(HLTGenValHistColl_filter(objType, filter, "HLT", dR2limit));
  }

}

// hist booking function
// this just calls the booking for each object in the collection_filter
void HLTGenValHistColl_path::bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs, std::vector<edm::ParameterSet>& histConfigs2D) {
  for (auto& collection_filter : collection_filter_) collection_filter.bookHists(iBooker, histConfigs, histConfigs2D);
}

// hist filling function
// this just calls the filling for each object in the collection_filter
void HLTGenValHistColl_path::fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent) {
  for (auto& collection_filter : collection_filter_) collection_filter.fillHists(obj, triggerEvent);
}

#endif
