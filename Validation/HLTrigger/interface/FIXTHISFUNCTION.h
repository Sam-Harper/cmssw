#ifndef FIXTHISFUNCTION_h
#define FIXTHISFUNCTION_h

//***************************************************************************
//
// Description:
//   These are the functions we wish to access via strings
//   The function returns a std::function holding the function
//   Have to admit, I'm not happy with the implimentation but fine
//   no time to do something better
//
//   There are various issues. The main is the awkward specialisations
//   needed for the different types. Its a nasty hack. Probably
//   can do it cleaner with ROOT dictionaries
//
// Useage:
//   1) define any extra functions you need at the top (seed scEtaFunc as example)
//   2) generic functions applicable to all normal objects are set in
//      getUnaryFuncFloat (if they are floats, other types will need seperate
//      functions which can be done with this as example
//   3) object specific functions are done with getUnaryFuncExtraFloat
//      by specialising that function approprately for the object
//   4) user should simply call getUnaryFuncFloat()
//
// Author: Sam Harper (RAL) , 2017
//
//***************************************************************************

#include "FWCore/Utilities/interface/Exception.h"

#include <vector>
#include <functional>

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"

#include "DQMOffline/Trigger/interface/FunctionDefs.h"

namespace hltdqm {

  template <>
  std::function<float(const reco::GsfElectron&)> getUnaryFuncFloat<reco::GsfElectron>(const std::string& varName);
  template <>
  std::function<float(const reco::Photon&)> getUnaryFuncFloat<reco::Photon>(const std::string& varName);

}  // namespace hltdqm

#endif
