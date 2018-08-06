#ifndef DQM_HLTEvF_TrigObjTnPHistColl_h
#define DQM_HLTEvF_TrigObjTnPHistColl_h

//********************************************************************************
//
// Description:
//   Manages a set of histograms intended for tag and probe efficiency measurements
//   using TriggerObjects stored in TriggerEvent as the input 
//   selection is limited to basic et/eta/phi cuts and trigger filter requirements
//   The idea that this can run on any of the following data formats RAW,RECO,AOD
//   or even as part of the HLT job
//
//   All histograms in a TrigObjTnPHistColl share the same tag defination and 
//   currently the same probe et/eta/phi cuts. The tag trigger requirements may be
//   to pass multiple triggers anded or ored together
//
//   The TrigObjTnPHistColl then has a series of histograms which are filled for 
//   probes which pass a specified filter. For each specified filter, a set of 
//   2D histograms are produced, et vs mass, eta vs mass, phi vs mass
//   This allows us to get the mass spectrum in each bin to allow signal & bkg fits 
//
// Author : Sam Harper , RAL, Aug 2018
//
//***********************************************************************************


#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "CommonTools/TriggerUtils/interface/GenericTriggerEventFlag.h"
#include "DQMOffline/Trigger/interface/VarRangeCutColl.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"


class TrigObjTnPHistColl {
public:
  class TrigObjCuts {
    class CutData {
    public:
      explicit CutData(const edm::ParameterSet& config);
      bool operator()(const trigger::TriggerObject& obj)const{
	const float val = isAbs_ ? std::abs((obj.*varFunc_)()) : (obj.*varFunc_)();
	return val>=min_ && val<max_;
      }
    private:
      float min_,max_;
      bool isAbs_;
      float (trigger::TriggerObject::*varFunc_)()const;
    };
  public:
    explicit TrigObjCuts(const edm::ParameterSet& config);
    bool operator()(const trigger::TriggerObject& obj)const{
      for(auto& cut : cuts_){
	if(!cut(obj)) return false;
      }
      return true;
    }
  private:
    std::vector<CutData> cuts_;
  };
  
  class FilterSelector {
  public:
    class FilterSet {
    public:
      explicit FilterSet(const edm::ParameterSet& config);
      static edm::ParameterSetDescription makePSetDescription();
      const trigger::Keys getPassingKeys(const trigger::TriggerEvent& trigEvt)const;
					 
    private: 
      std::vector<std::string> filters_;
      bool isAND_;
    };
    
  public:
    FilterSelector(const edm::ParameterSet& config); 
    static edm::ParameterSetDescription makePSetDescription();
    const trigger::Keys getPassingKeys(const trigger::TriggerEvent& trigEvt)const;
  private:
    //helper functions
    static void mergeTrigKeys(trigger::Keys& keys,const trigger::Keys& keysToMerge,bool isAND);
    static void cleanTrigKeys(trigger::Keys& keys);
    
    std::vector<FilterSet> filterSets_;
    bool isAND_;
    
  };
  
  class HistColl {
  public:
    struct Bins {
    public:
      explicit Bins(const edm::ParameterSet& config); 
      static edm::ParameterSetDescription makePSetDescription();
    public:
      std::vector<float> pt;
      std::vector<float> eta;
      std::vector<float> phi;
      std::vector<float> mass;
    };
  public:
    HistColl():ptHist_(nullptr),etaHist_(nullptr),phiHist_(nullptr){}
    void bookHists(DQMStore::IBooker& iBooker,const std::string& name,
		   const std::string& title,const Bins& bins);
    void fill(const trigger::TriggerObject& probe,float mass);
  private:
    MonitorElement* ptHist_; //we do not own this
    MonitorElement* etaHist_; //we do not own this
    MonitorElement* phiHist_; //we do not own this
  };

  class ProbeData {
  public:
    explicit ProbeData(std::string probeFilter):probeFilter_(std::move(probeFilter)){}
    void bookHists(const std::string& tagName,DQMStore::IBooker& iBooker,const HistColl::Bins& bins);
    void fill(const trigger::size_type tagKey,const trigger::TriggerEvent& trigEvt,const VarRangeCutColl<trigger::TriggerObject>& probeCuts);

  private:
    std::string probeFilter_;
    HistColl hists_;
  };
  
public:
  TrigObjTnPHistColl(const edm::ParameterSet& config,edm::ConsumesCollector&& cc);
  static edm::ParameterSetDescription makePSetDescription();
  void bookHists(DQMStore::IBooker& iBooker);
  void fill(const trigger::TriggerEvent& trigEvt,
	    const edm::Event& event,const edm::EventSetup& setup);

private: 
  //helper function, probably should go in a utilty class
  static const trigger::Keys getKeys(const trigger::TriggerEvent& trigEvt,const std::string& filterName);
  
  VarRangeCutColl<trigger::TriggerObject> tagCuts_,probeCuts_;
  FilterSelector tagFilters_;
  std::string collName_;
  std::string folderName_;
  HistColl::Bins histBins_;
  std::vector<ProbeData> probeHists_; 
  GenericTriggerEventFlag evtTrigSel_;

};


#endif
