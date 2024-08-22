isCrabJob=False #script seds this if its a crab job

# Import configurations
import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
from Configuration.Eras.Era_Run3_cff import Run3
process = cms.Process("L1Skim",Run3)

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.parseArguments()

print(options.inputFiles)
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),  
                          )

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(5000),
    limit = cms.untracked.int32(10000000)
)

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

#Load geometry
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '140X_dataRun3_Prompt_v4', '')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CaloEventSetup.CaloTowerConstituents_cfi")
process.load("Configuration.StandardSequences.Services_cff")

from RecoEgamma.EgammaTools.l1SeedModsInMini_cff import setL1SeedFilterToMini,setL1GTProducerToMini,setL1EGMatcherToMini

# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.hltGtStage2ObjectMap = cms.EDProducer( "L1TGlobalProducer",
    MuonInputTag = cms.InputTag( 'hltGtStage2Digis','Muon' ),
    MuonShowerInputTag = cms.InputTag(""),
    EGammaInputTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    TauInputTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    JetInputTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    EtSumInputTag = cms.InputTag( 'hltGtStage2Digis','EtSum' ),
    EtSumZdcInputTag = cms.InputTag(""),
    CICADAInputTag = cms.InputTag(""),
    ExtInputTag = cms.InputTag( "hltGtStage2Digis" ),
    AlgoBlkInputTag = cms.InputTag( "hltGtStage2Digis" ),
    GetPrescaleColumnFromData = cms.bool( False ),
    AlgorithmTriggersUnprescaled = cms.bool( False ),
    RequireMenuToMatchAlgoBlkInput = cms.bool( True ),
    AlgorithmTriggersUnmasked = cms.bool( True ),
    useMuonShowers = cms.bool( True ),
    resetPSCountersEachLumiSec = cms.bool( True ),
    semiRandomInitialPSCounters = cms.bool( False ),
    ProduceL1GtDaqRecord = cms.bool( True ),
    ProduceL1GtObjectMapRecord = cms.bool( True ),
    EmulateBxInEvent = cms.int32( 1 ),
    L1DataBxInEvent = cms.int32( 5 ),
    AlternativeNrBxBoardDaq = cms.uint32( 0 ),
    BstLengthBytes = cms.int32( -1 ),
    PrescaleSet = cms.uint32( 1 ),
    Verbosity = cms.untracked.int32( 0 ),
    PrintL1Menu = cms.untracked.bool( False ),
    TriggerMenuLuminosity = cms.string( "startup" )
)
#setL1GTProducerToMini(process.hltGtStage2ObjectMap)

process.GlobalParametersRcdSource = cms.ESSource( "EmptyESSource",
    recordName = cms.string( "L1TGlobalParametersRcd" ),
    iovIsRunNotTime = cms.bool( True ),
    firstValid = cms.vuint32( 1 )
)

process.GlobalParameters = cms.ESProducer( "StableParametersTrivialProducer",
    TotalBxInEvent = cms.int32( 5 ),
    NumberPhysTriggers = cms.uint32( 512 ),
    NumberL1Muon = cms.uint32( 8 ),
    NumberL1EGamma = cms.uint32( 12 ),
    NumberL1Jet = cms.uint32( 12 ),
    NumberL1Tau = cms.uint32( 12 ),
    NumberChips = cms.uint32( 1 ),
    PinsOnChip = cms.uint32( 512 ),
    OrderOfChip = cms.vint32( 1 ),
    NumberL1IsoEG = cms.uint32( 4 ),
    NumberL1JetCounts = cms.uint32( 12 ),
    UnitLength = cms.int32( 8 ),
    NumberL1ForJet = cms.uint32( 4 ),
    IfCaloEtaNumberBits = cms.uint32( 4 ),
    IfMuEtaNumberBits = cms.uint32( 6 ),
    NumberL1TauJet = cms.uint32( 4 ),
    NumberL1Mu = cms.uint32( 4 ),
    NumberConditionChips = cms.uint32( 1 ),
    NumberPsbBoards = cms.int32( 7 ),
    NumberL1CenJet = cms.uint32( 4 ),
    PinsOnConditionChip = cms.uint32( 512 ),
    NumberL1NoIsoEG = cms.uint32( 4 ),
    NumberTechnicalTriggers = cms.uint32( 64 ),
    NumberPhysTriggersExtended = cms.uint32( 64 ),
    WordLength = cms.int32( 64 ),
    OrderConditionChip = cms.vint32( 1 ),
    appendToDataLabel = cms.string( "" )
)


