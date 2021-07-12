
import FWCore.ParameterSet.Config as cms

def customiseHLTforRateStudies(process):
    delattr(process,"DQMOutput")
    delattr(process,"dqmOutput")
    delattr(process,"DQMStore")
    
    process.hltOutputMinimal.outputCommands = cms.untracked.vstring( 
        'drop *',
        'keep edmTriggerResults_*_*_*',
        'keep GlobalAlgBlkBXVector_*_*_*',
    )

    if 'MessageLogger' in process.__dict__:
        process.MessageLogger.suppressWarning.extend(["hltL3MuonsIterL3OIScoutingNoVtx","hltEcalRecHit","hltDoubletRecoveryPFlowCtfWithMaterialTracks","hltL3NoFiltersTkTracksFromL2IOHitNoVtx","hltL3NoFiltersNoVtxMuonsOIHit","hltEgammaGsfTracks","hltL3NoFiltersNoVtxMuonsOIState" ] )
        process.MessageLogger.suppressError.extend(["hltL3MuonsIterL3OIScoutingNoVtx","hltEcalRecHit","hltDoubletRecoveryPFlowCtfWithMaterialTracks","hltL3NoFiltersTkTracksFromL2IOHitNoVtx","hltL3NoFiltersNoVtxMuonsOIHit","hltEgammaGsfTracks","hltL3NoFiltersNoVtxMuonsOIState" ] )
    process.options.wantSummary = False
    return process
