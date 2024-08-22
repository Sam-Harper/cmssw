#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/L1Trigger/interface/BXVector.h"
#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "DataFormats/L1TGlobal/interface/GlobalExtBlk.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1Trigger/interface/Tau.h"
#include "DataFormats/L1Trigger/interface/Jet.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/EtSum.h"



class L1TDigisCopier : public edm::global::EDProducer<> {
public:

  template<typename T>
  class Copier {
    public:
    Copier(const edm::InputTag& tag,edm::ConsumesCollector&& cc,edm::ProducesCollector&& pc):
      getToken_(cc.consumes<T>(tag)),
      putToken_(pc.produces<T>(tag.instance()))
    {}
    
    void operator()(edm::Event& evt) const {
      auto& in = evt.get(getToken_);
      auto out = std::make_unique<T>();
      *out = in;
      evt.put(putToken_,std::move(out));
    }
    private:
    const edm::EDGetTokenT<T> getToken_;
    const edm::EDPutTokenT<T> putToken_;
  };

  L1TDigisCopier(const edm::ParameterSet& ps):
    egammaCopier_(ps.getParameter<edm::InputTag>("egamma"),consumesCollector(),producesCollector()),
    tauCopier_(ps.getParameter<edm::InputTag>("tau"),consumesCollector(),producesCollector()),
    jetCopier_(ps.getParameter<edm::InputTag>("jet"),consumesCollector(),producesCollector()),
    muonCopier_(ps.getParameter<edm::InputTag>("muon"),consumesCollector(),producesCollector()),
    etSumCopier_(ps.getParameter<edm::InputTag>("etSum"),consumesCollector(),producesCollector()),
    globalAlgBlkCopier_(ps.getParameter<edm::InputTag>("globalAlgBlk"),consumesCollector(),producesCollector()),
    globalExtBlkCopier_(ps.getParameter<edm::InputTag>("globalExtBlk"),consumesCollector(),producesCollector())
  {}
  ~L1TDigisCopier()override{};

  void produce(edm::StreamID sid, edm::Event& evt, const edm::EventSetup& es) const override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  Copier<BXVector<l1t::EGamma>> egammaCopier_;
  Copier<BXVector<l1t::Tau>> tauCopier_;
  Copier<BXVector<l1t::Jet>> jetCopier_;
  Copier<BXVector<l1t::Muon>> muonCopier_;
  Copier<BXVector<l1t::EtSum>> etSumCopier_;
  Copier<BXVector<GlobalAlgBlk>> globalAlgBlkCopier_;
  Copier<BXVector<GlobalExtBlk>> globalExtBlkCopier_;

};

void L1TDigisCopier::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>(("egamma"), edm::InputTag("caloStage2Digis:EGamma"));
  desc.add<edm::InputTag>(("tau"), edm::InputTag("caloStage2Digis:Tau"));
  desc.add<edm::InputTag>(("jet"), edm::InputTag("caloStage2Digis:Jet"));
  desc.add<edm::InputTag>(("muon"), edm::InputTag("gmtStage2Digis:Muon"));
  desc.add<edm::InputTag>(("etSum"), edm::InputTag("caloStage2Digis:EtSum"));
  desc.add<edm::InputTag>(("globalAlgBlk"), edm::InputTag("gtStage2Digis"));
  desc.add<edm::InputTag>(("globalExtBlk"), edm::InputTag("gtStage2Digis"));
  desc.add<std::vector<edm::InputTag>>(("filters"), {});  
  descriptions.add(("L1TDigisCopier"), desc);
}


void L1TDigisCopier::produce(edm::StreamID sid, edm::Event& evt, const edm::EventSetup& es) const {

  egammaCopier_(evt);
  tauCopier_(evt);
  jetCopier_(evt);
  muonCopier_(evt);
  etSumCopier_(evt);
  globalAlgBlkCopier_(evt);
  globalExtBlkCopier_(evt);

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1TDigisCopier);