process.hltEgammaCandidates =  cms.EDProducer( "RecoEcalCandidateFromPatTrigObjProducer",
                                              trigObjs= cms.InputTag("slimmedPatTrigger",processName=cms.InputTag.skipCurrentProcess()),
                                               collectionToMatch = cms.string("hltEgammaCandidates::HLT"),
)

process.hltL1sSingleEGor = cms.EDFilter( "HLTL1TSeed",
    saveTags = cms.bool( True ),
    L1SeedsLogicalExpression = cms.string( "L1_SingleLooseIsoEG26er2p5 OR L1_SingleLooseIsoEG26er1p5 OR L1_SingleLooseIsoEG28er2p5 OR L1_SingleLooseIsoEG28er2p1 OR L1_SingleLooseIsoEG28er1p5 OR L1_SingleLooseIsoEG30er2p5 OR L1_SingleLooseIsoEG30er1p5 OR L1_SingleEG26er2p5 OR L1_SingleEG38er2p5 OR L1_SingleEG40er2p5 OR L1_SingleEG42er2p5 OR L1_SingleEG45er2p5 OR L1_SingleEG60 OR L1_SingleEG34er2p5 OR L1_SingleEG36er2p5 OR L1_SingleIsoEG24er2p1 OR L1_SingleIsoEG26er2p1 OR L1_SingleIsoEG28er2p1 OR L1_SingleIsoEG30er2p1 OR L1_SingleIsoEG32er2p1 OR L1_SingleIsoEG26er2p5 OR L1_SingleIsoEG28er2p5 OR L1_SingleIsoEG30er2p5 OR L1_SingleIsoEG32er2p5 OR L1_SingleIsoEG34er2p5" ),
    L1ObjectMapInputTag = cms.InputTag( "hltGtStage2ObjectMap" ),
    L1GlobalInputTag = cms.InputTag( "hltGtStage2Digis" ),
    L1MuonInputTag = cms.InputTag( 'hltGtStage2Digis','Muon' ),
    L1MuonShowerInputTag = cms.InputTag( 'hltGtStage2Digis','MuonShower' ),
    L1EGammaInputTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    L1JetInputTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    L1TauInputTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    L1EtSumInputTag = cms.InputTag( 'hltGtStage2Digis','EtSum' ),
    L1EtSumZdcInputTag = cms.InputTag( 'hltGtStage2Digis','EtSumZDC' )
)

process.hltL1sSingleEGorProposed = cms.EDFilter( "HLTL1TSeed",
    saveTags = cms.bool( True ),
    L1SeedsLogicalExpression = cms.string( "L1_SingleLooseIsoEG28er2p5 OR L1_SingleLooseIsoEG28er2p1 OR L1_SingleLooseIsoEG28er1p5 OR L1_SingleLooseIsoEG30er2p5 OR L1_SingleLooseIsoEG30er1p5 OR L1_SingleEG38er2p5 OR L1_SingleEG40er2p5 OR L1_SingleEG42er2p5 OR L1_SingleEG45er2p5 OR L1_SingleEG60 OR L1_SingleEG34er2p5 OR L1_SingleEG36er2p5 OR L1_SingleIsoEG28er2p1 OR L1_SingleIsoEG30er2p1 OR L1_SingleIsoEG32er2p1 OR L1_SingleIsoEG28er2p5 OR L1_SingleIsoEG30er2p5 OR L1_SingleIsoEG32er2p5 OR L1_SingleIsoEG34er2p5" ),    
    L1ObjectMapInputTag = cms.InputTag( "hltGtStage2ObjectMap" ),
    L1GlobalInputTag = cms.InputTag( "hltGtStage2Digis" ),
    L1MuonInputTag = cms.InputTag( 'hltGtStage2Digis','Muon' ),
    L1MuonShowerInputTag = cms.InputTag( 'hltGtStage2Digis','MuonShower' ),
    L1EGammaInputTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    L1JetInputTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    L1TauInputTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    L1EtSumInputTag = cms.InputTag( 'hltGtStage2Digis','EtSum' ),
    L1EtSumZdcInputTag = cms.InputTag( 'hltGtStage2Digis','EtSumZDC' )
)
#setL1SeedFilterToMini(process.hltL1sSingleEGor)


