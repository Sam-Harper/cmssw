
import FWCore.ParameterSet.Config as cms

from HLTrigger.Configuration.common import *

def customizeEgPixelMatchingPhaseI(process):
    for edproducer in producers_by_type(process, "EgammaHLTFilteredSuperClusterProducer"):
        if not hasattr(edproducer,'cuts'):
            edproducer.cuts = cms.VPSet( 
                cms.PSet(
                    var = cms.InputTag("hltEgammaHoverE"),
                    barrelCut = cms.PSet(
                        cutOverE = cms.double(0.2),
                        useEt = cms.bool(False)
                        ),
                    endcapCut = cms.PSet(
                        cutOverE = cms.double(0.2),
                        useEt = cms.bool(False)
                        )
                    )
                )
            if edproducer.cands.moduleLabel.find("Unseeded")!=-1:
                edproducer.cuts[0].var=cms.InputTag("hltEgammaHoverEUnseeded")
    

    for edproducer in producers_by_type(process, "ElectronNHitSeedProducer"):
        if not hasattr(edproducer,'superClusters'):
            edproducer.superClusters=cms.VInputTag('hltEgammaSuperClustersToPixelMatch') 
            if edproducer.initialSeeds.moduleLabel.find("Unseeded")!=-1:
                edproducer.superClusters=cms.VInputTag('hltEgammaSuperClustersToPixelMatchUnseeded') 
    return process

