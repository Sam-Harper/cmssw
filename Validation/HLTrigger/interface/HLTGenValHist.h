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

#include "DQMOffline/Trigger/interface/VarRangeCutColl.h"

#include "FWCore/Framework/interface/Event.h"

#include "Validation/HLTrigger/interface/HLTGenValObject.h"

#include <TH1.h>
#include <TH2.h>

using namespace reco;

// base histogram class, with specific implementations following below
class HLTGenValHist {
public:
  HLTGenValHist() = default;
  virtual ~HLTGenValHist() = default;
  virtual void fill(const HLTGenValObject& objType) = 0;
};

// specific implimentation of a HLTGenValHist for 1D histograms
// it takes the histogram which it will fill
// it takes the variable to plot (func) and its name (varName)
// also, it takes additional cuts (rangeCuts) applied before filling
// to fill the histogram, an object is passed in the Fill function
class HLTGenValHist1D : public HLTGenValHist {
public:
  HLTGenValHist1D(TH1* hist,
               std::string varName,
               std::function<float(const HLTGenValObject&)> func,
               VarRangeCutColl<HLTGenValObject> rangeCuts)
      : var_(std::move(func)), varName_(std::move(varName)), rangeCuts_(std::move(rangeCuts)), hist_(hist) {}

  void fill(const HLTGenValObject& obj) override {
    if(rangeCuts_(obj)) hist_->Fill(var_(obj));
  }

private:
  std::function<float(const HLTGenValObject&)> var_;
  std::string varName_;
  VarRangeCutColl<HLTGenValObject> rangeCuts_;
  TH1* hist_;  //we do not own this
};

// specific implimentation of a HLTGenValHist for 2D histograms
// it takes the histogram which it will fill
// it takes the two variable to plot (func) and their name (varName)
// to fill the histogram, two objects are passed in the Fill function
class HLTGenValHist2D : public HLTGenValHist {
public:
  HLTGenValHist2D(TH2* hist,
               std::string varName_x,
               std::string varName_y,
               std::function<float(const HLTGenValObject&)> func_x,
               std::function<float(const HLTGenValObject&)> func_y)
      : var_x_(std::move(func_x)), var_y_(std::move(func_y)), varName_x_(std::move(varName_x)), varName_y_(std::move(varName_y)), hist_(hist) {}

  void fill(const HLTGenValObject& obj) override {
    hist_->Fill(var_x_(obj), var_y_(obj));
  }

private:
  std::function<float(const HLTGenValObject&)> var_x_;
  std::function<float(const HLTGenValObject&)> var_y_;
  std::string varName_x_;
  std::string varName_y_;
  TH2* hist_;  //we do not own this
};

#endif
