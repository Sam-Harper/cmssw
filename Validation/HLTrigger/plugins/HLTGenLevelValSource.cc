#include "DQMServices/Core/interface/DQMStore.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include <vector>
#include <string>

class HLTGenLevelValSource : public DQMEDAnalyzer {
public:
  explicit HLTGenLevelValSource(const edm::ParameterSet&);
  ~HLTGenLevelValSource() override = default;
  HLTGenLevelValSource(const HLTGenLevelValSource&) = delete;
  HLTGenLevelValSource& operator=(const HLTGenLevelValSource&) = delete;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const& run, edm::EventSetup const& c) override;

private:
  //member variables
};


HLTGenLevelValSource::HLTGenLevelValSource(
    const edm::ParameterSet& config) {

}


//all python parameters used to configure the module must be specified here
void HLTGenLevelValSource::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  descriptions.addDefault(desc);
}


void HLTGenLevelValSource::bookHistograms(
    DQMStore::IBooker& iBooker, const edm::Run& run, const edm::EventSetup& setup) {

}

void HLTGenLevelValSource::analyze(
    const edm::Event& event, const edm::EventSetup& setup) {

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HLTGenLevelValSource);
