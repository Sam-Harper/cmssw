// -*- C++ -*-
//
// Package:    EDAnalyzers/TreeMaker
// Class:      TreeMaker
//
/**\class TreeMaker TreeMaker.cc EDAnalyzers/TreeMaker/plugins/TreeMaker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  
//         Created:  Sat, 11 May 2019 13:14:55 GMT
//
//


// system include files
# include <memory>

// user include files



# include "CommonTools/UtilAlgos/interface/TFileService.h"
# include "DataFormats/CaloTowers/interface/CaloTowerDefs.h"
# include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
# include "DataFormats/ForwardDetId/interface/HGCEEDetId.h"
# include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
# include "DataFormats/FWLite/interface/ESHandle.h"
# include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
# include "DataFormats/HGCalReco/interface/Trackster.h"
# include "DataFormats/HGCRecHit/interface/HGCRecHit.h"
# include "DataFormats/HepMCCandidate/interface/GenParticle.h"
# include "DataFormats/JetReco/interface/PFJet.h"
# include "DataFormats/Math/interface/LorentzVector.h"
# include "DataFormats/ParticleFlowReco/interface/HGCalMultiCluster.h"
# include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
# include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
# include "DataFormats/TrackReco/interface/Track.h"
# include "DataFormats/TrackReco/interface/TrackFwd.h"
# include "DataFormats/VertexReco/interface/Vertex.h"
# include "FWCore/Framework/interface/Event.h"
# include "FWCore/Framework/interface/ESHandle.h"
# include "FWCore/Framework/interface/Frameworkfwd.h"
# include "FWCore/Framework/interface/MakerMacros.h"
# include "FWCore/Framework/interface/one/EDAnalyzer.h"
# include "FWCore/ParameterSet/interface/ParameterSet.h"
# include "FWCore/ServiceRegistry/interface/Service.h"
# include "FWCore/Utilities/interface/InputTag.h"
# include "Geometry/CaloTopology/interface/HGCalTopology.h"
# include "Geometry/Records/interface/IdealGeometryRecord.h"
# include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
# include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
# include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
# include "SimDataFormats/CaloHit/interface/PCaloHit.h"
# include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
# include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

# include "EDAnalyzers/TreeMaker/interface/Common.h"
# include "EDAnalyzers/TreeMaker/interface/Constants.h"
# include "EDAnalyzers/TreeMaker/interface/TreeOutputInfo.h"

# include <CLHEP/Matrix/Matrix.h>
# include <CLHEP/Vector/ThreeVector.h>
# include <CLHEP/Vector/ThreeVector.h>

# include <Compression.h>
# include <TH1F.h>
# include <TH2F.h>
# include <TMatrixD.h>
# include <TTree.h> 
# include <TVector2.h> 
# include <TVectorD.h> 

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.



double HGCal_minEta = 1.479;
double HGCal_maxEta = 3.0;

double el_minPt = 0; //15;
double el_maxPt = 99999; //30;


class TreeMaker : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
    public:
    
    explicit TreeMaker(const edm::ParameterSet&);
    ~TreeMaker();
    
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    
    
    private:
    
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;
    
    double getDeltaPhi(double phi1, double phi2);
    
    std::tuple <TMatrixD, TMatrixD, TVectorD> getMultiClusterPC(
        reco::HGCalMultiCluster *multiCluster,
        std::map <DetId, const HGCRecHit*> m_recHit
    );
    
    
    hgcal::RecHitTools recHitTools;
    
    int minLayer;
    int maxLayer;
    
    
    TreeOutputInfo::TreeOutput *treeOutput;
    
    
    // My stuff //
    bool debug;
    
    bool storeSimHit;
    bool storeRecHit;
    
    
    // Gen particles //
    edm::EDGetTokenT <std::vector <reco::GenParticle> > tok_genParticle;
    
    
    // Pileup //
    edm::EDGetTokenT <std::vector <PileupSummaryInfo> > tok_pileup;
    
    
    // Rho //
    edm::EDGetTokenT <double> tok_rho;
    
    
    // SimHits //
    edm::EDGetTokenT <std::vector <PCaloHit> > tok_HGCEESimHit;
    
    
    // RecHits //
    edm::EDGetTokenT <std::vector <reco::PFRecHit> > tok_PFRecHit;
    edm::EDGetTokenT <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > tok_HGCEERecHit;
    edm::EDGetTokenT <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > tok_HGCHEFRecHit;
    edm::EDGetTokenT <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > tok_HGCHEBRecHit;
    
    
    // HGCAL layer clusters //
    edm::EDGetTokenT <std::vector <reco::CaloCluster> > tok_HGCALlayerCluster;
    
    
    // TICL //
    edm::EDGetTokenT <std::vector <ticl::Trackster> > tok_TICLtrackster;
    edm::EDGetTokenT <std::vector <reco::HGCalMultiCluster> > tok_TICLmultiCluster;
    edm::EDGetTokenT <std::vector <reco::HGCalMultiCluster> > tok_TICLmultiClusterMIP;
    
    
    // Calo particles //
    edm::EDGetTokenT <std::vector <CaloParticle> > tok_caloParticle;
    
    
    // Gsf electrons from multiclusters //
    edm::EDGetTokenT <std::vector <reco::GsfElectron> > tok_gsfEleFromMultiClus;
    
    
    // Gsf electrons from TICL //
    edm::EDGetTokenT <std::vector <reco::GsfElectron> > tok_gsfEleFromTICL;
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
TreeMaker::TreeMaker(const edm::ParameterSet& iConfig)
{
    usesResource("TFileService");
    edm::Service<TFileService> fs;
    
    // Compression
    //fs->file().SetCompressionAlgorithm(ROOT::kLZMA);
    //fs->file().SetCompressionLevel(8);
    
    
    minLayer = +9999;
    maxLayer = -9999;
    
    
    //now do what ever initialization is needed
    
    treeOutput = new TreeOutputInfo::TreeOutput("tree", fs);
    
    
    // My stuff //
    debug = iConfig.getParameter <bool>("debug");
    
    storeSimHit = iConfig.getParameter <bool>("storeSimHit");
    storeRecHit = iConfig.getParameter <bool>("storeRecHit");
    
    
    // Gen particles //
    tok_genParticle = consumes <std::vector <reco::GenParticle> >(iConfig.getUntrackedParameter <edm::InputTag>("label_genParticle"));
    
    
    // Pileup //
    tok_pileup = consumes <std::vector <PileupSummaryInfo> >(iConfig.getUntrackedParameter <edm::InputTag>("label_pileup"));
    
    
    // Rho //
    tok_rho = consumes <double>(iConfig.getUntrackedParameter <edm::InputTag>("label_rho"));
    
    
    // SimHits //
    tok_HGCEESimHit = consumes <std::vector <PCaloHit> >(iConfig.getUntrackedParameter <edm::InputTag>("label_HGCEESimHit"));
    
    
    // RecHits //
    tok_PFRecHit = consumes <std::vector <reco::PFRecHit> >(iConfig.getUntrackedParameter <edm::InputTag>("label_PFRecHit"));
    
    tok_HGCEERecHit = consumes <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > >(iConfig.getUntrackedParameter <edm::InputTag>("label_HGCEERecHit"));
    tok_HGCHEFRecHit = consumes <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > >(iConfig.getUntrackedParameter <edm::InputTag>("label_HGCHEFRecHit"));
    tok_HGCHEBRecHit = consumes <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > >(iConfig.getUntrackedParameter <edm::InputTag>("label_HGCHEBRecHit"));
    
    
    // HGCAL layer clusters //
    tok_HGCALlayerCluster = consumes <std::vector <reco::CaloCluster> >(iConfig.getUntrackedParameter <edm::InputTag>("label_HGCALlayerCluster"));
    
    
    // TICL //
    tok_TICLtrackster = consumes <std::vector <ticl::Trackster> >(iConfig.getUntrackedParameter <edm::InputTag>("label_TICLtrackster"));
    tok_TICLmultiCluster = consumes <std::vector <reco::HGCalMultiCluster> >(iConfig.getUntrackedParameter <edm::InputTag>("label_TICLmultiCluster"));
    tok_TICLmultiClusterMIP = consumes <std::vector <reco::HGCalMultiCluster> >(iConfig.getUntrackedParameter <edm::InputTag>("label_TICLmultiClusterMIP"));
    
    
    // Calo particles //
    tok_caloParticle = consumes <std::vector <CaloParticle> >(iConfig.getUntrackedParameter <edm::InputTag>("label_caloParticle"));
    
    
    // Gsf electrons from multiclusters //
    tok_gsfEleFromMultiClus = consumes <std::vector <reco::GsfElectron> >(iConfig.getUntrackedParameter <edm::InputTag>("label_gsfEleFromMultiClus"));
    
    
    // Gsf electrons from TICL //
    tok_gsfEleFromTICL = consumes <std::vector <reco::GsfElectron> >(iConfig.getUntrackedParameter <edm::InputTag>("label_gsfEleFromTICL"));
}


TreeMaker::~TreeMaker()
{
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
    
    delete treeOutput;
}


//
// member functions
//

// ------------ method called for each event  ------------
void
TreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    
    long long eventNumber = iEvent.id().event();
    //printf("Event %llu \n", eventNumber);
    
    
    treeOutput->clear();
    
    recHitTools.getEventSetup(iSetup);
    
    
    //////////////////// Run info ////////////////////
    treeOutput->runNumber = iEvent.id().run();
    treeOutput->eventNumber = iEvent.id().event();
    treeOutput->luminosityNumber = iEvent.id().luminosityBlock();
    treeOutput->bunchCrossingNumber = iEvent.bunchCrossing();
    
    
    // HGCal Topology
    edm::ESHandle <HGCalTopology> handle_topo_HGCalEE;
    iSetup.get<IdealGeometryRecord>().get("HGCalEESensitive", handle_topo_HGCalEE);
    
    if(!handle_topo_HGCalEE.isValid())
    {
        printf("Error: Invalid HGCalEE topology. \n");
        
        exit(EXIT_FAILURE);
    }
    
    const auto& topo_HGCalEE = *handle_topo_HGCalEE;
    
    
    //////////////////// Gen particle ////////////////////
    edm::Handle <std::vector <reco::GenParticle> > v_genParticle;
    iEvent.getByToken(tok_genParticle, v_genParticle);
    
    std::vector <CLHEP::HepLorentzVector> v_genEl_4mom;
    
    std::vector <CLHEP::HepLorentzVector> v_genPh_4mom;
    
    
    for(int iPart = 0; iPart < (int) v_genParticle->size(); iPart++)
    {
        reco::GenParticle part = v_genParticle->at(iPart);
        
        int pdgId = part.pdgId();
        int status = part.status();
        
        // Gen ele
        //if(abs(pdgId) == 11 && status == 1)
        if(abs(pdgId) == 11 && (part.isHardProcess() || status == 1))
        {
            //printf("[%llu] Electron found: E %0.2f, pT %0.2f, eta %+0.2f \n", eventNumber, part.energy(), part.pt(), part.eta());
            
            if(fabs(part.eta()) > HGCal_minEta && fabs(part.eta()) < HGCal_maxEta && part.pt() > el_minPt && part.pt() < el_maxPt)
            {
                CLHEP::HepLorentzVector genEl_4mom;
                
                genEl_4mom.setT(part.energy());
                genEl_4mom.setX(part.px());
                genEl_4mom.setY(part.py());
                genEl_4mom.setZ(part.pz());
                
                v_genEl_4mom.push_back(genEl_4mom);
                
                treeOutput->v_genEl_E.push_back(genEl_4mom.e());
                treeOutput->v_genEl_px.push_back(genEl_4mom.px());
                treeOutput->v_genEl_py.push_back(genEl_4mom.py());
                treeOutput->v_genEl_pz.push_back(genEl_4mom.pz());
                treeOutput->v_genEl_pT.push_back(genEl_4mom.perp());
                treeOutput->v_genEl_eta.push_back(genEl_4mom.eta());
                treeOutput->v_genEl_phi.push_back(genEl_4mom.phi());
                
                treeOutput->v_genEl_multiClus_totE.push_back(0);
                
                if(part.eta() > 0)
                {
                    treeOutput->v_genEl_HGCalEEP_EsortedIndex.push_back(treeOutput->genEl_n);
                }
                
                else
                {
                    treeOutput->v_genEl_HGCalEEM_EsortedIndex.push_back(treeOutput->genEl_n);
                }
                
                treeOutput->genEl_n++;
            }
        }
        
        
        // Gen pi0
        if(abs(pdgId) == 111 && part.isLastCopy())
        {
            //printf("Pi0: E %0.2f, pT %0.2f, eta %+0.2f, phi +%0.2f \n", part.energy(), part.pt(), part.eta(), part.phi());
            
            const reco::GenParticle *photon1 = part.daughterRef(0).get();
            const reco::GenParticle *photon2 = part.daughterRef(1).get();
            
            //printf("Photon 1: E %0.2f, pT %0.2f, eta %+0.2f, phi +%0.2f \n", photon1->energy(), photon1->pt(), photon1->eta(), photon1->phi());
            //printf("Photon 2: E %0.2f, pT %0.2f, eta %+0.2f, phi +%0.2f \n", photon2->energy(), photon2->pt(), photon2->eta(), photon2->phi());
            
            CLHEP::HepLorentzVector genPh1_4mom;
            genPh1_4mom.setT(photon1->energy());
            genPh1_4mom.setX(photon1->px());
            genPh1_4mom.setY(photon1->py());
            genPh1_4mom.setZ(photon1->pz());
            
            CLHEP::HepLorentzVector genPh2_4mom;
            genPh2_4mom.setT(photon2->energy());
            genPh2_4mom.setX(photon2->px());
            genPh2_4mom.setY(photon2->py());
            genPh2_4mom.setZ(photon2->pz());
            
            v_genPh_4mom.push_back(genPh1_4mom);
            v_genPh_4mom.push_back(genPh2_4mom);
            
            //
            treeOutput->v_genPh_E.push_back(genPh1_4mom.e());
            treeOutput->v_genPh_px.push_back(genPh1_4mom.px());
            treeOutput->v_genPh_py.push_back(genPh1_4mom.py());
            treeOutput->v_genPh_pz.push_back(genPh1_4mom.pz());
            treeOutput->v_genPh_pT.push_back(genPh1_4mom.perp());
            treeOutput->v_genPh_eta.push_back(genPh1_4mom.eta());
            treeOutput->v_genPh_phi.push_back(genPh1_4mom.phi());
            treeOutput->v_genPh_multiClus_totE.push_back(0);
            
            //
            treeOutput->v_genPh_E.push_back(genPh2_4mom.e());
            treeOutput->v_genPh_px.push_back(genPh2_4mom.px());
            treeOutput->v_genPh_py.push_back(genPh2_4mom.py());
            treeOutput->v_genPh_pz.push_back(genPh2_4mom.pz());
            treeOutput->v_genPh_pT.push_back(genPh2_4mom.perp());
            treeOutput->v_genPh_eta.push_back(genPh2_4mom.eta());
            treeOutput->v_genPh_phi.push_back(genPh2_4mom.eta());
            treeOutput->v_genPh_multiClus_totE.push_back(0);
            
            if(part.eta() > 0)
            {
                treeOutput->v_genPh_HGCalEEP_EsortedIndex.push_back(treeOutput->genPh_n);
                treeOutput->v_genPh_HGCalEEP_EsortedIndex.push_back(treeOutput->genPh_n+1);
                
                treeOutput->v_genPh_HGCalEEP_deltaR.push_back(genPh1_4mom.deltaR(genPh2_4mom));
            }
            
            else
            {
                treeOutput->v_genPh_HGCalEEM_EsortedIndex.push_back(treeOutput->genPh_n);
                treeOutput->v_genPh_HGCalEEM_EsortedIndex.push_back(treeOutput->genPh_n+1);
                
                treeOutput->v_genPh_HGCalEEM_deltaR.push_back(genPh1_4mom.deltaR(genPh2_4mom));
            }
            
            treeOutput->genPh_n += 2;
        }
    }
    
    
    if(!treeOutput->genEl_n)
    {
        return;
    }
    
    
    // Sort the electrons
    std::sort(
        treeOutput->v_genEl_HGCalEEP_EsortedIndex.begin(), treeOutput->v_genEl_HGCalEEP_EsortedIndex.end(),
        [&](int iEle1, int iEle2)
        {
            return (treeOutput->v_genEl_E[iEle1] > treeOutput->v_genEl_E[iEle2]);
        }
    );
    
    std::sort(
        treeOutput->v_genEl_HGCalEEM_EsortedIndex.begin(), treeOutput->v_genEl_HGCalEEM_EsortedIndex.end(),
        [&](int iEle1, int iEle2)
        {
            return (treeOutput->v_genEl_E[iEle1] > treeOutput->v_genEl_E[iEle2]);
        }
    );
    
    
    // Sort the photons
    std::sort(
        treeOutput->v_genPh_HGCalEEP_EsortedIndex.begin(), treeOutput->v_genPh_HGCalEEP_EsortedIndex.end(),
        [&](int iEle1, int iEle2)
        {
            return (treeOutput->v_genPh_E[iEle1] > treeOutput->v_genPh_E[iEle2]);
        }
    );
    
    std::sort(
        treeOutput->v_genPh_HGCalEEM_EsortedIndex.begin(), treeOutput->v_genPh_HGCalEEM_EsortedIndex.end(),
        [&](int iEle1, int iEle2)
        {
            return (treeOutput->v_genPh_E[iEle1] > treeOutput->v_genPh_E[iEle2]);
        }
    );
    
    
    // Pileup
    edm::Handle <std::vector <PileupSummaryInfo> > pileUps_reco;
    iEvent.getByToken(tok_pileup, pileUps_reco);
    treeOutput->pileup_n = Common::getPileup(pileUps_reco);
    
    
    // Rho
    edm::Handle <double> handle_rho;
    iEvent.getByToken(tok_rho, handle_rho);
    double rho = *handle_rho;
    
    treeOutput->rho = rho;
    
    
    // SimHit dictionary
    edm::Handle <std::vector <PCaloHit> > v_HGCEESimHit;
    iEvent.getByToken(tok_HGCEESimHit, v_HGCEESimHit);
    
    std::map <DetId, const PCaloHit*> m_simHit;
    
    int nSimHit = v_HGCEESimHit->size();
    
    for(int iSimHit = 0; iSimHit < nSimHit; iSimHit++)
    {
        const PCaloHit *simHit = &(v_HGCEESimHit->at(iSimHit));
        
        DetId detId(simHit->id());
        
        m_simHit[detId] = simHit;
    }
    
    
    // RecHit dictionary
    edm::Handle <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > v_HGCEERecHit;
    iEvent.getByToken(tok_HGCEERecHit, v_HGCEERecHit);
    
    edm::Handle <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > v_HGCHEFRecHit;
    iEvent.getByToken(tok_HGCHEFRecHit, v_HGCHEFRecHit);
    
    edm::Handle <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > v_HGCHEBRecHit;
    iEvent.getByToken(tok_HGCHEBRecHit, v_HGCHEBRecHit);
    
    
    std::map <DetId, const HGCRecHit*> m_recHit;
    std::map <DetId, const HGCRecHit*> m_HGCEERecHit;
    
    int nHGCEERecHit = v_HGCEERecHit->size();
    
    for(int iRecHit = 0; iRecHit < nHGCEERecHit; iRecHit++)
    {
        const HGCRecHit *recHit = &(*v_HGCEERecHit)[iRecHit];
        
        m_recHit[recHit->id()] = recHit;
        m_HGCEERecHit[recHit->id()] = recHit;
    }
    
    
    //
    int nHGCHEFRecHit = v_HGCHEFRecHit->size();
    
    for(int iRecHit = 0; iRecHit < nHGCHEFRecHit; iRecHit++)
    {
        const HGCRecHit *recHit = &(*v_HGCHEFRecHit)[iRecHit];
        
        m_recHit[recHit->id()] = recHit;
    }
    
    
    //
    int nHGCHEBRecHit = v_HGCHEBRecHit->size();
    
    for(int iRecHit = 0; iRecHit < nHGCHEBRecHit; iRecHit++)
    {
        const HGCRecHit *recHit = &(*v_HGCHEBRecHit)[iRecHit];
        
        m_recHit[recHit->id()] = recHit;
    }
    
    
    /////////////////// HGCAL layer clusters ////////////////////
    edm::Handle <std::vector <reco::CaloCluster> > v_HGCALlayerCluster;
    iEvent.getByToken(tok_HGCALlayerCluster, v_HGCALlayerCluster);
    
    int nLayerClus = v_HGCALlayerCluster->size();
    
    std::map <DetId, int> m_HGCALlayerClusterHit;
    
    for(int iLayerClus = 0; iLayerClus < nLayerClus; iLayerClus++)
    {
        reco::CaloCluster cluster = v_HGCALlayerCluster->at(iLayerClus);
        std::vector <std::pair <DetId, float> > v_hit = cluster.hitsAndFractions();
        
        CLHEP::Hep3Vector cluster_3vec(
            cluster.x(),
            cluster.y(),
            cluster.z()
        );
        
        int layer = -1;
        
        int nHit = v_hit.size();
        double HGCALlayerClus_sigRecHit_totE = 0;
        
        for(int iHit = 0; iHit < nHit; iHit++)
        {
            auto hit = v_hit.at(iHit);
            
            DetId id = hit.first;
            double fraction = hit.second;
            
            m_HGCALlayerClusterHit[id] = iLayerClus;
            
            // Get the layer of the cluster from the hit
            if(iHit == 0)
            {
                layer = recHitTools.getLayer(id); // Start from 1
            }
            
            if(m_simHit.find(id) != m_simHit.end())
            {
                HGCALlayerClus_sigRecHit_totE += m_recHit.at(id)->energy() * fraction;
            }
        }
        
        treeOutput->HGCALlayerClus_n++;
        
        treeOutput->v_HGCALlayerClus_E.push_back(cluster.energy());
        treeOutput->v_HGCALlayerClus_x.push_back(cluster.x());
        treeOutput->v_HGCALlayerClus_y.push_back(cluster.y());
        treeOutput->v_HGCALlayerClus_z.push_back(cluster.z());
        treeOutput->v_HGCALlayerClus_eta.push_back(cluster.eta());
        treeOutput->v_HGCALlayerClus_phi.push_back(cluster.phi());
        treeOutput->v_HGCALlayerClus_ET.push_back(cluster.energy() * sin(cluster_3vec.theta()));
        treeOutput->v_HGCALlayerClus_layer.push_back(layer);
        
        treeOutput->v_HGCALlayerClus_sigRecHit_totE.push_back(HGCALlayerClus_sigRecHit_totE);
        treeOutput->v_HGCALlayerClus_sigRecHit_Efraction.push_back(HGCALlayerClus_sigRecHit_totE / cluster.energy());
    }
    
    
    //////////////////// TICL ////////////////////
    edm::Handle <std::vector <ticl::Trackster> > v_TICLtrackster;
    iEvent.getByToken(tok_TICLtrackster, v_TICLtrackster);
    
    edm::Handle <std::vector <reco::HGCalMultiCluster> > v_TICLmultiCluster;
    iEvent.getByToken(tok_TICLmultiCluster, v_TICLmultiCluster);
    
    //edm::Handle <std::vector <reco::HGCalMultiCluster> > v_TICLmultiClusterMIP;
    //iEvent.getByToken(tok_TICLmultiClusterMIP, v_TICLmultiClusterMIP);
    
    //printf("[%llu] # of TICL multiClusters: %d \n", eventNumber, (int) v_TICLmultiCluster->size());
    ////printf("[%llu] # of TICL multiClustersMIP: %d \n", eventNumber, (int) v_TICLmultiClusterMIP->size());
    
    std::vector <int> v_genEl_multiClus_n(v_genEl_4mom.size(), 0);
    std::vector <double> v_genEl_multiClus_E(v_genEl_4mom.size(), 0.0);
    std::vector <double> v_genEl_nearestMultiClus_E(v_genEl_4mom.size(), 0.0);
    
    std::vector <int> v_multiClus_genElIndex(v_TICLmultiCluster->size(), -1);
    
    for(int iGenEl = 0; iGenEl < (int) v_genEl_4mom.size(); iGenEl++)
    {
        CLHEP::HepLorentzVector genEl_4mom = v_genEl_4mom.at(iGenEl);
        
        double deltaR_min = 9999;
        
        for(int iTICLmultiCluster = 0; iTICLmultiCluster < (int) v_TICLmultiCluster->size(); iTICLmultiCluster++)
        {
            reco::HGCalMultiCluster TICLmultiCluster = v_TICLmultiCluster->at(iTICLmultiCluster);
            
            CLHEP::Hep3Vector TICLmultiCluster_3vec(
                TICLmultiCluster.x(),
                TICLmultiCluster.y(),
                TICLmultiCluster.z()
            );
            
            double deltaR = genEl_4mom.v().deltaR(TICLmultiCluster_3vec);
            
            if(deltaR < 0.4)
            {
                v_genEl_multiClus_n.at(iGenEl)++;
                v_genEl_multiClus_E.at(iGenEl) += TICLmultiCluster.energy();
                
                v_multiClus_genElIndex.at(iTICLmultiCluster) = iGenEl;
                
                if(deltaR < deltaR_min)
                {
                    deltaR_min = deltaR;
                    
                    v_genEl_nearestMultiClus_E.at(iGenEl) = TICLmultiCluster.energy();
                }
            }
        }
        
        printf(
            "[%llu] "
            "Gen-ele found: E %0.2f, pT %0.2f, eta %+0.2f, pz %+0.2f, "
            //"multiClus n %d, multiClus E %0.2f, "
            "\n",
            eventNumber,
            genEl_4mom.e(), genEl_4mom.perp(), genEl_4mom.eta(), genEl_4mom.pz()
            //v_genEl_multiClus_n.at(iGenEl),
            //v_genEl_multiClus_E.at(iGenEl)
        );
        
        treeOutput->v_genEl_multiClus_n.push_back(v_genEl_multiClus_n.at(iGenEl));
        
        treeOutput->v_genEl_nearestMultiClusEnRatio.push_back(v_genEl_nearestMultiClus_E.at(iGenEl) / genEl_4mom.e());
        treeOutput->v_genEl_multiClusEnRatio.push_back(v_genEl_multiClus_E.at(iGenEl) / genEl_4mom.e());
    }
    
    
    // Tracksters
    treeOutput->trackster_n = v_TICLtrackster->size();
    
    
    // Multiclusters
    int nTICLmultiCluster = v_TICLmultiCluster->size();
    
    int multiClus1_HGCalEEP_idx = -1;
    int multiClus1_HGCalEEM_idx = -1;
    
    double multiClus_HGCalEEP_meanX = 0;
    double multiClus_HGCalEEP_meanY = 0;
    double multiClus_HGCalEEP_meanZ = 0;
    double multiClus_HGCalEEP_meanEta = 0;
    double multiClus_HGCalEEP_meanPhi = 0;
    
    double multiClus_HGCalEEM_meanX = 0;
    double multiClus_HGCalEEM_meanY = 0;
    double multiClus_HGCalEEM_meanZ = 0;
    double multiClus_HGCalEEM_meanEta = 0;
    double multiClus_HGCalEEM_meanPhi = 0;
    
    double multiClus_HGCalEEP_totE = 0;
    double multiClus_HGCalEEP_totET = 0;
    
    double multiClus_HGCalEEM_totE = 0;
    double multiClus_HGCalEEM_totET = 0;
    
    std::vector <DetId> v_detId_temp;
    
    std::vector <std::vector <double> > v_clusterInfo_temp;
    
    // <pointer, count>
    std::map <const reco::CaloCluster*, int> m_TICLmultiClus_clus;
    
    int multiClust_clust_startIndex = 0;
    
    for(int iTICLmultiCluster = 0; iTICLmultiCluster < nTICLmultiCluster; iTICLmultiCluster++)
    {
        reco::HGCalMultiCluster TICLmultiCluster = v_TICLmultiCluster->at(iTICLmultiCluster);
        
        ticl::Trackster TICLtrackster = v_TICLtrackster->at(iTICLmultiCluster);
        
        CLHEP::Hep3Vector TICLmultiCluster_3vec(
            TICLmultiCluster.x(),
            TICLmultiCluster.y(),
            TICLmultiCluster.z()
        );
        
        treeOutput->multiClus_n++;
        
        treeOutput->v_multiClus_genElIndex.push_back(v_multiClus_genElIndex.at(iTICLmultiCluster));
        treeOutput->v_multiClus_E.push_back(TICLmultiCluster.energy());
        treeOutput->v_multiClus_x.push_back(TICLmultiCluster.x());
        treeOutput->v_multiClus_y.push_back(TICLmultiCluster.y());
        treeOutput->v_multiClus_z.push_back(TICLmultiCluster.z());
        treeOutput->v_multiClus_eta.push_back(TICLmultiCluster_3vec.eta());
        treeOutput->v_multiClus_phi.push_back(TICLmultiCluster_3vec.phi());
        treeOutput->v_multiClus_ET.push_back(TICLmultiCluster.energy() * sin(TICLmultiCluster_3vec.theta()));
        
        //treeOutput->v_multiClus_corrE.push_back(TICLtrackster.regressed_energy);
        //treeOutput->v_multiClus_corrET.push_back(TICLtrackster.regressed_energy * sin(TICLmultiCluster_3vec.theta()));
        
        //auto tup_TICLmultiClus_PCAinfo = Common::getMultiClusterPCAinfo(
        //    &TICLmultiCluster,
        //    m_recHit,
        //    &recHitTools,
        //    debug
        //);
        
        //if(iTICLmultiCluster < 2)
        //{
        //    printf(
        //        "[%llu] "
        //        "Multi-cluster %d/%d: \n",
        //        eventNumber,
        //        iTICLmultiCluster+1, nTICLmultiCluster
        //    );
        //    
        //    std::get<0>(tup_TICLmultiClus_PCAinfo).Print();
        //    std::get<1>(tup_TICLmultiClus_PCAinfo).Print();
        //    std::get<2>(tup_TICLmultiClus_PCAinfo).Print();
        //}
        
        //TMatrixD mat_TICLmultiClus_rEtaPhiCov = std::get<0>(tup_TICLmultiClus_PCAinfo);
        //TVectorD v_multiClus_rEtaPhiCov_eigVal = std::get<2>(tup_TICLmultiClus_PCAinfo);
        //
        //treeOutput->v_multiClus_sigma2rr.push_back(    mat_TICLmultiClus_rEtaPhiCov(0, 0));
        //treeOutput->v_multiClus_sigma2etaEta.push_back(mat_TICLmultiClus_rEtaPhiCov(1, 1));
        //treeOutput->v_multiClus_sigma2phiPhi.push_back(mat_TICLmultiClus_rEtaPhiCov(2, 2));
        //
        //treeOutput->v_multiClus_sigma2rEta.push_back(  mat_TICLmultiClus_rEtaPhiCov(0, 1));
        //treeOutput->v_multiClus_sigma2rPhi.push_back(  mat_TICLmultiClus_rEtaPhiCov(0, 2));
        //treeOutput->v_multiClus_sigma2etaPhi.push_back(mat_TICLmultiClus_rEtaPhiCov(1, 2));
        //
        //treeOutput->v_multiClus_sigma2diag1.push_back(v_multiClus_rEtaPhiCov_eigVal(0));
        //treeOutput->v_multiClus_sigma2diag2.push_back(v_multiClus_rEtaPhiCov_eigVal(1));
        //treeOutput->v_multiClus_sigma2diag3.push_back(v_multiClus_rEtaPhiCov_eigVal(2));
        
        treeOutput->v_multiClus_EsortedIndex.push_back(iTICLmultiCluster);
        
        
        if(TICLmultiCluster.z() > 0)
        {
            multiClus_HGCalEEP_meanX += TICLmultiCluster.energy() * TICLmultiCluster.x();
            multiClus_HGCalEEP_meanY += TICLmultiCluster.energy() * TICLmultiCluster.y();
            multiClus_HGCalEEP_meanZ += TICLmultiCluster.energy() * TICLmultiCluster.z();
            
            multiClus_HGCalEEP_meanEta += TICLmultiCluster.energy() * TICLmultiCluster_3vec.eta();
            multiClus_HGCalEEP_meanPhi += TICLmultiCluster.energy() * TICLmultiCluster_3vec.phi();
            
            multiClus_HGCalEEP_totE += TICLmultiCluster.energy();
            multiClus_HGCalEEP_totET += TICLmultiCluster.energy() * sin(TICLmultiCluster_3vec.theta());
            
            treeOutput->v_multiClus_HGCalEEP_EsortedIndex.push_back(iTICLmultiCluster);
            
            
            if(multiClus1_HGCalEEP_idx < 0 || TICLmultiCluster.energy() > v_TICLmultiCluster->at(multiClus1_HGCalEEP_idx).energy())
            {
                multiClus1_HGCalEEP_idx = iTICLmultiCluster;
            }
        }
        
        else
        {
            multiClus_HGCalEEM_meanX += TICLmultiCluster.energy() * TICLmultiCluster.x();
            multiClus_HGCalEEM_meanY += TICLmultiCluster.energy() * TICLmultiCluster.y();
            multiClus_HGCalEEM_meanZ += TICLmultiCluster.energy() * TICLmultiCluster.z();
            
            multiClus_HGCalEEM_meanEta += TICLmultiCluster.energy() * TICLmultiCluster_3vec.eta();
            multiClus_HGCalEEM_meanPhi += TICLmultiCluster.energy() * TICLmultiCluster_3vec.phi();
            
            multiClus_HGCalEEM_totE += TICLmultiCluster.energy();
            multiClus_HGCalEEM_totET += TICLmultiCluster.energy() * sin(TICLmultiCluster_3vec.theta());
            
            treeOutput->v_multiClus_HGCalEEM_EsortedIndex.push_back(iTICLmultiCluster);
            
            
            if(multiClus1_HGCalEEM_idx < 0 || TICLmultiCluster.energy() > v_TICLmultiCluster->at(multiClus1_HGCalEEM_idx).energy())
            {
                multiClus1_HGCalEEM_idx = iTICLmultiCluster;
            }
        }
        
        edm::PtrVector <reco::CaloCluster> v_cluster = TICLmultiCluster.clusters();
        
        int nCluster = v_cluster.size();
        treeOutput->v_multiClus_clus_n.push_back(nCluster);
        
        treeOutput->v_multiClus_clus_startIndex.push_back(multiClust_clust_startIndex);
        multiClust_clust_startIndex += nCluster;
        
        for(int iCluster = 0; iCluster < nCluster; iCluster++)
        {
            const reco::CaloCluster *cluster = v_cluster[iCluster].get();
            std::vector <std::pair <DetId, float> > v_hit = cluster->hitsAndFractions();
            
            CLHEP::Hep3Vector cluster_3vec(
                cluster->x(),
                cluster->y(),
                cluster->z()
            );
            
            bool isClusterRecorded = false;
            
            treeOutput->v_multiClus_clus_E.push_back(cluster->energy());
            treeOutput->v_multiClus_clus_x.push_back(cluster->x());
            treeOutput->v_multiClus_clus_y.push_back(cluster->y());
            treeOutput->v_multiClus_clus_z.push_back(cluster->z());
            treeOutput->v_multiClus_clus_eta.push_back(cluster->eta());
            treeOutput->v_multiClus_clus_phi.push_back(cluster->phi());
            treeOutput->v_multiClus_clus_ET.push_back(cluster->energy() * sin(cluster_3vec.theta()));
            
            treeOutput->v_multiClus_clus_nHit.push_back(cluster->size());
            
            //for(int iRecordedClus = 0; iRecordedClus < (int) v_clusterInfo_temp.size(); iRecordedClus++)
            //{
            //    if(
            //        v_clusterInfo_temp.at(iRecordedClus).at(0) == cluster->energy() &&
            //        v_clusterInfo_temp.at(iRecordedClus).at(1) == cluster->x() &&
            //        v_clusterInfo_temp.at(iRecordedClus).at(2) == cluster->y() &&
            //        v_clusterInfo_temp.at(iRecordedClus).at(3) == cluster->z()
            //    )
            //    {
            //        isClusterRecorded = true;
            //        break;
            //    }
            //}
            //
            //if(isClusterRecorded)
            //{
            //    printf(
            //        "[%llu] Cluster recorded before (%d): %0.2f, %0.2f, %0.2f, %0.2f \n",
            //        eventNumber,
            //        m_TICLmultiClus_clus.find(cluster) != m_TICLmultiClus_clus.end(),
            //        cluster->energy(),
            //        cluster->x(),
            //        cluster->y(),
            //        cluster->z()
            //    );
            //}
            //
            //else
            //{
            //    v_clusterInfo_temp.push_back({
            //        cluster->energy(),
            //        cluster->x(),
            //        cluster->y(),
            //        cluster->z()
            //    });
            //}
            
            if(m_TICLmultiClus_clus.find(cluster) == m_TICLmultiClus_clus.end())
            {
                m_TICLmultiClus_clus[cluster] = 1;
            }
            
            else
            {
                m_TICLmultiClus_clus[cluster]++;
            }
            
            int nHit = v_hit.size();
            
            for(int iHit = 0; iHit < nHit; iHit++)
            {
                std::pair <DetId, float> hit = v_hit.at(iHit);
                
                //HGCEEDetId detId(hit.first);
                HGCalDetId detId(hit.first);
                //HGCEEDetId detId(HGCalDetId(hit.first).rawId());
                
                //if(!detId.isHGCal())
                //{
                //    continue;
                //}
                
                //if(!detId.isEE())
                //{
                //    continue;
                //}
                
                // int layer = detId.layer();
                int layer = recHitTools.getLayer(hit.first); // Start from 1
                
                //int zside = detId.zside();
                int zside = recHitTools.zside(hit.first);
                
                if(iHit == 0)
                {
                    treeOutput->v_multiClus_clus_layer.push_back(layer);
                }
                
                //bool isDetIdRecorded = false;
                //
                //for(int iDetId = 0; iDetId < (int) v_detId_temp.size(); iDetId++)
                //{
                //    if(hit.first == v_detId_temp.at(iDetId))
                //    {
                //        isDetIdRecorded = true;
                //        break;
                //    }
                //}
                //
                //if(isDetIdRecorded)
                //{
                //    printf("[%llu] Hit recorded before: raw-detId %ud \n", eventNumber, hit.first.rawId());
                //}
                //
                //else
                //{
                //    v_detId_temp.push_back(hit.first);
                //}
                
                //if(v_multiClus_genElIndex.at(iTICLmultiCluster) >= 0)
                //{
                //    printf(
                //        "[%llu] "
                //        "multiCluster %d/%d: "
                //        "cluster %d/%d: "
                //        "hit %d/%d: "
                //        "raw detId %ud, "
                //        "layer %d, "
                //        "zside %+d, "
                //        "fraction %0.2f, "
                //        "ele z %+0.2f, "
                //        "\n",
                //        eventNumber,
                //        iTICLmultiCluster+1, nTICLmultiCluster,
                //        iCluster+1, nCluster,
                //        iHit+1, nHit,
                //        hit.first.rawId(),
                //        layer,
                //        zside,
                //        hit.second,
                //        v_genEl_4mom.at(v_multiClus_genElIndex.at(iTICLmultiCluster)).pz()
                //    );
                //}
                
                
                //if(detId.layer() < minLayer)
                //{
                //    minLayer = detId.layer();
                //}
                //
                //if(detId.layer() > maxLayer)
                //{
                //    maxLayer = detId.layer();
                //}
            }
        }
    }
    
    //printf("TICL_EEP_totE %0.2f, TICL_EEM_totE %0.2f, \n", multiClus_HGCalEEP_totE, multiClus_HGCalEEM_totE);
    
    
    // Sort the indices based on energy
    std::sort(
        treeOutput->v_multiClus_EsortedIndex.begin(), treeOutput->v_multiClus_EsortedIndex.end(),
        [&](int iEle1, int iEle2)
        {
            return (treeOutput->v_multiClus_E[iEle1] > treeOutput->v_multiClus_E[iEle2]);
        }
    );
    
    std::sort(
        treeOutput->v_multiClus_HGCalEEP_EsortedIndex.begin(), treeOutput->v_multiClus_HGCalEEP_EsortedIndex.end(),
        [&](int iEle1, int iEle2)
        {
            return (treeOutput->v_multiClus_E[iEle1] > treeOutput->v_multiClus_E[iEle2]);
        }
    );
    
    std::sort(
        treeOutput->v_multiClus_HGCalEEM_EsortedIndex.begin(), treeOutput->v_multiClus_HGCalEEM_EsortedIndex.end(),
        [&](int iEle1, int iEle2)
        {
            return (treeOutput->v_multiClus_E[iEle1] > treeOutput->v_multiClus_E[iEle2]);
        }
    );
    
    
    // Check if HGCAL layer clusters are used by TICL
    for(int iLayerClus = 0; iLayerClus < nLayerClus; iLayerClus++)
    {
        const reco::CaloCluster *cluster = &(v_HGCALlayerCluster->at(iLayerClus));
        
        bool HGCALlayerClus_isInMultiClus = false;
        
        if(m_TICLmultiClus_clus.find(cluster) != m_TICLmultiClus_clus.end())
        {
            //printf(
            //    "[%llu] HGCAL layer cluster %d used by TICL: "
            //    "E %0.2e (%0.2e), "
            //    "x %0.2e (%0.2e), "
            //    "y %0.2e (%0.2e), "
            //    "z %0.2e (%0.2e), "
            //    "\n",
            //    eventNumber,
            //    iLayerClus+1,
            //    cluster->energy(), m_TICLmultiClus_clus.find(cluster)->first->energy(),
            //    cluster->x(), m_TICLmultiClus_clus.find(cluster)->first->x(),
            //    cluster->y(), m_TICLmultiClus_clus.find(cluster)->first->y(),
            //    cluster->z(), m_TICLmultiClus_clus.find(cluster)->first->z()
            //);
            
            HGCALlayerClus_isInMultiClus = true;
        }
        
        treeOutput->v_HGCALlayerClus_isInMultiClus.push_back(HGCALlayerClus_isInMultiClus);
    }
    
    
    std::map <DetId, float> m_multiClus_clus_recHit;
    
    int iCluster = 0;
    
    // TICL multicluster clusters
    for(std::map <const reco::CaloCluster*, int>::iterator iter = m_TICLmultiClus_clus.begin(); iter != m_TICLmultiClus_clus.end(); iter++, iCluster++)
    {
        auto cluster = iter->first;
        
        CLHEP::Hep3Vector cluster_3vec(
            cluster->x(),
            cluster->y(),
            cluster->z()
        );
        
        treeOutput->v_multiClus_uniqueClus_E.push_back(cluster->energy());
        treeOutput->v_multiClus_uniqueClus_x.push_back(cluster->x());
        treeOutput->v_multiClus_uniqueClus_y.push_back(cluster->y());
        treeOutput->v_multiClus_uniqueClus_z.push_back(cluster->z());
        treeOutput->v_multiClus_uniqueClus_eta.push_back(cluster->eta());
        treeOutput->v_multiClus_uniqueClus_phi.push_back(cluster->phi());
        treeOutput->v_multiClus_uniqueClus_ET.push_back(cluster->energy() * sin(cluster_3vec.theta()));
        
        treeOutput->v_multiClus_uniqueClus_multiplicity.push_back(iter->second);
        treeOutput->v_multiClus_uniqueClus_nHit.push_back(cluster->size());
        
        std::vector <std::pair <DetId, float> > v_hit = cluster->hitsAndFractions();
        
        int nHit = v_hit.size();
        
        for(int iHit = 0; iHit < nHit; iHit++)
        {
            DetId detId = v_hit.at(iHit).first;
            double fraction = v_hit.at(iHit).second;
            
            if(iHit == 0)
            {
                treeOutput->v_multiClus_uniqueClus_layer.push_back(recHitTools.getLayer(detId));
            }
            
            double isRepeating = (m_multiClus_clus_recHit.find(detId) != m_multiClus_clus_recHit.end());
            
            std::string repeatStr = "";
            
            if(isRepeating)
            {
                repeatStr = " (repeating)";
            }
            
            else
            {
                m_multiClus_clus_recHit[detId] = fraction;
            }
            
            //printf(
            //    "[%llu] "
            //    "Cluster %d/%d: "
            //    "Hit %d/%d%s: "
            //    "raw detId %ud, "
            //    "fraction %0.2f, "
            //    "\n",
            //    eventNumber,
            //    iCluster+1, (int) m_TICLmultiClus_clus.size(),
            //    iHit+1, nHit, repeatStr.c_str(),
            //    detId.rawId(),
            //    fraction
            //);
        }
    }
    
    
    if(multiClus_HGCalEEP_totE > 0)
    {
        multiClus_HGCalEEP_meanX /= multiClus_HGCalEEP_totE;
        multiClus_HGCalEEP_meanY /= multiClus_HGCalEEP_totE;
        multiClus_HGCalEEP_meanZ /= multiClus_HGCalEEP_totE;
        
        multiClus_HGCalEEP_meanEta /= multiClus_HGCalEEP_totE;
        multiClus_HGCalEEP_meanPhi /= multiClus_HGCalEEP_totE;
    }
    
    if(multiClus_HGCalEEM_totE > 0)
    {
        multiClus_HGCalEEM_meanX /= multiClus_HGCalEEM_totE;
        multiClus_HGCalEEM_meanY /= multiClus_HGCalEEM_totE;
        multiClus_HGCalEEM_meanZ /= multiClus_HGCalEEM_totE;
        
        multiClus_HGCalEEM_meanEta /= multiClus_HGCalEEM_totE;
        multiClus_HGCalEEM_meanPhi /= multiClus_HGCalEEM_totE;
    }
    
    treeOutput->multiClus_HGCalEEP_meanX = multiClus_HGCalEEP_meanX;
    treeOutput->multiClus_HGCalEEP_meanY = multiClus_HGCalEEP_meanY;
    treeOutput->multiClus_HGCalEEP_meanZ = multiClus_HGCalEEP_meanZ;
    treeOutput->multiClus_HGCalEEP_totE = multiClus_HGCalEEP_totE;
    treeOutput->multiClus_HGCalEEP_totET = multiClus_HGCalEEP_totET;
    
    treeOutput->multiClus_HGCalEEM_meanX = multiClus_HGCalEEM_meanX;
    treeOutput->multiClus_HGCalEEM_meanY = multiClus_HGCalEEM_meanY;
    treeOutput->multiClus_HGCalEEM_meanZ = multiClus_HGCalEEM_meanZ;
    treeOutput->multiClus_HGCalEEM_totE = multiClus_HGCalEEM_totE;
    treeOutput->multiClus_HGCalEEM_totET = multiClus_HGCalEEM_totET;
    
    
    double multiClus_HGCalEEP_meanDx = 0;
    double multiClus_HGCalEEP_meanDy = 0;
    double multiClus_HGCalEEP_meanDz = 0;
    double multiClus_HGCalEEP_meanDetaSq = 0;
    double multiClus_HGCalEEP_meanDphiSq = 0;
    double multiClus_HGCalEEP_meanDetaDphi = 0;
    
    double multiClus_HGCalEEM_meanDx = 0;
    double multiClus_HGCalEEM_meanDy = 0;
    double multiClus_HGCalEEM_meanDz = 0;
    double multiClus_HGCalEEM_meanDetaSq = 0;
    double multiClus_HGCalEEM_meanDphiSq = 0;
    double multiClus_HGCalEEM_meanDetaDphi = 0;
    
    for(int iTICLmultiCluster = 0; iTICLmultiCluster < nTICLmultiCluster; iTICLmultiCluster++)
    {
        reco::HGCalMultiCluster TICLmultiCluster = v_TICLmultiCluster->at(iTICLmultiCluster);
        
        CLHEP::Hep3Vector TICLmultiCluster_3vec(
            TICLmultiCluster.x(),
            TICLmultiCluster.y(),
            TICLmultiCluster.z()
        );
        
        double multiClus1_dX = -999;
        double multiClus1_dY = -999;
        double multiClus1_dZ = -999;
        
        double multiClus1_dEta = -999;
        double multiClus1_dPhi = -999;
        double multiClus1_dR   = -999;
        
        if(TICLmultiCluster.z() > 0)
        {
            double dX = fabs(TICLmultiCluster.x() - treeOutput->multiClus_HGCalEEP_meanX);
            double dY = fabs(TICLmultiCluster.y() - treeOutput->multiClus_HGCalEEP_meanY);
            double dZ = fabs(TICLmultiCluster.z() - treeOutput->multiClus_HGCalEEP_meanZ);
            
            double dEta = fabs(TICLmultiCluster_3vec.eta() - multiClus_HGCalEEP_meanEta);
            double dPhi = fabs(TVector2::Phi_mpi_pi(TICLmultiCluster_3vec.phi() - multiClus_HGCalEEP_meanPhi));
            
            multiClus_HGCalEEP_meanDx += TICLmultiCluster.energy() * dX;
            multiClus_HGCalEEP_meanDy += TICLmultiCluster.energy() * dY;
            multiClus_HGCalEEP_meanDz += TICLmultiCluster.energy() * dZ;
            
            multiClus_HGCalEEP_meanDetaSq += TICLmultiCluster.energy() * dEta*dEta;
            multiClus_HGCalEEP_meanDphiSq += TICLmultiCluster.energy() * dPhi*dPhi;
            multiClus_HGCalEEP_meanDetaDphi += TICLmultiCluster.energy() * dEta*dPhi;
            
            treeOutput->v_multiClus_dX.push_back(dX);
            treeOutput->v_multiClus_dY.push_back(dY);
            treeOutput->v_multiClus_dZ.push_back(dZ);
            
            treeOutput->v_multiClus_dEta.push_back(dEta);
            treeOutput->v_multiClus_dPhi.push_back(dPhi);
            
            
            if(multiClus1_HGCalEEP_idx >= 0 && multiClus1_HGCalEEP_idx != iTICLmultiCluster)
            {
                multiClus1_dX = TICLmultiCluster.x() - v_TICLmultiCluster->at(multiClus1_HGCalEEP_idx).x();
                multiClus1_dY = TICLmultiCluster.y() - v_TICLmultiCluster->at(multiClus1_HGCalEEP_idx).y();
                multiClus1_dZ = TICLmultiCluster.z() - v_TICLmultiCluster->at(multiClus1_HGCalEEP_idx).z();
                
                multiClus1_dEta = fabs(TICLmultiCluster.eta()) - fabs(v_TICLmultiCluster->at(multiClus1_HGCalEEP_idx).eta());
                multiClus1_dPhi = TVector2::Phi_mpi_pi(TICLmultiCluster.phi() - v_TICLmultiCluster->at(multiClus1_HGCalEEP_idx).phi());
                multiClus1_dR   = std::sqrt(multiClus1_dX*multiClus1_dX + multiClus1_dY*multiClus1_dY + multiClus1_dZ*multiClus1_dZ);
            }
        }
        
        else
        {
            double dX = fabs(TICLmultiCluster.x() - treeOutput->multiClus_HGCalEEM_meanX);
            double dY = fabs(TICLmultiCluster.y() - treeOutput->multiClus_HGCalEEM_meanY);
            double dZ = fabs(TICLmultiCluster.z() - treeOutput->multiClus_HGCalEEM_meanZ);
            
            double dEta = fabs(TICLmultiCluster_3vec.eta() - multiClus_HGCalEEM_meanEta);
            double dPhi = fabs(TVector2::Phi_mpi_pi(TICLmultiCluster_3vec.phi() - multiClus_HGCalEEM_meanPhi));
            
            multiClus_HGCalEEM_meanDx += TICLmultiCluster.energy() * dX;
            multiClus_HGCalEEM_meanDy += TICLmultiCluster.energy() * dY;
            multiClus_HGCalEEM_meanDz += TICLmultiCluster.energy() * dZ;
            
            multiClus_HGCalEEM_meanDetaSq += TICLmultiCluster.energy() * dEta*dEta;
            multiClus_HGCalEEM_meanDphiSq += TICLmultiCluster.energy() * dPhi*dPhi;
            multiClus_HGCalEEM_meanDetaDphi += TICLmultiCluster.energy() * dEta*dPhi;
            
            treeOutput->v_multiClus_dX.push_back(dX);
            treeOutput->v_multiClus_dY.push_back(dY);
            treeOutput->v_multiClus_dZ.push_back(dZ);
            
            treeOutput->v_multiClus_dEta.push_back(dEta);
            treeOutput->v_multiClus_dPhi.push_back(dPhi);
            
            
            if(multiClus1_HGCalEEM_idx >= 0 && multiClus1_HGCalEEM_idx != iTICLmultiCluster)
            {
                multiClus1_dX = TICLmultiCluster.x() - v_TICLmultiCluster->at(multiClus1_HGCalEEM_idx).x();
                multiClus1_dY = TICLmultiCluster.y() - v_TICLmultiCluster->at(multiClus1_HGCalEEM_idx).y();
                multiClus1_dZ = TICLmultiCluster.z() - v_TICLmultiCluster->at(multiClus1_HGCalEEM_idx).z();
                
                multiClus1_dEta = fabs(TICLmultiCluster.eta()) - fabs(v_TICLmultiCluster->at(multiClus1_HGCalEEM_idx).eta());
                multiClus1_dPhi = TVector2::Phi_mpi_pi(TICLmultiCluster.phi() - v_TICLmultiCluster->at(multiClus1_HGCalEEM_idx).phi());
                multiClus1_dR   = std::sqrt(multiClus1_dX*multiClus1_dX + multiClus1_dY*multiClus1_dY + multiClus1_dZ*multiClus1_dZ);
            }
        }
        
        treeOutput->v_multiClus_mc1_dX.push_back(multiClus1_dX);
        treeOutput->v_multiClus_mc1_dY.push_back(multiClus1_dY);
        treeOutput->v_multiClus_mc1_dZ.push_back(multiClus1_dZ);
        
        treeOutput->v_multiClus_mc1_dEta.push_back(multiClus1_dEta);
        treeOutput->v_multiClus_mc1_dPhi.push_back(multiClus1_dPhi);
        treeOutput->v_multiClus_mc1_dR.push_back(multiClus1_dR);
        
        
        
        // Photon multicluster sum
        int nPhoton = v_genPh_4mom.size();
        
        int ph_index = -1;
        double ph_deltaR_min = 9999;
        
        for(int iPhoton = 0; iPhoton < nPhoton; iPhoton++)
        {
            double deltaR = TICLmultiCluster_3vec.deltaR(v_genPh_4mom.at(iPhoton).v());
            
            if(deltaR < ph_deltaR_min)
            {
                ph_deltaR_min = deltaR;
                ph_index = iPhoton;
            }
        }
        
        if(ph_index >= 0)
        {
            //printf("Adding multicluster %d to photon %d. \n", iTICLmultiCluster+1, ph_index+1);
            
            treeOutput->v_genPh_multiClus_totE.at(ph_index) += TICLmultiCluster.energy();
        }
        
        //else
        //{
        //    printf("Not adding multicluster %d to any photon. \n", iTICLmultiCluster+1);
        //}
        
        
        // Electron multicluster sum
        int nElectron = v_genEl_4mom.size();
        
        int el_index = -1;
        double el_deltaR_min = 9999;
        
        for(int iElectron = 0; iElectron < nElectron; iElectron++)
        {
            double deltaR = TICLmultiCluster_3vec.deltaR(v_genEl_4mom.at(iElectron).v());
            
            if(deltaR < el_deltaR_min)
            {
                el_deltaR_min = deltaR;
                el_index = iElectron;
            }
        }
        
        if(el_index >= 0)
        {
            //printf("Adding multicluster %d to electron %d. \n", iTICLmultiCluster+1, el_index+1);
            
            treeOutput->v_genEl_multiClus_totE.at(el_index) += TICLmultiCluster.energy();
        }
    }
    
    if(multiClus_HGCalEEP_totE > 0)
    {
        multiClus_HGCalEEP_meanDx /= multiClus_HGCalEEP_totE;
        multiClus_HGCalEEP_meanDy /= multiClus_HGCalEEP_totE;
        multiClus_HGCalEEP_meanDz /= multiClus_HGCalEEP_totE;
        
        multiClus_HGCalEEP_meanDetaSq /= multiClus_HGCalEEP_totE;
        multiClus_HGCalEEP_meanDphiSq /= multiClus_HGCalEEP_totE;
        multiClus_HGCalEEP_meanDetaDphi /= multiClus_HGCalEEP_totE;
    }
    
    if(multiClus_HGCalEEM_totE > 0)
    {
        multiClus_HGCalEEM_meanDx /= multiClus_HGCalEEM_totE;
        multiClus_HGCalEEM_meanDy /= multiClus_HGCalEEM_totE;
        multiClus_HGCalEEM_meanDz /= multiClus_HGCalEEM_totE;
        
        multiClus_HGCalEEM_meanDetaSq /= multiClus_HGCalEEM_totE;
        multiClus_HGCalEEM_meanDphiSq /= multiClus_HGCalEEM_totE;
        multiClus_HGCalEEM_meanDetaDphi /= multiClus_HGCalEEM_totE;
    }
    
    treeOutput->multiClus_HGCalEEP_meanDx = multiClus_HGCalEEP_meanDx;
    treeOutput->multiClus_HGCalEEP_meanDy = multiClus_HGCalEEP_meanDy;
    treeOutput->multiClus_HGCalEEP_meanDz = multiClus_HGCalEEP_meanDz;
    
    treeOutput->multiClus_HGCalEEM_meanDx = multiClus_HGCalEEM_meanDx;
    treeOutput->multiClus_HGCalEEM_meanDy = multiClus_HGCalEEM_meanDy;
    treeOutput->multiClus_HGCalEEM_meanDz = multiClus_HGCalEEM_meanDz;
    
    // Diagonalize (something like PCA)
    //TMatrixD matrix_HGCalEEP(2, 2);
    //
    //matrix_HGCalEEP(0, 0) = multiClus_HGCalEEP_meanDetaSq;
    //matrix_HGCalEEP(0, 1) = multiClus_HGCalEEP_meanDetaDphi;
    //matrix_HGCalEEP(1, 0) = multiClus_HGCalEEP_meanDetaDphi;
    //matrix_HGCalEEP(1, 1) = multiClus_HGCalEEP_meanDphiSq;
    //
    //TVectorD v_matrix_HGCalEEP_eigenVal;
    //TMatrixD matrix_HGCalEEP_eigenVec = matrix_HGCalEEP.EigenVectors(v_matrix_HGCalEEP_eigenVal);
    ////matrix_HGCalEEP.Print();
    ////matrix_HGCalEEP_eigenVec.Print();
    ////v_matrix_HGCalEEP_eigenVal.Print();
    //
    //treeOutput->multiClus_HGCalEEP_diag1 = sqrt(v_matrix_HGCalEEP_eigenVal[0]);
    //treeOutput->multiClus_HGCalEEP_diag2 = sqrt(v_matrix_HGCalEEP_eigenVal[1]);
    //
    //
    //// Diagonalize (something like PCA)
    //TMatrixD matrix_HGCalEEM(2, 2);
    //
    //matrix_HGCalEEM(0, 0) = multiClus_HGCalEEM_meanDetaSq;
    //matrix_HGCalEEM(0, 1) = multiClus_HGCalEEM_meanDetaDphi;
    //matrix_HGCalEEM(1, 0) = multiClus_HGCalEEM_meanDetaDphi;
    //matrix_HGCalEEM(1, 1) = multiClus_HGCalEEM_meanDphiSq;
    //
    //TVectorD v_matrix_HGCalEEM_eigenVal;
    //TMatrixD matrix_HGCalEEM_eigenVec = matrix_HGCalEEM.EigenVectors(v_matrix_HGCalEEM_eigenVal);
    ////matrix_HGCalEEM.Print();
    ////matrix_HGCalEEM_eigenVec.Print();
    ////v_matrix_HGCalEEM_eigenVal.Print();
    //
    //treeOutput->multiClus_HGCalEEM_diag1 = sqrt(v_matrix_HGCalEEM_eigenVal[0]);
    //treeOutput->multiClus_HGCalEEM_diag2 = sqrt(v_matrix_HGCalEEM_eigenVal[1]);
    
    //printf("minLayer = %d, maxLayer = %d \n", minLayer, maxLayer);
    
    
    // Calo particles
    edm::Handle <std::vector <CaloParticle> > v_caloParticle;
    iEvent.getByToken(tok_caloParticle, v_caloParticle);
    
    // <DetId, CaloParticle index>
    std::map <DetId, int> m_caloPart_simClustHit;
    
    int nCaloPart = v_caloParticle->size();
    
    for(int iCaloPart = 0; iCaloPart < nCaloPart; iCaloPart++)
    {
        CaloParticle caloPart = v_caloParticle->at(iCaloPart);
        
        treeOutput->v_caloParticle_E.push_back(caloPart.energy());
        treeOutput->v_caloParticle_px.push_back(caloPart.px());
        treeOutput->v_caloParticle_py.push_back(caloPart.py());
        treeOutput->v_caloParticle_pz.push_back(caloPart.pz());
        
        treeOutput->v_caloParticle_pT.push_back(caloPart.pt());
        treeOutput->v_caloParticle_eta.push_back(caloPart.eta());
        treeOutput->v_caloParticle_phi.push_back(caloPart.phi());
        
        treeOutput->v_caloParticle_pdgid.push_back(caloPart.pdgId());
        
        treeOutput->caloParticle_n++;
        
        //auto v_simCluster = caloPart.simClusters().refVector();
        
        //int nSimCluster = v_simCluster.size();
        
        int iSimCluster = 0;
        
        ////for(int iSimCluster = 0; iSimCluster < nSimCluster; iSimCluster++)
        //for(auto const& simCluster : caloPart.simClusters())
        //{
        //    //SimCluster simCluster = v_simCluster.at(iSimCluster);
        //    
        //    std::vector <std::pair <uint32_t, float> > v_hit = simCluster.get()->hits_and_fractions();
        //    
        //    //printf(
        //    //    "[%llu] "
        //    //    "CaloParticle %d/%d: "
        //    //    "SimCluster %d/%d: "
        //    //    "nSimHit %d "
        //    //    "nHGCEERecHit %d "
        //    //    "h&f size %d "
        //    //    "\n",
        //    //    eventNumber,
        //    //    iCaloPart+1, nCaloPart,
        //    //    iSimCluster+1, (int) caloPart.simClusters().size(),
        //    //    simCluster.get()->numberOfSimHits(),
        //    //    simCluster.get()->numberOfRecHits(),
        //    //    (int) v_hit.size()
        //    //);
        //    
        //    int nHit = v_hit.size();
        //    
        //    for(int iHit = 0; iHit < nHit; iHit++)
        //    {
        //        DetId detId(v_hit.at(iHit).first);
        //        
        //        m_caloPart_simClustHit[detId] = iCaloPart;
        //    }
        //    
        //    iSimCluster++;
        //}
    }
    
    
    // SimHits
    if(storeSimHit)
    {
        for(int iSimHit = 0; iSimHit < nSimHit; iSimHit++)
        {
            auto simHit = v_HGCEESimHit->at(iSimHit);
            
            DetId detId(simHit.id());
            
            int layer = recHitTools.getLayer(simHit.id()) - 1; // Start from 0
            int zside = recHitTools.zside(simHit.id());
            
            
            auto position = recHitTools.getPosition(simHit.id());
            
            //CLHEP::Hep3Vector recHit_3vec(position.x(), position.y(), position.z());
            //
            //printf(
            //    "[%llu] "
            //    "SimHit %d/%d: "
            //    "layer %d, "
            //    "E %0.2e, "
            //    "ET %0.2e (%0.2e), "
            //    "x %0.2f, y %0.2f, z %0.2f,"
            //    "\n",
            //    eventNumber,
            //    iSimHit+1, nSimHit,
            //    layer,
            //    simHit.energy(), 
            //    recHitTools.getPt(position, simHit.energy()), simHit.energy()*sin(recHit_3vec.theta()),
            //    position.x(), position.y(), position.z()
            //);
            
            //if(detId.layer() < minLayer)
            //{
            //    minLayer = detId.layer();
            //}
            //
            //if(detId.layer() > maxLayer)
            //{
            //    maxLayer = detId.layer();
            //}
            
            if(zside > 0)
            {
                treeOutput->v_simHit_HGCalEEPlayer_totE.at(layer) += simHit.energy();
            }
            
            else
            {
                treeOutput->v_simHit_HGCalEEMlayer_totE.at(layer) += simHit.energy();
            }
            
            bool isCaloParticleMatched = (m_caloPart_simClustHit.find(detId) != m_caloPart_simClustHit.end());
            
            
            treeOutput->v_simHit_E.push_back(simHit.energy());
            
            treeOutput->v_simHit_x.push_back(position.x());
            treeOutput->v_simHit_y.push_back(position.y());
            treeOutput->v_simHit_z.push_back(position.z());
            
            treeOutput->v_simHit_eta.push_back(position.eta());
            treeOutput->v_simHit_phi.push_back(position.phi());
            
            treeOutput->v_simHit_ET.push_back(recHitTools.getPt(position, simHit.energy()));
            
            treeOutput->v_simHit_layer.push_back(layer+1);
            treeOutput->v_simHit_zside.push_back(zside);
            treeOutput->v_simHit_isCaloParticleMatched.push_back(isCaloParticleMatched);
            
            
            treeOutput->simHit_n++;
        }
    }
    
    
    // RecHits
    if(storeRecHit)
    {
        std::vector <int> v_recHit_matchedSimHitIndex = Common::associateRecToSimHit(v_HGCEESimHit, v_HGCEERecHit);
        
        nHGCEERecHit = v_HGCEERecHit->size();
        
        for(int iRecHit = 0; iRecHit < nHGCEERecHit; iRecHit++)
        {
            auto recHit = (*v_HGCEERecHit)[iRecHit];
            
            int layer = recHitTools.getLayer(recHit.id()) - 1; // Start from 0
            int zside = recHitTools.zside(recHit.id());
            
            auto position = recHitTools.getPosition(recHit.id());
            
            //CLHEP::Hep3Vector recHit_3vec(position.x(), position.y(), position.z());
            //
            //printf(
            //    "[%llu] "
            //    "RecHit %d/%d: "
            //    "layer %d "
            //    "E %0.2f "
            //    "\n",
            //    eventNumber,
            //    iRecHit+1, nHGCEERecHit,
            //    layer,
            //    recHit.energy()
            //);
            
            //if(detId.layer() < minLayer)
            //{
            //    minLayer = detId.layer();
            //}
            //
            //if(detId.layer() > maxLayer)
            //{
            //    maxLayer = detId.layer();
            //}
            
            if(zside > 0)
            {
                treeOutput->v_recHit_HGCalEEPlayer_totE.at(layer) += recHit.energy();
            }
            
            else
            {
                treeOutput->v_recHit_HGCalEEMlayer_totE.at(layer) += recHit.energy();
            }
            
            bool isMultiClusMatched = (m_multiClus_clus_recHit.find(recHit.id()) != m_multiClus_clus_recHit.end());
            bool isCaloParticleMatched = (m_caloPart_simClustHit.find(recHit.id()) != m_caloPart_simClustHit.end());
            
            
            treeOutput->v_recHit_E.push_back(recHit.energy());
            
            treeOutput->v_recHit_x.push_back(position.x());
            treeOutput->v_recHit_y.push_back(position.y());
            treeOutput->v_recHit_z.push_back(position.z());
            
            treeOutput->v_recHit_eta.push_back(position.eta());
            treeOutput->v_recHit_phi.push_back(position.phi());
            
            treeOutput->v_recHit_ET.push_back(recHitTools.getPt(position, recHit.energy()));
            
            treeOutput->v_recHit_layer.push_back(layer+1);
            treeOutput->v_recHit_zside.push_back(zside);
            
            treeOutput->v_recHit_isMultiClusMatched.push_back(isMultiClusMatched);
            treeOutput->v_recHit_isCaloParticleMatched.push_back(isCaloParticleMatched);
            
            treeOutput->v_recHit_matchedSimHitIndex.push_back(v_recHit_matchedSimHitIndex.at(iRecHit));
            
            if(m_HGCALlayerClusterHit.find(recHit.id()) != m_HGCALlayerClusterHit.end())
            {
                treeOutput->v_recHit_matchedHGCALlayerClusIndex.push_back(
                    m_HGCALlayerClusterHit.at(recHit.id())
                );
            }
            
            else
            {
                treeOutput->v_recHit_matchedHGCALlayerClusIndex.push_back(-1);
            }
            
            //HGCalTopology::DecodedDetId decodetDetId = topo_HGCalEE.decode(recHit.id());
            //
            //treeOutput->v_recHit_iType.push_back(decodetDetId.iType);
            //treeOutput->v_recHit_iCell1.push_back(decodetDetId.iCell1);
            //treeOutput->v_recHit_iCell2.push_back(decodetDetId.iCell2);
            
            treeOutput->v_recHit_SiThickness.push_back(recHitTools.getSiThickness(recHit.id()));
            
            treeOutput->recHit_n++;
        }
    }
    
    
    
    // Gsf electrons from multiclusters
    edm::Handle <std::vector <reco::GsfElectron> > v_gsfEleFromMultiClus;
    iEvent.getByToken(tok_gsfEleFromMultiClus, v_gsfEleFromMultiClus);
    
    int nEleFromMultiClus = v_gsfEleFromMultiClus->size();
    
    std::vector <CLHEP::HepLorentzVector> v_gsfEleFromMultiClus_4mom;
    
    for(int iEle = 0; iEle < nEleFromMultiClus; iEle++)
    {
        reco::GsfElectron gsfEle = v_gsfEleFromMultiClus->at(iEle);
        
        CLHEP::HepLorentzVector gsfEleFromMultiClus_4mom;
        gsfEleFromMultiClus_4mom.setT(gsfEle.energy());
        gsfEleFromMultiClus_4mom.setX(gsfEle.px());
        gsfEleFromMultiClus_4mom.setY(gsfEle.py());
        gsfEleFromMultiClus_4mom.setZ(gsfEle.pz());
        
        v_gsfEleFromMultiClus_4mom.push_back(gsfEleFromMultiClus_4mom);
        
        treeOutput->v_gsfEleFromMultiClus_E.push_back(gsfEle.energy());
        treeOutput->v_gsfEleFromMultiClus_px.push_back(gsfEle.px());
        treeOutput->v_gsfEleFromMultiClus_py.push_back(gsfEle.py());
        treeOutput->v_gsfEleFromMultiClus_pz.push_back(gsfEle.pz());
        
        treeOutput->v_gsfEleFromMultiClus_pT.push_back(gsfEle.pt());
        treeOutput->v_gsfEleFromMultiClus_eta.push_back(gsfEle.eta());
        treeOutput->v_gsfEleFromMultiClus_phi.push_back(gsfEle.phi());
        
        
        treeOutput->gsfEleFromMultiClus_n++;
    }
    
    // TDR-ele gen-matching
    TMatrixD mat_gsfEleFromMultiClus_gelEl_deltaR;
    
    std::vector <int> v_gsfEleFromMultiClus_matchedGenEl_index;
    
    std::vector <double> v_gsfEleFromMultiClus_gelEl_minDeltaR = Common::getMinDeltaR(
        v_gsfEleFromMultiClus_4mom,
        v_genEl_4mom,
        mat_gsfEleFromMultiClus_gelEl_deltaR,
        v_gsfEleFromMultiClus_matchedGenEl_index
    );
    
    for(int iEle = 0; iEle < (int) v_gsfEleFromMultiClus_gelEl_minDeltaR.size(); iEle++)
    {
        treeOutput->v_gsfEleFromMultiClus_genEl_minDeltaR.push_back(v_gsfEleFromMultiClus_gelEl_minDeltaR.at(iEle));
        
        int index = v_gsfEleFromMultiClus_matchedGenEl_index.at(iEle);
        
        if(index >= 0)
        {
            treeOutput->v_gsfEleFromMultiClus_matchedGenEl_E.push_back(v_genEl_4mom.at(index).e());
        }
    }
    
    
    
    // Gsf electrons from TICL
    edm::Handle <std::vector <reco::GsfElectron> > v_gsfEleFromTICL;
    iEvent.getByToken(tok_gsfEleFromTICL, v_gsfEleFromTICL);
    
    int nEleFromTICL = v_gsfEleFromTICL->size();
    
    std::map <reco::SuperClusterRef, int> m_gsfEle_superClus;
    
    std::vector <CLHEP::HepLorentzVector> v_gsfEleFromTICL_4mom;
    
    for(int iEle = 0; iEle < nEleFromTICL; iEle++)
    {
        reco::GsfElectron gsfEle = v_gsfEleFromTICL->at(iEle);
        
        //if(fabs(gsfEle.eta()) < HGCal_minEta)
        //{
        //    continue;
        //}
        
        
        CLHEP::HepLorentzVector gsfEleFromTICL_4mom;
        gsfEleFromTICL_4mom.setT(gsfEle.energy());
        gsfEleFromTICL_4mom.setX(gsfEle.px());
        gsfEleFromTICL_4mom.setY(gsfEle.py());
        gsfEleFromTICL_4mom.setZ(gsfEle.pz());
        
        v_gsfEleFromTICL_4mom.push_back(gsfEleFromTICL_4mom);
        
        
        treeOutput->v_gsfEleFromTICL_E.push_back(gsfEle.energy());
        treeOutput->v_gsfEleFromTICL_px.push_back(gsfEle.px());
        treeOutput->v_gsfEleFromTICL_py.push_back(gsfEle.py());
        treeOutput->v_gsfEleFromTICL_pz.push_back(gsfEle.pz());
        
        treeOutput->v_gsfEleFromTICL_pT.push_back(gsfEle.pt());
        treeOutput->v_gsfEleFromTICL_eta.push_back(gsfEle.eta());
        treeOutput->v_gsfEleFromTICL_phi.push_back(gsfEle.phi());
        
        treeOutput->v_gsfEleFromTICL_ET.push_back(gsfEle.et());
        
        treeOutput->gsfEleFromTICL_n++;
        
        treeOutput->v_gsfEleFromTICL_superClus_E.push_back(gsfEle.superCluster()->energy());
        
        treeOutput->v_gsfEleFromTICL_superClus_etaWidth.push_back(gsfEle.superCluster()->etaWidth());
        treeOutput->v_gsfEleFromTICL_superClus_phiWidth.push_back(gsfEle.superCluster()->phiWidth());
        
        treeOutput->v_gsfEleFromTICL_superClus_nClus.push_back(gsfEle.superCluster()->clusters().size());
        treeOutput->v_gsfEleFromTICL_superClus_nHit.push_back(gsfEle.superCluster()->hitsAndFractions().size());
        
        std::vector <std::pair <DetId, float> > v_superClus_HandF = gsfEle.superCluster()->hitsAndFractions();
        math::XYZPoint superClus_xyz = gsfEle.superCluster()->position();
        
        std::vector <std::vector <std::pair <DetId, float> > > vv_superClus_layerHandF = Common::getLayerwiseHandF(
            v_superClus_HandF,
            &recHitTools,
            Constants::HGCalEE_nLayer
        );
        
        
        // PCA
        auto tup_gsfEleFromTICL_superClus_PCAinfo = Common::getSuperClusPCAinfo(
            gsfEle.superCluster(),
            m_recHit,
            &recHitTools,
            debug
        );
        
        TMatrixD mat_gsfEleFromTICL_superClus_rEtaPhiCov = std::get<0>(tup_gsfEleFromTICL_superClus_PCAinfo);
        TVectorD v_gsfEleFromTICL_superClus_rEtaPhiCov_eigVal = std::get<2>(tup_gsfEleFromTICL_superClus_PCAinfo);
        
        treeOutput->v_gsfEleFromTICL_superClus_sigma2rr.push_back(    mat_gsfEleFromTICL_superClus_rEtaPhiCov(0, 0));
        treeOutput->v_gsfEleFromTICL_superClus_sigma2etaEta.push_back(mat_gsfEleFromTICL_superClus_rEtaPhiCov(1, 1));
        treeOutput->v_gsfEleFromTICL_superClus_sigma2phiPhi.push_back(mat_gsfEleFromTICL_superClus_rEtaPhiCov(2, 2));
        
        treeOutput->v_gsfEleFromTICL_superClus_sigma2rEta.push_back(  mat_gsfEleFromTICL_superClus_rEtaPhiCov(0, 1));
        treeOutput->v_gsfEleFromTICL_superClus_sigma2rPhi.push_back(  mat_gsfEleFromTICL_superClus_rEtaPhiCov(0, 2));
        treeOutput->v_gsfEleFromTICL_superClus_sigma2etaPhi.push_back(mat_gsfEleFromTICL_superClus_rEtaPhiCov(1, 2));
        
        treeOutput->v_gsfEleFromTICL_superClus_sigma2diag1.push_back(v_gsfEleFromTICL_superClus_rEtaPhiCov_eigVal(0));
        treeOutput->v_gsfEleFromTICL_superClus_sigma2diag2.push_back(v_gsfEleFromTICL_superClus_rEtaPhiCov_eigVal(1));
        treeOutput->v_gsfEleFromTICL_superClus_sigma2diag3.push_back(v_gsfEleFromTICL_superClus_rEtaPhiCov_eigVal(2));
        
        
        treeOutput->v_gsfEleFromTICL_superClus_seed_dEta.push_back(gsfEle.superCluster()->eta() - gsfEle.superCluster()->seed().get()->eta());
        treeOutput->v_gsfEleFromTICL_superClus_seed_dPhi.push_back(TVector2::Phi_mpi_pi(gsfEle.superCluster()->phi() - gsfEle.superCluster()->seed().get()->phi()));
        
        
        // Sorted hit energies
        std::vector <std::pair <int, double> > vp_gsfEleFromTICL_superClus_sortedRecHit_index_energy = Common::getSortedHitIndex(
            gsfEle.superCluster()->hitsAndFractions(),
            m_recHit
        );
        
        double gsfEleFromTICL_superClus_recHit1_E = 0;
        double gsfEleFromTICL_superClus_recHit2_E = 0;
        
        if(vp_gsfEleFromTICL_superClus_sortedRecHit_index_energy.size() >= 1)
        {
            gsfEleFromTICL_superClus_recHit1_E = vp_gsfEleFromTICL_superClus_sortedRecHit_index_energy.at(0).second;
        }
        
        if(vp_gsfEleFromTICL_superClus_sortedRecHit_index_energy.size() >= 2)
        {
            gsfEleFromTICL_superClus_recHit2_E = vp_gsfEleFromTICL_superClus_sortedRecHit_index_energy.at(1).second;
        }
        
        treeOutput->v_gsfEleFromTICL_superClus_recHit1_E.push_back(gsfEleFromTICL_superClus_recHit1_E);
        treeOutput->v_gsfEleFromTICL_superClus_recHit2_E.push_back(gsfEleFromTICL_superClus_recHit2_E);
        
        
        //double dist_min = 9999;
        //DetId centroid_detId = 0;
        //
        //for(std::map <DetId, const HGCRecHit*>::iterator recHit_iter = m_recHit.begin(); recHit_iter != m_recHit.end(); recHit_iter++)
        //{
        //    DetId recHit_detId = recHit_iter->first;
        //    auto recHit_pos = recHitTools.getPosition(recHit_detId);
        //    
        //    //math::XYZPoint recHit_xyz(recHit_pos.x(), recHit_pos.y(), recHit_pos.z());
        //    
        //    double dX = superClus_xyz.x() - recHit_pos.x();
        //    double dY = superClus_xyz.y() - recHit_pos.y();
        //    
        //    //double dist = (superClus_xyz-recHit_xyz).rho();
        //    double dist = std::sqrt(dX*dX + dY*dY);
        //    
        //    if (dist < dist_min)
        //    {
        //        dist_min = dist;
        //        centroid_detId = recHit_detId;
        //    }
        //}
        
        
        std::pair <DetId, double> p_centroid_detId_dist = Common::getNearestCell(
            superClus_xyz.x(), superClus_xyz.y(), superClus_xyz.z(),
            v_superClus_HandF,
            &recHitTools
        );
        
        //printf("gsfEleFromTICL: pT %0.2f, eta %+0.2f, E %0.2f \n", gsfEle.pt(), gsfEle.eta(), gsfEle.energy());
        
        double dist_min = p_centroid_detId_dist.second;
        DetId centroid_detId = p_centroid_detId_dist.first;
        
        
        if(centroid_detId)
        {
            treeOutput->v_gsfEleFromTICL_superClus_nearestCellDist.push_back(dist_min);
        }
        
        else
        {
            treeOutput->v_gsfEleFromTICL_superClus_nearestCellDist.push_back(-1);
        }
        
        
        HGCEEDetId centroid_HGCEEDetId(centroid_detId.rawId());
        //auto centroidCell_pos = recHitTools.getPosition(centroid_detId);
        
        std::vector <DetId> v_neighbour7_detId = topo_HGCalEE.neighbors(centroid_detId);
        v_neighbour7_detId.push_back(centroid_detId);
        
        std::vector <DetId> v_neighbour19_detId = Common::getNeighbor19(centroid_detId, topo_HGCalEE);
        
        printf(
            "[%llu] "
            
            "gsfEleFromTICL %d/%d: "
            "E %0.2f, "
            "eta %+0.2f, "
            //"ambiguous %d, "
            
            //"\n"
            //"\t superClus E %0.2f, "
            //"size %d, "
            //"detId %llu, "
            ////"cell %d (x %+0.2f, y %+0.2f), "
            //"type %d, "
            //"neighbors %d, %d, "
            ////"sector %d, "
            //"dist %0.2e, "
            //
            //"\n"
            //"\t\t superClus seed: E %0.2f, eta %+0.2f, z %+0.2f, size %d"
            //
            //"\n"
            //"\t gsfTrack p %0.2f, "
            //
            //"\n"
            //"\t trackMomentumAtVtx p %0.2f, "// (%0.2f), "
            //"pT %0.2f, "// (%0.2f), "
            
            "\n",
            
            eventNumber,
            
            iEle+1, nEleFromTICL,
            gsfEle.energy(),
            gsfEle.eta()
            //gsfEle.ambiguous(),
            
            //gsfEle.superCluster()->energy(),
            //(int) gsfEle.superCluster()->size(),
            //(long long) centroid_detId.rawId(),
            ////centroid_HGCEEDetId.cell(), centroidCell_pos.x(), centroidCell_pos.y(),
            //topo_HGCalEE.decode(centroid_detId).iType,
            //(int) v_neighbour7_detId.size(), (int) v_neighbour19_detId.size(),
            ////centroid_HGCEEDetId.sector(),
            //dist_min,
            //
            //gsfEle.superCluster()->seed().get()->energy(),
            //gsfEle.superCluster()->seed().get()->eta(),
            //gsfEle.superCluster()->seed().get()->z(),
            //(int) gsfEle.superCluster()->seed().get()->size(),
            //
            //gsfEle.gsfTrack()->p(),
            //gsfEle.trackMomentumAtVtx().r(),// std::sqrt(gsfEle.trackMomentumAtVtx().mag2()),
            //gsfEle.trackMomentumAtVtx().rho()//, std::sqrt(gsfEle.trackMomentumAtVtx().perp2())
        );
        
        //printf("\t Same-SC: ");
        //
        //for(int jEle = 0; jEle < nEleFromTICL; jEle++)
        //{
        //    if(iEle == jEle)
        //    {
        //        continue;
        //    }
        //    
        //    reco::GsfElectron gsfEle_j = v_gsfEleFromTICL->at(jEle);
        //    
        //    printf("%d:%d, ", jEle+1, (int) (gsfEle.superCluster() == gsfEle_j.superCluster()));
        //}
        //
        //printf("\n");
        
        if(m_gsfEle_superClus.find(gsfEle.superCluster()) == m_gsfEle_superClus.end())
        {
            m_gsfEle_superClus[gsfEle.superCluster()] = 0;
        }
        
        else
        {
            m_gsfEle_superClus[gsfEle.superCluster()]++;
        }
        
        
        // Gsf-track
        treeOutput->v_gsfEleFromTICL_gsfTrack_p.push_back(gsfEle.gsfTrack()->p());
        treeOutput->v_gsfEleFromTICL_gsfTrack_px.push_back(gsfEle.gsfTrack()->px());
        treeOutput->v_gsfEleFromTICL_gsfTrack_py.push_back(gsfEle.gsfTrack()->py());
        treeOutput->v_gsfEleFromTICL_gsfTrack_pz.push_back(gsfEle.gsfTrack()->pz());
        treeOutput->v_gsfEleFromTICL_gsfTrack_pT.push_back(gsfEle.gsfTrack()->pt());
        treeOutput->v_gsfEleFromTICL_gsfTrack_eta.push_back(gsfEle.gsfTrack()->pt());
        treeOutput->v_gsfEleFromTICL_gsfTrack_phi.push_back(gsfEle.gsfTrack()->eta());
        
        // Track at vertex
        treeOutput->v_gsfEleFromTICL_trkAtVtx_p.push_back(gsfEle.trackMomentumAtVtx().r());
        treeOutput->v_gsfEleFromTICL_trkAtVtx_px.push_back(gsfEle.trackMomentumAtVtx().x());
        treeOutput->v_gsfEleFromTICL_trkAtVtx_py.push_back(gsfEle.trackMomentumAtVtx().y());
        treeOutput->v_gsfEleFromTICL_trkAtVtx_pz.push_back(gsfEle.trackMomentumAtVtx().z());
        treeOutput->v_gsfEleFromTICL_trkAtVtx_pT.push_back(gsfEle.trackMomentumAtVtx().rho());
        treeOutput->v_gsfEleFromTICL_trkAtVtx_eta.push_back(gsfEle.trackMomentumAtVtx().eta());
        treeOutput->v_gsfEleFromTICL_trkAtVtx_phi.push_back(gsfEle.trackMomentumAtVtx().phi());
        
        
        double totalE = 0;
        
        double energy7 = 0;
        double energy19 = 0;
        double energy2p8 = 0;
        
        for(int iLayer = 0; iLayer < Constants::HGCalEE_nLayer; iLayer++)
        {
            if(!vv_superClus_layerHandF.at(iLayer).size())
            {
                continue;
            }
            
            // Get the centroid in the layer
            std::pair <math::XYZPoint, double> p_iLayer_centroid_xyz_E = Common::getCentroid(
                vv_superClus_layerHandF.at(iLayer),
                m_recHit,
                &recHitTools
            );
            
            math::XYZPoint iLayer_centroid_xyz = p_iLayer_centroid_xyz_E.first;
            
            
            // Get the rec-hit cell nearest to the centroid in the layer
            std::pair <DetId, double> p_iLayer_centroid_detId_dist = Common::getNearestCell(
                iLayer_centroid_xyz.x(), iLayer_centroid_xyz.y(), iLayer_centroid_xyz.z(),
                vv_superClus_layerHandF.at(iLayer),
                &recHitTools
            );
            
            DetId iLayer_centroid_detId = p_iLayer_centroid_detId_dist.first;
            
            if(!iLayer_centroid_detId)
            {
                continue;
            }
            
            //auto iLayer_centroid_cellPos = recHitTools.getPosition(iLayer_centroid_detId);
            
            
            // R7 cells
            //std::vector <DetId> v_iLayer_neighbour7_detId = topo_HGCalEE.neighbors(iLayer_centroid_detId);
            //v_iLayer_neighbour7_detId.push_back(iLayer_centroid_detId);
            
            
            
            //printf("Layer %02d: (x, y) (%+0.2f. %+0.2f), neighbors %d, \n", iLayer+1, iLayer_centroid_cellPos.x(), iLayer_centroid_cellPos.y(), (int) v_iLayer_neighbour7_detId.size()-1);
            //printf("\t ");
            //
            //for(int iR7cell = 0; iR7cell < (int) v_iLayer_neighbour7_detId.size()-1; iR7cell++)
            //{
            //    auto pos_temp = recHitTools.getPosition(v_iLayer_neighbour7_detId.at(iR7cell));
            //    math::XYZPoint xyz_temp(pos_temp.x(), pos_temp.y(), pos_temp.z());
            //    
            //    //double dist_temp = std::sqrt(std::pow(pos_temp.x()-iLayer_centroid_cellPos.x(), 2) + std::pow(pos_temp.y()-iLayer_centroid_cellPos.y(), 2) + std::pow(pos_temp.z()-iLayer_centroid_cellPos.z(), 2));
            //    double dist_temp = std::sqrt(std::pow(pos_temp.x()-iLayer_centroid_cellPos.x(), 2) + std::pow(pos_temp.y()-iLayer_centroid_cellPos.y(), 2));
            //    
            //    printf("%0.4f, ", dist_temp);
            //}
            //
            //printf("\n");
            
            
            // R19 cells
            //std::vector <DetId> v_iLayer_neighbour19_detId = Common::getNeighbor19(iLayer_centroid_detId, topo_HGCalEE);
            //
            //
            //double iLayer_energy7 = Common::getEnergySum(
            //    v_iLayer_neighbour7_detId,
            //    vv_superClus_layerHandF.at(iLayer),
            //    m_recHit
            //);
            //
            //double iLayer_energy19 = Common::getEnergySum(
            //    v_iLayer_neighbour19_detId,
            //    vv_superClus_layerHandF.at(iLayer),
            //    m_recHit
            //);
            //
            //energy7 += iLayer_energy7;
            //energy19 += iLayer_energy19;
            
            
            // R2p8 cells
            std::vector <DetId> v_iLayer_neighbourR2p8_detId = Common::getNeighborR(
                iLayer_centroid_detId,
                vv_superClus_layerHandF.at(iLayer),
                2.8,
                topo_HGCalEE,
                &recHitTools
            );
            
            double iLayer_energy2p8 = Common::getEnergySum(
                v_iLayer_neighbourR2p8_detId,
                vv_superClus_layerHandF.at(iLayer),
                m_recHit
            );
            
            energy2p8 += iLayer_energy2p8;
            
            
            //printf(
            //    "Layer %02d/%02d: "
            //    "(%+0.2f, %+0.2f, %+0.2f), "
            //    "(%+0.2f, %+0.2f, %+0.2f), "
            //    "nHit %03d, "
            //    "E %0.2f, "
            //    "E7 (R7) %0.2f (%0.2f), "
            //    "E19 (R19) %0.2f (%0.2f), "
            //    "\n",
            //    
            //    iLayer+1, Constants::HGCalEE_nLayer,
            //    iLayer_centroid_xyz.x(), iLayer_centroid_xyz.y(), iLayer_centroid_xyz.z(),
            //    iLayer_centroid_cellPos.x(), iLayer_centroid_cellPos.y(), iLayer_centroid_cellPos.z(),
            //    (int) vv_superClus_layerHandF.at(iLayer).size(),
            //    p_iLayer_centroid_xyz_E.second,
            //    iLayer_energy7, iLayer_energy7/p_iLayer_centroid_xyz_E.second,
            //    iLayer_energy19, iLayer_energy19/p_iLayer_centroid_xyz_E.second
            //);
            
            totalE += p_iLayer_centroid_xyz_E.second;
        }
        
        double R7 = energy7/gsfEle.energy();
        double R19 = energy19/gsfEle.energy();
        double R2p8 = energy2p8/gsfEle.energy();
        
        treeOutput->v_gsfEleFromTICL_E7.push_back(energy7);
        treeOutput->v_gsfEleFromTICL_R7.push_back(R7);
        
        treeOutput->v_gsfEleFromTICL_E19.push_back(energy19);
        treeOutput->v_gsfEleFromTICL_R19.push_back(R19);
        
        treeOutput->v_gsfEleFromTICL_E2p8.push_back(energy2p8);
        treeOutput->v_gsfEleFromTICL_R2p8.push_back(R2p8);
        
        //printf("Sum[layer]: E %0.2f \n", totalE);
        //
        //printf(
        //    "E7 (R7) %0.2f (%0.2f), "
        //    "E19 (R19) %0.2f (%0.2f), "
        //    "E2p8 (R2p8) %0.2f (%0.2f), "
        //    "\n",
        //    
        //    energy7, energy7/gsfEle.superCluster()->energy(),
        //    energy19, energy19/gsfEle.superCluster()->energy(),
        //    energy2p8, energy2p8/gsfEle.superCluster()->energy()
        //);
        
        
        treeOutput->v_gsfEleFromTICL_superClus_cellNeighbour1ringWindow_n.push_back(v_neighbour7_detId.size());
        treeOutput->v_gsfEleFromTICL_superClus_cellNeighbour2ringWindow_n.push_back(v_neighbour19_detId.size());
        
        //for(int iCell = 0; iCell < (int) v_neighbour19_detId.size(); iCell++)
        //{
        //    DetId cell_detId = v_neighbour19_detId.at(iCell);
        //    auto cell_pos = recHitTools.getPosition(cell_detId);
        //    
        //    printf(
        //        "%02d/%02d: "
        //        "%llu, "
        //        "x %+0.2f, y %+0.2f, "
        //        "type %d "
        //        "\n",
        //        
        //        iCell+1, (int) v_neighbour19_detId.size(),
        //        (long long) cell_detId.rawId(),
        //        cell_pos.x(), cell_pos.y(),
        //        topo_HGCalEE.decode(cell_detId).iType
        //    );
        //}
        
        
        const reco::CaloCluster *superClus_seed = gsfEle.superCluster()->seed().get();
        
        CLHEP::Hep3Vector superClus_seed_3vec(
            superClus_seed->x(),
            superClus_seed->y(),
            superClus_seed->z()
        );
        
        treeOutput->v_gsfEleFromTICL_superClusSeed_E.push_back(superClus_seed->energy());
        treeOutput->v_gsfEleFromTICL_superClusSeed_ET.push_back(superClus_seed->energy() * sin(superClus_seed_3vec.theta()));
        treeOutput->v_gsfEleFromTICL_superClusSeed_eta.push_back(superClus_seed->eta());
        treeOutput->v_gsfEleFromTICL_superClusSeed_phi.push_back(superClus_seed->phi());
        
        
        //
        std::vector <double> v_gsfEleFromTICL_superClusSeed_clus_dEta;
        std::vector <double> v_gsfEleFromTICL_superClusSeed_clus_dPhi;
        
        // REMEMBER to add the vectors to this!
        std::vector <std::vector <double>* > vv_gsfEleFromTICL_superClus_clus = {
            &v_gsfEleFromTICL_superClusSeed_clus_dEta,
            &v_gsfEleFromTICL_superClusSeed_clus_dPhi,
        };
        
        int superClus_clus_seed_index = -1;
        double superClus_clus_dEta_min = 9999;
        double superClus_clus_dPhi_min = 9999;
        
        edm::PtrVector <reco::CaloCluster> v_superClus_clus = gsfEle.superCluster()->clusters();
        
        for(int iCluster = 0, count = 0; iCluster < (int) v_superClus_clus.size(); iCluster++)
        {
            const reco::CaloCluster *cluster = v_superClus_clus[iCluster].get();
            
            // Must be in the same half of the detector
            if(gsfEle.eta() && cluster->eta()/gsfEle.eta() < 0)
            {
                continue;
            }
            
            //CLHEP::Hep3Vector cluster_3vec(
            //    cluster->x(),
            //    cluster->y(),
            //    cluster->z()
            //);
            
            double dEta = fabs(cluster->eta()) - fabs(superClus_seed->eta());
            double dPhi = TVector2::Phi_mpi_pi(cluster->phi() - superClus_seed->phi());
            
            if(fabs(dEta) < superClus_clus_dEta_min && fabs(dPhi) < superClus_clus_dPhi_min)
            {
                superClus_clus_dEta_min = fabs(dEta);
                superClus_clus_dPhi_min = fabs(dPhi);
                
                superClus_clus_seed_index = count;
            }
            
            v_gsfEleFromTICL_superClusSeed_clus_dEta.push_back(dEta);
            v_gsfEleFromTICL_superClusSeed_clus_dPhi.push_back(dPhi);
            
            count++;
        }
        
        // Remove the seed
        if(superClus_clus_seed_index >= 0)
        {
            for(int i = 0; i < (int) vv_gsfEleFromTICL_superClus_clus.size(); i++)
            {
                if(superClus_clus_seed_index >= (int) vv_gsfEleFromTICL_superClus_clus.at(i)->size())
                {
                    printf(
                        "Error: Invalid index (%d/%d) when deleting seed from vv_gsfEleFromTICL_superClus_clus[%d]. \n",
                        superClus_clus_seed_index,
                        (int) vv_gsfEleFromTICL_superClus_clus.at(i)->size()-1,
                        i
                    );
                    exit(EXIT_FAILURE);
                }
                
                vv_gsfEleFromTICL_superClus_clus.at(i)->erase(
                    vv_gsfEleFromTICL_superClus_clus.at(i)->begin() + superClus_clus_seed_index
                );
            }
        }
        
        treeOutput->vv_gsfEleFromTICL_superClusSeed_clus_dEta.push_back(v_gsfEleFromTICL_superClusSeed_clus_dEta);
        treeOutput->vv_gsfEleFromTICL_superClusSeed_clus_dPhi.push_back(v_gsfEleFromTICL_superClusSeed_clus_dPhi);
        
        
        
        //
        std::vector <double> v_gsfEleFromTICL_superClus_TICLclus_E;
        std::vector <double> v_gsfEleFromTICL_superClus_TICLclus_ET;
        std::vector <double> v_gsfEleFromTICL_superClus_TICLclus_nClus;
        
        std::vector <double> v_gsfEleFromTICL_superClusSeed_TICLclus_dX;
        std::vector <double> v_gsfEleFromTICL_superClusSeed_TICLclus_dY;
        std::vector <double> v_gsfEleFromTICL_superClusSeed_TICLclus_dZ;
        
        std::vector <double> v_gsfEleFromTICL_superClusSeed_TICLclus_dEta;
        std::vector <double> v_gsfEleFromTICL_superClusSeed_TICLclus_dPhi;
        std::vector <double> v_gsfEleFromTICL_superClusSeed_TICLclus_dR;
        
        // REMEMBER to add the vectors to this!
        std::vector <std::vector <double>* > vv_gsfEleFromTICL_superClus_TICLclus = {
            &v_gsfEleFromTICL_superClus_TICLclus_E,
            &v_gsfEleFromTICL_superClus_TICLclus_ET,
            &v_gsfEleFromTICL_superClus_TICLclus_nClus,
            
            &v_gsfEleFromTICL_superClusSeed_TICLclus_dX,
            &v_gsfEleFromTICL_superClusSeed_TICLclus_dY,
            &v_gsfEleFromTICL_superClusSeed_TICLclus_dZ,
            
            &v_gsfEleFromTICL_superClusSeed_TICLclus_dEta,
            &v_gsfEleFromTICL_superClusSeed_TICLclus_dPhi,
            &v_gsfEleFromTICL_superClusSeed_TICLclus_dR,
        };
        
        int superClus_TICLclus_seed_index = -1;
        double superClus_TICLclus_dEta_min = 9999;
        double superClus_TICLclus_dPhi_min = 9999;
        
        for(int iTICLmultiCluster = 0, count = 0; iTICLmultiCluster < nTICLmultiCluster; iTICLmultiCluster++)
        {
            reco::HGCalMultiCluster TICLmultiCluster = v_TICLmultiCluster->at(iTICLmultiCluster);
            
            // Must be in the same half of the detector
            if(gsfEle.eta() && TICLmultiCluster.eta()/gsfEle.eta() < 0)
            {
                continue;
            }
            
            CLHEP::Hep3Vector TICLmultiCluster_3vec(
                TICLmultiCluster.x(),
                TICLmultiCluster.y(),
                TICLmultiCluster.z()
            );
            
            double dX = TICLmultiCluster.x() - superClus_seed->x();
            double dY = TICLmultiCluster.y() - superClus_seed->y();
            double dZ = TICLmultiCluster.z() - superClus_seed->z();
            
            double dEta = fabs(TICLmultiCluster.eta()) - fabs(superClus_seed->eta());
            double dPhi = TVector2::Phi_mpi_pi(TICLmultiCluster.phi() - superClus_seed->phi());
            double dR   = sqrt(dX*dX + dY*dY + dZ*dZ);
            //double dR = TICLmultiCluster_3vec.r() - superClus_seed->position().r();
            
            if(fabs(dEta) < superClus_TICLclus_dEta_min && fabs(dPhi) < superClus_TICLclus_dPhi_min)
            {
                superClus_TICLclus_dEta_min = fabs(dEta);
                superClus_TICLclus_dPhi_min = fabs(dPhi);
                
                superClus_TICLclus_seed_index = count;
            }
            
            // Skip the same cluster
            //if(fabs(dEta) < 1e-3 && fabs(dPhi) < 1e-3)
            //{
            //    continue;
            //}
            
            v_gsfEleFromTICL_superClus_TICLclus_E.push_back(TICLmultiCluster.energy());
            v_gsfEleFromTICL_superClus_TICLclus_ET.push_back(TICLmultiCluster.energy() * sin(TICLmultiCluster_3vec.theta()));
            v_gsfEleFromTICL_superClus_TICLclus_nClus.push_back(TICLmultiCluster.clusters().size());
            
            v_gsfEleFromTICL_superClusSeed_TICLclus_dX.push_back(dX);
            v_gsfEleFromTICL_superClusSeed_TICLclus_dY.push_back(dY);
            v_gsfEleFromTICL_superClusSeed_TICLclus_dZ.push_back(dZ);
            
            v_gsfEleFromTICL_superClusSeed_TICLclus_dEta.push_back(dEta);
            v_gsfEleFromTICL_superClusSeed_TICLclus_dPhi.push_back(dPhi);
            v_gsfEleFromTICL_superClusSeed_TICLclus_dR.push_back(dR);
            
            count++;
        }
        
        treeOutput->v_gsfEleFromTICL_superClus_TICLclus_n.push_back(v_gsfEleFromTICL_superClus_TICLclus_E.size());
        
        // Remove the seed
        if(superClus_TICLclus_seed_index >= 0)
        {
            for(int i = 0; i < (int) vv_gsfEleFromTICL_superClus_TICLclus.size(); i++)
            {
                if(superClus_TICLclus_seed_index >= (int) vv_gsfEleFromTICL_superClus_TICLclus.at(i)->size())
                {
                    printf(
                        "Error: Invalid index (%d/%d) when deleting seed from vv_gsfEleFromTICL_superClus_TICLclus[%d]. \n",
                        superClus_TICLclus_seed_index,
                        (int) vv_gsfEleFromTICL_superClus_TICLclus.at(i)->size()-1,
                        i
                    );
                    exit(EXIT_FAILURE);
                }
                
                vv_gsfEleFromTICL_superClus_TICLclus.at(i)->erase(
                    vv_gsfEleFromTICL_superClus_TICLclus.at(i)->begin() + superClus_TICLclus_seed_index
                );
            }
        }
        
        treeOutput->vv_gsfEleFromTICL_superClus_TICLclus_E.push_back(v_gsfEleFromTICL_superClus_TICLclus_E);
        treeOutput->vv_gsfEleFromTICL_superClus_TICLclus_ET.push_back(v_gsfEleFromTICL_superClus_TICLclus_ET);
        treeOutput->vv_gsfEleFromTICL_superClus_TICLclus_nClus.push_back(v_gsfEleFromTICL_superClus_TICLclus_nClus);
        
        treeOutput->vv_gsfEleFromTICL_superClusSeed_TICLclus_dX.push_back(v_gsfEleFromTICL_superClusSeed_TICLclus_dX);
        treeOutput->vv_gsfEleFromTICL_superClusSeed_TICLclus_dY.push_back(v_gsfEleFromTICL_superClusSeed_TICLclus_dY);
        treeOutput->vv_gsfEleFromTICL_superClusSeed_TICLclus_dZ.push_back(v_gsfEleFromTICL_superClusSeed_TICLclus_dZ);
        
        treeOutput->vv_gsfEleFromTICL_superClusSeed_TICLclus_dEta.push_back(v_gsfEleFromTICL_superClusSeed_TICLclus_dEta);
        treeOutput->vv_gsfEleFromTICL_superClusSeed_TICLclus_dPhi.push_back(v_gsfEleFromTICL_superClusSeed_TICLclus_dPhi);
        treeOutput->vv_gsfEleFromTICL_superClusSeed_TICLclus_dR.push_back(v_gsfEleFromTICL_superClusSeed_TICLclus_dR);
    }
    
    printf("m_gsfEle_superClus.size(): %d \n", (int) m_gsfEle_superClus.size() );
    
    
    // TICL-ele gen-matching
    TMatrixD mat_gsfEleFromTICL_gelEl_deltaR;
    
    std::vector <int> v_gsfEleFromTICL_matchedGenEl_index;
    
    std::vector <double> v_gsfEleFromTICL_gelEl_minDeltaR = Common::getMinDeltaR(
        v_gsfEleFromTICL_4mom,
        v_genEl_4mom,
        mat_gsfEleFromTICL_gelEl_deltaR,
        v_gsfEleFromTICL_matchedGenEl_index
    );
    
    for(int iEle = 0; iEle < (int) v_gsfEleFromTICL_gelEl_minDeltaR.size(); iEle++)
    {
        treeOutput->v_gsfEleFromTICL_genEl_minDeltaR.push_back(v_gsfEleFromTICL_gelEl_minDeltaR.at(iEle));
        
        int index = v_gsfEleFromTICL_matchedGenEl_index.at(iEle);
        
        if(index >= 0)
        {
            treeOutput->v_gsfEleFromTICL_matchedGenEl_E.push_back(v_genEl_4mom.at(index).e());
        }
    }
    
    
    
    // PFRecHits
    //edm::Handle <std::vector <reco::PFRecHit> > v_PFRecHit;
    //iEvent.getByToken(tok_PFRecHit, v_PFRecHit);
    //
    //int nPFRecHit = v_PFRecHit->size();
    ////printf("[%llu] # of PFRecHits: %d \n", eventNumber, nPFRecHit);
    //
    //for(int iPFRecHit = 0; iPFRecHit < nPFRecHit; iPFRecHit++)
    //{
    //    auto recHit = v_PFRecHit->at(iPFRecHit);
    //    
    //    //HGCEEDetId detId(recHit.id());
    //    HGCalDetId detId(recHit.detId());
    //    
    //    // int layer = detId.layer();
    //    int layer = recHitTools.getLayer(recHit.detId()) - 1; // Start from 0
    //    
    //    //int zside = detId.zside();
    //    int zside = recHitTools.zside(recHit.detId());
    //    
    //    printf(
    //        "[%llu] "
    //        "RecHit %d/%d: "
    //        "layer %d (%d), "
    //        "zside %+d, "
    //        "E %0.2f, "
    //        "\n",
    //        eventNumber,
    //        iPFRecHit+1, nPFRecHit,
    //        detId.layer(), recHit.layer(),
    //        zside,
    //        recHit.energy()
    //    );
    //    
    //    //if(detId.layer() < minLayer)
    //    //{
    //    //    minLayer = detId.layer();
    //    //}
    //    //
    //    //if(detId.layer() > maxLayer)
    //    //{
    //    //    maxLayer = detId.layer();
    //    //}
    //    
    //    //if(zside > 0)
    //    //{
    //    //    treeOutput->v_recHit_HGCalEEPlayer_totE.at(layer) += recHit.energy();
    //    //}
    //    //
    //    //else
    //    //{
    //    //    treeOutput->v_recHit_HGCalEEMlayer_totE.at(layer) += recHit.energy();
    //    //}
    //}
    
    // Fill tree
    treeOutput->fill();
    
    //#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
    //ESHandle<SetupData> pSetup;
    //iSetup.get<SetupRecord>().get(pSetup);
    //#endif
    
    printf("\n\n");
    
    fflush(stdout);
    fflush(stderr);
}


double TreeMaker::getDeltaPhi(double phi1, double phi2)
{
    double deltaPhi = phi1 - phi2;
    
    deltaPhi = (deltaPhi > +M_PI)? (deltaPhi - 2*M_PI): deltaPhi;
    deltaPhi = (deltaPhi < -M_PI)? (deltaPhi + 2*M_PI): deltaPhi;
    
    //deltaPhi = (deltaPhi > +M_PI)? (2*M_PI - deltaPhi): deltaPhi;
    //deltaPhi = (deltaPhi < -M_PI)? (2*M_PI + deltaPhi): deltaPhi;
    
    return deltaPhi;
}


// ------------ method called once each job just before starting event loop  ------------
void
TreeMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
TreeMaker::endJob()
{
    
    printf("minLayer = %d, maxLayer = %d \n", minLayer, maxLayer);
    
    
    fflush(stdout);
    fflush(stderr);
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TreeMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TreeMaker);
