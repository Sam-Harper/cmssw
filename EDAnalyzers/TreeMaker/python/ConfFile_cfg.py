import os

import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
#process = cms.Process("RECO", eras.Run2_2017)
#process = cms.Process("RECO", eras.Run2_2018)

#process = cms.Process("Demo", eras.phase2_hgcal)
process = cms.Process("Demo", eras.Phase2C8_timing_layer_bar)
#process = cms.Process("RECO", eras.Phase2C8_timing_layer_bar)

process.load("FWCore.MessageService.MessageLogger_cfi")
MessageLogger = cms.Service("MessageLogger")


# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
##process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
##process.load('SimGeneral.MixingModule.mixNoPU_cfi')
##process.load('Configuration.Geometry.GeometryExtended2023D41Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.RecoSim_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
##process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
#process.GlobalTag.globaltag = "94X_mc2017_realistic_v10"
#process.GlobalTag.globaltag = "100X_upgrade2018_realistic_v10"

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:phase2_realistic", "")
#process.GlobalTag = GlobalTag(process.GlobalTag, "94X_mc2017_realistic_v10")
#process.GlobalTag = GlobalTag(process.GlobalTag, "94X_mc2017_realistic_v11")


#process.load('Configuration.Geometry.GeometryExtended2023D41Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2026D41Reco_cff')

#process.load('Configuration.Geometry.GeometryExtended2023D41_cff')
process.load('Configuration.Geometry.GeometryExtended2026D41_cff')



############################## Parse arguments ##############################

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing("analysis")


options.register("sourceFile",
    "", # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.string, # string, int, or float
    "File containing list of input files" # Description
)

options.register("outputDir",
    "", # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.string, # string, int, or float
    "Output directory" # Description
)

options.register("outFileNumber",
    -1, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "File number (will be added to the filename if >= 0)" # Description
)

options.register("eventRange",
    "", # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.string, # string, int, or float
    "Syntax: Run1:Event1-Run2:Event2" # Description
)

options.register("debugFile",
    0, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Create debug file" # Description
)

options.register("rerunTICL",
    0, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Whether to rerun TICL" # Description
)

options.register("modTICLele",
    0, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Whether to use modified TICL-electron sequence" # Description
)

options.register("onRaw",
    0, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Running on RAW" # Description
)

options.register("modTICLeleWithRerunTICL",
    0, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Whether to use the rerun TICL for the modified TICL-electron sequence" # Description
)

options.register("storeSimHit",
    1, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Store sim-hits" # Description
)

options.register("storeRecHit",
    1, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Store rec-hits" # Description
)

options.register("trace",
    0, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Trace modules" # Description
)

options.register("memoryCheck",
    0, # Default value
    VarParsing.VarParsing.multiplicity.singleton, # singleton or list
    VarParsing.VarParsing.varType.int, # string, int, or float
    "Check memory usage" # Description
)

options.parseArguments()


#maxEvents = -1
#options.maxEvents = 15
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.maxEvents))


#sourceFile = "sourceFiles/SingleElectronFlatPtGun_pT-15_rajdeep/SingleElectronFlatPtGun_pT-15_rajdeep.txt"
#sourceFile = "sourceFiles/SingleElectronFlatPtGun_pT-15_eta-1p5-3p0_sobhatta-crab_SingleElectronFlatPtGun_pT-15_eta-1p5-3p0_GEN-SIM-RECO-ffc2278112c688bef3890fc698a39794_USER/SingleElectronFlatPtGun_pT-15_eta-1p5-3p0_sobhatta-crab_SingleElectronFlatPtGun_pT-15_eta-1p5-3p0_GEN-SIM-RECO-ffc2278112c688bef3890fc698a39794_USER.txt"
#sourceFile = "sourceFiles/SingleElectronFlatPtGun_pT-15_eta-1p5-3p0_withTICLfractions/SingleElectronFlatPtGun_pT-15_eta-1p5-3p0_withTICLfractions.txt"

