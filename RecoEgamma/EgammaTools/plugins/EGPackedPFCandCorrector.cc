#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

#include <vector>


namespace{
   
  template<typename T>  
  static edm::Handle<T> getHandle(const edm::Event& iEvent,
				  const edm::EDGetTokenT<T>& token){
    edm::Handle<T> handle;
    iEvent.getByToken(token,handle);
    return handle;
  }
}

class EGPackedPFCandCorrector : public edm::stream::EDProducer<> {
private:
public:
  explicit EGPackedPFCandCorrector(const edm::ParameterSet&);
  ~EGPackedPFCandCorrector() override;
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,edm::InputTag tag){token=consumes<T>(tag);}
  template <typename T> void setToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& tag){token=consumes<T>(iPara.getParameter<edm::InputTag>(tag));}
 
  void produce(edm::Event&, const edm::EventSetup&) override;

  const reco::Photon* getPhoMatch(const pat::PackedCandidate& cand,
				  edm::Handle<pat::PhotonCollection> & phos);
  const reco::Photon* getPhoMatch(const reco::Photon* pho,
				  edm::Handle<pat::PhotonCollection> otherPhosHandle);
    
  void correctCand(const math::XYZTLorentzVector& egP4,
		   const reco::SuperClusterRef& sc,
		   pat::PackedCandidate& cand);
    
  edm::EDGetTokenT<pat::ElectronCollection> elesToken_;
  edm::EDGetTokenT<pat::PhotonCollection> phosToken_;
  edm::EDGetTokenT<pat::PhotonCollection> oldPhosToken_;
  edm::EDGetTokenT<pat::PackedCandidateCollection> srcCandsToken_;

  bool correctEles_;
  bool correctPhos_;
  bool useSCEnergy_;
  float scEnergyEtThres_;
};

EGPackedPFCandCorrector::EGPackedPFCandCorrector(const edm::ParameterSet& iConfig)
{
  setToken(elesToken_,iConfig,"eles");
  setToken(phosToken_,iConfig,"phos");
  setToken(oldPhosToken_,iConfig,"oldPhos");
  setToken(srcCandsToken_,iConfig,"srcCands");
  correctEles_ = iConfig.getParameter<bool>("correctEles");
  correctPhos_ = iConfig.getParameter<bool>("correctPhos");
  useSCEnergy_ = iConfig.getParameter<bool>("useSCEnergy");
  scEnergyEtThres_ = iConfig.getParameter<double>("scEnergyEtThres");
  produces<pat::PackedCandidateCollection>();  
}

EGPackedPFCandCorrector::~EGPackedPFCandCorrector()
{

}


void EGPackedPFCandCorrector::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto elesHandle = getHandle(iEvent,elesToken_);
  auto srcCandsHandle = getHandle(iEvent,srcCandsToken_);

  auto newCands = std::make_unique<pat::PackedCandidateCollection>(*srcCandsHandle);
  
  std::vector<size_t> correctedCandIndices;
  if(correctEles_){
    for(auto& ele : *elesHandle){
      const auto& elePFCands = ele.associatedPackedPFCandidates();
      if(ele.passingPflowPreselection()){
	//std::cout <<"passed pf preselection but doesnt have one to one map to a candidate "<<std::endl;
	//      std::cout <<"ele : "<<ele.et()<<" eta "<<ele.eta()<<" phi "<<ele.phi()<<std::endl;
	for(auto & ref : elePFCands){
	  if(ref.key()<newCands->size()){
	    auto& cand = (*newCands)[ref.key()];
	    if(std::abs(cand.pdgId())==11){ //for some reason we get muons in this too...
	      //	    std::cout <<"  cands "<<cand.pdgId()<<" et "<<cand.pt()<<" eta "<<cand.eta()<<" phi "<<cand.phi()<<std::endl;
	      correctCand(ele.p4(),ele.superCluster(),cand);
	      correctedCandIndices.push_back(elePFCands[0].key());
	    }
	    
	  }else{
	    std::cout <<"error index is bad"<<elePFCands[0].key()<<" cands size "<<newCands->size()<<std::endl;
	  }
	}
      }
    }
    std::sort(correctedCandIndices.begin(),correctedCandIndices.end());
  }
  
  auto phosHandle = getHandle(iEvent,phosToken_);
  auto oldPhosHandle = getHandle(iEvent,oldPhosToken_);
  
  for(size_t candNr=0;candNr<newCands->size();candNr++){
    auto& cand = (*newCands)[candNr];
    if(std::abs(cand.pdgId())==11 && correctEles_){
      if(!std::binary_search(correctedCandIndices.begin(),correctedCandIndices.end(),candNr)){
	std::cout <<"found PF electron candidate not corrected "<<std::endl;	
      }
    }else if(std::abs(cand.pdgId())==22 && correctPhos_){
      auto matchedOldPho = getPhoMatch(cand,oldPhosHandle);
      auto matchedPho = getPhoMatch(matchedOldPho,phosHandle);
      if(matchedPho){
	//	std::cout <<"correcting cand from "<<cand.p4().energy()<<" "<<cand.eta()<<" "<<cand.phi()<<" to "<<matchedPho->p4().energy()<<" "<<matchedPho->p4().eta()<<" "<<matchedPho->p4().phi()<<" "<<cand.isGoodEgamma()<<std::endl;
	correctCand(matchedPho->p4(),matchedPho->superCluster(),cand);
      }
      
    }
  }
  iEvent.put(std::move(newCands));
}

