import FWCore.ParameterSet.Config as cms

process = cms.Process("DQM")

# DQM service
process.load("DQMServices.Core.DQMStore_cfi")

# MessageLogger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000


# Source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    'file:/opt/ppd/month/harper/dataFiles/EGamma_Run2018B_RAW_317182_LS52_1884CA4B-3664-E811-A7EC-FA163EB2E120.root'
  )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load("DQM.HLTEvF.trigObjTnPSource_cfi")

process.load('DQMServices.Components.DQMFileSaver_cfi')
process.dqmSaver.workflow = "/HLT/TrigObjTnpSource/All"

process.endp = cms.EndPath( process.trigObjTnPSource + process.dqmSaver )