#sourceFile = "sourceFiles/SinglePi0FlatPtGun_pT-15_eta-1p5-3p0_GEN-SIM-RECO/SinglePi0FlatPtGun_pT-15_eta-1p5-3p0_GEN-SIM-RECO.txt"
#sourceFile = "sourceFiles/SingleZprimeToEEflatPtGun_m-5_pT-35_eta-1p5-3p0_GEN-SIM-RECO/SingleZprimeToEEflatPtGun_m-5_pT-35_eta-1p5-3p0_GEN-SIM-RECO.txt"

#sourceFile = "sourceFiles/RelValElectronGunPt2To100_CMSSW_10_6_0_pre4-106X_upgrade2023_realistic_v2_2023D41noPU-v1_GEN-SIM-RECO/RelValElectronGunPt2To100_CMSSW_10_6_0_pre4-106X_upgrade2023_realistic_v2_2023D41noPU-v1_GEN-SIM-RECO.txt"
#sourceFile = "sourceFiles/RelValElectronGunPt2To100_CMSSW_10_6_0_patch2-106X_upgrade2023_realistic_v3_2023D41noPU-v1_GEN-SIM-RECO/RelValElectronGunPt2To100_CMSSW_10_6_0_patch2-106X_upgrade2023_realistic_v3_2023D41noPU-v1_GEN-SIM-RECO.txt"

sourceFile = "sourceFiles/SingleElectronFlatPtGun_fpantale_pT-0-200_eta-1p5-3p0_GEN-SIM-RECO/SingleElectronFlatPtGun_fpantale_pT-0-200_eta-1p5-3p0_GEN-SIM-RECO_mod.txt"

if (len(options.sourceFile)) :
    
    sourceFile = options.sourceFile


fNames = []

if (len(options.inputFiles)) :
    
    fNames = options.inputFiles

else :
    
    with open(sourceFile) as f:
        
        fNames = f.readlines()

#fNames = ["file:/afs/cern.ch/work/s/sobhatta/private/HGCal_ele-reco/CMSSW_11_0_0_pre4/src/output_GEN-SIM-RECO_numEvent20.root"]
#fNames = ["file:/afs/cern.ch/work/s/sobhatta/private/HGCal_ele-reco/CMSSW_11_0_0_pre4/src/output_GEN-SIM-RECO_numEvent100.root"]
#fNames = ["file:/afs/cern.ch/work/s/sobhatta/private/HGCal_ele-reco/CMSSW_11_0_0_pre4/src/output_GEN-SIM-RECO_numEvent2000.root"]
#fNames = ["file:/eos/cms/store/group/phys_egamma/fpantale/output_GEN-SIM-RECO.root"]

outFileSuffix = ""

if (options.rerunTICL) :
    
    outFileSuffix = "%s_rerunTICL" %(outFileSuffix)


if (options.modTICLele) :
    
    if (options.rerunTICL and options.modTICLeleWithRerunTICL) :
        
        outFileSuffix = "%s_modTICLeleWithRerunTICL" %(outFileSuffix)
        
    else :
        
        outFileSuffix = "%s_modTICLele" %(outFileSuffix)


if (options.onRaw) :
    
    outFileSuffix = "%s_onRaw" %(outFileSuffix)


if (options.outFileNumber >= 0) :
    
    outFileSuffix = "%s_%d" %(outFileSuffix, options.outFileNumber)


outFile = "ntupleTree%s.root" %(outFileSuffix)

if (len(options.outputDir)) :
    
    os.system("mkdir -p %s" %(options.outputDir))
    
    outFile = "%s/%s" %(options.outputDir, outFile)


sourceFileNames = cms.untracked.vstring(fNames)
#print sourceFileNames

process.source = cms.Source("PoolSource",
    fileNames = sourceFileNames,
    
    # Run1:Event1 to Run2:Event2
    #eventsToProcess = cms.untracked.VEventRange("1:78722-1:78722"),
    
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
)


