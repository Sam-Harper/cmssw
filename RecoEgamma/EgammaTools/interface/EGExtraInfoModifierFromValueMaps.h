#ifndef RecoEgamma_EgammaTools_EGExtraInfoModifierFromValueMaps_h
#define RecoEgamma_EgammaTools_EGExtraInfoModifierFromValueMaps_h

#include "CommonTools/CandAlgos/interface/ModifyObjectValueBase.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

#include <unordered_map>

namespace {
  const edm::InputTag empty_tag;
}

//class: EGExtraInfoModifierFromValueMaps
//  
//this is a generalisation of EGExtraInfoModiferFromFloatValueMaps
//orginal author of EGExtraInfoModiferFromFloatValueMaps : L. Gray (FNAL)
//converter to templated version: S. Harper (RAL)
//
//This class allows an data of an arbitrary type in a ValueMap for pat::Electrons or pat::Photons
//to be put in the pat::Electron/Photon as userData, userInt or userFloat
//
//It assumes that the object can be added via pat::PATObject::userData, see pat::PATObject for the 
//constraints here
//
//The class has two template arguements:
//  MapType : c++ type of the object stored in the value mape
//  OutputType : c++ type of how you want to store it in the pat::PATObject
//               this exists so you can specialise int and float (and future exceptions) to use
//               pat::PATObject::userInt and pat::PATObject::userFloat
//               The specialisations are done by EGXtraModFromVMObjFiller::addValueToObject
//               
// MapType and OutputType do not have to be same (but are by default). This is useful as it allows
// things like bools to and unsigned ints to be converted to ints to be stored as  a userInt
// rather than having to go to the bother of setting up userData hooks for them


namespace egmodifier{
  class EGID{};//dummy class to be used as a template arguement 
}

template<typename MapType>
struct ValueMapData {
  std::string name;
  edm::InputTag tag;
  edm::EDGetTokenT<edm::ValueMap<MapType> > token;
  edm::Handle<edm::ValueMap<MapType> > handle;
  ValueMapData(std::string iName,const edm::InputTag& iTag):name(std::move(iName)),tag(iTag){}
  void setHandle(const edm::Event& evt){
      evt.getByToken(token,handle);
    }
  void setToken(edm::ConsumesCollector& cc){
    token = cc.consumes<edm::ValueMap<MapType> >(tag);
  }
};


template<typename OutputType>
class EGXtraModFromVMObjFiller {
public:
  EGXtraModFromVMObjFiller()=delete;
  ~EGXtraModFromVMObjFiller()=delete;

  //will do a UserData add but specialisations exist for float and ints
  template<typename ObjType,typename MapType>
  static void 
  addValueToObject(ObjType& obj,
		   const ValueMapData<MapType>& vmData,
		   bool overrideExistingValues);
  
  template<typename ObjType,typename MapType>
  static void 
  addValuesToObject(ObjType& obj,
		    const std::vector<ValueMapData<MapType> >& vmapsData,
		    bool overrideExistingValues){
    for(auto& vmapData : vmapsData){
      addValueToObject(obj,vmapData,overrideExistingValues);
    }  
  }
};		    
		    

template<typename MapType,typename OutputType=MapType>
class EGExtraInfoModifierFromValueMaps : public ModifyObjectValueBase {
public:
  EGExtraInfoModifierFromValueMaps(const edm::ParameterSet& conf);
  
  void setEvent(const edm::Event&) final;
  void setEventContent(const edm::EventSetup&) final;
  void setConsumes(edm::ConsumesCollector&) final;
  
  void modifyObject(pat::Electron&) const final;
  void modifyObject(pat::Photon&) const final;
 
private:
  std::vector<ValueMapData<MapType> > eleVMData_;
  std::vector<ValueMapData<MapType> > phoVMData_;
  bool overrideExistingValues_;
};


template<typename MapType,typename OutputType>
EGExtraInfoModifierFromValueMaps<MapType,OutputType>::
EGExtraInfoModifierFromValueMaps(const edm::ParameterSet& conf) :
  ModifyObjectValueBase(conf) {
  overrideExistingValues_ = conf.exists("overrideExistingValues") ? conf.getParameter<bool>("overrideExistingValues") : false;
  if( conf.exists("electron_config") ) {
    const edm::ParameterSet& ele_cfg = conf.getParameter<edm::ParameterSet>("electron_config");
    const std::vector<std::string>& parameters = ele_cfg.getParameterNames();
    for( const std::string& name : parameters ) {
      if( ele_cfg.existsAs<edm::InputTag>(name) ) {
        eleVMData_.emplace_back(ValueMapData<MapType>(name,ele_cfg.getParameter<edm::InputTag>(name)));
      }
    }    
  }
  if( conf.exists("photon_config") ) {
    const edm::ParameterSet& pho_cfg = conf.getParameter<edm::ParameterSet>("photon_config");
    const std::vector<std::string>& parameters = pho_cfg.getParameterNames();
    for( const std::string& name : parameters ) {
      if( pho_cfg.existsAs<edm::InputTag>(name) ) {
        phoVMData_.emplace_back(ValueMapData<MapType>(name,pho_cfg.getParameter<edm::InputTag>(name)));
      }
    }    
  }
}


template<typename MapType,typename OutputType>
void EGExtraInfoModifierFromValueMaps<MapType,OutputType>::
setEvent(const edm::Event& evt) {

  for( auto& data : eleVMData_) data.setHandle(evt);
  for( auto& data : phoVMData_) data.setHandle(evt);
  
}

template<typename MapType,typename OutputType>
void EGExtraInfoModifierFromValueMaps<MapType,OutputType>::
setEventContent(const edm::EventSetup& evs) {
}