process.hltEGL1SingleEGOrRefFilter = cms.EDFilter( "HLTEgammaL1TMatchFilterRegional",
    saveTags = cms.bool( True ),
    candIsolatedTag = cms.InputTag( "hltEgammaCandidates" ),
    l1IsolatedTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    candNonIsolatedTag = cms.InputTag( "" ),
    l1NonIsolatedTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    L1SeedFilterTag = cms.InputTag( "hltL1sSingleEGor" ),
    l1CenJetsTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    l1TausTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    ncandcut = cms.int32( 1 ),
    doIsolated = cms.bool( False ),
    region_eta_size = cms.double( 0.522 ),
    region_eta_size_ecap = cms.double( 1.0 ),
    region_phi_size = cms.double( 1.044 ),
    barrel_end = cms.double( 1.4791 ),
    endcap_end = cms.double( 2.65 )
)
process.hltEGL1SingleEGOrProposedFilter = cms.EDFilter( "HLTEgammaL1TMatchFilterRegional",
    saveTags = cms.bool( True ),
    candIsolatedTag = cms.InputTag( "hltEgammaCandidates" ),
    l1IsolatedTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    candNonIsolatedTag = cms.InputTag( "" ),
    l1NonIsolatedTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    L1SeedFilterTag = cms.InputTag( "hltL1sSingleEGorProposed" ),
    l1CenJetsTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    l1TausTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    ncandcut = cms.int32( 1 ),
    doIsolated = cms.bool( False ),
    region_eta_size = cms.double( 0.522 ),
    region_eta_size_ecap = cms.double( 1.0 ),
    region_phi_size = cms.double( 1.044 ),
    barrel_end = cms.double( 1.4791 ),
    endcap_end = cms.double( 2.65 )
)
#setL1EGMatcherToMini(process.hltEGL1SingleEGOrFilter)

process.hltGtStage2Digis = cms.EDProducer("L1TDigisCopier",
                                            egamma = cms.InputTag("caloStage2Digis","EGamma"),
                                            jet = cms.InputTag("caloStage2Digis","Jet"),
                                            tau = cms.InputTag("caloStage2Digis","Tau"),
                                            etSum = cms.InputTag("caloStage2Digis","EtSum"),
                                            muon = cms.InputTag("gmtStage2Digis","Muon"),
                                            globalAlgBlk = cms.InputTag("gtStage2Digis"),
                                            globalExtBlk = cms.InputTag("gtStage2Digis"),
                                            )


process.path = cms.Path(process.hltGtStage2Digis*
                        process.hltGtStage2ObjectMap*
                        process.hltEgammaCandidates*
                        process.hltL1sSingleEGor*                        
                        process.hltEGL1SingleEGOrRefFilter*
                        process.hltL1sSingleEGorProposed*
                        process.hltEGL1SingleEGOrProposedFilter
                        )

process.hltTriggerSummaryAOD = cms.EDProducer( "TriggerSummaryProducerAOD",
    throw = cms.bool( False ),
    processName = cms.string( "@" ),
    moduleLabelPatternsToMatch = cms.vstring( 'hlt*'),
    moduleLabelPatternsToSkip = cms.vstring(  )
)

process.slimmedPatTrigger = cms.EDProducer("PatTrigObjFilterAdder",
                                           trigObjs = cms.InputTag("slimmedPatTrigger",processName=cms.InputTag.skipCurrentProcess()),
                                           trigEvt = cms.InputTag("hltTriggerSummaryAOD"),
                                           trigResults = cms.InputTag("TriggerResults","","HLT"),
                                           filters = cms.vstring("hltEGL1SingleEGOrRefFilter","hltEGL1SingleEGOrProposedFilter"),
)
process.finalPath = cms.Path( process.hltTriggerSummaryAOD * process.slimmedPatTrigger )



process.out = cms.OutputModule("PoolOutputModule",
                                fileName = cms.untracked.string(options.outputFile),
                                outputCommands = cms.untracked.vstring('keep *'),
)
process.outPath = cms.EndPath(process.out)

print(process.GlobalTag.globaltag)
process.options.wantSummary = True

