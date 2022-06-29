#include "Validation/HLTrigger/interface/HLTGenValPathSpecificCutParser.h"

HLTGenValPathSpecificCutParser::HLTGenValPathSpecificCutParser(std::string pathSpecificCuts) {

  // splitting the cutstring
  std::stringstream pathSpecificCutsStream(pathSpecificCuts);
  std::string pathSpecificCutsSegment;
  std::vector<std::string> pathSpecificCutsSeglist;
  while(std::getline(pathSpecificCutsStream, pathSpecificCutsSegment, ','))
  {
     pathSpecificCutsSeglist.push_back(pathSpecificCutsSegment);
  }

  for (auto pathSpecificCut : pathSpecificCutsSeglist) {

    // each of these strings is expected to contain exactly one equal sign
    std::stringstream pathSpecificCutStream(pathSpecificCut);
    std::string pathSpecificCutSegment;
    std::vector<std::string> pathSpecificCutSeglist;
    while(std::getline(pathSpecificCutStream, pathSpecificCutSegment, '='))
    {
       pathSpecificCutSeglist.push_back(pathSpecificCutSegment);
    }
    if(pathSpecificCutSeglist.size() != 2) throw cms::Exception("InputError") << "Path-specific cuts could not be parsed. Make sure that each parameter contains exactly one equal sign!.\n";
    const std::string cutVariable = pathSpecificCutSeglist.at(0);
    const std::string cutParameter = pathSpecificCutSeglist.at(1);

    edm::ParameterSet rangeCutConfig;
    if (cutVariable == "absEtaMax" || cutVariable == "absEtaCut") {
      rangeCutConfig.addParameter<std::string>("rangeVar", "eta");
      rangeCutConfig.addParameter<std::vector<std::string>>("allowedRanges", {"-"+cutParameter+":"+cutParameter} );
    } else if (cutVariable == "absEtaMin") {
      rangeCutConfig.addParameter<std::string>("rangeVar", "eta");
      rangeCutConfig.addParameter<std::vector<std::string>>("allowedRanges", {"-999:"+cutParameter, cutParameter+":999"} );
    } else if (cutVariable == "ptMax") {
      rangeCutConfig.addParameter<std::string>("rangeVar", "pt");
      rangeCutConfig.addParameter<std::vector<std::string>>("allowedRanges", {"0:"+cutParameter} );
    } else if (cutVariable == "ptMin" || cutVariable == "ptCut") {
      rangeCutConfig.addParameter<std::string>("rangeVar", "pt");
      rangeCutConfig.addParameter<std::vector<std::string>>("allowedRanges", {cutParameter+":999999"} );
    } else if (cutVariable == "etMax") {
      rangeCutConfig.addParameter<std::string>("rangeVar", "et");
      rangeCutConfig.addParameter<std::vector<std::string>>("allowedRanges", {"0:"+cutParameter} );
    } else if (cutVariable == "etMin" || cutVariable == "etCut") {
      rangeCutConfig.addParameter<std::string>("rangeVar", "et");
      rangeCutConfig.addParameter<std::vector<std::string>>("allowedRanges", {cutParameter+":999999"} );
    } else if (cutVariable == "region") {
      rangeCutConfig.addParameter<std::string>("rangeVar", "eta");

      // various predefined regions
      // multiple regions might used, which are then split by a plus sign
      std::stringstream cutParameterStream(cutParameter);
      std::string cutParameterSegment;
      std::vector<std::string> cutParameterSeglist;
      while(std::getline(cutParameterStream, cutParameterSegment, '+'))
      {
         cutParameterSeglist.push_back(cutParameterSegment);
      }

      for (const auto region : cutParameterSeglist) {
        if(region == "EB") {
          rangeCutConfig.addParameter<std::vector<std::string>>("allowedRanges", {"-1.2:1.2"} );
        } else if (region == "EE") {
          rangeCutConfig.addParameter<std::vector<std::string>>("allowedRanges", {"-999:-1.2", "1.2:999"} );
        } else {
          throw cms::Exception("InputError") << "Region "+region+" not recognized.\n";
        }
      }

    } else if (cutVariable == "bins") {

      // TODO add more binnings
      if(cutParameter == "somebinning") {
        if(pathSpecificBins_.size() > 0) throw cms::Exception("InputError") << "Multiple different binnings set for a path!.\n";
        else pathSpecificBins_ = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
      }

    } else {
      throw cms::Exception("InputError") << "Path-specific cut "+cutVariable+" not recognized.\n";
    }

    pathSpecificCutsVector_.push_back( rangeCutConfig );

  }

}