template<typename MapType,typename OutputType>
void EGExtraInfoModifierFromValueMaps<MapType,OutputType>::
setConsumes(edm::ConsumesCollector& cc) {  
  for( auto& data : eleVMData_) data.setToken(cc);
  for( auto& data : phoVMData_) data.setToken(cc);
}


template<typename MapType,typename OutputType>
void EGExtraInfoModifierFromValueMaps<MapType,OutputType>::
modifyObject(pat::Electron& ele) const {
  edm::Ptr<reco::GsfElectron> ptr;
  if(!ele.parentRefs().empty()) ptr = ele.parentRefs().back();

  //now we go through and modify the objects using the valuemaps we read in 
  EGXtraModFromVMObjFiller<OutputType>::addValuesToObject(ele,eleVMData_,overrideExistingValues_);
}


template<typename MapType,typename OutputType>
void EGExtraInfoModifierFromValueMaps<MapType,OutputType>::
modifyObject(pat::Photon& pho) const {
  edm::Ptr<reco::Photon> ptr;
  if(!pho.parentRefs().empty()) ptr = pho.parentRefs().back();
  //now we go through and modify the objects using the valuemaps we read in
  EGXtraModFromVMObjFiller<OutputType>::addValuesToObject(pho,phoVMData_,overrideExistingValues_);
							  
}


template<typename OutputType>
template<typename ObjType,typename MapType>
void EGXtraModFromVMObjFiller<OutputType>::
addValueToObject(ObjType& obj,
		 const ValueMapData<MapType>& mapData,
		 bool overrideExistingValues)
{

  if(obj.parentRefs().empty()){
    throw cms::Exception("LogicError") << " object "<<typeid(obj).name()<<" has no parent references, these should be set before calling the modifier";
  }
  auto ptr = obj.parentRefs().back();
  auto value = (*mapData.handle)[ptr];
  if( overrideExistingValues || !obj.hasUserData(mapData.name) ) {
    obj.addUserData(mapData.name,value,true);
  } else {
    throw cms::Exception("ValueNameAlreadyExists")
      << "Trying to add new UserData = " << mapData.name
      << " failed because it already exists and you didnt specify to override it (set in the config overrideExistingValues=cms.bool(True) )";
  }
}  

template<>
template<typename ObjType,typename MapType>
void EGXtraModFromVMObjFiller<float>::
addValueToObject(ObjType& obj,
		 const ValueMapData<MapType>& mapData,
		 bool overrideExistingValues)
{
  if(obj.parentRefs().empty()){
    throw cms::Exception("LogicError") << " object "<<typeid(obj).name()<<" has no parent references, these should be set before calling the modifier";
  }
  auto ptr = obj.parentRefs().back();
  auto value = (*mapData.handle)[ptr];

  if( overrideExistingValues || !obj.hasUserFloat(mapData.name) ) {
    obj.addUserFloat(mapData.name,value,true);
  } else {
    throw cms::Exception("ValueNameAlreadyExists")
      << "Trying to add new UserFloat = " << mapData.name
      << " failed because it already exists and you didnt specify to override it (set in the config overrideExistingValues=cms.bool(True) )";
  }
}

template<>
template<typename ObjType,typename MapType>
void EGXtraModFromVMObjFiller<int>::
addValueToObject(ObjType& obj,
		 const ValueMapData<MapType>& mapData,
		 bool overrideExistingValues)
{
  if(obj.parentRefs().empty()){
    throw cms::Exception("LogicError") << " object "<<typeid(obj).name()<<" has no parent references, these should be set before calling the modifier";
  }
  auto ptr = obj.parentRefs().back();
  auto value = (*mapData.handle)[ptr];
  if( overrideExistingValues || !obj.hasUserInt(mapData.name) ) {
    obj.addUserInt(mapData.name,value,true);
  } else {
    throw cms::Exception("ValueNameAlreadyExists")
      << "Trying to add new UserInt = " << mapData.name
      << " failed because it already exists and you didnt specify to override it (set in the config overrideExistingValues=cms.bool(True) )";
  }
}  

template<>
template<>
void EGXtraModFromVMObjFiller<egmodifier::EGID>::
addValuesToObject(pat::Electron& obj,
		  const std::vector<ValueMapData<float> >& vmapsData,
		  bool overrideExistingValues)
{
  std::vector<std::pair<std::string,float >> ids;
  if(obj.parentRefs().empty()){
    throw cms::Exception("LogicError") << " object "<<typeid(obj).name()<<" has no parent references, these should be set before calling the modifier";
  }    
  auto ptr = obj.parentRefs().back();
  for( auto& vmapData : vmapsData ) {
    float idVal = (*vmapData.handle)[ptr];
    ids.push_back({vmapData.name,idVal});
  }   
  std::sort(ids.begin(),ids.end(),[](auto& lhs,auto& rhs){return lhs.first<rhs.first;});
  obj.setElectronIDs(ids);
}

template<>
template<>
void EGXtraModFromVMObjFiller<egmodifier::EGID>::
addValuesToObject(pat::Photon& obj,
		  const std::vector<ValueMapData<float> >& vmapsData,
		  bool overrideExistingValues)
{
  //we do a float->bool conversion here to make things easier to be consistent with electrons
  std::vector<std::pair<std::string,bool> > ids;
  if(obj.parentRefs().empty()){
    throw cms::Exception("LogicError") << " object "<<typeid(obj).name()<<" has no parent references, these should be set before calling the modifier";
  }
  auto ptr = obj.parentRefs().back();
  for( auto& vmapData : vmapsData ) {
    float idVal = (*vmapData.handle)[ptr];
    ids.push_back({vmapData.name,idVal});
  }  
  std::sort(ids.begin(),ids.end(),[](auto& lhs,auto& rhs){return lhs.first<rhs.first;});
  obj.setPhotonIDs(ids);
}


#endif