if (len(options.eventRange)) :
    
    process.source.eventsToProcess = cms.untracked.VEventRange(options.eventRange)


#process.options = cms.untracked.PSet(
#    #SkipEvent = cms.untracked.vstring("ProductNotFound"),
#    
#    #printDependencies = cms.untracked.bool(True),
#)


if (options.onRaw) :
    
    from Configuration.StandardSequences.Reconstruction_cff import *
    
    # Local reco
    print "*"*50
    print "localreco    :", process.localreco
    localreco_mod = process.localreco.copyAndExclude([
        muonlocalreco,
        castorreco,
    ])
    print ""
    print "localreco_mod:", localreco_mod
    print ""
    
    
    # Global reco
    #from RecoTracker.IterativeTracking.MuonSeededStep_cff import *
    from RecoTracker.IterativeTracking.iterativeTk_cff import *
    from RecoVertex.Configuration.RecoVertex_cff import *
    
    print "*"*50
    print "globalreco_tracking    :", process.globalreco_tracking
    globalreco_tracking_mod = process.globalreco_tracking.copyAndExclude([
        standalonemuontracking,
        muonSeededStepTask,
        
        unsortedOfflinePrimaryVertices4Dfastsim,
        trackWithVertexRefSelectorBeforeSorting4Dfastsim,
        trackRefsForJetsBeforeSorting4Dfastsim,
        offlinePrimaryVertices4Dfastsim,
        offlinePrimaryVertices4DfastsimWithBS,
        unsortedOfflinePrimaryVertices4Dfastsim,
        trackWithVertexRefSelectorBeforeSorting4Dfastsim,
        trackRefsForJetsBeforeSorting4Dfastsim,
        offlinePrimaryVertices4Dfastsim,
        offlinePrimaryVertices4DfastsimWithBS,
    ])
    print ""
    print "globalreco_tracking_mod:", globalreco_tracking_mod
    print ""
    
    print "*"*50
    print "globalreco    :", process.globalreco
    globalreco_mod = process.globalreco.copyAndExclude([
        jetGlobalReco,
        muonGlobalReco,
        muoncosmicreco,
        CastorFullReco,
        
        #egammaGlobalReco,
    ])
    
    globalreco_mod.replace(globalreco_tracking, globalreco_tracking_mod)
    print ""
    print "globalreco_mod:", globalreco_mod
    print ""
    
    
    # High level reco
    print "*"*50
    print "highlevelreco    :", process.highlevelreco
    highlevelreco_mod = process.highlevelreco.copyAndExclude([
        particleFlowReco,
        muoncosmichighlevelreco,
        muonshighlevelreco,
        jetHighLevelReco,
        metrecoPlusHCALNoise,
        btagging,
        recoPFMET,
        PFTau,
        cosmicDCTracksSeq,
    ])
    print ""
    print "highlevelreco_mod:", highlevelreco_mod
    print ""
    
    
    # Reco
    print "*"*50
    print "reconstruction    :", process.reconstruction
    
    from RecoTracker.IterativeTracking.MuonSeededStep_cff import *
    
    reconstruction_mod = process.reconstruction.copy()
    
    #reconstruction_mod.replace(localreco, localreco_mod)
    #reconstruction_mod.replace(globalreco, globalreco_mod)
    ###reconstruction_mod.replace(highlevelreco, highlevelreco_mod)
    ###reconstruction_mod.replace(highlevelreco, cms.Sequence(process.egammaHighLevelRecoPrePF))
    #reconstruction_mod.replace(highlevelreco, cms.Sequence())
    
    print ""
    print "reconstruction_mod:", reconstruction_mod
    print ""
    
    
    options.rerunTICL = 1
    options.modTICLele = 1
    options.modTICLeleWithRerunTICL = 1




from EDProducers.EnergySharedTICLmultiClusterProducer.EnergySharedTICLmultiCluster_cfi import *

