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

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

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
  void fillHists(const GenParticle& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent);

  std::vector<std::string> moduleLabels(const std::string &);

  template <class T1, class T2>
  std::vector<size_t> matchByDeltaR(const std::vector<T1>&, const std::vector<T2>&, const double);

private:
  void book1D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig);

private:
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
    histname = objType_ + "_" + filter_ + "_vs" + vsVar + "_pass";
    histtitle = objType_ + " " + filter_ + " vs " + vsVar +" pass";
  }

  auto me = iBooker.book1D(histname.c_str(), histtitle.c_str(), binLowEdges.size() - 1, &binLowEdges[0]);

  std::unique_ptr<HLTGenValHist> hist;
  auto vsVarFunc = hltdqm::getUnaryFuncFloat<GenParticle>(vsVar);
  if (!vsVarFunc) {
    throw cms::Exception("ConfigError") << " vsVar " << vsVar << " is giving null ptr (likely empty) in " << __FILE__
                                        << "," << __LINE__ << std::endl;
  }
  hist = std::make_unique<HLTGenValHist1D>(me->getTH1(), vsVar, vsVarFunc);
  hists_.emplace_back(std::move(hist));
}

void HLTGenValHistColl_filter::fillHists(const GenParticle& obj, edm::Handle<trigger::TriggerEvent>& triggerEvent) {
  if(filter_ == "beforeAnyFilter") {
    for (auto& hist : hists_) hist->fill(obj);
  } else {
    // do matching here?
    edm::InputTag filterTag(filter_, "", "HLT"); // TODO replace the third argument hre
    size_t filterIndex = triggerEvent->filterIndex(filterTag);

    // get matching objects
    trigger::TriggerObjectCollection allTriggerObjects = triggerEvent->getObjects();
    trigger::TriggerObjectCollection selectedObjects;
    if (filterIndex < triggerEvent->sizeFilters()) {
      const auto& keys = triggerEvent->filterKeys(filterIndex);
      for (unsigned short key : keys) {
        trigger::TriggerObject foundObject = allTriggerObjects[key];
        selectedObjects.push_back(foundObject);
      }
    }

    // do the matching
    double mindR2 = 999;
    for (const auto & filterobj : selectedObjects) {
      double dR = deltaR2(obj, filterobj);
      if(dR < mindR2) mindR2 = dR;
    }

    // decide if pass
    double dR2limit = 0.1;
    if(mindR2 < dR2limit) for (auto& hist : hists_) hist->fill(obj);

  }
}

// A generic method to find the best deltaR matches from 2 collections.
// stolen from HLTMuonMatchAndPlot.cc
// should put somewhere else along other helper functions
template <class T1, class T2>
std::vector<size_t> HLTGenValHistColl_filter::matchByDeltaR(const std::vector<T1>& collection1,
                                                  const std::vector<T2>& collection2,
                                                  const double maxDeltaR) {
  const double NOMATCH = 999.;

  const size_t n1 = collection1.size();
  const size_t n2 = collection2.size();

  std::vector<size_t> result(n1, -1);
  std::vector<std::vector<double> > deltaRMatrix(n1, std::vector<double>(n2, NOMATCH));

  for (size_t i = 0; i < n1; i++)
    for (size_t j = 0; j < n2; j++) {
      deltaRMatrix[i][j] = deltaR(collection1[i], collection2[j]);
    }

  // Run through the matrix n1 times to make sure we've found all matches.
  for (size_t k = 0; k < n1; k++) {
    size_t i_min = -1;
    size_t j_min = -1;
    double minDeltaR = maxDeltaR;
    // find the smallest deltaR
    for (size_t i = 0; i < n1; i++)
      for (size_t j = 0; j < n2; j++)
        if (deltaRMatrix[i][j] < minDeltaR) {
          i_min = i;
          j_min = j;
          minDeltaR = deltaRMatrix[i][j];
        }
    // If a match has been made, save it and make those candidates unavailable.
    if (minDeltaR < maxDeltaR) {
      result[i_min] = j_min;
      deltaRMatrix[i_min] = std::vector<double>(n2, NOMATCH);
      for (size_t i = 0; i < n1; i++)
        deltaRMatrix[i][j_min] = NOMATCH;
    }
  }

  return result;
}

#endif
