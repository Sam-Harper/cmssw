import FWCore.ParameterSet.Config as cms

process = cms.Process("HLTGenValSource")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Core.DQMStore_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.load("DQMServices.Components.MEtoEDMConverter_cff")
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/mc/RunIISummer20UL18RECO/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/AODSIM/106X_upgrade2018_realistic_v11_L1v1-v2/00000/B4A06248-D09E-314A-ACD7-F157B86109E6.root")
    fileNames = cms.untracked.vstring(
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/66DECAFD-992F-E640-9F8C-5B24C46DF44C.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/67A99845-DA80-E048-BA70-CA5A4BF4F5E5.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/88469F37-B606-914B-9011-0296A98C5148.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/B17199CD-F17A-8C43-AB4E-FFC19BCD9DC3.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/B3818D4B-CE8C-FA4B-AD3F-458562EA0CEE.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/B636085B-1567-8644-B4CB-738D5F28FDC3.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/CB7C971A-DCBE-904D-9285-622FAAACBDBB.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/DC7751C3-B7AB-3245-BB8F-75AF564959F4.root",
    "root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/FB718118-108C-A848-BC16-B2F2A120E034.root",
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
