import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

"""
This file does the harvesting step, converts the monitoring elements in EDM format to
histograms which are then outputed in a DQM file to be used in ROOT/whatever

It also runs the client which makes histograms which can not be filled directly, eg 
efficiency histograms
"""


options = VarParsing.VarParsing("analysis")
options.register(
    "outTag", "DQMIO", options.multiplicity.singleton, options.varType.string, "outTag"
)
options.parseArguments()

process = cms.Process("HARVESTING")

process.source = cms.Source(
    "DQMRootSource", fileNames=cms.untracked.vstring(options.inputFiles)
)

# DQMStore service
process.load("DQMServices.Core.DQMStore_cfi")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(5000),
    limit = cms.untracked.int32(10000000)
)

from DQMServices.Core.DQMEDHarvester import DQMEDHarvester
process.exampleHarvester = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Example"),
                                               verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "elesExample_vsPt_eff         'Filter Efficiency;            ele pt [GeV]; efficiency'     elesExample_vsPt_pass elesExample_vsPt_tot",
        "elesExample_vsPtEta_eff         'Filter Efficiency;            ele pt [GeV]; ele #eta'     elesExample_vsPtEta_pass elesExample_vsPtEta_tot",
        "musExample_vsPt_eff         'Filter Efficiency;            ele pt [GeV]; efficiency'     musExample_vsPt_pass musExample_vsPt_tot",
        "musExample_vsPtEta_eff         'Filter Efficiency;            ele pt [GeV]; ele #eta'     musExample_vsPtEta_pass musExample_vsPtEta_tot",
    ),
    efficiencyProfile = cms.untracked.vstring(
    ),
)



# DQM file saver
process.load("DQMServices.Components.DQMFileSaver_cfi")
process.dqmSaver.workflow = "/EGamma0/PromptReco-v1/{}".format(options.outTag)
process.dqmSaver.saveByRun = 1

process.DQMFileSaverOutput = cms.EndPath(
    process.exampleHarvester + process.dqmSaver
)
