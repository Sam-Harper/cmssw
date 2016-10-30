#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EleTkIsolFromCands.h"

#include <memory>
#include <vector>

//Heavily inspired from ElectronIDValueMapProducer


class ElectronHEEPIDValueMapProducer : public edm::stream::EDProducer<> {

  public:
  
  explicit ElectronHEEPIDValueMapProducer(const edm::ParameterSet&);
  ~ElectronHEEPIDValueMapProducer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  private:
  
  virtual void produce(edm::Event&, const edm::EventSetup&) override;

  template<typename T>
  static void writeValueMap(edm::Event &iEvent,
			    const edm::Handle<edm::View<reco::GsfElectron> > & handle,
			    const std::vector<T> & values,
			    const std::string& label);
  
  static int nrSaturatedCrysIn5x5(const reco::GsfElectron& ele,
				  edm::Handle<EcalRecHitCollection>& ebHits,
				  edm::Handle<EcalRecHitCollection>& eeHits,
				  edm::ESHandle<CaloTopology>& caloTopo);

  float calTrkIso(const reco::GsfElectron& ele,		  
		  const edm::View<reco::GsfElectron>& eles,
		  const edm::Event& iEvent);
    
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
  template <typename T> void setToken(std::vector<edm::EDGetTokenT<T> >& tokens,const edm::ParameterSet& iPara,const std::string& tagName){
    auto tags =iPara.getParameter<std::vector<edm::InputTag> >(tagName);
    for(auto& tag : tags) {
      edm::EDGetTokenT<T> token;
      setToken(token,tag);
      tokens.push_back(token);
    }
  }
      
  template<typename T> edm::Handle<T> getHandle(const edm::Event& iEvent,const edm::EDGetTokenT<T>& token){
    edm::Handle<T> handle;
    iEvent.getByToken(token,handle);
    return handle;
  }
  

  edm::EDGetTokenT<EcalRecHitCollection> ebRecHitToken_;
  edm::EDGetTokenT<EcalRecHitCollection> eeRecHitToken_;
  edm::EDGetTokenT<edm::View<reco::GsfElectron> > eleToken_;
  std::vector<edm::EDGetTokenT<pat::PackedCandidateCollection> >candTokens_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;

  EleTkIsolFromCands trkIsoCalc_;
  
  static const std::string eleTrkPtIsoNoJetCoreLabel_;
  static const std::string eleNrSaturateIn5x5Label_;
};

const std::string ElectronHEEPIDValueMapProducer::eleTrkPtIsoNoJetCoreLabel_="eleTrkPtIsoNoJetCore";
const std::string ElectronHEEPIDValueMapProducer::eleNrSaturateIn5x5Label_="eleNrSaturateIn5x5";
 


ElectronHEEPIDValueMapProducer::ElectronHEEPIDValueMapProducer(const edm::ParameterSet& iConfig):
  trkIsoCalc_(iConfig.getParameter<edm::ParameterSet>("trkIsoConfig"))
{
  setToken(ebRecHitToken_,iConfig,"ebRecHits");
  setToken(eeRecHitToken_,iConfig,"eeRecHits");
  setToken(eleToken_,iConfig,"eles");
  setToken(candTokens_,iConfig,"cands");
  setToken(beamSpotToken_,iConfig,"beamSpot");
  
  produces<edm::ValueMap<float> >(eleTrkPtIsoNoJetCoreLabel_);  
  produces<edm::ValueMap<int> >(eleNrSaturateIn5x5Label_);  
}

ElectronHEEPIDValueMapProducer::~ElectronHEEPIDValueMapProducer()
{

}

void ElectronHEEPIDValueMapProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto eleHandle = getHandle(iEvent,eleToken_);
  auto ebRecHitHandle = getHandle(iEvent,ebRecHitToken_);
  auto eeRecHitHandle = getHandle(iEvent,eeRecHitToken_);
  auto beamSpotHandle = getHandle(iEvent,beamSpotToken_);
  
  edm::ESHandle<CaloTopology> caloTopoHandle;
  iSetup.get<CaloTopologyRecord>().get(caloTopoHandle);
  
  std::vector<float> eleTrkPtIsoNoJetCore;
  std::vector<int> eleNrSaturateIn5x5;

  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    auto elePtr = eleHandle->ptrAt(eleNr);
    eleTrkPtIsoNoJetCore.push_back(calTrkIso(*elePtr,*eleHandle,iEvent));
    eleNrSaturateIn5x5.push_back(nrSaturatedCrysIn5x5(*elePtr,ebRecHitHandle,eeRecHitHandle,caloTopoHandle));    
  }
  
  writeValueMap(iEvent,eleHandle,eleTrkPtIsoNoJetCore,eleTrkPtIsoNoJetCoreLabel_);  
  writeValueMap(iEvent,eleHandle,eleNrSaturateIn5x5,eleNrSaturateIn5x5Label_);  
}

int ElectronHEEPIDValueMapProducer::nrSaturatedCrysIn5x5(const reco::GsfElectron& ele,
							 edm::Handle<EcalRecHitCollection>& ebHits,
							 edm::Handle<EcalRecHitCollection>& eeHits,
							 edm::ESHandle<CaloTopology>& caloTopo)
{ 
  DetId id = ele.superCluster()->seed()->seed();
  auto recHits = id.subdetId()==EcalBarrel ? ebHits.product() : eeHits.product();
  return noZS::EcalClusterTools::nrSaturatedCrysIn5x5(id,recHits,caloTopo.product());

}

float ElectronHEEPIDValueMapProducer::calTrkIso(const reco::GsfElectron& ele,		  
						const edm::View<reco::GsfElectron>& eles,
						const edm::Event& iEvent)
{
  if(ele.gsfTrack().isNull()) return std::numeric_limits<float>::max();
  else{
    float trkIso=0.;
    for(auto& candToken: candTokens_){
      auto candHandle = getHandle(iEvent,candToken);
      if(candHandle.isValid()){
	trkIso+= trkIsoCalc_.calIsolPt(*ele.gsfTrack(),*candHandle,eles);
      }
    }
    return trkIso;
  }
}

template<typename T>
void ElectronHEEPIDValueMapProducer::writeValueMap(edm::Event &iEvent,
						   const edm::Handle<edm::View<reco::GsfElectron> > & handle,
						   const std::vector<T> & values,
						   const std::string& label)
{ 
  std::unique_ptr<edm::ValueMap<T> > valMap(new edm::ValueMap<T>());
  typename edm::ValueMap<T>::Filler filler(*valMap);
  filler.insert(handle, values.begin(), values.end());
  filler.fill();
  iEvent.put(std::move(valMap),label);
}

void ElectronHEEPIDValueMapProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
 
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("ebRecHits",edm::InputTag("reducedEcalRecHitsEB"));
  desc.add<edm::InputTag>("eeRecHits",edm::InputTag("reducedEcalRecHitsEE"));
  desc.add<edm::InputTag>("beamSpot",edm::InputTag("offlineBeamSpot"));
  desc.add<std::vector<edm::InputTag> >("cands",{edm::InputTag("packedCandidates")});
  desc.add<edm::InputTag>("eles",edm::InputTag("gedGsfElectrons"));
  
  desc.add("trkIsoConfig",EleTkIsolFromCands::pSetDescript());

  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(ElectronHEEPIDValueMapProducer);
