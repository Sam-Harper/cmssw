import FWCore.ParameterSet.Config as cms


EnergySharedTICLmultiClusters = cms.EDProducer(
    "EnergySharedTICLmultiClusterProducer",
    
    instanceName = cms.string("EnergySharedTICLmultiClusters"),
    
    label_TICLmultiCluster = cms.untracked.InputTag("MultiClustersFromTracksters", "MultiClustersFromTracksterByCA"),
    
    label_HGCEERecHit = cms.untracked.InputTag("HGCalRecHit", "HGCEERecHits", "RECO"),
    label_HGCHEFRecHit = cms.untracked.InputTag("HGCalRecHit", "HGCHEFRecHits", "RECO"),
    label_HGCHEBRecHit = cms.untracked.InputTag("HGCalRecHit", "HGCHEBRecHits", "RECO"),
    
    algoTypeStr = cms.string("Expo"),
    distTypeStr = cms.string("2Ddist"),
    
    # ~ Moliere radius [in cm]
    showerSpread = cms.double(1.5),
    
    debug = cms.bool(False),
)
