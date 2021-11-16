#ifndef DQMOnline_Trigger_HLTGenValHist_h
#define DQMOnline_Trigger_HLTGenValHist_h

//********************************************************************************
//
// Description:
//   Histogram "holder" class.
//   Stolen from Sam Harpers HLTDQMHist
//
// Implimentation:
//
//
// Author : Finn Labe, UHH, Oct. 2021
//
//***********************************************************************************

#include "DQMOffline/Trigger/interface/FunctionDefs.h"

// May be useful later for applying event level cuts
//#include "DQMOffline/Trigger/interface/VarRangeCutColl.h"

#include "FWCore/Framework/interface/Event.h"

#include "Validation/HLTrigger/interface/HLTGenValObject.h"

#include <TH1.h>
#include <TH2.h>

using namespace reco;

//our base class for our histograms
//takes an object, edm::Event,edm::EventSetup and fills the histogram
//with the predetermined variable (or varaibles)
class HLTGenValHist {
public:
  HLTGenValHist() = default;
  virtual ~HLTGenValHist() = default;
  virtual void fill(const HLTGenValObject& objType) = 0;
};

//this class is a specific implimentation of a HLTGenValHist
//it has the value with which to fill the histogram
//and the histogram itself
//we do not own the histogram
class HLTGenValHist1D : public HLTGenValHist {
public:
  HLTGenValHist1D(TH1* hist,
               std::string varName,
               std::function<float(const HLTGenValObject&)> func)
      : var_(std::move(func)), varName_(std::move(varName)), hist_(hist) {}

  void fill(const HLTGenValObject& obj) override {
    hist_->Fill(var_(obj));
  }

private:
  std::function<float(const HLTGenValObject&)> var_;
  std::string varName_;
  TH1* hist_;  //we do not own this
};

#endif
