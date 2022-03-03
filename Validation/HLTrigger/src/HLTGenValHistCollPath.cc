#include "Validation/HLTrigger/interface/HLTGenValHistCollPath.h"

// constructor
HLTGenValHistCollPath::HLTGenValHistCollPath(edm::ParameterSet pathCollConfig, HLTConfigProvider& hltConfig) :
  hltConfig_(hltConfig) {

  triggerPath_ = pathCollConfig.getParameter<std::string>("triggerPath");
  doOnlyLastFilter_ = pathCollConfig.getParameter<bool>("doOnlyLastFilter");

  // before creating the collections for each filter, we'll store the needed configurations in a pset
  // we'll copy this basis multiple times and add the respective path later
  edm::ParameterSet filterCollConfig;
  filterCollConfig.addParameter<std::string>("objType", pathCollConfig.getParameter<std::string>("objType"));
  filterCollConfig.addParameter<std::string>("tag", pathCollConfig.getParameter<std::string>("tag"));
  filterCollConfig.addParameter<std::string>("hltProcessName", pathCollConfig.getParameter<std::string>("hltProcessName"));
  filterCollConfig.addParameter<double>("dR2limit", pathCollConfig.getParameter<double>("dR2limit"));

  pathString_ = "";

  // as we want a "before" hist before any filter is applied, this dummy is added to the collection
  if(triggerPath_ == "beforeAnyPath") {
    edm::ParameterSet filterCollConfigBeforeAnyPath = filterCollConfig;
    filterCollConfigBeforeAnyPath.addParameter<std::string>("filterName", "beforeAnyFilter");
    collectionFilter_.emplace_back(HLTGenValHistCollFilter(filterCollConfigBeforeAnyPath));
  } else {

    // getting all filters from path
    filters_ = hltConfig_.saveTagsModules(triggerPath_);
    if(doOnlyLastFilter_) {
      edm::ParameterSet filterCollConfigOnlyLastFilter = filterCollConfig;
      filterCollConfigOnlyLastFilter.addParameter<std::string>("filterName", filters_.back());
      collectionFilter_.emplace_back(HLTGenValHistCollFilter(filterCollConfigOnlyLastFilter));
      pathString_ += filters_.back();
    } else {
      for (auto & filter : filters_) {
        edm::ParameterSet filterCollConfigStep = filterCollConfig;
        filterCollConfigStep.addParameter<std::string>("filterName", filter);
        collectionFilter_.emplace_back(HLTGenValHistCollFilter(filterCollConfigStep));
        pathString_ += filters_.back();
        if(filter != filters_.back()) pathString_ += ";";
      }
    }

  }

}

edm::ParameterSetDescription HLTGenValHistCollPath::makePSetDescription() {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("objType", "");
  desc.add<std::string>("tag", "");
  desc.add<double>("dR2limit", 0.1);
  desc.add<bool>("doOnlyLastFilter", false);
  desc.add<std::string>("hltProcessName", "HLT");
  desc.add<std::string>("triggerPath", "");
  return desc;
}

// hist booking function
// this just calls the booking for each object in the the filter collection
void HLTGenValHistCollPath::bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs, std::vector<edm::ParameterSet>& histConfigs2D) {

  if(pathString_ != "") iBooker.bookString("path_"+triggerPath_, pathString_);

  for (auto& collection_filter : collectionFilter_) collection_filter.bookHists(iBooker, histConfigs, histConfigs2D);
}

// hist filling function
// this just calls the filling for each object in the filter collection
void HLTGenValHistCollPath::fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent) {
  for (auto& collection_filter : collectionFilter_) collection_filter.fillHists(obj, triggerEvent);
}
