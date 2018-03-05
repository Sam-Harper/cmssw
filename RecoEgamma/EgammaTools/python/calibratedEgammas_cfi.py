import FWCore.ParameterSet.Config as cms

calibratedEgammaSettings = cms.PSet(minEtToCalibrate = cms.double(5.0),
                                    semiDeterministic = cms.bool(True),
                                    correctionFile = cms.string("EgammaAnalysis/ElectronTools/data/ScalesSmearings/Run2017_17Nov2017_v1_ele_unc"),
                                    recHitCollectionEB = cms.InputTag('reducedEcalRecHitsEB'),
                                    recHitCollectionEE = cms.InputTag('reducedEcalRecHitsEE'),
                                    produceCalibratedObjs = cms.bool(True)
                                    )

calibratedElectrons = cms.EDProducer("CalibratedElectronProducer",
                                     calibratedEgammaSettings,
                                     # input collections
                                     src = cms.InputTag('gedGsfElectrons'),
                                     gbrForestName = cms.vstring('electron_eb_ECALTRK_lowpt', 'electron_eb_ECALTRK',
                                                                 'electron_ee_ECALTRK_lowpt', 'electron_ee_ECALTRK',
                                                                 'electron_eb_ECALTRK_lowpt_var', 'electron_eb_ECALTRK_var',
                                                                 'electron_ee_ECALTRK_lowpt_var', 'electron_ee_ECALTRK_var'),     
                                     )

calibratedPatElectrons = cms.EDProducer("CalibratedPatElectronProducer",
                                        calibratedEgammaSettings,
                                        # input collections
                                        src = cms.InputTag('slimmedElectrons'),
                                        gbrForestName = cms.vstring('electron_eb_ECALTRK_lowpt', 'electron_eb_ECALTRK',
                                                                    'electron_ee_ECALTRK_lowpt', 'electron_ee_ECALTRK',
                                                                    'electron_eb_ECALTRK_lowpt_var', 'electron_eb_ECALTRK_var',
                                                                    'electron_ee_ECALTRK_lowpt_var', 'electron_ee_ECALTRK_var'),     
                                       )

calibratedPhotons = cms.EDProducer("CalibratedPhotonProducer",
                                   calibratedEgammaSettings,
                                   # input collections
                                   src = cms.InputTag('gedPhotons'),    
                                  )
calibratedPatPhotons = cms.EDProducer("CalibratedPatPhotonProducer",
                                      calibratedEgammaSettings,
                                      # input collections
                                      src = cms.InputTag('slimmedPhotons'),
                                      )
