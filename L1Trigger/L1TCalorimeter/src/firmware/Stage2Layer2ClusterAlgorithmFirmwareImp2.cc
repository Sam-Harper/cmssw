///
/// \class l1t::Stage2Layer2ClusterAlgorithmFirmwareImp2
///
/// \author: Sam Harper based on Jim Brooke and Jean Baptiste Sauvan's code implimenting algo proposed by Andrew Rose
///
/// Description: implimentation of basic 2x3 EG algo which repurposes jet code

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "L1Trigger/L1TCalorimeter/interface/Stage2Layer2ClusterAlgorithmFirmware.h"
//#include "DataFormats/Math/interface/LorentzVector.h "

#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"
#include "L1Trigger/L1TCalorimeter/interface/CaloStage2Nav.h"

#include "CondFormats/L1TObjects/interface/CaloParams.h"

l1t::Stage2Layer2ClusterAlgorithmFirmwareImp2::Stage2Layer2ClusterAlgorithmFirmwareImp2(CaloParams* params,l1t::CaloTools::SubDet clusEtType ) :
  clusEtType_(clusEtType),
  seedThreshold_(1),
  params_(params)
{

}


void l1t::Stage2Layer2ClusterAlgorithmFirmwareImp2::processEvent(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters) 
{
  if (clusEtType_==l1t::CaloTools::ECAL) {
    seedThreshold_    = floor(params_->egSeedThreshold()/params_->towerLsbE()); 
  }else if (clusEtType_==l1t::CaloTools::HCAL) {
    seedThreshold_    = floor(params_->egSeedThreshold()/params_->towerLsbH()); 
  }else if (clusEtType_==l1t::CaloTools::CALO) {
    seedThreshold_    = floor(params_->egSeedThreshold()/params_->towerLsbSum()); 
  }

  clustering(towers, clusters);
}

//2(eta)x3(phi) clustering code
//step 1) check if a tower is a seed, a seed is above threshold and is the highest Et (E+H) tower in the 3x9 region centred on that tower
//step 2) find the highest Et 2x3 region that contains the seed tower (Et can be either E, H, or E+H, for EG its just E)
//step 3) H/E = HadEt of 2x3 region / EmEt of 2x3 region
//step 4) fine grain veto is that of the seed tower
void l1t::Stage2Layer2ClusterAlgorithmFirmwareImp2::clustering(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters)const
{

  // Build clusters passing seed threshold
  for(const auto& tower : towers)
  {
    if(isSeed(tower,towers)){
      
      const int clus2x3RegionNr = best2x3(tower,towers);
	
      const int hwEtEm =  cal2x3Et(tower,towers,clus2x3RegionNr,l1t::CaloTools::ECAL);
      const int hwEtHad =  cal2x3Et(tower,towers,clus2x3RegionNr,l1t::CaloTools::HCAL);
      int hwEt = 0;
      if((clusEtType_& l1t::CaloTools::ECAL)) hwEt+=hwEtEm;
      if((clusEtType_& l1t::CaloTools::HCAL)) hwEt+=hwEtHad;
      
      math::XYZTLorentzVector emptyP4;
      clusters.push_back( l1t::CaloCluster(emptyP4, hwEt, tower.hwEta(), tower.hwPhi()) );
      l1t::CaloCluster& cluster = clusters.back();
      cluster.setHwPtEm(hwEtEm);
      cluster.setHwPtHad(hwEtHad);
      cluster.setHwSeedPt(0); //not yet filling
      
      //this bit I ripped straight from Jean-Baptist's code
      // H/E of the cluster is H/E of the seed
      // H/E is currently encoded on 9 bits, from 0 to 1
      int hOverE    = (hwEtEm>0 ? (hwEtHad<<9)/tower.hwEtEm() : 511);
      if(hOverE>511) hOverE = 511; 
      cluster.setHOverE(hOverE);
      // FG of the cluster is FG of the seed
      bool fg = (tower.hwQual() & (0x1<<2));
      cluster.setFgECAL((int)fg);

      //so here I'm trying to approximate to JB's code
      //fgEta: 0 = seed (not used just yet), 2 = cluster goes right of seed, 1 = cluster goes left of seed
      //fgPhi: 0 = seed, 2 = cluster goes below seed, 1= cluster goes above seed
      //btw Andy, its not clear to me how much you gain doing this just yet
      //probably helps for eta, probably not for phi
      int fgEta = clus2x3RegionNr%2==0 ? 2 : 1;  
      int fgPhi = 0;
      if(clus2x3RegionNr/2==0) fgPhi=2;
      else if(clus2x3RegionNr/2==2) fgPhi=1;
      cluster.setFgEta(fgEta);
      cluster.setFgPhi(fgPhi);
      
    }
  }

}

bool l1t::Stage2Layer2ClusterAlgorithmFirmwareImp2::isSeed(const l1t::CaloTower& seedCand,const std::vector<l1t::CaloTower>& towers)const
{
  if(seedCand.hwEtEm()+seedCand.hwEtHad()<seedThreshold_) return false;

  constexpr int minEtaNr=-1;
  constexpr int maxEtaNr=1;
  constexpr int minPhiNr=-4;
  constexpr int maxPhiNr=4;
  
  //Andy's greater than mask
  static const bool greaterThanMask[3][9] =
    {
      { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 },
      { 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 },
      { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 }
    }; 
  l1t::CaloStage2Nav caloNav(seedCand.hwEta(),seedCand.hwPhi());

  for(int etaNr=minEtaNr;etaNr<=maxEtaNr;etaNr++){
    for(int phiNr=minPhiNr;phiNr<=maxPhiNr;phiNr++){
      if(etaNr==0 && phiNr==0) continue;
      const l1t::CaloTower& tower = l1t::CaloTools::getTower(towers,caloNav.offsetFromCurrPos(etaNr,phiNr));
      if(greaterThanMask[etaNr+1][phiNr+4]){
	if(seedCand.hwEtEm() + seedCand.hwEtHad() <  tower.hwEtEm() + tower.hwEtHad()) return false;
      }else{
	if(seedCand.hwEtEm() + seedCand.hwEtHad() <= tower.hwEtEm() + tower.hwEtHad()) return false;
      }
    }
  }
  return true; 
}

//this returns the region number of the best2x3
//0=seed is bottom left, 1 seed is bottom right
//2=seed is middle left, 3 seed is middle right
//4=seed is top left, 5 seed is top right
int l1t::Stage2Layer2ClusterAlgorithmFirmwareImp2::best2x3(const l1t::CaloTower& seed,const std::vector<l1t::CaloTower>& towers)const
{
  int bestRegionNr=0;
  int maxEt=0;
  for(int regionNr=0;regionNr<6;regionNr++){
    int et = cal2x3Et(seed,towers,regionNr,clusEtType_);
    if(et>maxEt){ //biases towards lower region numbers incase of tie
      maxEt=et;
      bestRegionNr=regionNr;
    }
  }
  return bestRegionNr;
}

int l1t::Stage2Layer2ClusterAlgorithmFirmwareImp2::cal2x3Et(const l1t::CaloTower& seed,const std::vector<l1t::CaloTower>& towers,
							    const int regionNr,l1t::CaloTools::SubDet etType)const
{
  int dEtaMin = regionNr%2==0 ? 0 : -1;
  int dPhiMin = regionNr/2-2;

  return l1t::CaloTools::calHwEtSum(seed.hwEta(),seed.hwPhi(),towers,
				    dEtaMin,dEtaMin+1,dPhiMin,dPhiMin+2,etType);
  
}
