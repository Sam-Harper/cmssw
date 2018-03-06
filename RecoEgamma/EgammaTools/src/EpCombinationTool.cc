#include "RecoEgamma/EgammaTools/interface/EpCombinationTool.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

#include <cmath>
#include <vector>
#include <vdt/vdtMath.h>


EpCombinationTool::EpCombinationTool(const edm::ParameterSet& iConfig):
  ecalTrkEnergyRegress_(iConfig.getParameter<edm::ParameterSet>("ecalTrkRegressionConfig")),
  ecalTrkEnergyRegressUncert_(iConfig.getParameter<edm::ParameterSet>("ecalTrkRegressionUncertConfig")),
  maxEcalEnergyForComb_(iConfig.getParameter<double>("maxEcalEnergyForComb")),
  minEOverPForComb_(iConfig.getParameter<double>("minEOverPForComb")),
  maxEPDiffInSigmaForComb_(iConfig.getParameter<double>("maxEPDiffInSigmaForComb")),
  maxRelTrkMomErrForComb_(iConfig.getParameter<double>("maxRelTrkMomErrForComb"))
{

}
  
void EpCombinationTool::setEventContent(const edm::EventSetup& iSetup)
{
  ecalTrkEnergyRegress_.setEventContent(iSetup);
  ecalTrkEnergyRegressUncert_.setEventContent(iSetup);
}

std::pair<float, float> EpCombinationTool::combine(reco::GsfElectron& ele)const
{
  const float scRawEnergy = ele.superCluster()->rawEnergy(); 
  const float esEnergy = ele.superCluster()->preshowerEnergy();
  

  const float corrEcalEnergy = ele.correctedEcalEnergy();
  const float corrEcalEnergyErr = ele.correctedEcalEnergyError();
  const float ecalCorr = ele.correctedEcalEnergy() / (scRawEnergy+esEnergy);
  const float ecalCorrErr =  corrEcalEnergyErr / corrEcalEnergy;

  auto gsfTrk = ele.gsfTrack();

  const float trkP = gsfTrk->pMode();
  const float trkEta = gsfTrk->etaMode();
  const float trkPhi = gsfTrk->phiMode();
  const float trkPErr = std::abs(gsfTrk->qoverpModeError())*trkP*trkP; 
  const float eOverP = corrEcalEnergy/trkP;
  const float fbrem = ele.fbrem();

  if(corrEcalEnergy < maxEcalEnergyForComb_ &&
     eOverP > minEOverPForComb_ && 
     std::abs(corrEcalEnergy - trkP) < maxEPDiffInSigmaForComb_*std::sqrt(trkPErr*trkPErr+corrEcalEnergyErr*corrEcalEnergyErr) && 
     trkPErr < maxRelTrkMomErrForComb_*trkP) { 

    const float preCombinationEt = corrEcalEnergy/std::cosh(trkEta);
 
   
    std::array<float, 9> eval;  
    eval[0] = corrEcalEnergy;
    eval[1] = ecalCorrErr/ecalCorr;
    eval[2] = trkPErr/trkP;
    eval[3] = eOverP;
    eval[4] = ele.ecalDrivenSeed();
    eval[5] = ele.full5x5_showerShape().r9;
    eval[6] = fbrem;
    eval[7] = trkEta; 
    eval[8] = trkPhi; 

    float mean  = ecalTrkEnergyRegress_(preCombinationEt,ele.isEB(),eval.data());
    float sigma  = ecalTrkEnergyRegressUncert_(preCombinationEt,ele.isEB(),eval.data());
    // Final correction
    // A negative energy means that the correction went
    // outside the boundaries of the training. In this case uses raw.
    // The resolution estimation, on the other hand should be ok.
    if (mean < 0.) mean = 1.0;

    float rawCombEnergy = ( corrEcalEnergy*trkPErr*trkPErr + trkP*corrEcalEnergyErr*corrEcalEnergyErr ) / ( trkPErr*trkPErr + corrEcalEnergyErr*corrEcalEnergyErr );	
   
    return std::make_pair(mean*rawCombEnergy,sigma*rawCombEnergy);
  }else{
    return std::make_pair(corrEcalEnergy, corrEcalEnergyErr);
  }
}
