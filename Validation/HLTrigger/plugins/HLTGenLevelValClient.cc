#include "Validation/HLTrigger/interface/HLTGenLevelValClient.h"

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include "DQMOffline/Trigger/interface/EgHLTTrigTools.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include <boost/algorithm/string.hpp>

#include "TGraphAsymmErrors.h"

HLTGenLevelValClient::HLTGenLevelValClient(const edm::ParameterSet& iConfig) : isSetup_(false) {
  filterNames_ = iConfig.getParameter<std::vector<std::string> >("filterNames");

  effVars_ = iConfig.getParameter<std::vector<std::string> >("effVars");

  runClientEndLumiBlock_ = iConfig.getParameter<bool>("runClientEndLumiBlock");
  runClientEndRun_ = iConfig.getParameter<bool>("runClientEndRun");
  runClientEndJob_ = iConfig.getParameter<bool>("runClientEndJob");

  dirName_ = iConfig.getParameter<std::string>("DQMDirName");

  hltTag_ = iConfig.getParameter<std::string>("hltTag");
}

HLTGenLevelValClient::~HLTGenLevelValClient() = default;

void HLTGenLevelValClient::dqmEndJob(DQMStore::IBooker& ibooker_, DQMStore::IGetter& igetter_) {
  if (runClientEndJob_)
    runClient_(ibooker_, igetter_);
}

void HLTGenLevelValClient::beginRun(const edm::Run& run, const edm::EventSetup& c) {
  if (!isSetup_) isSetup_ = true; // very interesting
}

void HLTGenLevelValClient::dqmEndLuminosityBlock(DQMStore::IBooker& ibooker_,
                                               DQMStore::IGetter& igetter_,
                                               edm::LuminosityBlock const& iLumi,
                                               edm::EventSetup const& iSetup) {
  if (runClientEndLumiBlock_)
    runClient_(ibooker_, igetter_);
}

void HLTGenLevelValClient::runClient_(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter) {
  ibooker.setCurrentFolder(dirName_ + "/Client_Histos");

  std::vector<std::string> regions;
  regions.emplace_back("eb");
  regions.emplace_back("ee");

  for (auto const& filterName : filterNames_) {
    //std::cout<<"FilterName: "<<eleHLTFilterNames_[filterNr]<<std::endl;
      for (auto const& region : regions) {
        ibooker.setCurrentFolder(dirName_ + "/Client_Histos/" + filterName);
        createEffHists(filterName, filterName + "_gsfEle", region, effVars_, ibooker, igetter);
    }

    //----Morse-----
    ibooker.setCurrentFolder(dirName_);
    //----------

  }
}

void HLTGenLevelValClient::createEffHists(const std::string& filterName,
                                              const std::string& baseName,
                                              const std::string& region,
                                              const std::vector<std::string>& varNames,
                                              DQMStore::IBooker& ibooker,
                                              DQMStore::IGetter& igetter) {
  MonitorElement* denom = igetter.get(dirName_ + "/Source_Histos/" + filterName + "/" + baseName + "_noCuts_" + region); // get the denominator hist here!

  for (auto const& varName : varNames) {
    MonitorElement* numer = igetter.get(dirName_ + "/Source_Histos/" + filterName + "/" + baseName + "_single_" + varName + "_" + region); // get numerator hist here

    if (numer != nullptr && denom != nullptr) { // checking if both are filled
      std::string effHistName(baseName + "_singleEff_" + varName + "_" + region);
      std::string effHistTitle(effHistName);
      if (region == "eb" || region == "ee") {
        if (region == "eb")
          effHistTitle = "Barrel " + baseName + " SingleEff " + varName;
        if (region == "ee")
          effHistTitle = "Endcap " + baseName + " SingleEff " + varName;
      }  //std::cout<<effHistTitle<<std::endl;
      makeEffMonElemFromPassAndAll(filterName, effHistName, effHistTitle, numer, denom, ibooker, igetter);
      //--------------------
    }
  }  //end loop over varNames
}

//-----Morse-------
HLTGenLevelValClient::MonitorElement* HLTGenLevelValClient::makeEffMonElemFromPassAndAll(const std::string& filterName,
                                                                                     const std::string& name,
                                                                                     const std::string& title,
                                                                                     const MonitorElement* pass,
                                                                                     const MonitorElement* all,
                                                                                     DQMStore::IBooker& ibooker,
                                                                                     DQMStore::IGetter& igetter) {
  TH1F* passHist = pass->getTH1F(); // getting nominator hist
  if (passHist->GetSumw2N() == 0) passHist->Sumw2();

  TH1F* allHist = all->getTH1F(); // getting denominator hist
  if (allHist->GetSumw2N() == 0) allHist->Sumw2();

  auto* effHist = (TH1F*)passHist->Clone(name.c_str()); // calculating efficiency
  effHist->Divide(passHist, allHist, 1, 1, "B");

  //----Morse---------
  effHist->SetTitle(title.c_str());
  //------------------
  MonitorElement* eff = igetter.get(dirName_ + "/Client_Histos/" + filterName + "/" + name);
  if (eff == nullptr) {
    eff = ibooker.book1D(name, effHist);
  } else {  // I was having problems with collating the histograms, hence why I'm just resetting the histogram value // TODO FINN understand this
    *eff->getTH1F() = *effHist;
    delete effHist;
  }
  return eff;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HLTGenLevelValClient);
