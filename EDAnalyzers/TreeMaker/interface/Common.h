# ifndef Common_H
# define Common_H


# include <algorithm>
# include <iostream>
# include <map>
# include <stdlib.h>
# include <string>
# include <type_traits>
# include <utility>
# include <vector>

# include <TH1F.h>
# include <TH2F.h>
# include <TMatrixD.h>
# include <TTree.h> 
# include <TVectorD.h> 

# include "Constants.h"


namespace Common
{
    int getPileup(edm::Handle <std::vector <PileupSummaryInfo> > pileUps_reco)
    {
        int pileup_n = 0;
        
        // Start from the end to reach the In-time bunch-crossing quicker
        for(int iPileUp = (int) pileUps_reco->size() - 1; iPileUp >= 0; iPileUp--)
        {
            PileupSummaryInfo pileUpInfo = (*pileUps_reco)[iPileUp];
            
            int bunchCrossingNumber = pileUpInfo.getBunchCrossing();
            
            // In-time bunch-crossing pile-up
            if(bunchCrossingNumber == 0)
            {
                pileup_n = pileUpInfo.getPU_NumInteractions();
                
                break;
            }
        }
        
        return pileup_n;
    }
    
    
    double getCellSize(DetId detId, hgcal::RecHitTools *recHitTools)
    {
        double SiThickness = recHitTools->getSiThickness(detId);
        
        // HD wafers
        if(SiThickness < 150)
        {
            return 0.465;
        }
        
        // LD wafers
        else
        {
            return 0.698;
        }
    }
    
