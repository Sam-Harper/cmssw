import FWCore.ParameterSet.Config as cms

def setL1SeedFilterToMini(mod):
    mod.L1GlobalInputTag = cms.InputTag( "gtStage2Digis" )
    mod.L1MuonInputTag = cms.InputTag( 'gmtStage2Digis','Muon' )
    mod.L1MuonShowerInputTag = cms.InputTag( '' )
    mod.L1EGammaInputTag = cms.InputTag( 'caloStage2Digis','EGamma' )
    mod.L1JetInputTag = cms.InputTag( 'caloStage2Digis','Jet' )
    mod.L1TauInputTag = cms.InputTag( 'caloStage2Digis','Tau' )
    mod.L1EtSumInputTag = cms.InputTag( 'caloStage2Digis','EtSum' )
    mod.L1EtSumZdcInputTag = cms.InputTag( '' )
    return mod

def setL1GTProducerToMini(mod):
    mod.MuonInputTag = cms.InputTag( 'gmtStage2Digis','Muon' )
    mod.EGammaInputTag = cms.InputTag( 'caloStage2Digis','EGamma' )
    mod.TauInputTag = cms.InputTag( 'caloStage2Digis','Tau' )
    mod.JetInputTag = cms.InputTag( 'caloStage2Digis','Jet' )
    mod.EtSumInputTag = cms.InputTag( 'caloStage2Digis','EtSum' )
    mod.EtSumZdcInputTag = cms.InputTag( '' )
    mod.ExtInputTag = cms.InputTag( "gtStage2Digis" )
    mod.AlgoBlkInputTag = cms.InputTag( "gtStage2Digis" )
    return mod

def setL1EGMatcherToMini(mod):
    mod.l1IsolatedTag = cms.InputTag( 'caloStage2Digis','EGamma' )
    mod.l1NonIsolatedTag = cms.InputTag( 'caloStage2Digis','EGamma' )    
    mod.l1CenJetsTag = cms.InputTag( 'caloStage2Digis','Jet')
    mod.l1TausTag = cms.InputTag( 'caloStage2Digis','Tau' )