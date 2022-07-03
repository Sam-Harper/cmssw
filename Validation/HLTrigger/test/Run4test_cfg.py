import FWCore.ParameterSet.Config as cms

process = cms.Process("HLTGenValSource")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Core.DQMStore_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.load("DQMServices.Components.MEtoEDMConverter_cff")
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:input/RelValTTbar_14TeV_Phase2HLT.root'
    )
)

ptBinsLow = cms.vdouble(0, 10, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 90, 100)
ptBinsMed = cms.vdouble(0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350, 375, 400, 425, 450, 475, 500)
ptBinsHigh = cms.vdouble(0,  50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500)
etaBins=cms.vdouble(-4,-3, -2.5, -2, -1.5, -1, -0.5, 0, 0.5, 1, 1.5, 2, 2.5, 3, 4)

etaCut=cms.PSet(
    rangeVar=cms.string("eta"),
    allowedRanges=cms.vstring("-2.4:2.4")
)

# AK4 jets
process.HLTGenValSourceAK4 = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("AK4jet"),
    hltPathsToCheck = cms.vstring(
        "l1tReconstructionPath",
        "L1T_SinglePFPuppiJet230off",
        "HLT_AK4PFPuppiJet520",
        "L1T_PFHT400PT30_QuadPFPuppiJet_70_55_40_40_2p4",
        "L1T_DoublePFPuppiJets112_2p4_DEta1p6",
        "HLT_PFHT330PT30_QuadPFPuppiJet_75_60_45_40_TriplePFPuppiBTagDeepCSV_2p4",
        "HLT_PFHT200PT30_QuadPFPuppiJet_70_40_30_30_TriplePFPuppiBTagDeepCSV_2p4",
        "HLT_DoublePFPuppiJets128_DoublePFPuppiBTagDeepCSV_2p4",
        "HLT_PFHT330PT30_QuadPFPuppiJet_75_60_45_40_TriplePFPuppiBTagDeepFlavour_2p4",
        "HLT_PFHT200PT30_QuadPFPuppiJet_70_40_30_30_TriplePFPuppiBTagDeepFlavour_2p4",
        "HLT_DoublePFPuppiJets128_DoublePFPuppiBTagDeepFlavour_2p4",
    ),
    hltProcessName = cms.string("HLTX"),
    doOnlyLastFilter = cms.bool(False),
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBinsMed,
            rangeCuts = cms.VPSet(etaCut)
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
        ),
    ),
)

process.HLTGenValSourceAK4HT = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("AK4HT"),
    hltPathsToCheck = cms.vstring(
        "l1tReconstructionPath",
        "L1T_PFPuppiHT450off",
        "HLT_PFPuppiHT1070",
        "HLT_PFPuppiMETTypeOne140_PFPuppiMHT140",
        "L1T_PFHT400PT30_QuadPFPuppiJet_70_55_40_40_2p4",
        "HLT_PFHT330PT30_QuadPFPuppiJet_75_60_45_40_TriplePFPuppiBTagDeepCSV_2p4",
        "HLT_PFHT200PT30_QuadPFPuppiJet_70_40_30_30_TriplePFPuppiBTagDeepCSV_2p4",
        "HLT_PFHT330PT30_QuadPFPuppiJet_75_60_45_40_TriplePFPuppiBTagDeepFlavour_2p4",
        "HLT_PFHT200PT30_QuadPFPuppiJet_70_40_30_30_TriplePFPuppiBTagDeepFlavour_2p4",
    ),
    hltProcessName = cms.string("HLTX"),
    doOnlyLastFilter = cms.bool(False),
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBinsHigh,
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
        ),
    ),
)

process.HLTGenValSourceMET = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("MET"),
    hltPathsToCheck = cms.vstring(
        "l1tReconstructionPath",
        "L1T_PFPuppiMET220off",
        "HLT_PFPuppiMETTypeOne140_PFPuppiMHT140",
    ),
    hltProcessName = cms.string("HLTX"),
    doOnlyLastFilter = cms.bool(False),
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBinsMed,
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
        ),
    ),
)

process.HLTGenValSourceMU = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("mu"),
    hltPathsToCheck = cms.vstring(
        "L1T_SingleTkMuon_22",
        "L1T_DoubleTkMuon_15_7",
        "L1T_TripleTkMuon_5_3_3",
        "HLT_Mu50_FromL1TkMuon",
        "HLT_IsoMu24_FromL1TkMuon",
        "HLT_Mu37_Mu27_FromL1TkMuon",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_FromL1TkMuon",
        "HLT_TriMu_10_5_5_DZ_FromL1TkMuon",
    ),
    hltProcessName = cms.string("HLTX"),
    doOnlyLastFilter = cms.bool(False),
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBinsLow,
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
        ),
    ),
)

process.HLTGenValSourceELE = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("ele"),
    hltPathsToCheck = cms.vstring(
        "l1tReconstructionPath",
        "L1T_TkEm51",
        "L1T_TkEle36",
        "L1T_TkIsoEm36",
        "L1T_TkIsoEle28",
        "L1T_TkEm37TkEm24",
        "L1T_TkEle25TkEle12",
        "L1T_TkIsoEm22TkIsoEm12",
        "L1T_TkIsoEle22TkEm12",
        "HLT_Ele32_WPTight_Unseeded",
        "HLT_Ele26_WP70_Unseeded",
        "HLT_Ele32_WPTight_L1Seeded",
        "HLT_Ele26_WP70_L1Seeded",
        "MC_Ele5_Open_Unseeded",
        "MC_Ele5_WP70_Open_Unseeded",
        "MC_Ele5_Open_L1Seeded",
        "MC_Ele5_WP70_Open_L1Seeded",
    ),
    hltProcessName = cms.string("HLTX"),
    doOnlyLastFilter = cms.bool(False),
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBinsLow,
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
        ),
    ),
)