#energySharingAlgo = "Mean"
energySharingAlgo = "Expo"
#energySharingAlgo = "Gaus"

#distanceType = "2Ddist"
distanceType = "3Ddist"

process.EnergySharedTICLmultiClusters = EnergySharedTICLmultiClusters.clone()
process.EnergySharedTICLmultiClusters.algoTypeStr = energySharingAlgo
process.EnergySharedTICLmultiClusters.distTypeStr = distanceType


# Rerun TICL
label_TICLtrackster = cms.untracked.InputTag("trackstersEM", "", "RECO")
label_TICLmultiCluster = cms.untracked.InputTag("multiClustersFromTrackstersEM", "MultiClustersFromTracksterByCA", "RECO")

if (options.rerunTICL) :
    
    label_TICLtrackster = cms.untracked.InputTag("trackstersEM", "", "Demo")
    label_TICLmultiCluster = cms.untracked.InputTag("multiClustersFromTrackstersEM", "MultiClustersFromTracksterByCA", "Demo")


# Mod TICL-ele
label_gsfEleFromTICL = cms.untracked.InputTag("ecalDrivenGsfElectronsFromTICL", "", "RECO")

if (options.modTICLele) :
    
    label_gsfEleFromTICL = cms.untracked.InputTag("ecalDrivenGsfElectronsFromTICL", "", "Demo")


process.treeMaker = cms.EDAnalyzer(
    "TreeMaker",
    
    ############################## My stuff  ##############################
    debug = cms.bool(False),
    
    storeSimHit = cms.bool(bool(options.storeSimHit)),
    storeRecHit = cms.bool(bool(options.storeRecHit)),
    
    
    ############################## GEN ##############################
    
    label_generator = cms.untracked.InputTag("generator"),
    label_genParticle = cms.untracked.InputTag("genParticles"),
    
    
    ############################## RECO ##############################
    
    label_pileup = cms.untracked.InputTag("addPileupInfo"),
    label_rho = cms.untracked.InputTag("fixedGridRhoFastjetAll"),
    
    label_HGCEESimHit = cms.untracked.InputTag("g4SimHits", "HGCHitsEE"),
    #label_HGCEESimHit = cms.untracked.InputTag("g4SimHits", "HGCHitsEE", "SIM"),
    
    label_HGCEERecHit = cms.untracked.InputTag("HGCalRecHit", "HGCEERecHits"),
    label_HGCHEFRecHit = cms.untracked.InputTag("HGCalRecHit", "HGCHEFRecHits"),
    label_HGCHEBRecHit = cms.untracked.InputTag("HGCalRecHit", "HGCHEBRecHits"),
    
    label_HGCALlayerCluster = cms.untracked.InputTag("hgcalLayerClusters"),
    
    #label_TICLmultiCluster = cms.untracked.InputTag("MultiClustersFromTracksters", "MultiClustersFromTracksterByCA", "RECO"),
    #label_TICLmultiCluster = cms.untracked.InputTag("EnergySharedTICLmultiClusters", "EnergySharedTICLmultiClusters%s%s" %(energySharingAlgo, distanceType)),
    
    label_TICLtrackster = label_TICLtrackster,
    label_TICLmultiCluster = label_TICLmultiCluster,
    
    label_TICLmultiClusterMIP = cms.untracked.InputTag("MultiClustersFromTrackstersMIP", "MIPMultiClustersFromTracksterByCA"),
    
    label_PFRecHit = cms.untracked.InputTag("particleFlowRecHitHGC", "Cleaned"),
    
    label_caloParticle = cms.untracked.InputTag("mix", "MergedCaloTruth"),
    
    label_gsfEleFromMultiClus = cms.untracked.InputTag("ecalDrivenGsfElectronsFromMultiCl", ""),
    
    label_gsfEleFromTICL = label_gsfEleFromTICL,
    
    ########## AK4 jet ##########
    
    #label_ak4PFjet = cms.untracked.InputTag("ak4PFJets"),
    
    
)



