
import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_2025_cff import Run3_2025

process = cms.Process('HLTSCRecHits',Run3_2025)


from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.parseArguments()

process.MessageLogger = cms.Service( "MessageLogger",
    suppressWarning = cms.untracked.vstring( 'hltL3MuonsIOHit',
      'hltL3MuonsOIHit',
      'hltL3MuonsOIState',
      'hltLightPFTracks',
      'hltPixelTracks',
      'hltPixelTracksForHighMult',
      'hltSiPixelClusters',
      'hltSiPixelDigis' ),
    suppressFwkInfo = cms.untracked.vstring(  ),
    suppressInfo = cms.untracked.vstring(  ),
    suppressDebug = cms.untracked.vstring(  ),
    debugModules = cms.untracked.vstring(  ),
    cerr = cms.untracked.PSet( 
      INFO = cms.untracked.PSet(  limit = cms.untracked.int32( 0 ) ),
      noTimeStamps = cms.untracked.bool( False ),
      FwkReport = cms.untracked.PSet( 
        reportEvery = cms.untracked.int32( 1 ),
        limit = cms.untracked.int32( 0 )
      ),
      default = cms.untracked.PSet(  limit = cms.untracked.int32( 10000000 ) ),
      Root_NoDictionary = cms.untracked.PSet(  limit = cms.untracked.int32( 0 ) ),
      FwkJob = cms.untracked.PSet(  limit = cms.untracked.int32( 0 ) ),
      FwkSummary = cms.untracked.PSet( 
        reportEvery = cms.untracked.int32( 1 ),
        limit = cms.untracked.int32( 10000000 )
      ),
      threshold = cms.untracked.string( "INFO" ),
      enableStatistics = cms.untracked.bool( False )
    ),
    suppressError = cms.untracked.vstring( 'hltL3TkTracksFromL2IOHit',
      'hltL3TkTracksFromL2OIHit',
      'hltL3TkTracksFromL2OIState' )
)
process.hltOutputScoutingPF = cms.OutputModule( "PoolOutputModule",
    fileName = cms.untracked.string( "outputScoutingPF.root" ),
    compressionAlgorithm = cms.untracked.string( "ZSTD" ),
    compressionLevel = cms.untracked.int32( 3 ),
    fastCloning = cms.untracked.bool( False ),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string( "" ),
        dataTier = cms.untracked.string( "RAW" )
    ),
    #SelectEvents = cms.untracked.PSet(  SelectEvents = cms.vstring( '' ) ),
    outputCommands = cms.untracked.vstring( 'drop *',
      'keep *_hltFEDSelectorL1_*_*',
      'keep *_hltScoutingEgammaPacker_*_*',
      'keep *_hltScoutingMuonPackerNoVtx_*_*',
      'keep *_hltScoutingMuonPackerVtx_*_*',
      'keep *_hltScoutingPFPacker_*_*',
      'keep *_hltScoutingPrimaryVertexPacker_*_*',
      'keep *_hltScoutingTrackPacker_*_*',
      'keep edmTriggerResults_*_*_*' )
)


process.hltOutputScoutingPF.compressionAlgorithm = cms.untracked.string("LZMA")
process.hltOutputScoutingPF.compressionLevel = cms.untracked.int32(4)

process.hltOutputScoutingPF.fileName = options.outputFile

