#include "RecoEgamma/EgammaIsolationAlgos/interface/EleTkIsolFromTks.h"


EleTkIsolFromTks::EleTkIsolFromTks(const edm::ParameterSet& para):
  tkSelector_(para)
{
  
}

edm::ParameterSetDescription EleTkIsolFromTks::pSetDescript()
{
  return EleTkIsolTkSelector::pSetDescript();
}

std::pair<int,double> 
EleTkIsolFromTks::calIsol(const reco::TrackBase& eleTrk,
			  const reco::TrackCollection& trks)const
{
  return calIsol(eleTrk.eta(),eleTrk.phi(),eleTrk.vz(),trks);
}

std::pair<int,double> 
EleTkIsolFromTks::calIsol(const double eleEta,const double elePhi,
			  const double eleVZ,
			  const reco::TrackCollection& trks)const
{
  double ptSum=0.;
  int nrTrks=0;
  
  for(auto& trk  : trks){
    if(tkSelector_(trk,trk.pt(),eleEta,elePhi,eleVZ)){	
      ptSum+=trk.pt();
      nrTrks++;
    }
  }
  
  return {nrTrks,ptSum};	
}
