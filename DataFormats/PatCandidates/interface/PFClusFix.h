#ifndef DATAFORMATS_PATCANDIDATES_PFCLUSFIX_H
#define DATAFORMATS_PATCANDIDATES_PFCLUSFIX_H

#include "DataFormats/PatCandidates/interface/Electron.h"

void setClusIso(pat::Electron& ele,float isoEcalClus,float isoHcalClus){
  auto isoVar = ele.pfIsolationVariables();
  isoVar.sumEcalClusterEt = isoEcalClus;
  isoVar.sumHcalClusterEt = isoHcalClus;
  ele.setPfIsolationVariables(isoVar);
}

#endif
