#ifndef HLTGenLevelHists_h
#define HLTGenLevelHists_h

//********************************************************************************
//
// Description:
//   This contains a collection of HLTDQMHists used to measure the efficiency of a
//   specified filter. It is resonsible for booking and filling the histograms.
//
// Author : Finn Labe, Sep 2021
// Also shamelessly stolen from Sam
//
//***********************************************************************************

#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Validation/HLTrigger/interface/HLTValidationHist.h"
#include "Validation/HLTrigger/interface/FIXTHISFUNCTION.h"
#include "DQMOffline/Trigger/interface/UtilFuncs.h"

#include <string>

template <typename ObjType>
class HLTGenLevelHists {
public:

  typedef dqm::legacy::DQMStore DQMStore;

  explicit HLTGenLevelHists(const edm::ParameterSet& config, std::string baseHistName, std::string hltProcess);

  static edm::ParameterSetDescription makePSetDescription();
  static edm::ParameterSetDescription makePSetDescriptionHistConfigs();

  void bookHists(DQMStore::IBooker& iBooker, const std::vector<edm::ParameterSet>& histConfigs);
  void fillHists(const ObjType& obj,
                 const edm::Event& event,
                 const edm::EventSetup& setup,
                 const trigger::TriggerEvent& trigEvt);

private:
  // do 1D hists as an example, may extend later
  void book1D(DQMStore::IBooker& iBooker, const edm::ParameterSet& histConfig);

private:
  std::vector<std::unique_ptr<HLTValidationHist<ObjType> > > hists_;
  std::string filterName_;
  std::string histTitle_;
  std::string folderName_;
  std::string baseHistName_;
  std::string hltProcess_;
};

// constructor
template <typename ObjType>
HLTGenLevelHists<ObjType>::HLTGenLevelHists(const edm::ParameterSet& config,
                                                    std::string baseHistName,
                                                    std::string hltProcess)
    : filterName_(config.getParameter<std::string>("filterName")),
      histTitle_(config.getParameter<std::string>("histTitle")),
      folderName_(config.getParameter<std::string>("folderName")),
      baseHistName_(std::move(baseHistName)),
      hltProcess_(std::move(hltProcess)) {}


// not 100% sure what these do yet...
template <typename ObjType>
edm::ParameterSetDescription HLTGenLevelHists<ObjType>::makePSetDescription() {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("filterName", "");
  desc.add<std::string>("histTitle", "");
  desc.add<std::string>("folderName", "");
  return desc;
}

template <typename ObjType>
edm::ParameterSetDescription HLTGenLevelHists<ObjType>::makePSetDescriptionHistConfigs() {
  edm::ParameterSetDescription desc;

  //what this is doing is trival and is left as an exercise to the reader. HAHA very funny :D
  auto histDescCases =
      "1D" >> (edm::ParameterDescription<std::vector<double> >("binLowEdges", std::vector<double>(), true) and
               edm::ParameterDescription<std::string>("nameSuffex", "", true) and
               edm::ParameterDescription<std::string>("vsVar", "", true));

  desc.ifValue(edm::ParameterDescription<std::string>("histType", "1D", true), std::move(histDescCases));
  return desc;
}

// booking all histograms
// using book function shown below
template <typename ObjType>
void HLTGenLevelHists<ObjType>::bookHists(DQMStore::IBooker& iBooker,
                                              const std::vector<edm::ParameterSet>& histConfigs) {
  iBooker.setCurrentFolder(folderName_);
  for (auto& histConfig : histConfigs) {
    std::string histType = histConfig.getParameter<std::string>("histType");
    if (histType == "1D") {
      book1D(iBooker, histConfig);
    } else {
      throw cms::Exception("ConfigError") << " histType " << histType << " not recognised" << std::endl;
    }
  }
}

// 1D booking function
template <typename ObjType>
void HLTGenLevelHists<ObjType>::book1D(DQMStore::IBooker& iBooker, const edm::ParameterSet& histConfig) {
  auto binLowEdgesDouble = histConfig.getParameter<std::vector<double> >("binLowEdges");
  std::vector<float> binLowEdges;
  binLowEdges.reserve(binLowEdgesDouble.size());
  for (double lowEdge : binLowEdgesDouble)
    binLowEdges.push_back(lowEdge);

  auto vsVar = histConfig.getParameter<std::string>("vsVar");
  auto vsVarFunc = hltdqm::getUnaryFuncFloat<ObjType>(vsVar); // what does this do?
  if (!vsVarFunc) {
    throw cms::Exception("ConfigError") << " vsVar " << vsVar << " is giving null ptr (likely empty) in " << __FILE__
                                        << "," << __LINE__ << std::endl;
  }

  auto me = iBooker.book1D((baseHistName_ + filterName_ + "_tot").c_str(),
                              (histTitle_ + " Total").c_str(),
                              binLowEdges.size() - 1,
                              &binLowEdges[0]);

  std::unique_ptr<HLTValidationHist<ObjType> > hist;
  hist = std::make_unique<HLTValidationHist1D<ObjType, float> >(me->getTH1(), vsVar, vsVarFunc);
  hists_.emplace_back(std::move(hist));
}

// filling the hists
template <typename ObjType>
void HLTGenLevelHists<ObjType>::fillHists(const ObjType& obj,
                                              const edm::Event& event,
                                              const edm::EventSetup& setup,
                                              const trigger::TriggerEvent& trigEvt) {
  for (auto& hist : hists_) {
    hist->fill(obj, event, setup);
  }

}
#endif