process.hltScoutingRecHitPacker = cms.EDProducer("HLTScoutingRecHitProducer",
  pfRecHitsECAL = cms.InputTag('hltParticleFlowRecHitECALUnseeded'),
 # pfRecHitsECALCleaned = cms.InputTag('hltParticleFlowRecHitECALUnseeded','Cleaned'),
  pfRecHitsECALCleaned = cms.InputTag(""),
  minEnergyEB = cms.double(-1),
  minEnergyEE = cms.double(-1),
  pfRecHitsHBHE = cms.InputTag('hltParticleFlowRecHitHBHE'),
  minEnergyHBHE = cms.double(1),
  mantissaPrecision = cms.int32(10),
)
process.hltParticleFlowRecHitECALUnseeded = cms.EDProducer( "PFRecHitProducer",
    navigator = cms.PSet( 
      barrel = cms.PSet(  ),
      endcap = cms.PSet(  ),
      name = cms.string( "PFRecHitECALNavigator" )
    ),
    producers = cms.VPSet( 
      cms.PSet(  src = cms.InputTag( 'hltEcalRecHit','EcalRecHitsEB' ),
        srFlags = cms.InputTag( "" ),
        name = cms.string( "PFEBRecHitCreator" ),
        qualityTests = cms.VPSet( 
          cms.PSet(  name = cms.string( "PFRecHitQTestDBThreshold" ),
            applySelectionsToAllCrystals = cms.bool( True )
          ),
          cms.PSet(  topologicalCleaning = cms.bool( True ),
            skipTTRecoveredHits = cms.bool( True ),
            cleaningThreshold = cms.double( 2.0 ),
            name = cms.string( "PFRecHitQTestECAL" ),
            timingCleaning = cms.bool( True )
          )
        )
      ),
      cms.PSet(  src = cms.InputTag( 'hltEcalRecHit','EcalRecHitsEE' ),
        srFlags = cms.InputTag( "" ),
        name = cms.string( "PFEERecHitCreator" ),
        qualityTests = cms.VPSet( 
          cms.PSet(  name = cms.string( "PFRecHitQTestDBThreshold" ),
            applySelectionsToAllCrystals = cms.bool( True )
          ),
          cms.PSet(  topologicalCleaning = cms.bool( True ),
            skipTTRecoveredHits = cms.bool( True ),
            cleaningThreshold = cms.double( 2.0 ),
            name = cms.string( "PFRecHitQTestECAL" ),
            timingCleaning = cms.bool( True )
          )
        )
      )
    )
)


process.hltOutputScoutingPF.outputCommands += [
    'keep *_hltScoutingRecHitPacker_EB_*',
    'keep *_hltScoutingRecHitPacker_EE_*',
    'keep *_hltScoutingRecHitPacker_HBHE_*',
    'keep *_hltScoutingRecHitPacker_*_*',
    'drop *_hltScoutingRecHitPacker_*_HLTX',
]
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        options.inputFiles
    ),
)

process.p = cms.Path(process.hltParticleFlowRecHitECALUnseeded+
    process.hltScoutingRecHitPacker
)
process.out = cms.EndPath(
    process.hltOutputScoutingPF
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.GlobalTag = cms.ESSource( "PoolDBESSource",
    DBParameters = cms.PSet( 
      messageLevel = cms.untracked.int32( 0 ),
      authenticationPath = cms.untracked.string( "." )
    ),
    connect = cms.string( "frontier://FrontierProd/CMS_CONDITIONS" ),
    globaltag = cms.string( "None" ),
    snapshotTime = cms.string( "" ),
    frontierKey = cms.untracked.string( "" ),
    toGet = cms.VPSet( 
      cms.PSet(  refreshTime = cms.uint64( 2 ),
        record = cms.string( "BeamSpotOnlineLegacyObjectsRcd" )
      ),
      cms.PSet(  refreshTime = cms.uint64( 2 ),
        record = cms.string( "BeamSpotOnlineHLTObjectsRcd" )
      ),
      cms.PSet(  refreshTime = cms.uint64( 40 ),
        record = cms.string( "LHCInfoPerLSRcd" )
      ),
      cms.PSet(  refreshTime = cms.uint64( 40 ),
        record = cms.string( "LHCInfoPerFillRcd" )
      )
    ),
    JsonDumpFileName = cms.untracked.string( "" ),
    DumpStat = cms.untracked.bool( False ),
    ReconnectEachRun = cms.untracked.bool( True ),
    RefreshAlways = cms.untracked.bool( False ),
    RefreshEachRun = cms.untracked.bool( True ),
    RefreshOpenIOVs = cms.untracked.bool( False ),
    pfnPostfix = cms.untracked.string( "" ),
    pfnPrefix = cms.untracked.string( "" ),
    recordsToDebug = cms.untracked.vstring(  ),
    appendToDataLabel = cms.string( "" )
)
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
if 'GlobalTag' in process.__dict__:
    from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = '150X_dataRun3_HLT_v1')