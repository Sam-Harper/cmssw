# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: reco --filein file:Py8PtGun_cfi_py_GEN_SIM_DIGI_L1_L1TrackTrigger_DIGI2RAW_HLT.root --conditions auto:phase2_realistic -n 10 --era Phase2C8_timing_layer_bar --eventcontent FEVTDEBUGHLT --runUnscheduled -s RAW2DIGI,L1Reco,RECO,RECOSIM --datatier GEN-SIM-RECO --geometry Extended2023D41 --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('EGAMMA',eras.Phase2C8_timing_layer_bar)

process.load('Configuration.Geometry.GeometryExtended2026D41Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.RecoSim_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing("analysis")
options.parseArguments()   

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)


# Input source
process.source = cms.Source("PoolSource",
     fileNames = cms.untracked.vstring(options.inputFiles),
    secondaryFileNames = cms.untracked.vstring()
)


process.options = cms.untracked.PSet(

)

# Output definition

process.output = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RECO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string(options.outputFile),
    outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)
process.output.outputCommands.append('keep *_ecalDrivenGsfElectronsFromTICL_*_*')

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

process.output_path = cms.EndPath(process.output)


# Import TICL
#from ticl_iterations import TICL_iterations
from RecoHGCal.TICL.ticl_iterations import TICL_iterations

# Attach a sequence to process: process.TICL
process = TICL_iterations(process)

process.load("RecoLocalCalo.Configuration.hgcalLocalReco_cff")
process.load("MyModules.Test.ecalDrivenGsfElectronsFromTICL_cff")
process.ecalDrivenGsfElectronsFromTICL_step.associate(process.hgcalLocalRecoTask)

#so we need to remake the pixel seeds, we only need to remake the electron triplet seeds
#this is because we are only doing electrons, normally to save time, it makes quadruplets 
#and high pt triplets used by general tracking first. The tripletElectronSeeds are cleaned for these
#via the skipClusters so they dont remake the same seeds so the 
#initial + highPt Triplet + tripletElectron seeds are all possible triplets. However we're not interested
#in running the rest of the tracking so we just make the tripletElectrons on all possible combinations
#possibly faster if we did the other way though but easier this way
#note getting doublets back would probably be sensible here again, need to follow up on the port of HLT code
#
process.ecalDrivenGsfElectronsFromTICL_step.insert(0,process.siPixelRecHits)
process.ecalDrivenGsfElectronsFromTICL_step.insert(1,process.MeasurementTrackerEvent)
process.ecalDrivenGsfElectronsFromTICL_step.insert(2,process.tripletElectronSeedLayers)
process.ecalDrivenGsfElectronsFromTICL_step.insert(3,process.tripletElectronTrackingRegions)
process.ecalDrivenGsfElectronsFromTICL_step.insert(4,process.trackerClusterCheck)
process.ecalDrivenGsfElectronsFromTICL_step.insert(5,process.tripletElectronHitDoublets)
process.ecalDrivenGsfElectronsFromTICL_step.insert(6,process.tripletElectronHitTriplets)
process.ecalDrivenGsfElectronsFromTICL_step.insert(7,process.tripletElectronSeeds)
del process.tripletElectronSeedLayers.BPix.skipClusters
del process.tripletElectronSeedLayers.FPix.skipClusters
#process.tripletElectronHitDoublets.produceSeedingHitSets = True
process.ecalDrivenElectronSeedsFromTICL.SeedConfiguration.initialSeedsVector = cms.VInputTag("tripletElectronSeeds",)

#do not add changes to your config after this point (unless you know what you are doing)
from FWCore.ParameterSet.Utilities import convertToUnscheduled
process=convertToUnscheduled(process)


# Customisation from command line

#Have logErrorHarvester wait for the same EDProducers to finish as those providing data for the OutputModule
from FWCore.Modules.logErrorHarvester_cff import customiseLogErrorHarvesterUsingOutputCommands
process = customiseLogErrorHarvesterUsingOutputCommands(process)

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

