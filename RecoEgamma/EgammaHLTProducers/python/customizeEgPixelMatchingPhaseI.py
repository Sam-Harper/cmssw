
import FWCore.ParameterSet.Config as cms

def customizeEgPixelMatchingPhaseI(process):
    

    process.hltEgammaSuperClustersToPixelMatch = cms.EDProducer("EgammaHLTFilteredSuperClusterProducer",
                                                            cands = cms.InputTag("hltEgammaCandidates"),
                                                                cuts = cms.VPSet(
            cms.PSet(
                var = cms.InputTag("hltEgammaHoverE"),
                barrelCut = cms.PSet(
                    cutOverE = cms.double(0.2),
                    useEt = cms.bool(False)
                    ),
                endcapCut = cms.PSet(
                    cutOverE = cms.double(0.2),
                    useEt = cms.bool(False)
                    )
                )
            )
                                                                )
    
    process.hltEleSeedsTrackingRegions = cms.EDProducer("TrackingRegionsFromSuperClustersEDProducer",
                                                        RegionPSet = cms.PSet( 
            ptMin=cms.double(1.5),
            originRadius=cms.double(0.2,),
            originHalfLength=cms.double(15.0),
            deltaPhiRegion=cms.double(0.4),
            deltaEtaRegion=cms.double(0.1),
            useZInVertex=cms.bool(False),
            whereToUseMeasTracker = cms.string("kNever"),
            beamSpot = cms.InputTag("hltOnlineBeamSpot" ),
            vertices = cms.InputTag(""),
            measurementTrackerEvent = cms.InputTag(""),
            superClusters=cms.VInputTag('hltEgammaSuperClustersToPixelMatch'),
            precise = cms.bool( True )
            )
                                                        )
    
    
    process.hltEgammaElectronPixelSeeds = cms.EDProducer("ElectronNSeedProducer",
                                                         initialSeeds = cms.InputTag("hltElePixelSeedsCombined"),
                                                         vertices = cms.InputTag(""),
                                                         navSchool = cms.string("SimpleNavigationSchool"),
                                                         detLayerGeom = cms.string("hltESPGlobalDetLayerGeometry"),
                                                         beamSpot = cms.InputTag("hltOnlineBeamSpot"),
                                                         measTkEvt = cms.InputTag( "hltSiStripClusters" ),
                                                         superClusters=cms.VInputTag('hltEgammaSuperClustersToPixelMatch'),                                                    
                                                         useRecoVertex=cms.bool(False),
                                                         matchingCuts=cms.VPSet( 
            matchingCuts=cms.VPSet(  
                cms.PSet(dPhiMax=cms.double(0.03),
                         dRZMax=cms.double(-1),
                         dRZMaxLowEtThres=cms.double(-1),
                         dRZMaxLowEtEtaBins=cms.vdouble(),
                         dRZMaxLowEt=cms.vdouble(-1),
                         ),
                cms.PSet(dPhiMax=cms.double(0.003),
                         dRZMax=cms.double(0.03),
                         dRZMaxLowEtThres=cms.double(-1),
                         dRZMaxLowEtEtaBins=cms.vdouble(),
                         dRZMaxLowEt=cms.vdouble(0.2),
                         ),
                cms.PSet(dPhiMax=cms.double(0.003),
                         dRZMax=cms.double(0.05),
                         dRZMaxLowEtThres=cms.double(-1),
                         dRZMaxLowEtEtaBins=cms.vdouble(),
                         dRZMaxLowEt=cms.vdouble(0.2),
                         ),
                )
            )
                                                         )

    process.hltEgammaSuperClustersToPixelMatchUnseeded  = process.hltEgammaSuperClustersToPixelMatch.clone()
    process.hltEgammaSuperClustersToPixelMatchUnseeded.cands=cms.InputTag("hltEgammaCandidatesUnseeded")
    process.hltEgammaSuperClustersToPixelMatchUnseeded.cuts[0].var=cms.InputTag("hltEgammaHoverEUnseeded")

    process.hltEleSeedsTrackingRegionsUnseeded=process.hltEleSeedsTrackingRegions.clone()
    process.hltEleSeedsTrackingRegionsUnseeded.RegionPSet.superClusters=cms.VInputTag('hltEgammaSuperClustersToPixelMatchUnseeded')

    process.hltEgammaElectronPixelSeedsUnseeded = process.hltEgammaElectronPixelSeeds.clone(initialSeeds = cms.InputTag("hltElePixelSeedsCombinedUnseeded"),superClusters=cms.VInputTag('hltEgammaSuperClustersToPixelMatchUnseeded'))

    return process
