import FWCore.ParameterSet.Config as cms

def customiseForECALLaser(process):
    process.hltEcalRecHitUL.ecalLaserDbLabel = cms.string("UL")
    process.hltEcalRecHitRun2.ecalLaserDbLabel = cms.string("Run2")
    process.hltEcalRecHitRun3.ecalLaserDbLabel = cms.string("Run3")
    process.EcalLaserCorrectionServiceUL = cms.ESProducer( "EcalLaserCorrectionService",
                                                           appendToDataLabel = cms.string( "" ),
                                                           maxExtrapolationTimeInSec = cms.uint32( 0 )
)
    process.EcalLaserCorrectionServiceRun2 = cms.ESProducer( "EcalLaserCorrectionService",
                                                             appendToDataLabel = cms.string( "" ),
                                                             maxExtrapolationTimeInSec = cms.uint32( 0 )
)
    process.EcalLaserCorrectionServiceRun3 = cms.ESProducer( "EcalLaserCorrectionService",
                                                             appendToDataLabel = cms.string( "" ),
                                                             maxExtrapolationTimeInSec = cms.uint32( 0 )
                                                         )

    process.EcalLaserCorrectionServiceRun2.serviceLabel = cms.string("Run2")
    process.EcalLaserCorrectionServiceRun2.apdpnLabel = cms.string("Run2")
    process.EcalLaserCorrectionServiceRun3.serviceLabel = cms.string("Run3")
    process.EcalLaserCorrectionServiceRun3.apdpnLabel = cms.string("Run3")
    process.EcalLaserCorrectionServiceUL.serviceLabel = cms.string("UL")
    process.EcalLaserCorrectionServiceUL.apdpnLabel = cms.string("UL")

    process.GlobalTag.toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                 label = cms.untracked.string("UL"),
                 tag = cms.string("EcalLaserAPDPNRatios_rereco2018_v3")
             ),
        cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                 label = cms.untracked.string("Run2"),
                 tag = cms.string("EcalLaserAPDPNRatios_weekly_hlt_UL2018")
             ),
        cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                 label = cms.untracked.string("Run3"),
                 tag = cms.string("EcalLaserAPDPNRatios_UL2018FillShift")
             ),
    )
    
    if hasattr(process,"hltOutputMinimal"):
        process.hltOutputMinimal.outputCommands.extend([
            "keep recoSuperClusters_*_*_*",
            "keep recoCaloClusters_*_*_*",
            "keep recoRecoEcalCandidates_*_*_*"
        ])
        
    return process
