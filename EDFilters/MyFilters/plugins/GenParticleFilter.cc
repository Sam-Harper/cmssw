// -*- C++ -*-
//
// Package:    EDFilters/GenParticleFilter
// Class:      GenParticleFilter
// 
/**\class GenParticleFilter GenParticleFilter.cc EDFilters/GenParticleFilter/plugins/GenParticleFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Soham Bhattacharya
//         Created:  Mon, 11 Nov 2019 13:03:23 GMT
//
//


// system include files
# include <memory>

// user include files
//# include "CommonTools/UtilAlgos/interface/TFileService.h"
//# include "DataFormats/CaloTowers/interface/CaloTowerDefs.h"
//# include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
//# include "DataFormats/FWLite/interface/ESHandle.h"
//# include "DataFormats/ForwardDetId/interface/HGCEEDetId.h"
//# include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
//# include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
//# include "DataFormats/HGCRecHit/interface/HGCRecHit.h"
//# include "DataFormats/HGCalReco/interface/Trackster.h"
# include "DataFormats/HepMCCandidate/interface/GenParticle.h"
//# include "DataFormats/JetReco/interface/PFJet.h"
//# include "DataFormats/Math/interface/LorentzVector.h"
//# include "DataFormats/ParticleFlowReco/interface/HGCalMultiCluster.h"
//# include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
//# include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
//# include "DataFormats/TrackReco/interface/Track.h"
//# include "DataFormats/TrackReco/interface/TrackFwd.h"
//# include "DataFormats/VertexReco/interface/Vertex.h"
//# include "FWCore/Framework/interface/ESHandle.h"
//# include "FWCore/Framework/interface/stream/EDFilter.h"
//# include "FWCore/ServiceRegistry/interface/Service.h"
//# include "FWCore/Utilities/interface/InputTag.h"
//# include "FWCore/Utilities/interface/StreamID.h"
//# include "Geometry/CaloTopology/interface/HGCalTopology.h"
//# include "Geometry/Records/interface/IdealGeometryRecord.h"
//# include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
//# include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
//# include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
//# include "SimDataFormats/CaloHit/interface/PCaloHit.h"
//# include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
//# include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"


# include "FWCore/Framework/interface/Frameworkfwd.h"
# include "FWCore/Framework/interface/stream/EDFilter.h"
# include "FWCore/Framework/interface/Event.h"
# include "FWCore/Framework/interface/MakerMacros.h"
# include "FWCore/ParameterSet/interface/ParameterSet.h"
# include "FWCore/Utilities/interface/StreamID.h"



//
// class declaration
//

class GenParticleFilter : public edm::stream::EDFilter<>
{
    public:
    
    explicit GenParticleFilter(const edm::ParameterSet&);
    ~GenParticleFilter();
    
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    
    private:
    
    virtual void beginStream(edm::StreamID) override;
    virtual bool filter(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    
    // ----------member data ---------------------------
    
    
    // My stuff //
    int _atLeastN;
    int _pdgId;
    
    double _minEta;
    double _maxEta;
    
    bool _debug;
    
    
    // Gen particles //
    edm::EDGetTokenT <std::vector <reco::GenParticle> > tok_genParticle;
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
GenParticleFilter::GenParticleFilter(const edm::ParameterSet& iConfig)
{
    //now do what ever initialization is needed
    
    
    _atLeastN = iConfig.getParameter <int>("atLeastN");
    _pdgId = iConfig.getParameter <int>("pdgId");
    
    _minEta = iConfig.getParameter <double>("minEta");
    _maxEta = iConfig.getParameter <double>("maxEta");
    
    _debug = iConfig.getParameter <bool>("debug");
    
    
    // Gen particles //
    tok_genParticle = consumes <std::vector <reco::GenParticle> >(iConfig.getUntrackedParameter <edm::InputTag>("label_genParticle"));
}


GenParticleFilter::~GenParticleFilter()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool GenParticleFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    
    //////////////////// Gen particle ////////////////////
    edm::Handle <std::vector <reco::GenParticle> > v_genParticle;
    iEvent.getByToken(tok_genParticle, v_genParticle);
    
    
    int genPart_n = 0;
    
    for(int iPart = 0; iPart < (int) v_genParticle->size(); iPart++)
    {
        reco::GenParticle part = v_genParticle->at(iPart);
        
        int pdgId = part.pdgId();
        int status = part.status();
        
        // Gen ele
        //if(abs(pdgId) == 11 && status == 1)
        if(abs(pdgId) == _pdgId && (part.isHardProcess() || status == 1))
        {
            //printf("[%llu] Electron found: E %0.2f, pT %0.2f, eta %+0.2f \n", eventNumber, part.energy(), part.pt(), part.eta());
            
            if(fabs(part.eta()) > _minEta && fabs(part.eta()) < _maxEta)
            {
                genPart_n++;
            }
        }
    }
    
    
    if(genPart_n >= _atLeastN)
    {
        return true;
    }
    
    
    return false;
    
    
//#ifdef THIS_IS_AN_EVENT_EXAMPLE
//   Handle<ExampleData> pIn;
//   iEvent.getByLabel("example",pIn);
//#endif
//
//#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
//   ESHandle<SetupData> pSetup;
//   iSetup.get<SetupRecord>().get(pSetup);
//#endif
//   return true;
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
GenParticleFilter::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
GenParticleFilter::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
GenParticleFilter::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
GenParticleFilter::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
GenParticleFilter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
GenParticleFilter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void GenParticleFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    //desc.setUnknown();
    desc.setAllowAnything();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(GenParticleFilter);
