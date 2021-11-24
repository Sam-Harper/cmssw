// -*- C++ -*-
//
// Package:    Validation/HLTrigger
// Class:      HLTGenValSource
//
/**\class HLTGenValSource HLTGenValSource.cc Validation/HLTrigger/plugins/HLTGenValSource.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Finn Jonathan Labe
//         Created:  Thu, 21 Oct 2021 11:51:30 GMT
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

// including GenParticles
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

// includes needed for histogram creation
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// FunctionDefs
#include "DQMOffline/Trigger/interface/FunctionDefs.h"

// includes of histogram collection class
#include "Validation/HLTrigger/interface/HLTGenValHistColl_path.h"

// DQMStore
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

// trigger Event
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

// object that can be a GenJet, GenParticle or energy sum
#include "Validation/HLTrigger/interface/HLTGenValObject.h"


//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

using namespace reco;

class HLTGenValSource : public DQMEDAnalyzer {
public:

  explicit HLTGenValSource(const edm::ParameterSet&);
  ~HLTGenValSource() override = default;
  HLTGenValSource(const HLTGenValSource&) = delete;
  HLTGenValSource& operator=(const HLTGenValSource&) = delete;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const& run, edm::EventSetup const& c) override;
  void dqmBeginRun(const edm::Run &, const edm::EventSetup &) override;
  void fillObjectCollection(const edm::Event&);

  // ----------member data ---------------------------

  // tokens to get collections
  const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
  const edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
  const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;

  // config strings/Psets
  std::string objType_;
  std::string dirName_;
  std::vector<edm::ParameterSet> histConfigs_;
  std::string hltProcessName_;

  // histogram colelction
  std::vector<HLTGenValHistColl_path> collection_path_;

  // HLT config provider/getter
  HLTConfigProvider hltConfig_;
  std::vector<std::string> hltPathsToCheck_;
  std::set<std::string> hltPaths;

  std::vector<HLTGenValObject> objects_;

};

HLTGenValSource::HLTGenValSource(const edm::ParameterSet& iConfig)
    : genParticleToken_( consumes<reco::GenParticleCollection>( iConfig.getParameterSet("inputCollections").getParameter<edm::InputTag>("genParticles") ) ),
      genJetToken_(consumes<reco::GenJetCollection>( iConfig.getParameterSet("inputCollections").getParameter<edm::InputTag>("ak4GenJets"))),
      trigEventToken_(consumes<trigger::TriggerEvent>( iConfig.getParameterSet("inputCollections").getParameter<edm::InputTag>("TrigEvent") )) {

      histConfigs_ = iConfig.getParameterSetVector("histConfigs");

      dirName_ = iConfig.getParameter<std::string>("DQMDirName");
      objType_ = iConfig.getParameter<std::string>("objType");
      hltProcessName_ = iConfig.getParameter<std::string>("hltProcessName");

      hltPathsToCheck_ = iConfig.getParameter<std::vector<std::string>>("hltPathsToCheck");

      std::cout << "Started job for " << objType_ << std::endl;

}

void HLTGenValSource::dqmBeginRun(const edm::Run &iRun, const edm::EventSetup &iSetup) {

  // Initialize hltConfig
  // for cross-checking whether paths make sense
  // later this could be used to determine all paths that relevant for certain GEN object
  bool changedConfig;
  if (!hltConfig_.init(iRun, iSetup, hltProcessName_, changedConfig)) {
    edm::LogError("HLTGenVal") << "Initialization of HLTConfigProvider failed!!";
    return;
  }

  // Get the set of trigger paths we want to make plots for
  // not quite sure why this is needed here -> will be removed if auto-determining paths!
  std::vector<std::string> notFoundPaths;
  for (auto const &i : hltPathsToCheck_) {
    bool pathfound = false;
    for (auto const &j : hltConfig_.triggerNames()) {
      if (j.find(i) != std::string::npos) {
        hltPaths.insert(j);
        pathfound = true;
      }
    }
    if(!pathfound) notFoundPaths.push_back(i);
  }
  if(notFoundPaths.size() > 0) {
    std::cout << "The following paths could not be found: " << std::endl;
    for (auto & path : notFoundPaths) std::cout << "   " << path << std::endl;
    std::cout << "The list of all available paths is: " << std::endl;
    for (auto & path : hltConfig_.triggerNames()) std::cout << "   " << path << std::endl;
  }

  // creating a histogram collection for each path
  std::set<std::string>::iterator iPath;
  for (iPath = hltPaths.begin(); iPath != hltPaths.end(); iPath++) {
    collection_path_.emplace_back(HLTGenValHistColl_path(objType_, *iPath, hltConfig_));
  }

}

// ------------ method called for each event  ------------
void HLTGenValSource::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // creating the collection of HLTGenValObjects
  fillObjectCollection(iEvent);

  edm::Handle<trigger::TriggerEvent> triggerEvent;
  iEvent.getByToken(trigEventToken_, triggerEvent);

  // loop over all objects and fill hists
  for (auto & object : objects_) {
    for (auto& collection_path : collection_path_) {
      collection_path.fillHists(object, triggerEvent);
    }
  }

}

// ------------ method called once each job just before starting event loop  ------------
void HLTGenValSource::bookHistograms(DQMStore::IBooker& iBooker, const edm::Run& run, const edm::EventSetup& setup) {
  for (auto& collection_path : collection_path_) {
    iBooker.setCurrentFolder(dirName_+"/"+objType_+"/"+collection_path.triggerPath_);
    collection_path.bookHists(iBooker, histConfigs_);
  }
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HLTGenValSource::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  // basic parameter strings
  desc.add<std::string>("objType"); // this deliberately has no default, as this is the main thing the user needs to chose
  desc.add<std::vector<std::string>>("hltPathsToCheck"); // this for the moment also has no default: maybe there can be some way to handle this later?
  desc.add<std::string>("DQMDirName", "HLTGenVal");
  desc.add<std::string>("hltProcessName", "HLT");

  // input collections, a PSet
  edm::ParameterSetDescription inputCollections;
  inputCollections.add<edm::InputTag>("genParticles", edm::InputTag("genParticles"));
  inputCollections.add<edm::InputTag>("ak4GenJets", edm::InputTag("ak4GenJets"));
  inputCollections.add<edm::InputTag>("ak8GenJets", edm::InputTag("ak8GenJets"));
  inputCollections.add<edm::InputTag>("TrigEvent", edm::InputTag("hltTriggerSummaryAOD"));
  desc.add<edm::ParameterSetDescription>("inputCollections", inputCollections);

  // hist descriptors, which are a vector of PSets

  // defining single histConfig
  // this is generally without default, but a default set of histConfigs is specified below
  edm::ParameterSetDescription histConfig;
  histConfig.add<std::string>("vsVar");
  histConfig.add<std::vector<double>>("binLowEdges");

  // default set of histConfigs
  std::vector<edm::ParameterSet> histConfigDefaults;

  edm::ParameterSet histConfigDefault0;
  histConfigDefault0.addParameter<std::string>("vsVar", "pt");
  std::vector<double> defaultPtBinning{ 0,5,10,12.5,15,17.5,20,22.5,25,30,35,40,45,50,60,80,100,150,200,250,300,350,400 };
  histConfigDefault0.addParameter<std::vector<double>>("binLowEdges", defaultPtBinning );
  histConfigDefaults.push_back(histConfigDefault0);

  edm::ParameterSet histConfigDefault1;
  histConfigDefault1.addParameter<std::string>("vsVar", "eta");
  std::vector<double> defaultetaBinning{ -10, -8, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 8, 10 };
  histConfigDefault1.addParameter<std::vector<double>>("binLowEdges", defaultetaBinning );
  histConfigDefaults.push_back(histConfigDefault1);

  desc.addVPSet("histConfigs", histConfig, histConfigDefaults);

  descriptions.addDefault(desc);
}

void HLTGenValSource::fillObjectCollection(const edm::Event& iEvent) {
  int GENobjectPDGID_ = -1;

  // convert input GENobject to pdgID
  // maybe there is a smarter way to do this? -> at least put it in a function somewhere ;)
  if(objType_ == "ele") GENobjectPDGID_ = 11;
  else if(objType_ == "pho") GENobjectPDGID_ = 22;
  else if(objType_ == "mu") GENobjectPDGID_ = 13;
  else if(objType_ == "tau") GENobjectPDGID_ = 15;
  else if(objType_ == "jet") {
    const auto& genJets = iEvent.getHandle(genJetToken_);
    for(size_t i = 0; i < genJets->size(); ++ i) {
      const GenJet p = (*genJets)[i];
      objects_.emplace_back(p);
    }
  }
  else if(objType_ == "HT") throw cms::Exception("InputError") << "Generator-level validation for HT is not yet implemented.\n";
  else if(objType_ == "MET") throw cms::Exception("InputError") << "Generator-level validation for MET is not yet implemented.\n";
  else throw cms::Exception("InputError") << "Generator-level validation is not available for type " << objType_ << ".\n" << "Please check for a potential spelling error.\n";
  // handle jets here -> probably using GenJets collection?
  // handle HT, MET here -> using something else entirely. GenMET?

  if(GENobjectPDGID_ != -1) {
    const auto& genParticles = iEvent.getHandle(genParticleToken_);
    for(size_t i = 0; i < genParticles->size(); ++ i) {
      const GenParticle p = (*genParticles)[i];
      if ( (abs(p.pdgId()) == GENobjectPDGID_) && (p.status() == 1)) objects_.emplace_back(p);
    }
  }

}




//define this as a plug-in
DEFINE_FWK_MODULE(HLTGenValSource);
