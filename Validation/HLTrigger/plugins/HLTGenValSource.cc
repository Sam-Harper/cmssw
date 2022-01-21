// -*- C++ -*-
//
// Package:    Validation/HLTrigger
// Class:      HLTGenValSource
//
/**\class HLTGenValSource HLTGenValSource.cc Validation/HLTrigger/plugins/HLTGenValSource.cc

 Description: Producing and filling histograms for generator-level HLT path efficiency histograms, used by a HLTGenValCLient.

 Implementation:
   Histograms for objects of a certain type are created for multiple paths chosen by the user: for all objects,
   and for objects passing filters in the path, determined by deltaR matching.
   Each HLTGenValSource can handle a single object type and any number of paths (and filters in them).
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

// icnluding GenMET
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"

// includes needed for histogram creation
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// FunctionDefs
#include "DQMOffline/Trigger/interface/FunctionDefs.h"

// includes of histogram collection class
#include "Validation/HLTrigger/interface/HLTGenValHistCollPath.h"

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

  // functions to get correct object collection for chosen object type
  std::vector<HLTGenValObject> getObjectCollection(const edm::Event&);
  std::vector<HLTGenValObject> getGenParticles(const edm::Event&);
  GenParticle getLastCopyPreFSR(GenParticle part);
  GenParticle getLastCopy(GenParticle part);

  // ----------member data ---------------------------

  // tokens to get collections
  const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
  const edm::EDGetTokenT<reco::GenMETCollection> genMETToken_;
  const edm::EDGetTokenT<reco::GenJetCollection> AK4genJetToken_;
  const edm::EDGetTokenT<reco::GenJetCollection> AK8genJetToken_;
  const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;

  // config strings/Psets
  std::string objType_;
  std::string dirName_;
  std::vector<edm::ParameterSet> histConfigs_;
  std::vector<edm::ParameterSet> histConfigs2D_;
  std::string hltProcessName_;

  // histogram collection per path
  std::vector<HLTGenValHistCollPath> collection_path_;

  // HLT config provider/getter
  HLTConfigProvider hltConfig_;
  std::vector<std::string> hltPathsToCheck_;
  std::set<std::string> hltPaths;
  double dR2limit_;
  bool doOnlyLastFilter_;

};

HLTGenValSource::HLTGenValSource(const edm::ParameterSet& iConfig)
    : genParticleToken_( consumes<reco::GenParticleCollection>( iConfig.getParameterSet("inputCollections").getParameter<edm::InputTag>("genParticles") ) ),
      genMETToken_( consumes<reco::GenMETCollection>( iConfig.getParameterSet("inputCollections").getParameter<edm::InputTag>("genMET") ) ),
      AK4genJetToken_(consumes<reco::GenJetCollection>( iConfig.getParameterSet("inputCollections").getParameter<edm::InputTag>("ak4GenJets"))),
      AK8genJetToken_(consumes<reco::GenJetCollection>( iConfig.getParameterSet("inputCollections").getParameter<edm::InputTag>("ak8GenJets"))),
      trigEventToken_(consumes<trigger::TriggerEvent>( iConfig.getParameterSet("inputCollections").getParameter<edm::InputTag>("TrigEvent") )) {

      // getting the histogram configurations
      histConfigs_ = iConfig.getParameterSetVector("histConfigs");
      histConfigs2D_ = iConfig.getParameterSetVector("histConfigs2D");

      // getting all other configurations
      dirName_ = iConfig.getParameter<std::string>("DQMDirName");
      objType_ = iConfig.getParameter<std::string>("objType");
      dR2limit_ = iConfig.getParameter<double>("dR2limit");
      doOnlyLastFilter_ = iConfig.getParameter<bool>("doOnlyLastFilter");
      hltProcessName_ = iConfig.getParameter<std::string>("hltProcessName");
      hltPathsToCheck_ = iConfig.getParameter<std::vector<std::string>>("hltPathsToCheck");

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

  // we want "before" histograms, which we will use this dummy path for
  collection_path_.emplace_back(HLTGenValHistCollPath(objType_, "beforeAnyPath", hltConfig_, dR2limit_, doOnlyLastFilter_));

  // creating a histogram collection for each path
  std::set<std::string>::iterator iPath;
  for (iPath = hltPaths.begin(); iPath != hltPaths.end(); iPath++) {
    collection_path_.emplace_back(HLTGenValHistCollPath(objType_, *iPath, hltConfig_, dR2limit_, doOnlyLastFilter_));
  }

}

// ------------ method called for each event  ------------
void HLTGenValSource::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // creating the collection of HLTGenValObjects
  std::vector<HLTGenValObject> objects = getObjectCollection(iEvent);

  // init triggerEvent, which is always needed
  edm::Handle<trigger::TriggerEvent> triggerEvent;
  iEvent.getByToken(trigEventToken_, triggerEvent);

  // loop over all objects and fill hists
  for (auto & object : objects) {
    for (auto& collection_path : collection_path_) {
      collection_path.fillHists(object, triggerEvent);
    }
  }

}

// ------------ method called once each job just before starting event loop  ------------
void HLTGenValSource::bookHistograms(DQMStore::IBooker& iBooker, const edm::Run& run, const edm::EventSetup& setup) {

  // booking all histograms
  for (auto& collection_path : collection_path_) {
    // currently splitting into multiple folders
    // this will most likely be changed later
    // currently only the "before" hists are present at top level
    if(collection_path.triggerPath_ == "beforeAnyPath") iBooker.setCurrentFolder(dirName_+"/"+objType_);
    else iBooker.setCurrentFolder(dirName_+"/"+objType_+"/"+collection_path.triggerPath_);
    collection_path.bookHists(iBooker, histConfigs_, histConfigs2D_);
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
  desc.add<double>("dR2limit", 0.1);
  desc.add<bool>("doOnlyLastFilter", false);

  // input collections, a PSet
  edm::ParameterSetDescription inputCollections;
  inputCollections.add<edm::InputTag>("genParticles", edm::InputTag("genParticles"));
  inputCollections.add<edm::InputTag>("genMET", edm::InputTag("genMetTrue"));
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
  histConfig.addVPSet("rangeCuts", VarRangeCut<HLTGenValObject>::makePSetDescription(), std::vector<edm::ParameterSet>());

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

  // defining single histConfig2D
  edm::ParameterSetDescription histConfig2D;
  histConfig2D.add<std::string>("vsVar_x");
  histConfig2D.add<std::string>("vsVar_y");
  histConfig2D.add<std::vector<double>>("binLowEdges_x");
  histConfig2D.add<std::vector<double>>("binLowEdges_y");

  // default set of histConfigs
  std::vector<edm::ParameterSet> histConfigDefaults2D;

  edm::ParameterSet histConfigDefault2D0;
  histConfigDefault2D0.addParameter<std::string>("vsVar_x", "pt");
  histConfigDefault2D0.addParameter<std::string>("vsVar_y", "eta");
  histConfigDefault2D0.addParameter<std::vector<double>>("binLowEdges_x", defaultPtBinning);
  histConfigDefault2D0.addParameter<std::vector<double>>("binLowEdges_y", defaultetaBinning);
  histConfigDefaults2D.push_back(histConfigDefault2D0);

  desc.addVPSet("histConfigs2D", histConfig2D, histConfigDefaults2D);

  descriptions.addDefault(desc);
}

std::vector<HLTGenValObject> HLTGenValSource::getObjectCollection(const edm::Event& iEvent) {

  std::vector<HLTGenValObject> objects; // the vector of objects to be filled

  // handle object type
  std::vector<std::string> implemented_GenParticles = {"ele", "pho", "mu", "tau"};
  if(std::find(implemented_GenParticles.begin(), implemented_GenParticles.end(), objType_) != implemented_GenParticles.end()) {
    objects = getGenParticles(iEvent);
  }
  else if(objType_ == "AK4jet") { // ak4 jets, using the ak4GenJets collection
    const auto& genJets = iEvent.getHandle(AK4genJetToken_);
    for(size_t i = 0; i < genJets->size(); i++) {
      const GenJet p = (*genJets)[i];
      objects.emplace_back(p);
    }
  }
  else if(objType_ == "AK8jet") { // ak8 jets, using the ak8GenJets collection
    const auto& genJets = iEvent.getHandle(AK8genJetToken_);
    for(size_t i = 0; i < genJets->size(); i++) {
      const GenJet p = (*genJets)[i];
      objects.emplace_back(p);
    }
  }
  else if(objType_ == "AK4HT") { // ak4-based HT, using the ak4GenJets collection
    const auto& genJets = iEvent.getHandle(AK4genJetToken_);
    if(genJets->size() > 0){
      auto HTsum = (*genJets)[0].pt();
      for(size_t i = 1; i < genJets->size(); i++) HTsum += (*genJets)[i].pt();
      objects.emplace_back(Candidate::PolarLorentzVector(HTsum, 0, 0, 0));
    }
  }
  else if(objType_ == "AK8HT") { // ak8-based HT, using the ak8GenJets collection
    const auto& genJets = iEvent.getHandle(AK8genJetToken_);
    if(genJets->size() > 0){
      auto HTsum = (*genJets)[0].pt();
      for(size_t i = 1; i < genJets->size(); i++) HTsum += (*genJets)[i].pt();
      objects.emplace_back(Candidate::PolarLorentzVector(HTsum, 0, 0, 0));
    }
  }
  else if(objType_ == "MET") {
    const auto& genMET = iEvent.getHandle(genMETToken_);
    if(genMET->size() > 0){
      auto genMETpt = (*genMET)[0].pt();
      objects.emplace_back(Candidate::PolarLorentzVector(genMETpt, 0, 0, 0));
    }
  }
  else throw cms::Exception("InputError") << "Generator-level validation is not available for type " << objType_ << ".\n" << "Please check for a potential spelling error.\n";

  return objects;
}

std::vector<HLTGenValObject> HLTGenValSource::getGenParticles(const edm::Event& iEvent) {

  std::vector<HLTGenValObject> objects; // vector to be filled

  const auto& genParticles = iEvent.getHandle(genParticleToken_); // getting all GenParticles

  // we need to ge the ID corresponding to the desired GenParticle type
  int pdgID;
  if(objType_ == "ele") pdgID = 11;
  else if(objType_ == "pho") pdgID = 22;
  else if(objType_ == "mu") pdgID = 13;
  else if(objType_ == "tau") pdgID = 15;

  // main loop over GenParticles
  for(size_t i = 0; i < genParticles->size(); ++ i) {
    const GenParticle p = (*genParticles)[i];

    // only select status 1 with correct ID
    if (p.status() != 1) continue;
    if (abs(p.pdgId()) != pdgID) continue;

    // checking if particle comes from "hard process"
    if(p.isHardProcess()) {

      // depending on the particle type, last particle before or after FSR is chosen
      if( (objType_ == "ele") || (objType_ == "pho") ) objects.emplace_back( getLastCopyPreFSR(p) );
      else if( (objType_ == "mu") || (objType_ == "tau") ) objects.emplace_back( getLastCopy(p) );

    }

  }

  return objects;

}

// function returning the last GenParticle in a decay chain before FSR
GenParticle HLTGenValSource::getLastCopyPreFSR(GenParticle part) {
    auto daughters = part.daughterRefVector();
    if (daughters.size() == 1 && daughters.at(0)->pdgId() == part.pdgId()) return getLastCopyPreFSR(*daughters.at(0).get()); // recursion, whooo
    else return part;
}

// function returning the last GenParticle in a decay chain
GenParticle HLTGenValSource::getLastCopy(GenParticle part) {
  for (const auto & daughter : part.daughterRefVector()){
    if (daughter->pdgId() == part.pdgId()) return getLastCopy(*daughter.get());
  }
  return part;
}

//define this as a framework plug-in
DEFINE_FWK_MODULE(HLTGenValSource);
