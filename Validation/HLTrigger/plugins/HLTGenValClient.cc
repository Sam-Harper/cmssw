// -*- C++ -*-
//
// Package:    Validation/HLTrigger
// Class:      HLTGenValClient
//
/**\class HLTGenValClient HLTGenValClient.cc Validation/HLTrigger/plugins/HLTGenValClient.cc

 Description: [one line class summary]

 Implementation:
     Getting all MEs in folder. For each of them, calculate the needed efficiency hists and store them in the *same* path where the source hists were found!
*/
//
// Original Author:  Finn Jonathan Labe
//         Created:  Fri, 05 Nov 2021 11:51:30 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

// includes needed for histogram creation
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// DQMStore
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"

// trigger Event
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

using namespace reco;

class HLTGenValClient : public DQMEDHarvester {
public:

  explicit HLTGenValClient(const edm::ParameterSet&);
  ~HLTGenValClient() override = default;
  HLTGenValClient(const HLTGenValClient&) = delete;

  void dqmEndJob(DQMStore::IBooker&, DQMStore::IGetter&) override;  //performed in the endJob
  void beginRun(const edm::Run&, const edm::EventSetup&) override;

  void makeEfficiencyPlots(DQMStore::IBooker&, DQMStore::IGetter&, std::string, std::string);
  std::vector<std::string> moduleLabels(const std::string&);

private:

  // ----------member data ---------------------------

  // config strings
  std::string objType_;
  std::string inputDirName_;
  std::string outputDirName_;

  std::vector<edm::ParameterSet> histConfigs_;

  std::vector<std::string> hltPathsToCheck_;
  HLTConfigProvider hltConfig_;
  std::set<std::string> hltPaths;
  std::string hltProcessName_;


};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HLTGenValClient::HLTGenValClient(const edm::ParameterSet& iConfig) {

  inputDirName_ = iConfig.getParameter<std::string>("InputDirName");
  outputDirName_ = iConfig.getParameter<std::string>("OutputDirName");
  objType_ = iConfig.getParameter<std::string>("objType");

  histConfigs_ = iConfig.getParameterSetVector("histConfigs");

  hltPathsToCheck_ = iConfig.getParameter<std::vector<std::string>>("hltPathsToCheck");

  hltProcessName_ = iConfig.getParameter<std::string>("hltProcessName");


}

void HLTGenValClient::beginRun(const edm::Run &iRun, const edm::EventSetup &iSetup) {

  // initializing the hltConfig
  bool changedConfig;
  if (!hltConfig_.init(iRun, iSetup, hltProcessName_, changedConfig)) {
    edm::LogError("HLTGenVal") << "Initialization of HLTConfigProvider failed!!";
    return;
  }

}

void HLTGenValClient::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter) {

  // Get the set of trigger paths we want to make plots for
  // not quite sure why this is needed here -> will be removed if auto-determining paths!
  for (auto const &i : hltPathsToCheck_) {
    for (auto const &j : hltConfig_.triggerNames()) {
      if (j.find(i) != std::string::npos) {
        hltPaths.insert(j);
      }
    }
  } // should maybe add error handling for typos in paths...


  for (auto const& hltPath : hltPaths) {
    auto filters_ = moduleLabels(hltPath);


    // main loop over all filters
    for (auto & filter : filters_) {
      makeEfficiencyPlots(ibooker, igetter, hltPath, filter);
    }

  }

}

// Returns list of filters by path
// some filters are not taken -> TODO ask why!
std::vector<std::string> HLTGenValClient::moduleLabels(const std::string &path) {
  std::vector<std::string> modules = hltConfig_.moduleLabels(path);
  auto iter = modules.begin();
  while (iter != modules.end()) {
    if ((iter->find("Filtered") == std::string::npos) && (iter->find("hltL1s") == std::string::npos)) {
      iter = modules.erase(iter);
    } else if (iter->find("L1Filtered0") != std::string::npos)
      iter = modules.erase(iter);
    else
      ++iter;
  }

  return modules;
}

void HLTGenValClient::makeEfficiencyPlots(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter, std::string path, std::string filter) {

  // loop over vsVars
  for (auto& histConfig : histConfigs_) {
    auto vsVar = histConfig.getParameter<std::string>("vsVar");

    auto contents = igetter.getAllContents("*");
    for (auto & content : contents) {
      std::cout << content->getName() << std::endl;
    }

    std::cout << "<<<<<<<<<<<<" << std::endl;

    // getting the input hists/MEs
    igetter.setCurrentFolder(inputDirName_+"/"+objType_+"/"+path);
    std::cout << "PWD: " << igetter.pwd() << std::endl;
    MonitorElement* denom = igetter.getElement(objType_ + "_vs" + vsVar + "_before");
    MonitorElement* numer = igetter.getElement(objType_ + "_" + filter + "_vs" + vsVar + "_after");

    if (numer != nullptr && denom != nullptr) {
      std::cout << "Now we should store something..." << std::endl;
      std::string effHistName(objType_ + "_" + filter + "_vs" + vsVar + "_pass");
      std::string effHistTitle(objType_ + " " + filter + " vs " + vsVar + " pass");

      TH1F* beforeHist = denom->getTH1F();
      if (beforeHist->GetSumw2N() == 0) beforeHist->Sumw2();
      TH1F* afterHist = numer->getTH1F();
      if (afterHist->GetSumw2N() == 0) afterHist->Sumw2();

      auto* effHist = (TH1F*) afterHist->Clone();
      effHist->Divide(afterHist, beforeHist, 1, 1, "B");
      effHist->SetTitle(effHistTitle.c_str());

      ibooker.setCurrentFolder(outputDirName_ + "/" + objType_ + "/" + path);
      ibooker.book1D(effHistName.c_str(), effHist);

    }
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(HLTGenValClient);
