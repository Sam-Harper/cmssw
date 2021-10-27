#ifndef DQMOnline_Trigger_HLTGenValHistColl_h
#define DQMOnline_Trigger_HLTGenValHistColl_h

//********************************************************************************
//
// Description:
//   This contains a collection of HLTGenvalHists used to measure the efficiency of a
//   specified filter. It is resonsible for booking and filling the histograms.
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

using namespace reco;

class HLTGenValHistColl {
public:
  typedef dqm::legacy::MonitorElement MonitorElement;
  typedef dqm::legacy::DQMStore DQMStore;

  explicit HLTGenValHistColl(std::string objType, edm::ParameterSet& config);

  static edm::ParameterSetDescription makePSetDescription();
  static edm::ParameterSetDescription makePSetDescriptionHistConfigs();

  void bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs);
  void fillHists(const GenParticle& obj,
                 const edm::Event& event,
                 const edm::EventSetup& setup);

private:
  void book1D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig);

private:
  std::vector<std::unique_ptr<HLTGenValHist> > hists_;
  std::string objType_;
  std::string filterName_;
};

HLTGenValHistColl::HLTGenValHistColl(std::string objType, edm::ParameterSet& config)
    : objType_(objType),
      filterName_(config.getParameter<std::string>("filterName")) {}

edm::ParameterSetDescription HLTGenValHistColl::makePSetDescription() {
  // TODO
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  return desc;
}

edm::ParameterSetDescription HLTGenValHistColl::makePSetDescriptionHistConfigs() {
  // TODO
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  return desc;
}

void HLTGenValHistColl::bookHists(DQMStore::IBooker& iBooker, std::vector<edm::ParameterSet>& histConfigs) {
  for (auto& histConfig : histConfigs) book1D(iBooker, histConfig);
}

void HLTGenValHistColl::book1D(DQMStore::IBooker& iBooker, edm::ParameterSet& histConfig) {
  auto binLowEdgesDouble = histConfig.getParameter<std::vector<double> >("binLowEdges");
  std::vector<float> binLowEdges;
  binLowEdges.reserve(binLowEdgesDouble.size());
  for (double lowEdge : binLowEdgesDouble) binLowEdges.push_back(lowEdge);
  auto vsVar = histConfig.getParameter<std::string>("vsVar");

  auto me = iBooker.book1D((objType_ + "_" + filterName_ + "_vs" + vsVar + "_before").c_str(),
                               (objType_ + " " + filterName_ + "_vs" + vsVar +" before").c_str(),
                               binLowEdges.size() - 1,
                               &binLowEdges[0]);

  std::unique_ptr<HLTGenValHist> hist;
  auto vsVarFunc = hltdqm::getUnaryFuncFloat<GenParticle>(vsVar);
  if (!vsVarFunc) {
    throw cms::Exception("ConfigError") << " vsVar " << vsVar << " is giving null ptr (likely empty) in " << __FILE__
                                        << "," << __LINE__ << std::endl;
  }
  hist = std::make_unique<HLTGenValHist1D>(me->getTH1(), vsVar, vsVarFunc);
  hists_.emplace_back(std::move(hist));
}

void HLTGenValHistColl::fillHists(const GenParticle& obj,
                                              const edm::Event& event,
                                              const edm::EventSetup& setup) {
  for (auto& hist : hists_) {
    hist->fill(obj, event, setup);
  }
}

#endif
