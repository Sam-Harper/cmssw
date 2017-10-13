import FWCore.ParameterSet.Config as cms

from RecoEgamma.EgammaTools.regressionModifier_cfi import regressionModifier

#remakes the slimmedElectrons, photons with a new energy regression
slimmedElectrons = cms.EDProducer("ModifiedElectronProducer",
    src = cms.InputTag("slimmedElectrons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet(regressionModifier) )
)
slimmedPhotons = cms.EDProducer("ModifiedPhotonProducer",
    src = cms.InputTag("slimmedPhotons",processName=cms.InputTag.skipCurrentProcess()),
    modifierConfig = cms.PSet( modifications = cms.VPSet(regressionModifier) )
)    


packedPFCandidates = cms.EDProducer("EGPackedPFCandCorrector",
                                    eles = cms.InputTag("slimmedElectrons"),
                                    phos = cms.InputTag("slimmedPhotons"),
                                    oldPhos = cms.InputTag("slimmedPhotons",processName=cms.InputTag.skipCurrentProcess()),
                                    
                                    srcCands = cms.InputTag("packedPFCandidates",processName=cms.InputTag.skipCurrentProcess()),
                                    correctEles = cms.bool(True), 
                                    correctPhos = cms.bool(True), 
                                    useSCEnergy = cms.bool(False), 
                                    scEnergyEtThres = cms.double(300)
                                    
                                    )
egCorrectedPackedCandsSeq = cms.Sequence(slimmedElectrons * slimmedPhotons * packedPFCandidates)
