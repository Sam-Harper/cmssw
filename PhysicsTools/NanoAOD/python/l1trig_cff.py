import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.nano_eras_cff import *
from PhysicsTools.NanoAOD.common_cff import *

l1PtVars = cms.PSet(
    pt  = Var("pt",  float, precision=10),
    phi = Var("phi", float, precision=10),
)
l1P3Vars = cms.PSet(
    l1PtVars,
    eta = Var("eta", float, precision=10),
)

l1ObjVars = cms.PSet(
    l1P3Vars,
    hwPt = Var("hwPt()",int,doc="hardware pt"),
    hwEta = Var("hwEta()",int,doc="hardware eta"),
    hwPhi = Var("hwPhi()",int,doc="hardware phi"),
    hwQual = Var("hwQual()",int,doc="hardware qual"),
    hwIso = Var("hwIso()",int,doc="hardware iso")
)

l1JetReducedVars = cms.PSet(
    l1P3Vars
)

l1EtSumReducedVars = cms.PSet(
    l1PtVars,
    etSumType = Var("getType()",int,doc="et sum type"),
)
l1EGReducedVars = cms.PSet(
    l1P3Vars,
    hwIso = Var("hwIso()",int,doc="hardware iso")
)

l1TauReducedVars = cms.PSet(
    l1P3Vars,
    hwIso = Var("hwIso()",int,doc="hardware iso")
)

l1MuonReducedVars = cms.PSet(
    l1P3Vars,
    hwQual = Var("hwQual()",int,doc="hardware qual"),
    hwCharge = Var("hwCharge()",int,doc=""), 
    etaAtVtx = Var("etaAtVtx()",float,doc=""),
    phiAtVtx = Var("phiAtVtx()",float,doc=""),
    ptUnconstrained = Var("ptUnconstrained()",float,doc=""),
    hwDXY = Var("hwDXY()",int,doc=""),
)

l1MuTable = cms.EDProducer("SimpleTriggerL1MuonFlatTableProducer",
    src = cms.InputTag("gmtStage2Digis","Muon"),
    minBX = cms.int32(-2),
    maxBX = cms.int32(2),                           
    cut = cms.string(""), 
    name= cms.string("L1Mu"),
    doc = cms.string(""),
    extension = cms.bool(False), 
    variables = cms.PSet(l1ObjVars,
                         hwCharge = Var("hwCharge()",int,doc=""),
                         hwChargeValid = Var("hwChargeValid()",int,doc=""),
                         tfMuonIndex = Var("tfMuonIndex()",int,doc=""),
                         hwTag = Var("hwTag()",int,doc=""),
                         hwEtaAtVtx = Var("hwEtaAtVtx()",int,doc=""),
                         hwPhiAtVtx = Var("hwPhiAtVtx()",int,doc=""),
                         etaAtVtx = Var("etaAtVtx()",float,doc=""),
                         phiAtVtx = Var("phiAtVtx()",float,doc=""),
                         hwIsoSum = Var("hwIsoSum()",int,doc=""),
                         hwDPhiExtra = Var("hwDPhiExtra()",int,doc=""),
                         hwDEtaExtra = Var("hwDEtaExtra()",int,doc=""),
                         hwRank = Var("hwRank()",int,doc=""),
                         hwPtUnconstrained = Var("hwPtUnconstrained()",int,doc=""),
                         ptUnconstrained = Var("ptUnconstrained()",float,doc=""),
                         hwDXY = Var("hwDXY()",int,doc=""),
                     )
)


l1JetTable = cms.EDProducer("SimpleTriggerL1JetFlatTableProducer",
    src = cms.InputTag("caloStage2Digis","Jet"),
    minBX = cms.int32(-2),
    maxBX = cms.int32(2),                           
    cut = cms.string(""), 
    name= cms.string("L1Jet"),
    doc = cms.string(""),
    extension = cms.bool(False),
    variables = cms.PSet(l1ObjVars,
                         towerIEta = Var("towerIEta()",int,doc=""),
                         towerIPhi = Var("towerIPhi()",int,doc=""),
                         rawEt = Var("rawEt()",int,doc=""),
                         seedEt = Var("seedEt()",int,doc=""),
                         puEt = Var("puEt()",int,doc=""),
                         puDonutEt0 = Var("puDonutEt(0)",int,doc=""),
                         puDonutEt1 = Var("puDonutEt(1)",int,doc=""),
                         puDonutEt2 = Var("puDonutEt(2)",int,doc=""),
                         puDonutEt3 = Var("puDonutEt(3)",int,doc=""),
                     )
)

