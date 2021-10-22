import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/mc/RunIISummer20UL18RECO/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/AODSIM/106X_upgrade2018_realistic_v11_L1v1-v2/00000/B4A06248-D09E-314A-ACD7-F157B86109E6.root")
)

process.demo = cms.EDAnalyzer('DemoAnalyzer',
   genParticles = cms.InputTag("genParticles"),
   GENobject = cms.string("ele"),
   vsVar = cms.string("pt"),
                              )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('histodemo.root')
                                   )

process.p = cms.Path(process.demo)