if (options.modTICLele) :
    
    ###from MyModules.Test.ecalDrivenGsfElectronsFromTICL_cff_orig import ecalDrivenGsfElectronsFromTICL_customizeProcess
    from MyModules.Test.ecalDrivenGsfElectronsFromTICL_cff import ecalDrivenGsfElectronsFromTICL_customizeProcess
    
    process = ecalDrivenGsfElectronsFromTICL_customizeProcess(process, onReco = (not options.onRaw))
    
    if (options.rerunTICL and options.modTICLeleWithRerunTICL) :
        
        #print label_TICLmultiCluster
        #print label_TICLmultiCluster.__dict__
        
        #process.particleFlowClusterHGCalFromTICL.initialClusteringStep.clusterSrc = label_TICLmultiCluster
        
        process.particleFlowClusterHGCalFromTICL.initialClusteringStep.clusterSrc = cms.InputTag(
            label_TICLmultiCluster.__dict__["_InputTag__moduleLabel"],
            label_TICLmultiCluster.__dict__["_InputTag__productInstance"],
            label_TICLmultiCluster.__dict__["_InputTag__processName"],
        )


########## Filters ##########

from EDFilters.MyFilters.GenParticleFilter_cfi import *

process.GenParticleFilter_ele = GenParticleFilter.clone()
process.GenParticleFilter_ele.atLeastN = cms.int32(2)
process.GenParticleFilter_ele.pdgId = cms.int32(11)
process.GenParticleFilter_ele.minEta = cms.double(1.479)
process.GenParticleFilter_ele.maxEta = cms.double(3.0)


process.filter_seq = cms.Sequence(
    process.GenParticleFilter_ele
)


process.p = cms.Path(
    process.filter_seq *
    process.treeMaker
)


# Output
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string(outFile)
)


#process.schedule = cms.Schedule()
process.schedule = cms.Schedule(
    process.p
)


# Aging
from SLHCUpgradeSimulations.Configuration.aging import customise_aging_1000
customise_aging_1000(process)


# TICL
if (options.rerunTICL) :
    
    #from RecoHGCal.TICL.ticl_iterations import TICL_iterations
    #TICL_iterations(process)
    
    from RecoHGCal.TICL.ticl_iterations import TICL_iterations_withReco
    TICL_iterations_withReco(process)


if (options.modTICLele) :
    
    process.schedule.insert(0, process.ecalDrivenGsfElectronsFromTICL_step)


if (options.onRaw) :
    
    process.raw2digi_step = cms.Path(process.RawToDigi)
    process.L1Reco_step = cms.Path(process.L1Reco)
    process.reconstruction_step = cms.Path(reconstruction_mod)
    
    
    process.schedule.insert(0, process.reconstruction_step)
    process.schedule.insert(0, process.L1Reco_step)
    process.schedule.insert(0, process.raw2digi_step)


print "\n"
print "*"*50
print "process.schedule:", process.schedule
print "*"*50
#print "process.schedule.__dict__:", process.schedule.__dict__
#print "*"*50
print "\n"


# Tracer
if (options.trace) :
    
    process.Tracer = cms.Service("Tracer")


if (options.memoryCheck) :
    
    process.SimpleMemoryCheck = cms.Service(
        "SimpleMemoryCheck",
        moduleMemorySummary = cms.untracked.bool(True),
    )


# Debug
if (options.debugFile) :
    
    process.out = cms.OutputModule("PoolOutputModule",
        fileName = cms.untracked.string("debug.root")
    )
    
    process.output_step = cms.EndPath(process.out)
    process.schedule.extend([process.output_step])


from FWCore.ParameterSet.Utilities import convertToUnscheduled
process = convertToUnscheduled(process)


# Add early deletion of temporary data products to reduce peak memory need
#from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
#process = customiseEarlyDelete(process)
# End adding early deletion
