import FWCore.ParameterSet.Config as cms

hltEgammaHLTExtra = cms.EDProducer("EgammaHLTExtraProducer",
                                   egCands = cms.VPSet(
                                       cms.PSet(
                                           pixelSeeds = cms.InputTag("hltEgammaElectronPixelSeeds"),
                                           ecalCands = cms.InputTag("hltEgammaCandidates"),
                                           gsfTracks = cms.InputTag("hltEgammaGsfTracks"),
                                           label = cms.string('')
                                       ),
                                       cms.PSet(
                                           pixelSeeds = cms.InputTag("hltEgammaElectronPixelSeedsUnseeded"),
                                           ecalCands = cms.InputTag("hltEgammaCandidatesUnseeded"),
                                           gsfTracks = cms.InputTag("hltEgammaGsfTracksUnseeded"),
                                           label = cms.string('Unseeded')
                                       ),
                                   ),                 
                                   ecal = cms.VPSet(
                                       cms.PSet(
                                           src= cms.InputTag("hltEcalRecHit","EcalRecHitEB"),
                                           label = cms.string("EcalRecHitsEB")
                                       ),
                                       cms.PSet(
                                           src= cms.InputTag("hltEcalRecHit","EcalRecHitEE"),
                                           label = cms.string("EcalRecHitsEE")
                                       )
                                   ),
                                   hcal = cms.VPSet(cms.PSet(src=cms.InputTag("hltHbhereco"),label=cms.string(""))),
                                   trks = cms.VPSet(cms.PSet(src=cms.InputTag("hltMergedTracks"),label=cms.string(""))),
                                   pixelSeeds = cms.InputTag("hltEgammaElectronPixelSeeds"),
                                   gsfTracks = cms.InputTag("hltEgammaGsfTracks"),
                                   minPtToSaveHits = cms.double(8.)
)

