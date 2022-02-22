#include "Validation/HLTrigger/interface/HLTGenValHistCollFilter.h"

// constructor
HLTGenValHistCollFilter::HLTGenValHistCollFilter(edm::ParameterSet filterCollConfig) {
  objType_ = filterCollConfig.getParameter<std::string>("objType");
  filter_ = filterCollConfig.getParameter<std::string>("filterName");
  hltprocessname_ = filterCollConfig.getParameter<std::string>("hltProcessName");
  dR2limit_ = filterCollConfig.getParameter<double>("dR2limit");
}

edm::ParameterSetDescription HLTGenValHistCollFilter::makePSetDescription() {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("objType", "");
  desc.add<std::string>("hltProcessName", "HLT");
  desc.add<double>("dR2limit", 0.1);
  desc.add<std::string>("filterName", "");
  return desc;
}

// general hist booking function, receiving configurations for 1D and 2D hists and calling the respective functions
void HLTGenValHistCollFilter::bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs, std::vector<edm::ParameterSet>& histConfigs2D) {
  for (auto& histConfig : histConfigs) book1D(iBooker, histConfig);
  for (auto& histConfig : histConfigs2D) book2D(iBooker, histConfig);
}

// histogram filling routine
void HLTGenValHistCollFilter::fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent) {

  // this handles the "before" step, denoted by a "dummy" filter called "beforeAnyFilter"
  // the histogram is filled without any additional requirements for all objects
  if(filter_ == "beforeAnyFilter") {
    for (auto& hist : hists_) hist->fill(obj);
  } else {
    // main filling code

    // get filter object from filter name
    edm::InputTag filterTag(filter_, "", hltprocessname_);
    size_t filterIndex = triggerEvent->filterIndex(filterTag);

    // get trigger objects passing filter in question
    trigger::TriggerObjectCollection allTriggerObjects = triggerEvent->getObjects(); // all objects
    trigger::TriggerObjectCollection selectedObjects; // vector to fill with objects passing our filter
    if (filterIndex < triggerEvent->sizeFilters()) {
      const auto& keys = triggerEvent->filterKeys(filterIndex);
      for (unsigned short key : keys) {
        trigger::TriggerObject foundObject = allTriggerObjects[key];
        selectedObjects.push_back(foundObject);
      }
    }

    // differentiate between event level and particle level variables
    std::vector<std::string> event_level_variables = {"AK4HT", "AK8HT", "MET"};
    if(std::find(event_level_variables.begin(), event_level_variables.end(), objType_) != event_level_variables.end()) {
      // for these event level variables we only require the existence of a trigger object, but no matching
      if(selectedObjects.size() > 0) for (auto& hist : hists_) hist->fill(obj);
    } else {
      // do a deltaR matching between trigger object and GEN object
      double mindR2 = 99999;
      for (const auto & filterobj : selectedObjects) {
        double dR = deltaR2(obj, filterobj);
        if(dR < mindR2) mindR2 = dR;
      }
      if(mindR2 < dR2limit_) for (auto& hist : hists_) hist->fill(obj);
    }

  }
}

