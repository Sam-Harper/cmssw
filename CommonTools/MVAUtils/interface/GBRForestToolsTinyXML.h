#ifndef CommonToolsTinyXMLs_MVAUtils_GBRForestToolsTinyXML_h
#define CommonToolsTinyXMLs_MVAUtils_GBRForestToolsTinyXML_h

//--------------------------------------------------------------------------------------------------
//
// GRBForestToolsTinyXML
//
// Utility to parse an XML weights files specifying an ensemble of decision trees into a GRBForest.
//
// Author: Jonas Rembser
//--------------------------------------------------------------------------------------------------


#include "CondFormats/EgammaObjects/interface/GBRForest.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include <memory>

// Create a GBRForest from an XML weight file
std::unique_ptr<const GBRForest> createGBRForestTinyXML(const std::string     &weightsFile);
std::unique_ptr<const GBRForest> createGBRForestTinyXML(const edm::FileInPath &weightsFile);

// Overloaded versions which are taking string vectors by reference to strore the variable names in
std::unique_ptr<const GBRForest> createGBRForestTinyXML(const std::string     &weightsFile, std::vector<std::string> &varNames);
std::unique_ptr<const GBRForest> createGBRForestTinyXML(const edm::FileInPath &weightsFile, std::vector<std::string> &varNames);

#endif
