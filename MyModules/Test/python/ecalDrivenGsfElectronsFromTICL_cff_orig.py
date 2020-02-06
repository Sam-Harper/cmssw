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
    
    
    process.particleFlowClusterHGCalFromTICL.initialClusteringStep.clusterSrc = cms.InputTag("MultiClustersFromTracksters", "MultiClustersFromTracksterByCA", "RECO")
    #process.particleFlowClusterHGCalFromTICL.recHitsSource = cms.InputTag("particleFlowRecHitHGC", "Cleaned", "RECO")
    process.particleFlowSuperClusterHGCalFromTICL.PFClusters = cms.InputTag("particleFlowClusterHGCalFromTICL")
    process.particleFlowSuperClusterHGCalFromTICL.use_preshower = cms.bool(False)
    process.ecalDrivenElectronSeedsFromTICL.endcapSuperClusters = "particleFlowSuperClusterHGCalFromTICL"
    process.electronMergedSeedsFromTICL.EcalBasedSeeds = "ecalDrivenElectronSeedsFromTICL"
    process.electronCkfTrackCandidatesFromTICL.src = "electronMergedSeedsFromTICL"
    process.electronGsfTracksFromTICL.src = "electronCkfTrackCandidatesFromTICL"
    process.ecalDrivenGsfElectronCoresFromTICL.gsfTracks = "electronGsfTracksFromTICL"
    process.ecalDrivenGsfElectronsFromTICL.gsfElectronCoresTag = "ecalDrivenGsfElectronCoresFromTICL"
    
    
    
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
        
        process.ecalDrivenGsfElectronsFromTICL_step.associate(process.hgcalLocalRecoTask)
        
        process.ecalDrivenGsfElectronsFromTICL_step.insert(0, process.siPixelRecHits)
        process.ecalDrivenGsfElectronsFromTICL_step.insert(1, process.MeasurementTrackerEvent)
        process.ecalDrivenGsfElectronsFromTICL_step.insert(2, process.tripletElectronSeedLayers)
        process.ecalDrivenGsfElectronsFromTICL_step.insert(3, process.tripletElectronTrackingRegions)
        process.ecalDrivenGsfElectronsFromTICL_step.insert(4, process.trackerClusterCheck)
        process.ecalDrivenGsfElectronsFromTICL_step.insert(5, process.tripletElectronHitDoublets)
        process.ecalDrivenGsfElectronsFromTICL_step.insert(6, process.tripletElectronHitTriplets)
        process.ecalDrivenGsfElectronsFromTICL_step.insert(7, process.tripletElectronSeeds)

        del process.tripletElectronSeedLayers.BPix.skipClusters
        del process.tripletElectronSeedLayers.FPix.skipClusters
        #process.tripletElectronHitDoublets.produceSeedingHitSets = True
        
        process.ecalDrivenElectronSeedsFromTICL.SeedConfiguration.initialSeedsVector = cms.VInputTag("tripletElectronSeeds",)
    
    return process
