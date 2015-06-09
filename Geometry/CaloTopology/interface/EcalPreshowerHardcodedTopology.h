#ifndef GEOMETRY_CALOTOPOLOGY_ECALPRESHOWERHARDCODEDTOPOLOGY_H
#define GEOMETRY_CALOTOPOLOGY_ECALPRESHOWERHARDCODEDTOPOLOGY_H 1

#include <vector>
#include <iostream>
#include "DataFormats/EcalDetId/interface/ESDetId.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"

class EcalPreshowerHardcodedTopology GCC11_FINAL : public CaloSubdetectorTopology
{

 public:
  /// create a new Topology
  EcalPreshowerHardcodedTopology() {};

  virtual ~EcalPreshowerHardcodedTopology() {};
  
   /// move the Topology north (increment iy)  
  virtual DetId  goNorth(const DetId& id) const {
    return incrementIy(ESDetId(id));
  }
  virtual std::vector<DetId> north(const DetId& id) const
  { 
    ESDetId nextId= goNorth(id);
    std::vector<DetId> vNeighborsDetId;
    if (! (nextId==ESDetId(0)))
      vNeighborsDetId.push_back(DetId(nextId.rawId()));
    return vNeighborsDetId;
  }
  
  /// move the Topology south (decrement iy)
  virtual DetId goSouth(const DetId& id) const {
    return decrementIy(ESDetId(id));
  }
  virtual std::vector<DetId> south(const DetId& id) const
  { 
    ESDetId nextId= goSouth(id);
    std::vector<DetId> vNeighborsDetId;
    if (! (nextId==ESDetId(0)))
      vNeighborsDetId.push_back(DetId(nextId.rawId()));
    return vNeighborsDetId;
  }
  
  /// move the Topology east (positive ix)
  virtual DetId  goEast(const DetId& id) const {
    return incrementIx(ESDetId(id));
  }
  virtual std::vector<DetId> east(const DetId& id) const
  { 
    ESDetId nextId=goEast(id);
    std::vector<DetId> vNeighborsDetId;
    if (! (nextId==ESDetId(0)))
      vNeighborsDetId.push_back(DetId(nextId.rawId()));
    return vNeighborsDetId;
  }
  
  /// move the Topology west (negative ix)
  virtual DetId goWest(const DetId& id) const {
    return decrementIx(ESDetId(id));
  }
  virtual std::vector<DetId> west(const DetId& id) const
  { 
    ESDetId nextId=goWest(id);
    std::vector<DetId> vNeighborsDetId;
    if (! (nextId==ESDetId(0)))
      vNeighborsDetId.push_back(DetId(nextId.rawId()));
    return vNeighborsDetId;
  }
  


  virtual DetId goUp(const DetId& id) const {
    return incrementIz(ESDetId(id));
  }
  virtual std::vector<DetId> up(const DetId& id) const
  {
    ESDetId nextId=goUp(id);
    std::vector<DetId> vNeighborsDetId;
    if (! (nextId==ESDetId(0)))
      vNeighborsDetId.push_back(DetId(nextId.rawId()));
    return  vNeighborsDetId;
  }
  
  virtual DetId goDown(const DetId& id) const {
    return decrementIz(ESDetId(id));
  }
  virtual std::vector<DetId> down(const DetId& id) const
  {
    ESDetId nextId=goDown(id);
    std::vector<DetId> vNeighborsDetId;
    if (! (nextId==ESDetId(0)))
      vNeighborsDetId.push_back(DetId(nextId.rawId()));
    return  vNeighborsDetId;
  }
  
 private:
  
  /// move the nagivator to larger ix
  ESDetId incrementIx(const ESDetId&) const ;

  /// move the nagivator to smaller ix
  ESDetId decrementIx(const ESDetId&) const ;

  /// move the nagivator to larger iy
  ESDetId incrementIy(const ESDetId&) const ;

  /// move the nagivator to smaller iy
  ESDetId decrementIy(const ESDetId&) const;

   /// move the nagivator to larger iz
  ESDetId incrementIz(const ESDetId& id) const;

  /// move the nagivator to smaller iz
  ESDetId decrementIz(const ESDetId& id) const; 
 
  //and these two functions to all the work for the previous 6 functions
  static ESDetId offsetInXYZ(const ESDetId& id,int x,int y,int plane);
  static int offsetStrip(int planeNr,int stripNr,int& x,int &y);
  

};

#endif
