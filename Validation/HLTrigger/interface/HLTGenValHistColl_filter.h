#ifndef DQMOnline_Trigger_HLTGenValHistColl_filter_h
#define DQMOnline_Trigger_HLTGenValHistColl_filter_h

//********************************************************************************
//
// Description:
//   This contains a collection of HLTGenvalHists used to measure the efficiency of a
//   specified filter. It is resonsible for booking and filling the histograms of all vsVars
//   histograms are desired for a specific filter.
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

// class containing a collection of HLTGenValHist for a specific filter
// at object creation time, the object type (used for systematically naming the histogram),
// path and dR2limit (for deltaR matching) need to be specified
// functions for initial booking of hists, and filling of hists for a single object, are available
class HLTGenValHistColl_filter {
public:
  typedef dqm::legacy::MonitorElement MonitorElement;
  typedef dqm::legacy::DQMStore DQMStore;

  explicit HLTGenValHistColl_filter(std::string objType, std::string path, std::string hltprocessname, double dR2limit);

  void bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs, std::vector<edm::ParameterSet>& histConfigs2D);
  void fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent);

private:
  void book1D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig);
  void book2D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig2D);

  std::vector<std::unique_ptr<HLTGenValHist>> hists_; // the collection of histograms
  std::string objType_;
  std::string filter_;
  std::string hltprocessname_;
  double dR2limit_;
};

// constructor
HLTGenValHistColl_filter::HLTGenValHistColl_filter(std::string objType, std::string filter, std::string hltprocessname, double dR2limit)
    : objType_(objType),
      filter_(filter),
      hltprocessname_(hltprocessname),
      dR2limit_(dR2limit) {}

// general hist booking function, receiving configurations for 1D and 2D hists and calling the respective functions
void HLTGenValHistColl_filter::bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs, std::vector<edm::ParameterSet>& histConfigs2D) {
  for (auto& histConfig : histConfigs) book1D(iBooker, histConfig);
  for (auto& histConfig : histConfigs2D) book2D(iBooker, histConfig);
}

// booker function for 1D hists
void HLTGenValHistColl_filter::book1D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig) {
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

  // name and title will be systematically constructed
  std::string histname, histtitle;
  if(filter_ == "beforeAnyFilter") { // this handles the naming of the "before" hist
    histname = objType_ + "_GEN_vs" + vsVar ;
    histtitle = objType_ + " GEN vs " + vsVar;
  } else { // naming of all regular hists
    histname = objType_ + "_" + filter_ + "_vs" + vsVar;
    histtitle = objType_ + "_" + filter_ + "_vs" + vsVar;
  }

  auto me = iBooker.book1D(histname.c_str(), histtitle.c_str(), binLowEdges.size() - 1, &binLowEdges[0]);   // booking MonitorElement

  std::unique_ptr<HLTGenValHist> hist; // creating the hist object

  hist = std::make_unique<HLTGenValHist1D>(me->getTH1(), vsVar, vsVarFunc, rangeCuts);

  hists_.emplace_back(std::move(hist));
}

// booker function for 2D hists
void HLTGenValHistColl_filter::book2D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig2D) {
  // extracting parameters from configuration
  auto vsVar_x = histConfig2D.getParameter<std::string>("vsVar_x");
  auto vsVar_y = histConfig2D.getParameter<std::string>("vsVar_y");
  auto vsVarFunc_x = hltdqm::getUnaryFuncFloat<HLTGenValObject>(vsVar_x);
  auto vsVarFunc_y = hltdqm::getUnaryFuncFloat<HLTGenValObject>(vsVar_y);
  auto binLowEdgesDouble_x = histConfig2D.getParameter<std::vector<double> >("binLowEdges_x");
  auto binLowEdgesDouble_y = histConfig2D.getParameter<std::vector<double> >("binLowEdges_y");

  // checking validity of vsVar
  if (!vsVarFunc_x) {throw cms::Exception("ConfigError") << " vsVar " << vsVar_x << " is giving null ptr (likely empty) in " << __FILE__ << "," << __LINE__ << std::endl;}
  if (!vsVarFunc_y) {throw cms::Exception("ConfigError") << " vsVar " << vsVar_y << " is giving null ptr (likely empty) in " << __FILE__ << "," << __LINE__ << std::endl;}

  // converting bin edges to float
  std::vector<float> binLowEdges_x;
  std::vector<float> binLowEdges_y;
  binLowEdges_x.reserve(binLowEdgesDouble_x.size());
  binLowEdges_y.reserve(binLowEdgesDouble_y.size());
  for (double lowEdge : binLowEdgesDouble_x) binLowEdges_x.push_back(lowEdge);
  for (double lowEdge : binLowEdgesDouble_y) binLowEdges_y.push_back(lowEdge);

  // name and title will be systematically constructed
  std::string histname, histtitle;
  if(filter_ == "beforeAnyFilter") {
    histname = objType_ + "_GEN_2Dvs" + vsVar_x + "_" + vsVar_y;
    histtitle = objType_ + " GEN 2D vs " + vsVar_x + " " + vsVar_y;
  } else {
    histname = objType_ + "_" + filter_ + "_2Dvs" + vsVar_x + vsVar_y;
    histtitle = objType_ + " " + filter_ + " 2D vs" + vsVar_x + " " + vsVar_y;
  }

  auto me = iBooker.book2D(histname.c_str(), histtitle.c_str(), binLowEdges_x.size() - 1, &binLowEdges_x[0], binLowEdges_y.size() - 1, &binLowEdges_y[0]);

  std::unique_ptr<HLTGenValHist> hist;

  hist = std::make_unique<HLTGenValHist2D>(me->getTH2F(), vsVar_x, vsVar_y, vsVarFunc_x, vsVarFunc_y);

  hists_.emplace_back(std::move(hist));
}

// histogram filling routine
void HLTGenValHistColl_filter::fillHists(const HLTGenValObject& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent) {

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

    std::vector<std::string> event_level_variables = {"AK4HT", "AK8HT", "MET"};
    if(std::find(event_level_variables.begin(), event_level_variables.end(), objType_) != event_level_variables.end()) {
      // for these we only require the existence of a trigger object, but no matching
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

#endif
