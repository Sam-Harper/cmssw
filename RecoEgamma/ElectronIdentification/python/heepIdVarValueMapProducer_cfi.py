import FWCore.ParameterSet.Config as cms

heepIDVarValueMaps = cms.EDProducer("ElectronHEEPIDValueMapProducer",
                                    ebRecHits=cms.InputTag("reducedEcalRecHitsEB"),
                                    eeRecHits=cms.InputTag("reducedEcalRecHitsEB"),
                                    beamSpot=cms.InputTag("offlineBeamSpot"),
                                    cands=cms.VInputTag("packedCandidates",
                                                        "lostTracks",
                                                        "packedCandsForTkIso"),
                                    eles=cms.InputTag("gedGsfElectrons"),
                                    trkIsoConfig= cms.PSet(
                                       barrelCuts=cms.PSet(
                                          minPt=cms.double(1.0),
                                          maxDR=cms.double(0.3),
                                          minDR=cms.double(0.0),
                                          minDEta=cms.double(0.005),
                                          maxDZ=cms.double(0.1),
                                          maxDPtPt=cms.double(-1),
                                          minHits=cms.int32(8),
                                          minPixelHits=cms.int32(1),
                                          allowedQualities=cms.vstring(),
                                          algosToReject=cms.vstring()
                                          ),
                                       endcapCuts=cms.PSet(
                                          minPt=cms.double(1.0),
                                          maxDR=cms.double(0.3),
                                          minDR=cms.double(0.0),
                                          minDEta=cms.double(0.005),
                                          maxDZ=cms.double(0.5),
                                          maxDPtPt=cms.double(-1),
                                          minHits=cms.int32(8),
                                          minPixelHits=cms.int32(1),
                                          allowedQualities=cms.vstring(),
                                          algosToReject=cms.vstring()
                                          )
                                       )
                                    )
