#ifndef HLTValidationHist_h
#define HLTValidationHist_h

//********************************************************************************
//
// Description:
//   Histogramm class
//
// Author : Finn Labe, Sep 2021
// stolen from Sam Harper
//
//***********************************************************************************

#include "DQMOffline/Trigger/interface/FunctionDefs.h"
#include "FWCore/Framework/interface/Event.h"

#include <TH1.h>
#include <TH2.h>

//our base class for our histograms
//takes an object, edm::Event,edm::EventSetup and fills the histogram
//with the predetermined variable (or variables)
template <typename ObjType>
class HLTValidationHist {
public:
  HLTValidationHist() = default;
  virtual ~HLTValidationHist() = default;
  virtual void fill(const ObjType& objType,
                    const edm::Event& event,
                    const edm::EventSetup& setup);
};

//this class is a specific implimentation of a HLTValidationHist
//it has the value with which to fill the histogram
//and the histogram itself
//we do not own the histogram
template <typename ObjType, typename ValType>
class HLTValidationHist1D : public HLTValidationHist<ObjType> {
public:
  HLTValidationHist1D(TH1* hist,
               std::string varName,
               std::function<ValType(const ObjType&)> func)
      : var_(std::move(func)), varName_(std::move(varName)), hist_(hist) {}

  void fill(const ObjType& obj,
            const edm::Event& event,
            const edm::EventSetup& setup) override {
      hist_->Fill(var_(obj));
  }

private:
  std::function<ValType(const ObjType&)> var_;
  std::string varName_;
  TH1* hist_;  //we do not own this
};

#endif
