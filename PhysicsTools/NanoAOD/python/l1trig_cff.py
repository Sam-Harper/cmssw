import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.nano_eras_cff import *
from PhysicsTools.NanoAOD.common_cff import *

l1ObjVars = cms.PSet(
    P4Vars, 
    hwPt = Var("hwPt()",int,doc="hardware pt"),
    hwEta = Var("hwEta()",int,doc="hardware eta"),
    hwPhi = Var("hwPhi()",int,doc="hardware phi"),
    hwQual = Var("hwQual()",int,doc="hardware qual"),
    hwIso = Var("hwIso()",int,doc="hardware iso")
)



l1EGTable = cms.EDProducer("SimpleTriggerL1EGFlatTableProducer",
    src = cms.InputTag("caloStage2Digis","EGamma"),
    minBX = cms.int32(0),
    maxBX = cms.int32(0),                           
    cut = cms.string(""), 
    name= cms.string("L1EG"),
    doc = cms.string(""),
    extension = cms.bool(False), # this is the main table for L1 EGs
    variables = cms.PSet(l1ObjVars,
                         towerIEta = Var("towerIEta()",int,doc="tower ieta"),
                         towerIPhi = Var("towerIPhi()",int,doc="tower iphi"),
                         rawEt = Var("rawEt()",int,doc="raw et"),
                         isoEt = Var("isoEt()",int,doc="iso et"),
                         footprintEt = Var("footprintEt()",int,doc=" footprint et"),
                         nTT = Var("nTT()",int,doc="nr trig towers"),
                         shape = Var("shape()",int,doc="shape"),
                         towerHoE = Var("towerHoE()",int,doc="tower H/E"),
                     )
)

l1TablesTask = cms.Task(l1EGTable)
