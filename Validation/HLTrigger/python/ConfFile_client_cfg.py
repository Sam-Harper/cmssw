import FWCore.ParameterSet.Config as cms

process = cms.Process("HLTGenValClient")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Core.DQMStore_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

# parameter definitions
ptBins=cms.vdouble(5,10,12.5,15,17.5,20,22.5,25,30,35,40,45,50,60,80,100,150,200,250,300,350,400)
etaBins=cms.vdouble(-10, -8, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 8, 10)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.source = cms.Source("PoolSource",
    processingMode = cms.untracked.string('RunsAndLumis'),
    fileNames = cms.untracked.vstring("file:/nfs/dust/cms/user/flabe/HLTValidation/CMSSW_1200_HLTVal/src/Validation/HLTrigger/sourceoutput.root")
)

# the client
process.HLTGenValClient = cms.EDProducer('HLTGenValClient',

    ### general configurations
    InputDirName = cms.string("HLTGenVal_parts"),
    OutputDirName = cms.string("HLTGenVal_effs"),
    hltProcessName = cms.string("HLT"), # this should be used to replace the following one if that one is needed
    objType = cms.string("mu"),

    ### path configs
    # vector of paths that will be looked at. Need to get this automatically for certain objType later
    # not sure yet whether I need this, the list of filters, or both
    hltPathsToCheck = cms.vstring(
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
    outputFileName= cms.untracked.string('clientoutput.root'),
    commands       = cms.vstring(),
    resolution     = cms.vstring(),
    subDirs        = cms.untracked.vstring("HLTGenVal_effs"),
    efficiency     = cms.vstring("EFF 'title; label; label' mu_vspt_before mu_hltL1sSingleMu22or25_vspt_pass")
)

process.p = cms.Path(process.HLTGenValClient)

#process.dqmSaver.producer = 'HLTGenValSource'
#process.dqmSaver.workflow = '/HLTGenValSource/HLTGenValHistColl/HLTGenValHist'
process.endpath = cms.EndPath(process.harvester)
