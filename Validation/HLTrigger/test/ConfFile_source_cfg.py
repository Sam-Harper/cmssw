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


# using: CMSSW_12_3_0_pre4__fullsim_noPU_2021_14TeV-TTbar_14TeV-00001
process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/mc/RunIISummer20UL18RECO/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/AODSIM/106X_upgrade2018_realistic_v11_L1v1-v2/00000/B4A06248-D09E-314A-ACD7-F157B86109E6.root")
    fileNames = cms.untracked.vstring(
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_12_3_0_pre4/RelValTTbar_14TeV/GEN-SIM/123X_mcRun3_2021_realistic_v4-v1/2580000/299fa756-9ac5-4cd6-95f5-b9e5482c34e6.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_12_3_0_pre4/RelValTTbar_14TeV/GEN-SIM/123X_mcRun3_2021_realistic_v4-v1/2580000/968d62a1-4053-4d1e-b50b-2843aca5b693.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_12_3_0_pre4/RelValTTbar_14TeV/GEN-SIM/123X_mcRun3_2021_realistic_v4-v1/2580000/a8872a98-50ca-4f48-992b-b136d4131040.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_12_3_0_pre4/RelValTTbar_14TeV/GEN-SIM/123X_mcRun3_2021_realistic_v4-v1/2580000/edcec532-ce30-4db5-bc36-bead4ee733ed.root",
    )
)

ptBins=cms.vdouble(0, 10,  20,  30,  40,  50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 275, 300)
etaBins=cms.vdouble(-4,-3, -2.5, -2, -1.5, -1, -0.5, 0, 0.5, 1, 1.5, 2, 2.5, 3, 4)

etaCut=cms.PSet(
    rangeVar=cms.string("eta"),
    allowedRanges=cms.vstring("-2.4:2.4")
)
ptCut=cms.PSet(
    rangeVar=cms.string("pt"),
    allowedRanges=cms.vstring("40:9999")
)

process.HLTGenValSourceMET = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("MET"),
    hltPathsToCheck = cms.vstring(
      "HLT_PFMET120_PFMHT120_IDTight_v",
      "HLT_PFMET110_PFMHT110_IDTight_v",
    ),
    doOnlyLastFilter = cms.bool(True),
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBins,
            rangeCuts = cms.VPSet(etaCut)
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
            rangeCuts = cms.VPSet(ptCut)
        ),
    ),
)


process.p = cms.Path(process.HLTGenValSourceMET)

# the harvester
process.harvester = DQMEDHarvester("HLTGenValClient",
    outputFileName = cms.untracked.string('sourceoutput_harvest.root'),
    subDirs        = cms.untracked.vstring("HLTGenVal/*"),
)

process.outpath = cms.EndPath(process.harvester)
