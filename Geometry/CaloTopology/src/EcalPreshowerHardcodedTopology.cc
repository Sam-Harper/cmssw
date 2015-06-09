#include "Geometry/CaloTopology/interface/EcalPreshowerHardcodedTopology.h"

ESDetId EcalPreshowerHardcodedTopology::incrementIx(const ESDetId& id) const {
  //emulating the rather (in my opinion) annoying behaviour of once invalid always invalid of normal topology classes
  if(!ESDetId::validDetId(id.strip(),id.six(),id.siy(),id.plane(),id.zside())) return ESDetId(0);
  return offsetInXYZ(id,1,0,0);
  	 
}

ESDetId EcalPreshowerHardcodedTopology::decrementIx(const ESDetId& id) const {
  //emulating the rather (in my opinion) annoying behaviour of once invalid always invalid of normal topology classes
  if(!ESDetId::validDetId(id.strip(),id.six(),id.siy(),id.plane(),id.zside())) return ESDetId(0);
  
  return offsetInXYZ(id,-1,0,0); 
}

ESDetId EcalPreshowerHardcodedTopology::incrementIy(const ESDetId& id) const {
  //emulating the rather (in my opinion) annoying behaviour of once invalid always invalid of normal topology classes
  if(!ESDetId::validDetId(id.strip(),id.six(),id.siy(),id.plane(),id.zside())) return ESDetId(0);
 

  return offsetInXYZ(id,0,1,0);  
}

ESDetId EcalPreshowerHardcodedTopology::decrementIy(const ESDetId& id) const {
  //emulating the rather (in my opinion) annoying behaviour of once invalid always invalid of normal topology classes
  if(!ESDetId::validDetId(id.strip(),id.six(),id.siy(),id.plane(),id.zside())) return ESDetId(0);
  return offsetInXYZ(id,0,-1,0);  

}
ESDetId EcalPreshowerHardcodedTopology::incrementIz(const ESDetId& id) const {
 //emulating the rather (in my opinion) annoying behaviour of once invalid always invalid of normal topology classes
  if(!ESDetId::validDetId(id.strip(),id.six(),id.siy(),id.plane(),id.zside())) return ESDetId(0);
  return offsetInXYZ(id,0,0,1);  
}

ESDetId EcalPreshowerHardcodedTopology::decrementIz(const ESDetId& id) const {
 //emulating the rather (in my opinion) annoying behaviour of once invalid always invalid of normal topology classes
  if(!ESDetId::validDetId(id.strip(),id.six(),id.siy(),id.plane(),id.zside())) return ESDetId(0);
  return offsetInXYZ(id,0,0,1);  

}


ESDetId EcalPreshowerHardcodedTopology::offsetInXYZ(const ESDetId& id,int x,int y,int plane)
{  
  int newStrip = offsetStrip(id.plane(),id.strip(),x,y);
  int newX = id.six()+x;
  int newY = id.siy()+y;
  int newPlane = id.plane()+plane;

  if(!ESDetId::validDetId(newStrip,newX,newY,newPlane,id.zside())) return ESDetId(0);
  else return ESDetId(newStrip,newX,newY,newPlane,id.zside());
  
}

//this function takes the plane nr, the strip nr and how much to increment in x,y
//it returns the new strip number and the remaining increments in x,y
//ie for plane 2 increamenting y by 2 for stripNr 4 will return a stripNr 6
//and the remaining y incremenation is now zero as we didng leave the strip
//if it was for strip 31, the strip number would be 1 and the y inc would be 1
int EcalPreshowerHardcodedTopology::offsetStrip(int planeNr,int stripNr,int& x,int& y)
{
  int& planeVar = planeNr==1 ? x : y; 
  stripNr+=planeVar;

  planeVar=0;
  
  while(stripNr<ESDetId::ISTRIP_MIN){
    stripNr+=ESDetId::ISTRIP_MAX;
    planeVar--;
  }
  while(stripNr>ESDetId::ISTRIP_MAX){
    stripNr-=ESDetId::ISTRIP_MAX;
    planeVar++; 
  }

  return stripNr;
}
