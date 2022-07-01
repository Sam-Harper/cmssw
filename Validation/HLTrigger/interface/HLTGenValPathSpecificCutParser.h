#ifndef Validation_HLTrigger_HLTGenValPathSpecificCutParser_h
#define Validation_HLTrigger_HLTGenValSpecificCutParser_h

//********************************************************************************
//
// Description:
//   This class handles parsing of additional cuts that can be set for each path in the generator-level validation module
//
// Author : Finn Labe, UHH, Jun. 2022
//
//********************************************************************************

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include <vector>

class HLTGenValPathSpecificCutParser {
public:

  // constructor
  HLTGenValPathSpecificCutParser(std::string pathSpecificCuts, std::vector<edm::ParameterSet> binnings, std::string vsVar);

  std::vector<edm::ParameterSet> getPathSpecificCuts() {return pathSpecificCutsVector_;}
  std::vector<double> getPathSpecificBins() {return pathSpecificBins_;}
  bool havePathSpecificBins() { return (pathSpecificBins_.size() > 0); }
  std::string getTag() {return tag_;}

private:
  std::vector<edm::ParameterSet> pathSpecificCutsVector_;
  std::vector<double> pathSpecificBins_;
  std::string tag_ = "";

};

#endif