process.HLTGenValSourcePHO = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("pho"),
    hltPathsToCheck = cms.vstring(
        "l1tReconstructionPath",
        "HLT_Photon108EB_TightID_TightIso_Unseeded",
        "HLT_Photon187_Unseeded",
        "HLT_DoubleEle25_CaloIdL_PMS2_Unseeded",
        "HLT_Diphoton30_23_IsoCaloId_Unseeded",
        "HLT_Photon108EB_TightID_TightIso_L1Seeded",
        "HLT_Photon187_L1Seeded",
        "HLT_DoubleEle23_12_Iso_L1Seeded",
        "HLT_Diphoton30_23_IsoCaloId_L1Seeded",
        "MC_Photon100_Open_Unseeded",
        "MC_Photon100EB_TightID_TightIso_Open_Unseeded",
        "MC_Photon100_Open_L1Seeded",
        "MC_Photon100EB_TightID_TightIso_Open_L1Seeded"
    ),
    hltProcessName = cms.string("HLTX"),
    doOnlyLastFilter = cms.bool(False),
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBinsLow,
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
        ),
    ),
)

process.HLTGenValSourceTEMPLATE = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("TEMPLATE DONT USE"),
    hltPathsToCheck = cms.vstring(
        "l1tReconstructionPath",
        "L1T_SinglePFPuppiJet230off",
        "L1T_PFPuppiHT450off",
        "L1T_PFPuppiMET220off",
        "HLT_AK4PFPuppiJet520",
        "HLT_PFPuppiHT1070",
        "HLT_PFPuppiMETTypeOne140_PFPuppiMHT140",
        "L1T_PFHT400PT30_QuadPFPuppiJet_70_55_40_40_2p4",
        "L1T_DoublePFPuppiJets112_2p4_DEta1p6",
        "HLT_PFHT330PT30_QuadPFPuppiJet_75_60_45_40_TriplePFPuppiBTagDeepCSV_2p4",
        "HLT_PFHT200PT30_QuadPFPuppiJet_70_40_30_30_TriplePFPuppiBTagDeepCSV_2p4",
        "HLT_DoublePFPuppiJets128_DoublePFPuppiBTagDeepCSV_2p4",
        "HLT_PFHT330PT30_QuadPFPuppiJet_75_60_45_40_TriplePFPuppiBTagDeepFlavour_2p4",
        "HLT_PFHT200PT30_QuadPFPuppiJet_70_40_30_30_TriplePFPuppiBTagDeepFlavour_2p4",
        "HLT_DoublePFPuppiJets128_DoublePFPuppiBTagDeepFlavour_2p4",
        "L1T_SingleTkMuon_22",
        "L1T_DoubleTkMuon_15_7",
        "L1T_TripleTkMuon_5_3_3",
        "HLT_Mu50_FromL1TkMuon",
        "HLT_IsoMu24_FromL1TkMuon",
        "HLT_Mu37_Mu27_FromL1TkMuon",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_FromL1TkMuon",
        "HLT_TriMu_10_5_5_DZ_FromL1TkMuon",
        "L1T_TkEm51",
        "L1T_TkEle36",
        "L1T_TkIsoEm36",
        "L1T_TkIsoEle28",
        "L1T_TkEm37TkEm24",
        "L1T_TkEle25TkEle12",
        "L1T_TkIsoEm22TkIsoEm12",
        "L1T_TkIsoEle22TkEm12",
        "HLT_Ele32_WPTight_Unseeded",
        "HLT_Ele26_WP70_Unseeded",
        "HLT_Photon108EB_TightID_TightIso_Unseeded",
        "HLT_Photon187_Unseeded",
        "HLT_DoubleEle25_CaloIdL_PMS2_Unseeded",
        "HLT_Diphoton30_23_IsoCaloId_Unseeded",
        "HLT_Ele32_WPTight_L1Seeded",
        "HLT_Ele26_WP70_L1Seeded",
        "HLT_Photon108EB_TightID_TightIso_L1Seeded",
        "HLT_Photon187_L1Seeded",
        "HLT_DoubleEle25_CaloIdL_PMS2_L1Seeded",
        "HLT_DoubleEle23_12_Iso_L1Seeded",
        "HLT_Diphoton30_23_IsoCaloId_L1Seeded",
        "L1T_DoubleNNTau52",
        "L1T_SingleNNTau150",
        "MC_JME",
        "MC_BTV",
        "MC_Ele5_Open_Unseeded",
        "MC_Ele5_WP70_Open_Unseeded",
        "MC_Ele5_Open_L1Seeded",
        "MC_Ele5_WP70_Open_L1Seeded",
        "MC_Photon100_Open_Unseeded",
        "MC_Photon100EB_TightID_TightIso_Open_Unseeded",
        "MC_Photon100_Open_L1Seeded",
        "MC_Photon100EB_TightID_TightIso_Open_L1Seeded"
    ),
    hltProcessName = cms.string("HLTX"),
    doOnlyLastFilter = cms.bool(False),
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBinsHigh,
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
        ),
    ),
)

process.p = cms.Path(process.HLTGenValSourceAK4*process.HLTGenValSourceAK4HT*process.HLTGenValSourceMU*process.HLTGenValSourceMET*process.HLTGenValSourceELE*process.HLTGenValSourcePHO)

# the harvester
process.harvester = DQMEDHarvester("HLTGenValClient",
    outputFileName = cms.untracked.string('upgrade_run4_test.root'),
    subDirs        = cms.untracked.vstring("HLTGenVal"),
)

process.outpath = cms.EndPath(process.harvester)