    template <typename T1, typename T2> std::vector <int> associateRecToSimHit(
        T1 simHitCollPtr,
        T2 recHitCollPtr
    )
    {
        int nSimHit = simHitCollPtr->size();
        int nRecHit = recHitCollPtr->size();
        
        std::vector <int> v_matchedSimHitIndex(nRecHit, -1);
        
        # pragma omp parallel for
        for(int iRecHit = 0; iRecHit < nRecHit; iRecHit++)
        {
            auto recHit = (*recHitCollPtr)[iRecHit];
            
            unsigned int recHitId = recHit.id();
            
            for(int iSimHit = 0; iSimHit < nSimHit; iSimHit++)
            {
                auto simHit = (*simHitCollPtr)[iSimHit];
                
                unsigned int simHitId = simHit.id();
                
                if(recHitId == simHitId)
                {
                    v_matchedSimHitIndex.at(iRecHit) = iSimHit;
                    
                    break;
                }
            }
        }
        
        fflush(stdout);
        fflush(stderr);
        
        return v_matchedSimHitIndex;
    }
    
    
    // Returns the vector of pairs (energy sorted): < <idx1, energy1>, ...., <<idxN, energyN>> >
    std::vector <std::pair <int, double> > getSortedHitIndex(
        std::vector <std::pair <DetId, float> > v_HandF,
        std::map <DetId, const HGCRecHit*> m_hit
    )
    {
        int nHit = v_HandF.size();
        
        std::vector <std::pair <int, double> > vp_index_energy;
        
        for(int iHit = 0; iHit < nHit; iHit++)
        {
            double energy = 0;
            
            if(m_hit.find(v_HandF.at(iHit).first) != m_hit.end())
            {
                energy = m_hit[v_HandF.at(iHit).first]->energy() * v_HandF.at(iHit).second;
            }
            
            vp_index_energy.push_back(std::make_pair(iHit, energy));
        }
        
        std::sort(
            vp_index_energy.begin(), vp_index_energy.end(),
            [&](std::pair <int, double> ele1, std::pair <int, double> ele2)
            {
                return ele1.second > ele2.second;
            }
        );
        
        return vp_index_energy;
    }
    
    
    double getEnergySum(
        std::vector <DetId> v_hit_detId,
        std::vector <std::pair <DetId, float> > v_HandF,
        std::map <DetId, const HGCRecHit*> m_hit,
        bool useHandF = false
    )
    {
        double totalE = 0;
        
        for(int iHit = 0; iHit < (int) v_hit_detId.size(); iHit++)
        {
            DetId iHit_detId = v_hit_detId.at(iHit);
            
            if(m_hit.find(iHit_detId) == m_hit.end())
            {
                continue;
            }
            
            //auto iter = std::find_if(
            //    v_HandF.begin(), v_HandF.end(),
            //    
            //    [&iHit_detId](std::pair <DetId, float> &HandF)
            //    {
            //        return HandF.first == iHit_detId;
            //    }
            //);
            //
            //if(iter == v_HandF.end())
            //{
            //    continue;
            //}
            //
            ////int HandF_idx = std::distance(v_HandF.begin(), iter);
            ////
            ////std::pair <DetId, float> HandF = v_HandF.at(HandF_idx);
            //
            //
            //double energy = m_hit.at(iHit_detId)->energy();// * HandF.second;
            //
            //totalE += energy;
            
            for(int iHandF = 0; iHandF < (int) v_HandF.size(); iHandF++)
            {
                std::pair <DetId, float> HandF = v_HandF.at(iHandF);
                
                if(iHit_detId == HandF.first)
                {
                    double energy = m_hit.at(iHit_detId)->energy() * HandF.second;
                    
                    totalE += energy;
                }
            }
            
            //std::vector<Type> v = ....;
            //std::string myString = ....;
            //auto it = find_if(v.begin(), v.end(), [&myString](const Type& obj) {return obj.getName() == myString;})
            //
            //if (it != v.end())
            //{
            //// found element. it is an iterator to the first matching element.
            //// if you really need the index, you can also get it:
            //auto index = std::distance(v.begin(), it);
            //}
        }
        
        //double totalE_test = 0;
        //
        //for(int iHit = 0; iHit < (int) v_hit_detId.size(); iHit++)
        //{
        //    DetId iHit_detId = v_hit_detId.at(iHit);
        //    
        //    for(int iFrac = 0; iFrac < (int) v_HandF.size(); iFrac++)
        //    {
        //        std::pair <DetId, float> HandF = v_HandF.at(iFrac);
        //        
        //        if(iHit_detId == HandF.first)
        //        {
        //            totalE_test += m_hit.at(HandF.first)->energy() * HandF.second;
        //        }
        //    }
        //}
        //
        //printf("totalE %0.2f, totalE_test %0.2f \n", totalE, totalE_test);
        
        return totalE;
    }
    
    
    std::vector <DetId> getNeighborR(
        DetId detId,
        std::vector <std::pair <DetId, float> > v_HandF,
        double R,
        const HGCalTopology topo,
        hgcal::RecHitTools *recHitTools
    )
    {
        std::map <DetId, bool> m_detId_isIncluded;
        
        std::vector <DetId> v_neighborR_detId;
        
        v_neighborR_detId.push_back(detId);
        m_detId_isIncluded[detId] = true;
        
        auto center_pos = recHitTools->getPosition(detId);
        
        for(int iHit = 0; iHit < (int) v_HandF.size(); iHit++)
        {
            std::pair <DetId, float> p_HandF = v_HandF.at(iHit);
            
            if(p_HandF.first.det() != DetId::HGCalEE)
            {
                continue;
            }
            
            auto iHit_pos = recHitTools->getPosition(p_HandF.first);
            
            double dX = iHit_pos.x() - center_pos.x();
            double dY = iHit_pos.y() - center_pos.y();
            double dZ = iHit_pos.z() - center_pos.z();
            
            double dist = sqrt(dX*dX + dY*dY + dZ*dZ);
            
            double cellSize = getCellSize(detId, recHitTools);
            
            double R_max = R + cellSize;
            
            if(
                dist < R_max &&
                m_detId_isIncluded.find(p_HandF.first) == m_detId_isIncluded.end()
            )
            {
                v_neighborR_detId.push_back(p_HandF.first);
                m_detId_isIncluded[p_HandF.first] = true;
            }
        }
        
        
        return v_neighborR_detId;
    }
    
    
    std::vector <DetId> getNeighbor19(DetId detId, const HGCalTopology topo)
    {
        if(!detId)
        {
            return {};
        }
        
        std::vector <DetId> v_neighbor_detId = topo.neighbors(detId);
        
        std::map <DetId, bool> m_detId_isIncluded;
        
        int neighbor_n = v_neighbor_detId.size();
        
        for(int iNeighbor = 0; iNeighbor < neighbor_n; iNeighbor++)
        {
            DetId iNeighbor_detId = v_neighbor_detId.at(iNeighbor);
            
            m_detId_isIncluded[iNeighbor_detId] = true;
        }
        
        for(int iNeighbor = 0; iNeighbor < neighbor_n; iNeighbor++)
        {
            DetId iNeighbor_detId = v_neighbor_detId.at(iNeighbor);
            
            std::vector <DetId> v_jNneighbor_detId = topo.neighbors(iNeighbor_detId);
            
            for(int jNeighbor = 0; jNeighbor < (int) v_jNneighbor_detId.size(); jNeighbor++)
            {
                DetId jNeighbor_detId = v_jNneighbor_detId.at(jNeighbor);
                
                if(m_detId_isIncluded.find(jNeighbor_detId) == m_detId_isIncluded.end())
                {
                    m_detId_isIncluded[jNeighbor_detId] = true;
                    
                    v_neighbor_detId.push_back(jNeighbor_detId);
                }
            }
        }
        
        return v_neighbor_detId;
    }
    
    
    // Returns <XYZPoint, totalEnergy>
    std::pair <math::XYZPoint, double> getCentroid(
        std::vector <std::pair <DetId, float> > v_HandF,
        std::map <DetId, const HGCRecHit*> m_hit,
        hgcal::RecHitTools *recHitTools
    )
    {
        double centroidX = 0;
        double centroidY = 0;
        double centroidZ = 0;
        double totalE = 0;
        
        for(int iHit = 0; iHit < (int) v_HandF.size(); iHit++)
        {
            DetId detId = v_HandF.at(iHit).first;
            
            auto hit_pos = recHitTools->getPosition(detId);
            
            double energy = m_hit.at(detId)->energy() * v_HandF.at(iHit).second;
            
            centroidX += hit_pos.x() * energy;
            centroidY += hit_pos.y() * energy;
            centroidZ += hit_pos.z() * energy;
            
            totalE += energy;
        }
        
        if(totalE)
        {
            centroidX /= totalE;
            centroidY /= totalE;
            centroidZ /= totalE;
        }
        
        math::XYZPoint centroid_xyz(centroidX, centroidY, centroidZ);
        
        return std::make_pair(centroid_xyz, totalE);
    }
    
    
    
