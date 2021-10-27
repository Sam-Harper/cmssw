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
dirname = "test"

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://cmsxrootd.fnal.gov//store/mc/RunIISummer20UL18RECO/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/AODSIM/106X_upgrade2018_realistic_v11_L1v1-v2/00000/B4A06248-D09E-314A-ACD7-F157B86109E6.root")
)

# the source
process.HLTGenValSource = cms.EDProducer('HLTGenValSource',

    ### general configurations
    DQMDirName = cms.string(dirname),
    genParticles = cms.InputTag("genParticles"),
    objType = cms.string("ele"),

    ### filter configs
    # A vector of Psets, currently only expected to have a "filterName"
    # More parameters can be added, for example event level cuts for each filter
    filterConfigs = cms.VPSet(
        cms.PSet(
            filterName = cms.string("none"), # filters not yet implemented, this doesn't do anything!
        ),
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
    subDirs        = cms.untracked.vstring(dirname),
    efficiency     = cms.vstring("EFF 'title; label; label' ele_none_vspt ele_none_vspt")
)

process.p = cms.Path(process.HLTGenValSource)

#process.dqmSaver.producer = 'HLTGenValSource'
#process.dqmSaver.workflow = '/HLTGenValSource/HLTGenValHistColl/HLTGenValHist'
process.endpath = cms.EndPath(process.harvester)
