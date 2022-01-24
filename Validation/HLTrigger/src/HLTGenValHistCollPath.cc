#include "Validation/HLTrigger/interface/HLTGenValHistCollPath.h"

HLTGenValHistCollPath::HLTGenValHistCollPath(std::string objType, std::string triggerPath, HLTConfigProvider& hltConfig, double dR2limit, bool doOnlyLastFilter)
    : triggerPath_(triggerPath), hltConfig_(hltConfig) {

  // as we want a "before" hist before any filter is applied, this dummy is added to the collection
  if(triggerPath == "beforeAnyPath") {
    collection_filter_.emplace_back(HLTGenValHistCollFilter(objType, "beforeAnyFilter", "HLT", dR2limit));
  } else {

    // getting all filters from path
    filters_ = hltConfig_.saveTagsModules(triggerPath_);
    if(doOnlyLastFilter) {
      std::cout << "Wer are doing only one filter!" << std::endl; // debug
      collection_filter_.emplace_back(HLTGenValHistCollFilter(objType, filters_.back(), "HLT", dR2limit));
    } else {
      for (auto & filter : filters_) {
        std::cout << filter << std::endl; // debug to see which filter we found
        collection_filter_.emplace_back(HLTGenValHistCollFilter(objType, filter, "HLT", dR2limit));
      }
    }

  }
}

// hist booking function
// this just calls the booking for each object in the collection_filter
void HLTGenValHistCollPath::bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs, std::vector<edm::ParameterSet>& histConfigs2D) {
  for (auto& collection_filter : collection_filter_) collection_filter.bookHists(iBooker, histConfigs, histConfigs2D);
}

// hist filling function
// this just calls the filling for each object in the collection_filter
void HLTGenValHistCollPath::fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent) {
  for (auto& collection_filter : collection_filter_) collection_filter.fillHists(obj, triggerEvent);
}
