import FWCore.ParameterSet.Config as cms

process = cms.Process("HLTGenValSource")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Core.DQMStore_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.load("DQMServices.Components.MEtoEDMConverter_cff")
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/mc/RunIISummer20UL18RECO/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/AODSIM/106X_upgrade2018_realistic_v11_L1v1-v2/00000/B4A06248-D09E-314A-ACD7-F157B86109E6.root")
    fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/67A99845-DA80-E048-BA70-CA5A4BF4F5E5.root")
)

process.HLTGenValSourceMU = cms.EDProducer('HLTGenValSource',
    # these are the only one the user needs to specify
    objType = cms.string("mu"),
    hltPathsToCheck = cms.vstring(
      "HLT_IsoMu24_v",
    ),
)

process.p = cms.Path(process.HLTGenValSourceMU)

# the harvester
process.harvester = DQMEDHarvester("HLTGenValClient",
    outputFileName = cms.untracked.string('sourceoutput_harvest.root'),
    subDirs        = cms.untracked.vstring("HLTGenVal/*"),
)

process.outpath = cms.EndPath(process.harvester)
