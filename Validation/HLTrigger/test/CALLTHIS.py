import FWCore.ParameterSet.Config as cms

# setting up the process object
process = cms.Process("TEST")

process.load("DQMServices.Core.DQM_cfg") # dqm service
process.load("CondCore.CondDB.CondDB_cfi") # dqm service
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("DQMServices.Components.DQMEnvironment_cfi")

# set max events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

# Configure the object that reads the input file
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_11_CANDIDATE/RelValTTbarLepton_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_preVFP_v6_hlt16-v1/10000/07EAA5B0-3C0F-B749-AFC7-2243156D7946.root")
)

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))

process.load("DQMServices.Components.DQMStoreStats_cfi")

process.load("Validation.HLTrigger.HLTGenLevelValSource_cfi")
process.load("Validation.HLTrigger.HLTGenLevelValClient_cfi")

process.content = cms.EDAnalyzer("EventContentAnalyzer")

process.allPath = cms.Path(
    process.OLD_HLTEleGenLevelValSource *
    process.content *
    process.HLTGenLevelValClient *
    process.dqmStoreStats
)

process.p = cms.EndPath(process.dqmSaver)
process.dqmSaver.convention = 'Offline'
process.dqmSaver.saveByRun = -1
process.dqmSaver.saveAtJobEnd = True
workflowName = '/Global/CMSSW_12_0_0/TEST'
process.dqmSaver.workflow = workflowName
