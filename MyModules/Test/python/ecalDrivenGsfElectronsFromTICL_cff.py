import FWCore.ParameterSet.Config as cms

from RecoParticleFlow.PFClusterProducer.particleFlowRecHitHGC_cfi import *
from RecoParticleFlow.PFClusterProducer.particleFlowClusterHGC_cfi import *
from RecoEcal.EgammaClusterProducers.particleFlowSuperClusteringSequence_cff import *
from RecoEgamma.EgammaElectronProducers.ecalDrivenElectronSeeds_cfi import *
from RecoParticleFlow.PFTracking.mergedElectronSeeds_cfi import *
from TrackingTools.GsfTracking.CkfElectronCandidateMaker_cff import *
from TrackingTools.GsfTracking.GsfElectronGsfFit_cff import *
from RecoEgamma.EgammaElectronProducers.gsfElectronCores_cfi import *
from RecoEgamma.EgammaElectronProducers.gsfElectrons_cfi import *
from RecoTracker.IterativeTracking.ElectronSeeds_cff import *
from RecoEgamma.EgammaElectronProducers.ecalDrivenGsfElectronCoresFromMultiCl_cff import *

from RecoEgamma.EgammaElectronProducers.ecalDrivenElectronSeeds_cff import *


def ecalDrivenGsfElectronsFromTICL_customizeProcess(process, onReco = False) :
    
    process.load("RecoLocalCalo.Configuration.hgcalLocalReco_cff")
    
    process.particleFlowClusterHGCalFromTICL         = particleFlowClusterHGCalFromMultiCl.clone()
    process.particleFlowSuperClusterHGCalFromTICL    = particleFlowSuperClusterHGCalFromMultiCl.clone()
    process.ecalDrivenElectronSeedsFromTICL          = ecalDrivenElectronSeedsFromMultiCl.clone()
    process.electronMergedSeedsFromTICL              = electronMergedSeedsFromMultiCl.clone()
    process.electronCkfTrackCandidatesFromTICL       = electronCkfTrackCandidatesFromMultiCl.clone()
    process.electronGsfTracksFromTICL                = electronGsfTracksFromMultiCl.clone()
    process.ecalDrivenGsfElectronCoresFromTICL       = ecalDrivenGsfElectronCoresFromMultiCl.clone()
    process.ecalDrivenGsfElectronsFromTICL           = ecalDrivenGsfElectronsFromMultiCl.clone()
    
    
    #process.particleFlowClusterHGCalFromTICL.initialClusteringStep.clusterSrc = cms.InputTag("MultiClustersFromTracksters", "MultiClustersFromTracksterByCA", "RECO")
    process.particleFlowClusterHGCalFromTICL.initialClusteringStep.clusterSrc = cms.InputTag("multiClustersFromTrackstersEM", "MultiClustersFromTracksterByCA", "RECO")
    #process.particleFlowClusterHGCalFromTICL.recHitsSource = cms.InputTag("particleFlowRecHitHGC", "Cleaned", "RECO")
    process.particleFlowSuperClusterHGCalFromTICL.PFClusters = cms.InputTag("particleFlowClusterHGCalFromTICL")
    #process.particleFlowSuperClusterHGCalFromTICL.thresh_PFClusterEndcap = cms.double(10.0)
    #process.particleFlowSuperClusterHGCalFromTICL.doSatelliteClusterMerge = cms.bool(True)
    #process.particleFlowSuperClusterHGCalFromTICL.satelliteClusterSeedThreshold = cms.double(1.0)
    process.particleFlowSuperClusterHGCalFromTICL.use_preshower = cms.bool(False)
    #process.particleFlowSuperClusterHGCalFromTICL.doSatelliteClusterMerge = cms.bool(True)
    #process.particleFlowSuperClusterHGCalFromTICL.satelliteClusterSeedThreshold = cms.double(1.0)
    #process.particleFlowSuperClusterHGCalFromTICL.satelliteMajorityFraction = cms.double(0.1)
    process.particleFlowSuperClusterHGCalFromTICL.useDynamicDPhiWindow = cms.bool(True)
    process.particleFlowSuperClusterHGCalFromTICL.useHGCalParam = cms.bool(True)
    #process.ecalDrivenElectronSeedsFromTICL.barrelSuperClusters = ""
    process.ecalDrivenElectronSeedsFromTICL.endcapSuperClusters = cms.InputTag("particleFlowSuperClusterHGCalFromTICL", "")
    process.electronMergedSeedsFromTICL.EcalBasedSeeds = cms.InputTag("ecalDrivenElectronSeedsFromTICL", "")
    process.electronMergedSeedsFromTICL.TkBasedSeeds = ""
    process.electronCkfTrackCandidatesFromTICL.src = "electronMergedSeedsFromTICL"
    process.electronGsfTracksFromTICL.src = "electronCkfTrackCandidatesFromTICL"
    process.ecalDrivenGsfElectronCoresFromTICL.gsfTracks = "electronGsfTracksFromTICL"
    process.ecalDrivenGsfElectronsFromTICL.gsfElectronCoresTag = "ecalDrivenGsfElectronCoresFromTICL"
    process.ecalDrivenGsfElectronsFromTICL.useDefaultEnergyCorrection = cms.bool(False)
    #process.ecalDrivenGsfElectronsFromTICL.ecalDrivenEcalEnergyFromClassBasedParameterization = cms.bool(False)
    #process.ecalDrivenGsfElectronsFromTICL.ecalDrivenEcalErrorFromClassBasedParameterization = cms.bool(False)
    
    
    # Thresholds
    process.particleFlowSuperClusterHGCalFromTICL.thresh_SCEt = cms.double(5.0)
    process.particleFlowSuperClusterHGCalFromTICL.seedThresholdIsET = cms.bool(True) # True is the default
    process.particleFlowSuperClusterHGCalFromTICL.thresh_PFClusterSeedEndcap = cms.double(5.0)
    process.particleFlowSuperClusterHGCalFromTICL.thresh_PFClusterEndcap = cms.double(2.0) # Cut on E (not ET)
    
    
    process.ecalDrivenGsfElectronsFromTICL_step = cms.Path(
        process.particleFlowClusterHGCalFromTICL *
        process.particleFlowSuperClusterHGCalFromTICL *
        process.ecalDrivenElectronSeedsFromTICL *
        process.electronMergedSeedsFromTICL *
        process.electronCkfTrackCandidatesFromTICL *
        process.electronGsfTracksFromTICL *
        process.ecalDrivenGsfElectronCoresFromTICL *
        process.ecalDrivenGsfElectronsFromTICL
    )
    
    #process.ecalDrivenGsfElectronsFromTICL_step = cms.Path(process.ecalDrivenGsfElectronsFromTICL_task)
    
    if (onReco) :
        
        l_task = [
            process.hgcalLocalRecoTask,
            
            process.InitialStepTask,
            process.PixelPairStepTask,
            process.HighPtTripletStepTask,
            process.LowPtTripletStepTask,
            process.DetachedQuadStepTask,
            process.LowPtQuadStepTask,
            
            process.electronSeedsSeqTask,
        ]
        
        print process.electronSeedsSeqTask
        
        for iTask in range(0, len(l_task)) :
            
            process.ecalDrivenGsfElectronsFromTICL_step.associate(l_task[iTask])
        
        
        #process.ecalDrivenGsfElectronsFromTICL_step.associate(process.hgcalLocalRecoTask)
        #
        #process.ecalDrivenGsfElectronsFromTICL_step.associate(process.InitialStepTask)
        #process.ecalDrivenGsfElectronsFromTICL_step.associate(process.PixelPairStepTask)
        #process.ecalDrivenGsfElectronsFromTICL_step.associate(process.HighPtTripletStepTask)
        #process.ecalDrivenGsfElectronsFromTICL_step.associate(process.LowPtTripletStepTask)
        #process.ecalDrivenGsfElectronsFromTICL_step.associate(process.DetachedQuadStepTask)
        #process.ecalDrivenGsfElectronsFromTICL_step.associate(process.LowPtQuadStepTask)
        #process.ecalDrivenGsfElectronsFromTICL_step.associate(process.electronSeedsSeqTask)
        
        l_module = [
            process.siPixelRecHits,
            process.siPixelClusterShapeCache,
            
            process.MeasurementTrackerEvent,
            
            process.trackerClusterCheck,
            
            #process.tripletElectronSeedLayers,
            #process.tripletElectronTrackingRegions,
            #process.tripletElectronHitDoublets,
            #process.tripletElectronHitTriplets,
            #process.tripletElectronSeeds,
            
        ]
        
        for iModule in range(0, len(l_module)) :
            
            process.ecalDrivenGsfElectronsFromTICL_step.insert(iModule, l_module[iModule])
        
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(0, process.siPixelRecHits)
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(1, process.MeasurementTrackerEvent)
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(2, process.tripletElectronSeedLayers)
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(3, process.tripletElectronTrackingRegions)
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(4, process.trackerClusterCheck)
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(5, process.tripletElectronHitDoublets)
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(6, process.tripletElectronHitTriplets)
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(7, process.tripletElectronSeeds)
        #process.ecalDrivenGsfElectronsFromTICL_step.insert(8, process.siPixelClusterShapeCache)
        
        
        
        #del process.tripletElectronSeedLayers.BPix.skipClusters
        #del process.tripletElectronSeedLayers.FPix.skipClusters
        #process.tripletElectronHitDoublets.produceSeedingHitSets = True
        
        #process.ecalDrivenElectronSeedsFromTICL.SeedConfiguration.initialSeedsVector = cms.VInputTag("tripletElectronSeeds",)
    
    return process
