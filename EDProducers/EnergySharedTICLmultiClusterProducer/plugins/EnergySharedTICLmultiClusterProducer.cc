// -*- C++ -*-
//
// Package:    EDProducers/EnergySharedTICLmultiClusterProducer
// Class:      EnergySharedTICLmultiClusterProducer
// 
/**\class EnergySharedTICLmultiClusterProducer EnergySharedTICLmultiClusterProducer.cc EDProducers/EnergySharedTICLmultiClusterProducer/plugins/EnergySharedTICLmultiClusterProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Soham Bhattacharya
//         Created:  Fri, 31 May 2019 19:55:49 GMT
//
//


// system include files
#include <memory>

// user include files
//# include "CommonTools/UtilAlgos/interface/TFileService.h"
//# include "DataFormats/CaloTowers/interface/CaloTowerDefs.h"
//# include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
//# include "DataFormats/ForwardDetId/interface/HGCEEDetId.h"
//# include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
//# include "DataFormats/HGCRecHit/interface/HGCRecHit.h"
//# include "DataFormats/HepMCCandidate/interface/GenParticle.h"
//# include "DataFormats/JetReco/interface/PFJet.h"
//# include "DataFormats/Math/interface/LorentzVector.h"
//# include "DataFormats/ParticleFlowReco/interface/HGCalMultiCluster.h"
//# include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
//# include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
//# include "DataFormats/TrackReco/interface/Track.h"
//# include "DataFormats/TrackReco/interface/TrackFwd.h"
//# include "DataFormats/VertexReco/interface/Vertex.h"
//# include "FWCore/Framework/interface/stream/EDProducer.h"
//# include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
//# include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
//# include "FWCore/ServiceRegistry/interface/Service.h"
//# include "FWCore/Utilities/interface/InputTag.h"
//# include "FWCore/Utilities/interface/StreamID.h"
//# include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
//# include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
//# include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
//# include "SimDataFormats/CaloHit/interface/PCaloHit.h"
//# include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
//# include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

# include "FWCore/Framework/interface/Frameworkfwd.h"
# include "FWCore/Framework/interface/stream/EDProducer.h"
# include "FWCore/Framework/interface/Event.h"
# include "FWCore/Framework/interface/MakerMacros.h"
# include "FWCore/ParameterSet/interface/ParameterSet.h"
# include "FWCore/Utilities/interface/StreamID.h"
# include "DataFormats/ParticleFlowReco/interface/HGCalMultiCluster.h"

# include "DataFormats/ForwardDetId/interface/HGCEEDetId.h"
# include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
# include "DataFormats/HGCRecHit/interface/HGCRecHit.h"
# include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
# include "DataFormats/Common/interface/SortedCollection.h"


# include <CLHEP/Matrix/Matrix.h>
# include <CLHEP/Vector/ThreeVector.h>
# include <CLHEP/Vector/ThreeVector.h>

# include <Compression.h>
# include <TH1F.h>
# include <TH2F.h>
# include <TMatrixD.h>
# include <TTree.h> 
# include <TVectorD.h> 

//
// class declaration
//

class EnergySharedTICLmultiClusterProducer : public edm::stream::EDProducer<>
{
    public:
    
    explicit EnergySharedTICLmultiClusterProducer(const edm::ParameterSet&);
    ~EnergySharedTICLmultiClusterProducer();
    
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    
    private:
    
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;
    
    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    
    // ----------member data ---------------------------
    
    double showerSpread;
    
    std::vector <std::string> v_algoTypeStr;
    std::vector <std::string> v_distTypeStr;
    
    hgcal::RecHitTools recHitTools;
    
    
    // From config file
    bool debug;
    
    std::string instanceName;
    std::string algoTypeStr;
    std::string distTypeStr;
    
    edm::EDGetTokenT <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > tok_HGCEERecHit;
    edm::EDGetTokenT <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > tok_HGCHEFRecHit;
    edm::EDGetTokenT <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > tok_HGCHEBRecHit;
    
    edm::EDGetTokenT <std::vector <reco::HGCalMultiCluster> > tok_TICLmultiCluster;
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
EnergySharedTICLmultiClusterProducer::EnergySharedTICLmultiClusterProducer(const edm::ParameterSet& iConfig)
{
    //register your products
    /* Examples
    produces<ExampleData2>();
    
    //if do put with a label
    produces<ExampleData2>("label");
    
    //if you want to put into the Run
    produces<ExampleData2,InRun>();
    */
    //now do what ever other initialization is needed
    
    v_algoTypeStr = {"Mean", "Expo", "Gaus"};
    v_distTypeStr = {"2Ddist", "3Ddist"};
    
    
    // From config file
    showerSpread = iConfig.getParameter <double>("showerSpread");
    
    debug = iConfig.getParameter <bool>("debug");
    
    instanceName = iConfig.getParameter <std::string>("instanceName");
    algoTypeStr = iConfig.getParameter <std::string>("algoTypeStr");
    distTypeStr = iConfig.getParameter <std::string>("distTypeStr");
    
    if(std::find(v_algoTypeStr.begin(), v_algoTypeStr.end(), algoTypeStr) == v_algoTypeStr.end())
    {
        printf("Error in EnergySharedTICLmultiClusterProducer::EnergySharedTICLmultiClusterProducer(...). \n");
        printf("Invalid algoTypeStr provided (\"%s\"); must be one of the following: \n", algoTypeStr.c_str());
        
        for(int iAlgoType = 0; iAlgoType < (int) v_algoTypeStr.size(); iAlgoType++)
        {
            printf("%s, ", v_algoTypeStr.at(iAlgoType).c_str());
        }
        
        printf("\n");
        
        exit(EXIT_FAILURE);
    }
    
    if(std::find(v_distTypeStr.begin(), v_distTypeStr.end(), distTypeStr) == v_distTypeStr.end())
    {
        printf("Error in EnergySharedTICLmultiClusterProducer::EnergySharedTICLmultiClusterProducer(...). \n");
        printf("Invalid distTypeStr provided (\"%s\"); must be one of the following: \n", distTypeStr.c_str());
        
        for(int iDistType = 0; iDistType < (int) v_distTypeStr.size(); iDistType++)
        {
            printf("%s, ", v_distTypeStr.at(iDistType).c_str());
        }
        
        printf("\n");
        
        exit(EXIT_FAILURE);
    }
    
    tok_HGCEERecHit = consumes <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > >(iConfig.getUntrackedParameter <edm::InputTag>("label_HGCEERecHit"));
    tok_HGCHEFRecHit = consumes <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > >(iConfig.getUntrackedParameter <edm::InputTag>("label_HGCHEFRecHit"));
    tok_HGCHEBRecHit = consumes <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > >(iConfig.getUntrackedParameter <edm::InputTag>("label_HGCHEBRecHit"));
    
    tok_TICLmultiCluster = consumes <std::vector <reco::HGCalMultiCluster> >(iConfig.getUntrackedParameter <edm::InputTag>("label_TICLmultiCluster"));
    
    
    // Produces
    instanceName = instanceName + algoTypeStr + distTypeStr;
    
    produces <std::vector <reco::HGCalMultiCluster>> (instanceName);
}