l1TauTable = cms.EDProducer("SimpleTriggerL1TauFlatTableProducer",
    src = cms.InputTag("caloStage2Digis","Tau"),
    minBX = cms.int32(-2),
    maxBX = cms.int32(2),                           
    cut = cms.string(""), 
    name= cms.string("L1Tau"),
    doc = cms.string(""),
    extension = cms.bool(False), # this is the main table for L1 EGs
    variables = cms.PSet(l1ObjVars,
                         towerIEta = Var("towerIEta()",int,doc=""),
                         towerIPhi = Var("towerIPhi()",int,doc=""),
                         rawEt = Var("rawEt()",int,doc=""),
                         isoEt = Var("isoEt()",int,doc=""),
                         nTT = Var("nTT()",int,doc=""),                         
                         hasEM = Var("hasEM()",int,doc=""),
                         isMerged = Var("isMerged()",int,doc=""),

                     )
)

l1EtSumTable = cms.EDProducer("SimpleTriggerL1EtSumFlatTableProducer",
    src = cms.InputTag("caloStage2Digis","EtSum"),
    minBX = cms.int32(-2),
    maxBX = cms.int32(2),                           
    cut = cms.string(""), 
    name= cms.string("L1EtSum"),
    doc = cms.string(""),
    extension = cms.bool(False), 
    variables = cms.PSet(l1PtVars,
                         hwPt = Var("hwPt()",int,doc="hardware pt"),
                         hwPhi = Var("hwPhi()",int,doc="hardware phi"),
                         etSumType = Var("getType()",int,doc=""),
                     )
)

l1EGTable = cms.EDProducer("SimpleTriggerL1EGFlatTableProducer",
    src = cms.InputTag("caloStage2Digis","EGamma"),
    minBX = cms.int32(-2),
    maxBX = cms.int32(2),                           
    cut = cms.string(""), 
    name= cms.string("L1EG"),
    doc = cms.string(""),
    extension = cms.bool(False), 
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

l1TablesTask = cms.Task(l1EGTable,l1EtSumTable,l1TauTable,l1JetTable,l1MuTable)

def setL1NanoToReduced(process):
    """
    sets the L1 objects only have reduced information which is necessary 
    for central nano
    """
    #reduce the variables to the core variables
    #note et sum variables are already reduced
    process.l1EGTable.variables = cms.PSet(l1EGReducedVars)
    process.l1MuTable.variables = cms.PSet(l1MuonReducedVars)
    process.l1JetTable.variables = cms.PSet(l1JetReducedVars)
    process.l1TauTable.variables = cms.PSet(l1TauReducedVars)
    process.l1EtSumTable.variables = cms.PSet(l1EtSumReducedVars)
   
    #restrict bx
    process.l1EGTable.minBX = 0
    process.l1EGTable.maxBX = 0
    process.l1MuTable.minBX = 0
    process.l1MuTable.maxBX = 0
    process.l1JetTable.minBX = 0 
    process.l1JetTable.maxBX = 0
    process.l1TauTable.minBX = 0 
    process.l1TauTable.maxBX = 0
    process.l1EtSumTable.minBX = 0 
    process.l1EtSumTable.maxBX = 0
    
    #apply cuts
    process.l1EGTable.cut="pt>=10"
    process.l1TauTable.cut="pt>=24"
    process.l1JetTable.cut="pt>=30"
    process.l1MuTable.cut="pt>=3 && hwQual>=8"
    process.l1EtSumTable.cut="(getType==8 || getType==1 || getType==2 || getType==3)"
    
    return process
