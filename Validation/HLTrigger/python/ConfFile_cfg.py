import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Core.DQMStore_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

# parameter definitions
ptBins=cms.vdouble(5,10,12.5,15,17.5,20,22.5,25,30,35,40,45,50,60,80,100,150,200,250,300,350,400)
etaBins=cms.vdouble(-10, -8, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 8, 10)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/mc/RunIISummer20UL18RECO/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/AODSIM/106X_upgrade2018_realistic_v11_L1v1-v2/00000/B4A06248-D09E-314A-ACD7-F157B86109E6.root")
    #fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_12/RelValTTbar_13UP16/AODSIM/PU25ns_106X_mcRun2_asymptotic_v13_hltul16_postVFP-v1/20000/67A99845-DA80-E048-BA70-CA5A4BF4F5E5.root")
)

# the source
process.HLTGenValSource = cms.EDProducer('HLTGenValSource',

    ### general configurations
    DQMDirName = cms.string("HLTGenVal"),
    genParticles = cms.InputTag("genParticles"),
    hltProcessName = cms.string("HLT"), # this should be used to replace the following one if that one is needed
    TrigEvent = cms.InputTag("hltTriggerSummaryAOD"),
    objType = cms.string("mu"),

    ### path configs
    # vector of paths that will be looked at. Need to get this automatically for certain objType later
    # not sure yet whether I need this, the list of filters, or both
    hltPathsToCheck = cms.vstring(
      "HLT_IsoMu24_v",
      "HLT_Mu50_v",
    ),

    ### histogram configs
    # Configuring individual histograms, expecting vsVar as string and bin edges
    # this histogram is created once for each filter specified above
    histConfigs = cms.VPSet(
        cms.PSet(
            vsVar = cms.string("pt"),
            binLowEdges = ptBins,
        ),
        cms.PSet(
            vsVar = cms.string("eta"),
            binLowEdges = etaBins,
        ),
    ),
)

# the harvester
process.harvester = DQMEDHarvester("DQMGenericClient",
    verbose        = cms.untracked.uint32(0), #set this to zero!
    outputFileName= cms.untracked.string('testoutput.root'),
    commands       = cms.vstring(),
    resolution     = cms.vstring(),
    subDirs        = cms.untracked.vstring("HLTGenVal"),
    efficiency     = cms.vstring("EFF 'title; label; label' ele_none_vspt ele_none_vspt")
)

process.p = cms.Path(process.HLTGenValSource)

#process.dqmSaver.producer = 'HLTGenValSource'
#process.dqmSaver.workflow = '/HLTGenValSource/HLTGenValHistColl/HLTGenValHist'
process.endpath = cms.EndPath(process.harvester)