// booker function for 1D hists
void HLTGenValHistCollFilter::book1D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig) {
  // extracting parameters from configuration
  auto vsVar = histConfig.getParameter<std::string>("vsVar");
  auto vsVarFunc = hltdqm::getUnaryFuncFloat<HLTGenValObject>(vsVar);
  auto binLowEdgesDouble = histConfig.getParameter<std::vector<double> >("binLowEdges");
  VarRangeCutColl<HLTGenValObject> rangeCuts(histConfig.getParameter<std::vector<edm::ParameterSet> >("rangeCuts"));

  // checking validity of vsVar
  if (!vsVarFunc) { throw cms::Exception("ConfigError") << " vsVar " << vsVar << " is giving null ptr (likely empty) in " << __FILE__ << "," << __LINE__ << std::endl;}

  // converting bin edges to float
  std::vector<float> binLowEdges;
  binLowEdges.reserve(binLowEdgesDouble.size());
  for (double lowEdge : binLowEdgesDouble) binLowEdges.push_back(lowEdge);

  // name and title are systematically constructed

  // remove potential leading "-" (which denotes that that trigger is ignored)
  std::string filtername = filter_;
  if(filtername.rfind("-", 0) == 0) filtername.erase(0, 1);

  std::string histname, histtitle;
  if(filter_ == "beforeAnyFilter") { // this handles the naming of the "before" hist
    histname = objType_ + "_GEN_vs" + vsVar ;
    histtitle = objType_ + " GEN vs " + vsVar;
  } else { // naming of all regular hists
    histname = objType_ + "_" + filtername + "_vs" + vsVar;
    histtitle = objType_ + "_" + filtername + "_vs" + vsVar;
  }

  auto me = iBooker.book1D(histname.c_str(), histtitle.c_str(), binLowEdges.size() - 1, &binLowEdges[0]);   // booking MonitorElement

  std::unique_ptr<HLTGenValHist> hist; // creating the hist object

  hist = std::make_unique<HLTGenValHist1D>(me->getTH1(), vsVar, vsVarFunc, rangeCuts);

  hists_.emplace_back(std::move(hist));
}

// booker function for 2D hists
void HLTGenValHistCollFilter::book2D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig2D) {
  // extracting parameters from configuration
  auto vsVarX_ = histConfig2D.getParameter<std::string>("vsVarX");
  auto vsVarY_ = histConfig2D.getParameter<std::string>("vsVarY");
  auto vsVarFunc_x = hltdqm::getUnaryFuncFloat<HLTGenValObject>(vsVarX_);
  auto vsVarFunc_y = hltdqm::getUnaryFuncFloat<HLTGenValObject>(vsVarY_);
  auto binLowEdgesDoubleX = histConfig2D.getParameter<std::vector<double> >("binLowEdgesX");
  auto binLowEdgesDoubleY = histConfig2D.getParameter<std::vector<double> >("binLowEdgesY");

  // checking validity of vsVar
  if (!vsVarFunc_x) {throw cms::Exception("ConfigError") << " vsVar " << vsVarX_ << " is giving null ptr (likely empty) in " << __FILE__ << "," << __LINE__ << std::endl;}
  if (!vsVarFunc_y) {throw cms::Exception("ConfigError") << " vsVar " << vsVarY_ << " is giving null ptr (likely empty) in " << __FILE__ << "," << __LINE__ << std::endl;}

  // converting bin edges to float
  std::vector<float> binLowEdgesX;
  std::vector<float> binLowEdgesY;
  binLowEdgesX.reserve(binLowEdgesDoubleX.size());
  binLowEdgesY.reserve(binLowEdgesDoubleY.size());
  for (double lowEdge : binLowEdgesDoubleX) binLowEdgesX.push_back(lowEdge);
  for (double lowEdge : binLowEdgesDoubleY) binLowEdgesY.push_back(lowEdge);

  // name and title are systematically constructed

  // remove potential leading "-" (which denotes that that trigger is ignored)
  std::string filtername = filter_;
  if(filtername.rfind("-", 0) == 0) filtername.erase(0, 1);

  std::string histname, histtitle;
  if(filter_ == "beforeAnyFilter") {
    histname = objType_ + "_GEN_2Dvs" + vsVarX_ + "_" + vsVarY_;
    histtitle = objType_ + " GEN 2D vs " + vsVarX_ + " " + vsVarY_;
  } else {
    histname = objType_ + "_" + filtername + "_2Dvs" + vsVarX_ + vsVarY_;
    histtitle = objType_ + " " + filtername + " 2D vs" + vsVarX_ + " " + vsVarY_;
  }

  auto me = iBooker.book2D(histname.c_str(), histtitle.c_str(), binLowEdgesX.size() - 1, &binLowEdgesX[0], binLowEdgesY.size() - 1, &binLowEdgesY[0]);

  std::unique_ptr<HLTGenValHist> hist;

  hist = std::make_unique<HLTGenValHist2D>(me->getTH2F(), vsVarX_, vsVarY_, vsVarFunc_x, vsVarFunc_y);

  hists_.emplace_back(std::move(hist));
}
