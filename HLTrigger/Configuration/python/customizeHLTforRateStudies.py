
import FWCore.ParameterSet.Config as cms

def customiseHLTforRateStudies(process):
    delattr(process,"DQMOutput")
    delattr(process,"dqmOutput")
    delattr(process,"DQMStore")
    
    process.hltOutputMinimal.outputCommands = cms.untracked.vstring( 
        'drop *',
        'keep edmTriggerResults_*_*_*',
        'keep *_hltGtStage2Digis_*_*',
    )
    return process

