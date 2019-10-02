#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/METReco/interface/CorrMETData.h"

#include "RecoEgamma/EgammaTools/interface/EGMiniAODTypeIMETCorrector.h"

#include <vector>

class EGMiniAODType1METCorrProducer : public edm::stream::EDProducer<> {
  
public:
  
  explicit EGMiniAODType1METCorrProducer(const edm::ParameterSet& cfg)
    : corrector_(cfg.getParameter<edm::ParameterSet>("correctorCfg")),
      elesToken_(consumes<std::vector<pat::Electron> >(cfg.getParameter<edm::InputTag>("eles"))),
      jetsToken_(consumes<std::vector<pat::Jet> >(cfg.getParameter<edm::InputTag>("jets")))
  {
    produces<CorrMETData>();
  }
  

  ~EGMiniAODType1METCorrProducer() override { }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::ParameterSetDescription>("correctorCfg", EGMiniAODTypeIMETCorrector::makePSetDescription());
    desc.add<edm::InputTag>("eles",edm::InputTag("slimmedElectrons"));
    desc.add<edm::InputTag>("jets",edm::InputTag("slimmedJets"));
    descriptions.add("egMiniAODTypeIMETCorrector",desc);
  }

private:
  void produce(edm::Event& event, const edm::EventSetup& eventSetup) override;

  EGMiniAODTypeIMETCorrector corrector_;
  edm::EDGetTokenT<std::vector<pat::Electron> > elesToken_;
  edm::EDGetTokenT<std::vector<pat::Jet> > jetsToken_;


};

namespace{
  template<typename T> edm::Handle<T> getHandle(const edm::Event& event,const edm::EDGetTokenT<T> token){
    edm::Handle<T> handle;
    event.getByToken(token,handle);
    return handle;
  }
}

void EGMiniAODType1METCorrProducer::produce(edm::Event& event, const edm::EventSetup& eventSetup)
  {
 
    auto eles = getHandle(event,elesToken_);
    auto jets = getHandle(event,jetsToken_);
    
    CorrMETData data = corrector_(*eles,*jets);
    auto dataForEvent = std::make_unique<CorrMETData>(data);
    event.put(std::move(dataForEvent));
  }


DEFINE_FWK_MODULE(EGMiniAODType1METCorrProducer);