EnergySharedTICLmultiClusterProducer::~EnergySharedTICLmultiClusterProducer()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
EnergySharedTICLmultiClusterProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    
    /* This is an event example
    //Read 'ExampleData' from the Event
    Handle<ExampleData> pIn;
    iEvent.getByLabel("example",pIn);
    
    //Use the ExampleData to create an ExampleData2 which 
    // is put into the Event
    iEvent.put(std::make_unique<ExampleData2>(*pIn));
    */
    
    /* this is an EventSetup example
    //Read SetupData from the SetupRecord in the EventSetup
    ESHandle<SetupData> pSetup;
    iSetup.get<SetupRecord>().get(pSetup);
    */
    
    recHitTools.getEventSetup(iSetup);
    
    
    // RecHit dictionary
    edm::Handle <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > v_HGCEERecHit;
    iEvent.getByToken(tok_HGCEERecHit, v_HGCEERecHit);
    
    edm::Handle <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > v_HGCHEFRecHit;
    iEvent.getByToken(tok_HGCHEFRecHit, v_HGCHEFRecHit);
    
    edm::Handle <edm::SortedCollection <HGCRecHit,edm::StrictWeakOrdering <HGCRecHit> > > v_HGCHEBRecHit;
    iEvent.getByToken(tok_HGCHEBRecHit, v_HGCHEBRecHit);
    
    std::map <DetId, const HGCRecHit*> m_recHit;
    std::map <DetId, int> m_multiClus_recHit_multiplicity;
    
    
    //
    int nHGCEERecHit = v_HGCEERecHit->size();
    
    for(int iRecHit = 0; iRecHit < nHGCEERecHit; iRecHit++)
    {
        const HGCRecHit *recHit = &(*v_HGCEERecHit)[iRecHit];
        
        m_recHit[recHit->id()] = recHit;
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
    
    
    
    // TICL
    edm::Handle <std::vector <reco::HGCalMultiCluster> > v_TICLmultiCluster;
    iEvent.getByToken(tok_TICLmultiCluster, v_TICLmultiCluster);
    
    std::map <DetId, double> m_hitNormFactor; 
    std::vector <std::map <DetId, double> > vm_hitWeight;
    
    int nMultiClus = v_TICLmultiCluster->size();
    
    
    // Calculate the weights
    for(int iMultiClus = 0; iMultiClus < nMultiClus; iMultiClus++)
    {
        reco::HGCalMultiCluster multiCluster = v_TICLmultiCluster->at(iMultiClus);
        
        CLHEP::Hep3Vector multiCluster_3mom(
            multiCluster.x(),
            multiCluster.y(),
            multiCluster.z()
        );
        
        std::map <DetId, double> m_hitWeight;
        
        // hitsAndFractions not filled in the TICL version without fractions
        /*std::vector <std::pair <DetId, float> > v_hit = multiCluster.hitsAndFractions();
        
        int nHit = v_hit.size();
        
        for(int iHit = 0; iHit < nHit; iHit++)
        {
            DetId hitId = v_hit.at(iHit).first;
            
            auto hitPosition = recHitTools.getPosition(hitId);
            
            CLHEP::Hep3Vector hit_3mom(
                hitPosition.x(),
                hitPosition.y(),
                hitPosition.z()
            );
            
            double dR = multiCluster_3mom.deltaR(hit_3mom);
            
            double weight = multiCluster.energy() * exp(-dR);
            
            if(m_hitNormFactor.find(hitId) == m_hitNormFactor.end())
            {
                m_hitNormFactor[hitId] = weight;
            }
            
            else
            {
                m_hitNormFactor[hitId] += weight;
            }
            
            m_hitWeight[hitId] = weight;
        }*/
        
        edm::PtrVector <reco::CaloCluster> v_cluster = multiCluster.clusters();
        int nCluster = v_cluster.size();
        
        for(int iCluster = 0; iCluster < nCluster; iCluster++)
        {
            auto cluster = v_cluster[iCluster].get();
            
            std::vector <std::pair <DetId, float> > v_clusterHit = cluster->hitsAndFractions();
            int nClusterHit = v_clusterHit.size();
            
            for(int iClusterHit = 0; iClusterHit < nClusterHit; iClusterHit++)
            {
                DetId hitId = v_clusterHit.at(iClusterHit).first;
                
                auto hitPosition = recHitTools.getPosition(hitId);
                
                CLHEP::Hep3Vector hit_3mom(
                    hitPosition.x(),
                    hitPosition.y(),
                    hitPosition.z()
                );
                
                //double dR = multiCluster_3mom.deltaR(hit_3mom);
                double dR = 0;
                
                // Calculate distance based on the provided distance type
                if(!distTypeStr.compare("2Ddist"))
                {
                    dR = (multiCluster_3mom-hit_3mom).perp();
                }
                
                else if(!distTypeStr.compare("3Ddist"))
                {
                    dR = (multiCluster_3mom-hit_3mom).mag();
                }
                
                else
                {
                    printf("Invalid distTypeStr.");
                    exit(EXIT_FAILURE);
                }
                
                //printf(
                //    "MultiClus %d/%d, "
                //    "Clus %d/%d, "
                //    "Hit %d/%d : "
                //    "dR %0.2f, "
                //    "\n",
                //    iMultiClus+1, nMultiClus,
                //    iCluster+1, nCluster,
                //    iClusterHit+1, nClusterHit,
                //    dR
                //);
                
                double dist = dR / showerSpread;
                
                double weight = 1.0;
                
                // Calculate weight based on the provided algo type
                if(!algoTypeStr.compare("Mean"))
                {
                    weight = 1.0;
                }
                
                else if(!algoTypeStr.compare("Expo"))
                {
                    weight = multiCluster.energy() * exp(-dist);
                }
                
                else if(!algoTypeStr.compare("Gaus"))
                {
                    weight = multiCluster.energy() * exp(-dist*dist);
                }
                
                else
                {
                    printf("Invalid algoTypeStr.");
                    exit(EXIT_FAILURE);
                }
                
                
                // Store weight
                if(m_hitNormFactor.find(hitId) == m_hitNormFactor.end())
                {
                    m_hitNormFactor[hitId] = weight;
                }
                
                else
                {
                    m_hitNormFactor[hitId] += weight;
                }
                
                m_hitWeight[hitId] = weight;
            }
        }
        
        vm_hitWeight.push_back(m_hitWeight);
        
        //printf("***** m_hitNormFactor %d,  vm_hitWeight(%d) %d \n", m_hitNormFactor.size(), iMultiClus, vm_hitWeight.at(iMultiClus).size());
    }
    
    
    double multiClus_totE = 0;
    double multiClus_recHit_totE = 0;
    
    // Apply the weights
    auto v_multiCluster_output = std::make_unique <std::vector <reco::HGCalMultiCluster> >();
    
    for(int iMultiClus = 0; iMultiClus < nMultiClus; iMultiClus++)
    {
        reco::HGCalMultiCluster multiCluster_temp;
        reco::HGCalMultiCluster multiCluster = v_TICLmultiCluster->at(iMultiClus);
        
        double multiClus_E = 0;
        double multiClus_x = 0;
        double multiClus_y = 0;
        double multiClus_z = 0;
        
        edm::PtrVector <reco::CaloCluster> v_cluster = multiCluster.clusters();
        
        int nCluster = v_cluster.size();
        
        for(int iCluster = 0; iCluster < nCluster; iCluster++)
        {
            multiCluster_temp.push_back(v_cluster[iCluster]);
            
            auto cluster = v_cluster[iCluster].get();
            
            double cluster_sharedE = 0;
            
            std::vector <std::pair <DetId, float> > v_clusterHit = cluster->hitsAndFractions();
            
            int nClusterHit = v_clusterHit.size();
            
            for(int iClusterHit = 0; iClusterHit < nClusterHit; iClusterHit++)
            {
                DetId hitId = v_clusterHit.at(iClusterHit).first;
                
                if(m_recHit.find(hitId) == m_recHit.end())
                {
                    printf("Warning in EnergySharedTICLmultiClusterProducer::produce(...): Cluster-hit not in rec-hit map. \n");
                    
                    multiCluster_temp.addHitAndFraction(hitId, 1.0);
                    
                    //exit(EXIT_FAILURE);
                    continue;
                }
                
                const HGCRecHit *recHit = m_recHit.at(hitId);
                
                if(m_multiClus_recHit_multiplicity.find(hitId) == m_multiClus_recHit_multiplicity.end())
                {
                    multiClus_recHit_totE += recHit->energy();
                    
                    m_multiClus_recHit_multiplicity[hitId] = 0;
                }
                
                else
                {
                    m_multiClus_recHit_multiplicity.at(hitId)++;
                }
                
                /*if(vm_hitWeight.at(iMultiClus).find(hitId) == vm_hitWeight.at(iMultiClus).end())
                {
                    printf("Err 1 \n");
                    printf(
                        "MultiClus %d/%d, "
                        "Clus %d/%d, "
                        "Hit %d/%d (%d): "
                        ""
                        "\n",
                        iMultiClus+1, nMultiClus,
                        iCluster+1, nCluster,
                        iClusterHit+1, nClusterHit, (int) vm_hitWeight.at(iMultiClus).size()
                    );
                    
                    fflush(stdout);
                    continue;
                }
                
                if(m_hitNormFactor.find(hitId) == m_hitNormFactor.end())
                {
                    printf("Err 2 \n");
                    printf(
                        "MultiClus %d/%d, "
                        "Clus %d/%d, "
                        "Hit %d/%d (%d): "
                        ""
                        "\n",
                        iMultiClus+1, nMultiClus,
                        iCluster+1, nCluster,
                        iClusterHit+1, nClusterHit, (int) m_hitNormFactor.size()
                    );
                    
                    fflush(stdout);
                    continue;
                }*/
                
                double fraction = vm_hitWeight.at(iMultiClus).at(hitId) / m_hitNormFactor.at(hitId);
                
                cluster_sharedE += recHit->energy() * fraction;
                
                multiCluster_temp.addHitAndFraction(hitId, fraction);
            }
            
            multiClus_E += cluster_sharedE;
            multiClus_x += cluster->x() * cluster_sharedE;
            multiClus_y += cluster->y() * cluster_sharedE;
            multiClus_z += cluster->z() * cluster_sharedE;
        }
        
        if(multiClus_E > 0)
        {
            multiClus_x /= multiClus_E;
            multiClus_y /= multiClus_E;
            multiClus_z /= multiClus_E;
        }
        
        else
        {
            multiClus_x = 0;
            multiClus_y = 0;
            multiClus_z = 0;
        }
        
        multiCluster_temp.setEnergy(multiClus_E);
        multiCluster_temp.setCorrectedEnergy(multiClus_E);
        multiCluster_temp.setPosition(math::XYZPoint(multiClus_x, multiClus_y, multiClus_z));
        multiCluster_temp.setAlgoId(multiCluster.algoID());
        
        v_multiCluster_output->push_back(multiCluster_temp);
        
        multiClus_totE += multiClus_E;
    }
    
    //printf("multiClus_totE        %0.2f \n", multiClus_totE);
    //printf("multiClus_recHit_totE %0.2f \n", multiClus_recHit_totE);
    //printf("ratio %0.2f \n", multiClus_totE/multiClus_recHit_totE);
    
    // Put the collection in the event
    iEvent.put(std::move(v_multiCluster_output), instanceName);
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
EnergySharedTICLmultiClusterProducer::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
EnergySharedTICLmultiClusterProducer::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
EnergySharedTICLmultiClusterProducer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
EnergySharedTICLmultiClusterProducer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
EnergySharedTICLmultiClusterProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
EnergySharedTICLmultiClusterProducer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void EnergySharedTICLmultiClusterProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    //desc.setUnknown();
    desc.setAllowAnything();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EnergySharedTICLmultiClusterProducer);