    std::vector <std::vector <std::pair <DetId, float> > > getLayerwiseHandF(
        std::vector <std::pair <DetId, float> > v_HandF,
        hgcal::RecHitTools *recHitTools,
        int nLayer
    )
    {
        std::vector <std::vector <std::pair <DetId, float> > > vv_layerHandF;
        
        for(int iLayer = 0; iLayer < nLayer; iLayer++)
        {
            vv_layerHandF.push_back({});
        }
        
        for(int iHit = 0; iHit < (int) v_HandF.size(); iHit++)
        {
            DetId detId = v_HandF.at(iHit).first;
            
            if(detId.det() != DetId::HGCalEE)
            {
                continue;
            }
            
            int layer = recHitTools->getLayer(detId) - 1;
            
            if(layer >= nLayer)
            {
                continue;
            }
            
            vv_layerHandF.at(layer).push_back(v_HandF.at(iHit));
        }
        
        return vv_layerHandF;
    }
    
    
    // Returns <DetId, distance>
    // Layer starts from 1
    std::pair <DetId, double> getNearestCell(
        double x, double y, double z,
        std::vector <std::pair <DetId, float> > v_hit,
        hgcal::RecHitTools *recHitTools,
        int layer = -1
    )
    {
        double dist_min = 9999;
        DetId nearestCell_detId = 0;
        
        for(int iHit = 0; iHit < (int) v_hit.size(); iHit++)
        {
            DetId iHit_detId = v_hit.at(iHit).first;
            
            int iHit_layer = recHitTools->getLayer(iHit_detId);
            
            if(iHit_detId.det() != DetId::HGCalEE)
            {
                //printf("Invalid hit (Total hits %d). layer %d, det %d \n", (int) v_hit.size(), iHit_layer, (int) iHit_detId.det());
                continue;
            }
            
            if(layer > 0 && layer != (int) iHit_layer)
            {
                continue;
            }
            
            auto recHit_pos = recHitTools->getPosition(iHit_detId);
            
            //math::XYZPoint recHit_xyz(recHit_pos.x(), recHit_pos.y(), recHit_pos.z());
            
            double dX = x - recHit_pos.x();
            double dY = y - recHit_pos.y();
            double dZ = z - recHit_pos.z();
            
            //double dist = (superClus_xyz-recHit_xyz).rho();
            double dist = std::sqrt(dX*dX + dY*dY + dZ*dZ);
            
            if (dist < dist_min)
            {
                dist_min = dist;
                nearestCell_detId = iHit_detId;
            }
        }
        
        std::pair <DetId, double> p_detId_dist = std::make_pair(nearestCell_detId, dist_min);
        
        return p_detId_dist;
    }
    
    
    // PCA
    // Returns <(r, eta, phi) cov matrix, eigen vector matrix, eigen values>
    std::tuple <TMatrixD, TMatrixD, TVectorD> getMultiClusterPCAinfo(
        reco::HGCalMultiCluster *multiCluster,
        std::map <DetId, const HGCRecHit*> m_recHit,
        hgcal::RecHitTools *recHitTools,
        bool debug = false
    )
    {
        double recHit_meanDrSq = 0;
        double recHit_meanDetaSq = 0;
        double recHit_meanDphiSq = 0;
        
        double recHit_meanDrDeta = 0;
        double recHit_meanDrDphi = 0;
        double recHit_meanDetaDphi = 0;
        
        edm::PtrVector <reco::CaloCluster> v_cluster = multiCluster->clusters();
        
        int nCluster = v_cluster.size();
        
        CLHEP::Hep3Vector multiCluster_3mom(
            multiCluster->x(),
            multiCluster->y(),
            multiCluster->z()
        );
        
        int nHitTotal = 0;
        double totE = 0;
        
        for(int iCluster = 0; iCluster < nCluster; iCluster++)
        {
            auto cluster = v_cluster[iCluster].get();
            std::vector <std::pair <DetId, float> > v_clusterHit = cluster->hitsAndFractions();
            
            int nClusterHit = v_clusterHit.size();
            
            for(int iClusterHit = 0; iClusterHit < nClusterHit; iClusterHit++)
            {
                if(m_recHit.find(v_clusterHit.at(iClusterHit).first) == m_recHit.end())
                {
                    printf("Warning in Common::getMultiClusterPCAinfo(...): Cluster-hit not in rec-hit map. \n");
                    //exit(EXIT_FAILURE);
                    continue;
                }
                
                const HGCRecHit *recHit = m_recHit[v_clusterHit.at(iClusterHit).first];
                
                auto position = recHitTools->getPosition(recHit->id());
                
                CLHEP::Hep3Vector recHit_3mom(
                    position.x(),
                    position.y(),
                    position.z()
                );
                
                // Note that we're calculating sigma^2(rr) here.
                // So the dR is is simply the difference, and not the 3D difference
                double dR = multiCluster_3mom.r() - recHit_3mom.r();
                double dEta = multiCluster_3mom.eta() - recHit_3mom.eta();
                double dPhi = multiCluster_3mom.deltaPhi(recHit_3mom);
                
                recHit_meanDrSq   += recHit->energy() * dR*dR;
                recHit_meanDetaSq += recHit->energy() * dEta*dEta;
                recHit_meanDphiSq += recHit->energy() * dPhi*dPhi;
                
                recHit_meanDrDeta   += recHit->energy() * dR*dEta;
                recHit_meanDrDphi   += recHit->energy() * dR*dPhi;
                recHit_meanDetaDphi += recHit->energy() * dEta*dPhi;
                
                nHitTotal++;
                totE += recHit->energy();
            }
        }
        
        if(totE > 0)
        {
            recHit_meanDrSq /= totE;
            recHit_meanDetaSq /= totE;
            recHit_meanDphiSq /= totE;
            
            recHit_meanDrDeta /= totE;
            recHit_meanDrDphi /= totE;
            recHit_meanDetaDphi /= totE;
        }
        
        else
        {
            recHit_meanDrSq = 0;
            recHit_meanDetaSq = 0;
            recHit_meanDphiSq = 0;
            
            recHit_meanDrDeta = 0;
            recHit_meanDrDphi = 0;
            recHit_meanDetaDphi = 0;
            
            printf("Warning in Common::getMultiClusterPCAinfo(...): Encountered multicluster with zero energy. \n");
        }
        
        int dimension = 3;
        
        // Covariance matrix
        TMatrixD mat_multiClus_rEtaPhiCov(dimension, dimension);
        
        mat_multiClus_rEtaPhiCov(0, 0) = recHit_meanDrSq;
        mat_multiClus_rEtaPhiCov(1, 1) = recHit_meanDetaSq;
        mat_multiClus_rEtaPhiCov(2, 2) = recHit_meanDphiSq;
        
        mat_multiClus_rEtaPhiCov(0, 1) = mat_multiClus_rEtaPhiCov(1, 0) = recHit_meanDrDeta;
        mat_multiClus_rEtaPhiCov(0, 2) = mat_multiClus_rEtaPhiCov(2, 0) = recHit_meanDrDphi;
        mat_multiClus_rEtaPhiCov(1, 2) = mat_multiClus_rEtaPhiCov(2, 1) = recHit_meanDetaDphi;
        
        
        // Get eigen values and vectors
        TVectorD v_multiClus_rEtaPhiCov_eigVal(dimension);
        TMatrixD mat_multiClus_rEtaPhiCov_eigVec(dimension, dimension);
        
        if(totE > 0 && multiCluster->energy() > 0)
        {
            try
            {
                mat_multiClus_rEtaPhiCov_eigVec = mat_multiClus_rEtaPhiCov.EigenVectors(v_multiClus_rEtaPhiCov_eigVal);
            }
            
            catch(...)
            {
                printf("Error in Common::getMultiClusterPCAinfo(...): Cannot get eigen values and vectors. \n");
                
                printf(
                    "Multicluster: "
                    "E %0.2e, "
                    "x %0.2e, y %0.2e, z %0.2e \n",
                    multiCluster->energy(),
                    multiCluster_3mom.x(), multiCluster_3mom.y(), multiCluster_3mom.z()
                );
                
                printf("recHit_meanDrSq %0.2f \n", recHit_meanDrSq);
                printf("recHit_meanDetaSq %0.2f \n", recHit_meanDetaSq);
                printf("recHit_meanDphiSq %0.2f \n", recHit_meanDphiSq);
                printf("recHit_meanDrDeta %0.2f \n", recHit_meanDrDeta);
                printf("recHit_meanDrDphi %0.2f \n", recHit_meanDrDphi);
                printf("recHit_meanDetaDphi %0.2f \n", recHit_meanDetaDphi);
                
                //printf("Total energy: %0.2f \n", totE);
                
                mat_multiClus_rEtaPhiCov.Print();
                
                fflush(stdout);
                fflush(stderr);
            }
        }
        
        if(debug)
        {
            printf("In Common::getMultiClusterPCAinfo(...): \n");
            
            printf(
                "Multicluster: "
                "E %0.2e, "
                "x %0.2e, y %0.2e, z %0.2e \n",
                multiCluster->energy(),
                multiCluster_3mom.x(), multiCluster_3mom.y(), multiCluster_3mom.z()
            );
            
            printf("Covariance matrix: \n");
            mat_multiClus_rEtaPhiCov.Print();
            
            printf("Eigen values: \n");
            v_multiClus_rEtaPhiCov_eigVal.Print();
            
            printf("Eigen vectors: \n");
            mat_multiClus_rEtaPhiCov_eigVec.Print();
            
            fflush(stdout);
            fflush(stderr);
        }
        
        auto info = std::make_tuple(
            mat_multiClus_rEtaPhiCov,
            mat_multiClus_rEtaPhiCov_eigVec,
            v_multiClus_rEtaPhiCov_eigVal
        );
        
        fflush(stdout);
        fflush(stderr);
        
        return info;
    }
    
    
    // PCA
    // Returns <(r, eta, phi) cov matrix, eigen vector matrix, eigen values>
    std::tuple <TMatrixD, TMatrixD, TVectorD> getSuperClusPCAinfo(
        reco::SuperClusterRef superClus,
        std::map <DetId, const HGCRecHit*> m_recHit,
        hgcal::RecHitTools *recHitTools,
        bool debug = false
    )
    {
        double recHit_meanDrSq = 0;
        double recHit_meanDetaSq = 0;
        double recHit_meanDphiSq = 0;
        
        double recHit_meanDrDeta = 0;
        double recHit_meanDrDphi = 0;
        double recHit_meanDetaDphi = 0;
        
        int nHitTotal = 0;
        double totE = 0;
        
        std::vector <std::pair <DetId, float> > v_superClus_HandF = superClus->hitsAndFractions();
        math::XYZPoint superClus_xyz = superClus->position();
        
        CLHEP::Hep3Vector superClus_3mom(
            superClus_xyz.x(),
            superClus_xyz.y(),
            superClus_xyz.z()
        );
        
        int nHit = v_superClus_HandF.size();
        
        for(int iHit = 0; iHit < nHit; iHit++)
        {
            if(m_recHit.find(v_superClus_HandF.at(iHit).first) == m_recHit.end())
            {
                printf("Warning in Common::getSuperClusPCAinfo(...): Cluster-hit not in rec-hit map. \n");
                //exit(EXIT_FAILURE);
                continue;
            }
            
            const HGCRecHit *recHit = m_recHit[v_superClus_HandF.at(iHit).first];
            
            auto position = recHitTools->getPosition(recHit->id());
            
            CLHEP::Hep3Vector recHit_3mom(
                position.x(),
                position.y(),
                position.z()
            );
            
            //double dX = superClus_3mom.x() - recHit_3mom.x();
            //double dY = superClus_3mom.y() - recHit_3mom.y();
            //double dZ = superClus_3mom.z() - recHit_3mom.z();
            //
            //double dR = sqrt(dX*dX + dY*dY + dZ*dZ);
            
            // Note that we're calculating sigma^2(rr) here.
            // So the dR is is simply the difference, and not the 3D difference
            double dR = superClus_3mom.r() - recHit_3mom.r();
            double dEta = superClus_xyz.eta() - recHit_3mom.eta();
            double dPhi = superClus_3mom.deltaPhi(recHit_3mom);
            
            double recHit_E = recHit->energy() * v_superClus_HandF.at(iHit).second;
            
            recHit_meanDrSq   += recHit_E * dR*dR;
            recHit_meanDetaSq += recHit_E * dEta*dEta;
            recHit_meanDphiSq += recHit_E * dPhi*dPhi;
            
            recHit_meanDrDeta   += recHit_E * dR*dEta;
            recHit_meanDrDphi   += recHit_E * dR*dPhi;
            recHit_meanDetaDphi += recHit_E * dEta*dPhi;
            
            nHitTotal++;
            totE += recHit_E;
        }
        
        if(totE > 0)
        {
            recHit_meanDrSq /= totE;
            recHit_meanDetaSq /= totE;
            recHit_meanDphiSq /= totE;
            
            recHit_meanDrDeta /= totE;
            recHit_meanDrDphi /= totE;
            recHit_meanDetaDphi /= totE;
        }
        
        else
        {
            recHit_meanDrSq = 0;
            recHit_meanDetaSq = 0;
            recHit_meanDphiSq = 0;
            
            recHit_meanDrDeta = 0;
            recHit_meanDrDphi = 0;
            recHit_meanDetaDphi = 0;
            
            printf("Warning in Common::getSuperClusPCAinfo(...): Encountered multicluster with zero energy. \n");
        }
        
        int dimension = 3;
        
        // Covariance matrix
        TMatrixD mat_superClus_rEtaPhiCov(dimension, dimension);
        
        mat_superClus_rEtaPhiCov(0, 0) = recHit_meanDrSq;
        mat_superClus_rEtaPhiCov(1, 1) = recHit_meanDetaSq;
        mat_superClus_rEtaPhiCov(2, 2) = recHit_meanDphiSq;
        
        mat_superClus_rEtaPhiCov(0, 1) = mat_superClus_rEtaPhiCov(1, 0) = recHit_meanDrDeta;
        mat_superClus_rEtaPhiCov(0, 2) = mat_superClus_rEtaPhiCov(2, 0) = recHit_meanDrDphi;
        mat_superClus_rEtaPhiCov(1, 2) = mat_superClus_rEtaPhiCov(2, 1) = recHit_meanDetaDphi;
        
        
        // Get eigen values and vectors
        TVectorD v_superClus_rEtaPhiCov_eigVal(dimension);
        TMatrixD mat_superClus_rEtaPhiCov_eigVec(dimension, dimension);
        
        if(totE > 0 && superClus->energy() > 0)
        {
            try
            {
                mat_superClus_rEtaPhiCov_eigVec = mat_superClus_rEtaPhiCov.EigenVectors(v_superClus_rEtaPhiCov_eigVal);
            }
            
            catch(...)
            {
                printf("Error in Common::getSuperClusPCAinfo(...): Cannot get eigen values and vectors. \n");
                
                printf(
                    "Multicluster: "
                    "E %0.2e, "
                    "x %0.2e, y %0.2e, z %0.2e \n",
                    superClus->energy(),
                    superClus_3mom.x(), superClus_3mom.y(), superClus_3mom.z()
                );
                
                printf("recHit_meanDrSq %0.2f \n", recHit_meanDrSq);
                printf("recHit_meanDetaSq %0.2f \n", recHit_meanDetaSq);
                printf("recHit_meanDphiSq %0.2f \n", recHit_meanDphiSq);
                printf("recHit_meanDrDeta %0.2f \n", recHit_meanDrDeta);
                printf("recHit_meanDrDphi %0.2f \n", recHit_meanDrDphi);
                printf("recHit_meanDetaDphi %0.2f \n", recHit_meanDetaDphi);
                
                //printf("Total energy: %0.2f \n", totE);
                
                mat_superClus_rEtaPhiCov.Print();
                
                fflush(stdout);
                fflush(stderr);
            }
        }
        
        if(debug)
        {
            printf("In Common::getSuperClusPCAinfo(...): \n");
            
            printf(
                "Multicluster: "
                "E %0.2e, "
                "x %0.2e, y %0.2e, z %0.2e \n",
                superClus->energy(),
                superClus_3mom.x(), superClus_3mom.y(), superClus_3mom.z()
            );
            
            printf("Covariance matrix: \n");
            mat_superClus_rEtaPhiCov.Print();
            
            printf("Eigen values: \n");
            v_superClus_rEtaPhiCov_eigVal.Print();
            
            printf("Eigen vectors: \n");
            mat_superClus_rEtaPhiCov_eigVec.Print();
            
            fflush(stdout);
            fflush(stderr);
        }
        
        auto info = std::make_tuple(
            mat_superClus_rEtaPhiCov,
            mat_superClus_rEtaPhiCov_eigVec,
            v_superClus_rEtaPhiCov_eigVal
        );
        
        fflush(stdout);
        fflush(stderr);
        
        return info;
    }
    
    
    std::pair <std::pair <int, int>, double> findMatrixMinimum(TMatrixD matrix)
    {
        int minRow = -1;
        int minCol = -1;
        
        double minVal = matrix.Max() + 1;
        
        for(int iRow = 0; iRow < matrix.GetNrows(); iRow++)
        {
            for(int iCol = 0; iCol < matrix.GetNcols(); iCol++)
            {
                if(matrix(iRow, iCol) < minVal)
                {
                    minVal = matrix(iRow, iCol);
                    
                    minRow = iRow;
                    minCol = iCol;
                }
            }
        }
        
        std::pair <std::pair <int, int>, double> result = std::make_pair(
            std::make_pair(minRow, minCol),
            minVal
        );
        
        return result;
    }
    
    
    // Will return {dRmin1, ... dRminN} of size v_obj1_4mom.size()
    std::vector <double> getMinDeltaR(
        std::vector <CLHEP::HepLorentzVector> v_obj1_4mom,
        std::vector <CLHEP::HepLorentzVector> v_obj2_4mom,
        TMatrixD &mat_deltaR_result, // Will fill this with the dR values
        std::vector <int> &v_obj1_matchedObj2_index, // Will be of v_obj1_4mom.size(). Will be filled with the index of the matched obj1; if not matched, then -1.
        double defaultVal = Constants::_defaultVal_pos1
    )
    {
        int nObj1 = v_obj1_4mom.size();
        int nObj2 = v_obj2_4mom.size();
        
        TMatrixD tmat_deltaR(nObj1, nObj2);
        
        for(int iObj1 = 0; iObj1 < nObj1; iObj1++)
        {
            for(int iObj2 = 0; iObj2 < nObj2; iObj2++)
            {
                double deltaR = v_obj1_4mom.at(iObj1).deltaR(v_obj2_4mom.at(iObj2));
                
                tmat_deltaR(iObj1, iObj2) = deltaR;
            }
        }
        
        mat_deltaR_result.ResizeTo(nObj1, nObj2);
        
        mat_deltaR_result = tmat_deltaR;
        
        std::vector <double> v_deltaR_min(nObj1, defaultVal);
        
        v_obj1_matchedObj2_index.clear();
        v_obj1_matchedObj2_index.resize(v_obj1_4mom.size(), -1);
        
        int nIter = std::min(nObj1, nObj2);
        
        for(int iIter = 0; iIter < nIter; iIter++)
        {
            std::pair <std::pair <int, int>, double> matrixMinResult = findMatrixMinimum(tmat_deltaR);
            
            int minRow = matrixMinResult.first.first;
            int minCol = matrixMinResult.first.second;
            
            double minVal = matrixMinResult.second;
            
            if(minVal == defaultVal)
            {
                break;
            }
            
            v_deltaR_min.at(minRow) = minVal;
            v_obj1_matchedObj2_index.at(minRow) = minCol;
            
            //printf("tmat_deltaR before masking: \n");
            //tmat_deltaR.Print();
            
            // Mask minRow and minCol
            TMatrixDRow(tmat_deltaR, minRow).Assign(defaultVal);
            TMatrixDColumn(tmat_deltaR, minCol).Assign(defaultVal);
            
            //printf("tmat_deltaR after masking: \n");
            //tmat_deltaR.Print();
            
            //v_deltaR_min.at(iIter) = minVal;
            //
            //deleteMatrixRowCol(tmat_deltaR, minRow, minCol);
            //
            //if(!tmat_deltaR.GetNrows() || !tmat_deltaR.GetNcols())
            //{
            //    break;
            //}
        }
        
        return v_deltaR_min;
    }
}


# endif
