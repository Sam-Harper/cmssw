///
/// Description: Firmware headers
///
/// Implementation:
///    Concrete firmware implementations
///
/// \author: Jim Brooke - University of Bristol
///

//
//

#ifndef Stage2Layer2ClusterAlgorithmFirmware_H
#define Stage2Layer2ClusterAlgorithmFirmware_H

#include "L1Trigger/L1TCalorimeter/interface/Stage2Layer2ClusterAlgorithm.h"
#include "CondFormats/L1TObjects/interface/CaloParams.h"
#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"

namespace l1t 
{

  // Imp1 is for v1 and v2
  class Stage2Layer2ClusterAlgorithmFirmwareImp1 : public Stage2Layer2ClusterAlgorithm 
  {
    public:
      enum ClusterInput
      {
        E  = 0,
        H  = 1,
        EH = 2
      };

      Stage2Layer2ClusterAlgorithmFirmwareImp1(CaloParams* params, ClusterInput clusterInput);
      virtual ~Stage2Layer2ClusterAlgorithmFirmwareImp1();
      virtual void processEvent(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters);

    private:
      void clustering(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters);
      void filtering(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters);
      void sharing(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters);
      void refining(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters);

      // parameters
      ClusterInput clusterInput_;
      int seedThreshold_;
      int clusterThreshold_;   
      int hcalThreshold_;
      CaloParams* params_;
  };


  // Imp2 is for 2x3 clustering in a 3x9 area
  class Stage2Layer2ClusterAlgorithmFirmwareImp2 : public Stage2Layer2ClusterAlgorithm {
  public:
    
    Stage2Layer2ClusterAlgorithmFirmwareImp2(CaloParams* params, l1t::CaloTools::SubDet clusEtType);
    virtual ~Stage2Layer2ClusterAlgorithmFirmwareImp2(){}
    
    virtual void processEvent(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters) override;
    
  private:
    bool isSeed(const l1t::CaloTower& seedCand,const std::vector<l1t::CaloTower>& towers)const;
    int best2x3(const l1t::CaloTower& seed,const std::vector<l1t::CaloTower>& towers)const;
    int cal2x3Et(const l1t::CaloTower& seed,const std::vector<l1t::CaloTower>& towers,
		 const int regionNr,l1t::CaloTools::SubDet etType)const;
    void clustering(const std::vector<l1t::CaloTower>& towers, std::vector<l1t::CaloCluster>& clusters)const;
    

    // parameters
    l1t::CaloTools::SubDet clusEtType_;
    int seedThreshold_;
    CaloParams* params_; //we dont own this
  };

}

#endif
