import FWCore.ParameterSet.Config as cms

from DQM.HLTEvF.HLTObjectsMonitor_cfi import *
from Validation.HLTrigger.DEFINITIONSTUFF_cfi import *

# monitoring of efficiencies of HLT paths and filters
from DQMOffline.Trigger.hltFiltersDQMonitor_cfi import *
hltFiltersDQM = hltFiltersDQMonitor.clone(
  folderName = 'HLT/Filters',
  efficPlotNamePrefix = 'effic_',
  triggerResults = 'TriggerResults::HLT',
  triggerSummaryAOD = 'hltTriggerSummaryAOD::HLT',
  triggerSummaryRAW = 'hltTriggerSummaryRAW::HLT',
)

processName = "TEST"
process = cms.Process(processName)

# Configure the object that reads the input file
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("test.root")
)

offlineHLTSourceOnAOD = cms.Sequence(
      dqmEnvHLT
    * hltFiltersDQM
    * HLTGenLevelValSource
)

process.mypath = cms.Path(offlineHLTSourceOnAOD)

process.outpath = cms.EndPath(process.out)
