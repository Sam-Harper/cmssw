#ifndef DQMOFFLINE_TRIGGER_HLTGENLEVELSOURCE
#define DQMOFFLINE_TRIGGER_HLTGENLEVELSOURCE

// -*- C++ -*-
//
// Package:    EgammaHLTOffline
// Class:      EgammaHLTOffline
//
/*
 Description: This is a DQM source meant to plot high-level HLT trigger
 quantities as stored in the HLT results object TriggerResults for the Egamma triggers
 Notes:
  Currently I would like to plot simple histograms of three seperate types of variables
  1) global event quantities: eg nr of electrons
  2) di-object quanities: transverse mass, di-electron mass
  3) single object kinematic and id variables: eg et,eta,isolation
*/
//
// Original Author:  Sam Harper
//         Created:  June 2008
//
//
//

//#include "DataFormats/HLTReco/interface/TriggerEvent.h"
//#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"

#include "DQMOffline/Trigger/interface/EgHLTBinData.h"
#include "DQMOffline/Trigger/interface/EgHLTCutMasks.h"
#include "DQMOffline/Trigger/interface/EgHLTMonElemContainer.h"
#include "DQMOffline/Trigger/interface/EgHLTMonElemFuncs.h"
#include "DQMOffline/Trigger/interface/EgHLTOffHelper.h"
#include "DQMOffline/Trigger/interface/EgHLTOffEvt.h"
#include "DQMOffline/Trigger/interface/EgHLTTrigCodes.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"

class HLTConfigProvider;

namespace HLTGenLevelVal {
  class HLTFilterMon;
}  // namespace egHLT

// namespace trigger{
//   class TriggerObject;

// }

class
 : public DQMEDAnalyzer {
private:
  MonitorElement* nrEventsProcessedMonElem_;  //number of events processed mon elem
  int nrEventsProcessed_;                     //number of events processed

  std::vector<HLTGenValMon*> filterMonHists_;  //monitoring histograms for different trigger paths, we own them

  std::string dirName_;
  std::string subdirName_;
  egHLT::OffEvt offEvt_;
  egHLT::OffHelper offEvtHelper_;  // this is where up wrap up nasty code which will be replaced by offical tools at some point
  std::unique_ptr<egHLT::TrigCodes> trigCodes;  // the only place instantiate them


  std::vector<std::string> HLTFilterNames_;  //names of the filter names to use for gen particles

  egHLT::BinData binData_;
  egHLT::CutMasks cutMasks_;

  std::string hltTag_;

public:
  explicit HLTGenValSource(const edm::ParameterSet&);

  //disabling copying/assignment (copying this class would be bad, mkay) // I love these comments
  EgHLTOfflineSource(const EgHLTOfflineSource& rhs) = delete;
  EgHLTOfflineSource& operator=(const EgHLTOfflineSource& rhs) = delete;

  ~EgHLTOfflineSource() override;

  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  void addEleTrigPath(egHLT::MonElemFuncs& monElemFuncs, const std::string& name);
  void getHLTFilterNamesUsed(std::vector<std::string>& filterNames) const;
  void filterTriggers(const HLTConfigProvider& hltConfig);
};

#endif
