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

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

using namespace reco;

class HLTGenValSource : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit HLTGenValSource(const edm::ParameterSet&);
  ~HLTGenValSource();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // ----------member data ---------------------------
  std::string GENobject_;
  std::string vsVar_;
  int GENobjectPDGID_;
  const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
  TH1D *demohisto;
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  edm::ESGetToken<SetupData, SetupRecord> setupToken_;
#endif
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
    : genParticleToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticles"))){
      GENobject_ = iConfig.getParameter<std::string>("GENobject");
      vsVar_ = iConfig.getParameter<std::string>("vsVar");

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
      setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif

      //now do what ever initialization is needed
      edm::Service<TFileService> fs;
      demohisto = fs->make<TH1D>((TString)vsVar_ , (TString)vsVar_ , 100 , 0 , 5000 );
}

HLTGenValSource::~HLTGenValSource() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

//
// member functions
//

// ------------ method called for each event  ------------
void HLTGenValSource::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // get "sub"-collection of GenParticles
  const auto& genParticles = iEvent.getHandle(genParticleToken_);
  for(size_t i = 0; i < genParticles->size(); ++ i) {
    const GenParticle p = (*genParticles)[i];
    int id = p.pdgId();
    if (abs(id) == GENobjectPDGID_) {

      // main loop over all "correct" GEN particles

      // fill test histogram
      auto vsVarFunc = hltdqm::getUnaryFuncFloat<GenParticle>(vsVar_);
      demohisto->Fill(vsVarFunc(p));
    }
  }


#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  // if the SetupData is always needed
  auto setup = iSetup.getData(setupToken_);
  // if need the ESHandle to check if the SetupData was there or not
  auto pSetup = iSetup.getHandle(setupToken_);
#endif
}

// ------------ method called once each job just before starting event loop  ------------
void HLTGenValSource::beginJob() {

  // convert input GENobject to pdgID
  // maybe there is a smarter way to do this? -> at least put it in a function somewhere ;)
  if(GENobject_ == "ele") GENobjectPDGID_ = 11;
  else if(GENobject_ == "pho") GENobjectPDGID_ = 22;
  else if(GENobject_ == "mu") GENobjectPDGID_ = 13;
  else if(GENobject_ == "tau") GENobjectPDGID_ = 15;
  else if(GENobject_ == "jet") throw cms::Exception("InputError") << "Generator-level validation for jets is not yet implemented.\n";
  else if(GENobject_ == "HT") throw cms::Exception("InputError") << "Generator-level validation for HT is not yet implemented.\n";
  else if(GENobject_ == "MET") throw cms::Exception("InputError") << "Generator-level validation for MET is not yet implemented.\n";
  else throw cms::Exception("InputError") << "Generator-level validation is not available for type " << GENobject_ << ".\n" << "Please check for a potential spelling error.\n";
  // handle jets here -> probably using GenJets collection?
  // handle HT, MET here -> using something else entirely. GenMET?

  std::cout << "Started job for " << GENobject_ << std::endl;
}

// ------------ method called once each job just after ending the event loop  ------------
void HLTGenValSource::endJob() {
  // please remove this method if not needed
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
