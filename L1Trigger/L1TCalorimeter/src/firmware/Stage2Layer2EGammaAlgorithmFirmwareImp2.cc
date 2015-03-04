///
/// \class l1t::Stage2Layer2EGammaAlgorithmFirmwareImp1
///
/// \author: Sam Harper based on Jim Brooke and Jean Baptiste Sauvan's code implimenting algo proposed by Andrew Rose
///
/// Description: implimentation of basic 2x3 EG algo which repurposes jet code

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "L1Trigger/L1TCalorimeter/interface/Stage2Layer2EGammaAlgorithmFirmware.h"

#include "L1Trigger/L1TCalorimeter/interface/CaloStage2Nav.h"
#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"



l1t::Stage2Layer2EGammaAlgorithmFirmwareImp2::Stage2Layer2EGammaAlgorithmFirmwareImp2(CaloParams* params) :
  params_(params)
{

}



void l1t::Stage2Layer2EGammaAlgorithmFirmwareImp2::processEvent(const std::vector<l1t::CaloCluster>& clusters, const std::vector<l1t::CaloTower>& towers, std::vector<l1t::EGamma>& egammas) 
{
  l1t::CaloStage2Nav caloNav;
  egammas.clear();
  for(const auto& cluster : clusters)
  {
    // Keep only valid clusters
    if(cluster.isValid())
    { 
      egammas.push_back(cluster);
      l1t::EGamma& egamma = egammas.back();

      //isolation is 3(eta)x9(phi) E+H region centred on the seed tower with cluster 2x3 (E+H) subtracted off
      int isolEt = l1t::CaloTools::calHwEtSum(cluster.hwEta(),cluster.hwPhi(),towers,
					      -1,1,-4,4,l1t::CaloTools::CALO);
      isolEt-=cluster.hwPtEm();
      isolEt-=cluster.hwPtHad();
      
      // Identification of the egamma
      // Based on the seed tower FG bit, the H/E ratio of the seed toswer, and the shape of the cluster
      //bits are 1 = pass, 0 = fail
      bool hOverEBit = cluster.hOverE() <= 100; //HARDCODED FOR NEW, FIX SOON
      bool fgBit  = cluster.hwSeedPt()<=6 || !cluster.fgECAL(); 
      int qual = 0;
      if(fgBit)     qual |= (0x1); // first bit = FG
      if(hOverEBit) qual |= (0x1<<1); // second bit = H/E
      egamma.setHwQual( qual ); 
      
      int isolBit = isolEt <= 7; //HARDCODED FOR NOW, FIX SOON
      egamma.setHwIso(isolBit);
    
      // Corrections function of ieta, possibly et
      int calibPt = calPtCalib(cluster);

      // Physical eta/phi. Computed from ieta/iphi of the seed tower and the fine-grain position within the seed
      float eta = calEtaPhysical(cluster);
      float phi = calPhiPhysical(cluster);
      
      // Set 4-vector
      math::PtEtaPhiMLorentzVector calibP4((double)calibPt*params_->egLsb(), eta, phi, 0.);
      egamma.setP4(calibP4);

    }//end of cuts on cluster to make EGamma
  }//end of cluster loop
}



int l1t::Stage2Layer2EGammaAlgorithmFirmwareImp2::calPtCalib(const l1t::CaloCluster& clus)const
{
  return clus.hwPt();
}
 
float l1t::Stage2Layer2EGammaAlgorithmFirmwareImp2::calEtaPhysical(const l1t::CaloCluster& clus)const
{  
  float eta=0;
  float seedEta     = CaloTools::towerEta(clus.hwEta());
  float seedEtaSize = CaloTools::towerEtaSize(clus.hwEta());
  if(clus.fgEta()==0)      eta = seedEta; // center
  else if(clus.fgEta()==2) eta = seedEta + seedEtaSize*0.25; // center + 1/4
  else if(clus.fgEta()==1) eta = seedEta - seedEtaSize*0.25; // center - 1/4
  return eta;
}

float l1t::Stage2Layer2EGammaAlgorithmFirmwareImp2::calPhiPhysical(const l1t::CaloCluster& clus)const
{  
  float phi=0.;
  float seedPhi     = CaloTools::towerPhi(clus.hwEta(), clus.hwPhi());
  float seedPhiSize = CaloTools::towerPhiSize(clus.hwEta());
  if(clus.fgPhi()==0)      phi = seedPhi; // center
  else if(clus.fgPhi()==2) phi = seedPhi + seedPhiSize*0.25; // center + 1/4
  else if(clus.fgPhi()==1) phi = seedPhi - seedPhiSize*0.25; // center - 1/4
  return phi;
}

