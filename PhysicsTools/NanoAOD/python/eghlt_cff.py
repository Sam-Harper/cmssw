import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.nano_eras_cff import *
from PhysicsTools.NanoAOD.common_cff import *



egHLTLowPtTable = cms.EDProducer("SimpleTriggerEgammaObjectFlatTableProducer",
    src = cms.InputTag("hltEgammaHLTExtra","LowPt"),
    cut = cms.string(""), 
    name= cms.string("EgHLTLowPt"),
    doc = cms.string(""),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the electrons
    variables = cms.PSet(P4Vars,
                         sigmaIEtaIEta5x5NoiseCleaned = Var("var('hltEgammaClusterShape_sigmaIEtaIEta5x5NoiseCleaned')",float,doc="noise cleaned sigmaIEtaIEta full 5x5"),
                         hForHOverE = Var("var('hltEgammaHoverE')",float,doc="H for H/E")
                     )
)

egHLTLowPtUSTable = cms.EDProducer("SimpleTriggerEgammaObjectFlatTableProducer",
    src = cms.InputTag("hltEgammaHLTExtra","LowPtUnseeded"),
    cut = cms.string(""), 
    name= cms.string("EgHLTLowPtUS"),
    doc = cms.string(""),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the electrons
    variables = cms.PSet(P4Vars,
                         sigmaIEtaIEta5x5NoiseCleaned = Var("var('hltEgammaClusterShapeUnseeded_sigmaIEtaIEta5x5NoiseCleaned')",float,doc="noise cleaned sigmaIEtaIEta full 5x5"),
                         hForHOverE = Var("var('hltEgammaHoverEUnseeded')",float,doc="H for H/E")
                     )
)

egHLTTable = cms.EDProducer("SimpleTriggerEgammaObjectFlatTableProducer",
    src = cms.InputTag("hltEgammaHLTExtra",""),
    cut = cms.string(""), 
    name= cms.string("EgHLT"),
    doc = cms.string(""),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the electrons
    variables = cms.PSet(P4Vars,
                         sigmaIEtaIEta5x5NoiseCleaned = Var("var('hltEgammaClusterShape_sigmaIEtaIEta5x5NoiseCleaned')",float,doc="noise cleaned sigmaIEtaIEta full 5x5"),
                         hForHOverE = Var("var('hltEgammaHoverE')",float,doc="H for H/E")
                     )
)
egHLTUSTable = cms.EDProducer("SimpleTriggerEgammaObjectFlatTableProducer",
    src = cms.InputTag("hltEgammaHLTExtra","Unseeded"),
    cut = cms.string(""), 
    name= cms.string("EgHLTUS"),
    doc = cms.string(""),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the electrons
    variables = cms.PSet(P4Vars,
                         sigmaIEtaIEta5x5NoiseCleaned = Var("var('hltEgammaClusterShapeUnseeded_sigmaIEtaIEta5x5NoiseCleaned')",float,doc="noise cleaned sigmaIEtaIEta full 5x5"),
                         hForHOverE = Var("var('hltEgammaHoverEUnseeded')",float,doc="H for H/E")
                     )
)

egHLTTablesTask = cms.Task(egHLTTable,egHLTUSTable,egHLTLowPtTable,egHLTLowPtUSTable)
