import FWCore.ParameterSet.Config as cms

process = cms.Process("DQM")

# Load the standard sequences
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')

# Configure the source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
       "file:/home/ppd/mjf21517/CMSSW_mercury/CMSSW_13010_EXODQM_EL8/src/jetMET0_370300_30_AOD.root",
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Message Logger
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# Load the DQM services
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('DQMServices.Core.DQMStoreNonLegacy_cff')
process.load('DQMOffline.Configuration.DQMOffline_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff'

# Configure the module
process.EXOPreFireMonitor = cms.EDProducer("EXOPreFireMonitor",
                                           folderName = cms.string("HLT/EXOPreFire"),
                                           sampleTrigRequirements = cms.PSet(
                                               hltInputTag = cms.InputTag("TriggerResults","","HLT"),
                                               hltPaths = cms.vstring("HLT_*")
       
                                           ),
                                           met       = cms.InputTag("pfMet"), 
                                           jets      = cms.InputTag("ak4PFJets"),
                                           electrons = cms.InputTag("gedGsfElectrons"), 
                                           photons = cms.InputTag("gedPhotons"),
                                           l1Decision  = cms.InputTag("gtStage2Digis")
                                           

)
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = '130X_dataRun3_HLT_v2')


process.dqmSaver.workflow = "/DQM/JetMET/AOD"

# load the DQMStore and DQMRootOutputModule
process.load( "DQMServices.Core.DQMStore_cfi" )

process.dqmOutput = cms.OutputModule("DQMRootOutputModule",
    fileName = cms.untracked.string("DQMIO.root")
)

process.DQMOutput = cms.EndPath(
    process.dqmOutput
)
# Define the path
process.p = cms.Path(
    process.EXOPreFireMonitor
)

# Endpath to save the results
process.ep = cms.EndPath(
    process.dqmSaver
)

# Configure the process schedule
process.schedule = cms.Schedule(process.p, process.ep)