const reco::Photon*
EGPackedPFCandCorrector::getPhoMatch(const pat::PackedCandidate& cand,
				     edm::Handle<pat::PhotonCollection> & phos)
{
  if(!phos.isValid()) return nullptr;
     
  const float maxDR2 = 0.01*0.01;
  const float maxEAbsDiff = 0.1;
  const float maxERelDiff = 0.01;
  for(const auto& pho : *phos){
    const float eDiff = std::abs(pho.energy()-cand.energy());
    //  std::cout <<" dR2 "<<reco::deltaR2(pho->eta(),pho->phi(),cand.eta(),cand.phi())<<" eDiff "<<eDiff<<" pho "<<pho->energy()<<" "<<pho->eta()<<" "<<pho->phi()<<" cand "<<cand.energy()<<" "<<cand.eta()<<" "<<cand.phi()<<" seed clus e "<<pho->superCluster()->seed()->energy()<<std::endl;
    if(reco::deltaR2(pho.eta(),pho.phi(),cand.eta(),cand.phi())<maxDR2 &&
       (eDiff<maxEAbsDiff || eDiff/pho.energy() < maxERelDiff) ) {
      return &pho;
    }
  }
  return nullptr;
}

const reco::Photon*
EGPackedPFCandCorrector::getPhoMatch(const reco::Photon* pho,
				     edm::Handle<pat::PhotonCollection> otherPhosHandle)
{
  if(!pho) return nullptr;
  if(!otherPhosHandle.isValid()) return nullptr;

  for(const auto& otherPho : *otherPhosHandle){
    if(pho->superCluster()->seed()->seed().rawId()!=0 && pho->superCluster()->seed()->seed()==otherPho.superCluster()->seed()->seed()) return &otherPho;
  }
  std::cout <<"warning did not find a match for "<<pho->energy()<<" "<<pho->eta()<<" "<<pho->phi()<<std::endl;
  return nullptr;
}


void EGPackedPFCandCorrector::correctCand(const math::XYZTLorentzVector& egP4,
					  const reco::SuperClusterRef& sc,
					  pat::PackedCandidate& cand)
{
  const float rawSCEt = sc->rawEnergy()*sc->position().rho()/sc->position().r();
  if(useSCEnergy_ && rawSCEt>=scEnergyEtThres_){	 
    const float corrFac = sc->energy()/cand.p4().energy();
    math::XYZTLorentzVector newP4(cand.p4().x()*corrFac,cand.p4().y()*corrFac,cand.p4().z()*corrFac,
				  sc->energy());
    cand.setP4(newP4);
  }else cand.setP4(egP4);
}

void EGPackedPFCandCorrector::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
 
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("eles",edm::InputTag("slimmedElectrons"));
  desc.add<edm::InputTag>("phos",edm::InputTag("slimmedPhotons"));
  desc.add<edm::InputTag>("oldPhos",edm::InputTag("slimmedPhotons"));
  desc.add<edm::InputTag>("srcCands",{edm::InputTag("packedPFCandidates")});
  desc.add<bool>("correctEles",true);
  desc.add<bool>("correctPhos",true);
  desc.add<bool>("useSCEnergy",false);
  desc.add<double>("scEnergyEtThres",300.);
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(EGPackedPFCandCorrector);
