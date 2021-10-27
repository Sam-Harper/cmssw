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
#include "TH1.h"

// FunctionDefs
#include "DQMOffline/Trigger/interface/FunctionDefs.h"

// includes of histogram collection class
#include "Validation/HLTrigger/interface/HLTGenValHistColl.h"

// DQMStore
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

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

  // ----------member data ---------------------------
  std::string objType_;
  std::string dirName_;
  const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
  std::vector<edm::ParameterSet> filterConfigs_;
  std::vector<edm::ParameterSet> histConfigs_;

  std::vector<HLTGenValHistColl> hists_;
  int GENobjectPDGID_;

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
HLTGenValSource::HLTGenValSource(const edm::ParameterSet& iConfig)
    : genParticleToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticles"))) {

      filterConfigs_ = iConfig.getParameterSetVector("filterConfigs");
      histConfigs_ = iConfig.getParameterSetVector("histConfigs");

      dirName_ = iConfig.getParameter<std::string>("DQMDirName");
      objType_ = iConfig.getParameter<std::string>("objType");

      // convert input GENobject to pdgID
      // maybe there is a smarter way to do this? -> at least put it in a function somewhere ;)
      if(objType_ == "ele") GENobjectPDGID_ = 11;
      else if(objType_ == "pho") GENobjectPDGID_ = 22;
      else if(objType_ == "mu") GENobjectPDGID_ = 13;
      else if(objType_ == "tau") GENobjectPDGID_ = 15;
      else if(objType_ == "jet") throw cms::Exception("InputError") << "Generator-level validation for jets is not yet implemented.\n";
      else if(objType_ == "HT") throw cms::Exception("InputError") << "Generator-level validation for HT is not yet implemented.\n";
      else if(objType_ == "MET") throw cms::Exception("InputError") << "Generator-level validation for MET is not yet implemented.\n";
      else throw cms::Exception("InputError") << "Generator-level validation is not available for type " << objType_ << ".\n" << "Please check for a potential spelling error.\n";
      // handle jets here -> probably using GenJets collection?
      // handle HT, MET here -> using something else entirely. GenMET?

      std::cout << "Started job for " << objType_ << std::endl;

      for (auto & filterConfig : filterConfigs_) hists_.emplace_back(HLTGenValHistColl(objType_, filterConfig));

}

// ------------ method called for each event  ------------
void HLTGenValSource::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  const auto& genParticles = iEvent.getHandle(genParticleToken_);
  for(size_t i = 0; i < genParticles->size(); ++ i) {
    const GenParticle p = (*genParticles)[i];
    int id = p.pdgId();
    if (abs(id) == GENobjectPDGID_) {
      // main loop over all "correct" GEN particles

      // fill test histogram
      hists_.at(0).fillHists(p, iEvent, iSetup);
    }
  }

}

// ------------ method called once each job just before starting event loop  ------------
void HLTGenValSource::bookHistograms(DQMStore::IBooker& iBooker, const edm::Run& run, const edm::EventSetup& setup) {
  iBooker.setCurrentFolder(dirName_);
  hists_.at(0).bookHists(iBooker, histConfigs_);
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HLTGenValSource::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HLTGenValSource);
