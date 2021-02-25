import FWCore.ParameterSet.Config as cms

l1emulator = cms.Sequence()

from SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff import *
from CalibCalorimetry.CaloTPG.CaloTPGTranscoder_cfi import *

from L1Trigger.L1THGCal.hgcalTriggerPrimitives_cff import *
l1emulator += hgcalTriggerPrimitives

from SimCalorimetry.EcalEBTrigPrimProducers.ecalEBTriggerPrimitiveDigis_cff import *
l1emulator += simEcalEBTriggerPrimitiveDigis

from L1Trigger.TrackFindingTracklet.Tracklet_cfi import *
L1TRK_NAME  = "TTTracksFromTrackletEmulation"
L1TRK_LABEL = "Level1TTTracks"

from RecoVertex.BeamSpotProducer.BeamSpot_cfi import *
l1emulator += offlineBeamSpot

l1emulator += TTTracksFromTrackletEmulation

from SimTracker.TrackTriggerAssociation.TrackTriggerAssociator_cff import *
TTTrackAssociatorFromPixelDigis.TTTracks = cms.VInputTag( cms.InputTag(L1TRK_NAME, L1TRK_LABEL) )
l1emulator += TrackTriggerAssociatorTracks

from L1Trigger.L1TTrackMatch.L1TkPrimaryVertexProducer_cfi import *
l1emulator += L1TkPrimaryVertex

from Configuration.StandardSequences.SimL1Emulator_cff import *
l1emulator += SimL1Emulator

from L1Trigger.Phase2L1ParticleFlow.pfTracksFromL1Tracks_cfi import *
l1emulator += pfTracksFromL1Tracks

from L1Trigger.Phase2L1ParticleFlow.l1ParticleFlow_cff import *
l1emulator += l1ParticleFlow

from L1Trigger.Phase2L1ParticleFlow.l1pfJetMet_cff import *
l1emulator += l1PFJets

kt6L1PFJetsPF = ak4PFL1PF.clone(
    jetAlgorithm = cms.string("Kt"),
    rParam       = cms.double(0.6),
    doRhoFastjet = cms.bool(True),
    Rho_EtaMax   = cms.double(3.0)
)
l1emulator += kt6L1PFJetsPF
l1pfNeutralCandidatesPF = cms.EDFilter("L1TPFCandSelector",
    src = cms.InputTag('l1pfCandidates:PF'),
    cut = cms.string("pdgId = 22"), # CV: cms.string("id = Photon") does not work (does not select any l1t::PFCandidates)                                                                                                                                                       
    filter = cms.bool(False)
)
l1emulator += l1pfNeutralCandidatesPF
kt6L1PFJetsNeutralsPF = kt6L1PFJetsPF.clone(
    src = cms.InputTag('l1pfNeutralCandidatesPF')
)
l1emulator += kt6L1PFJetsNeutralsPF

kt6L1PFJetsPuppi = kt6L1PFJetsPF.clone(
    src = cms.InputTag('l1pfCandidates:Puppi')
)
l1emulator += kt6L1PFJetsPuppi
l1pfNeutralCandidatesPuppi = l1pfNeutralCandidatesPF.clone(
    src = cms.InputTag('l1pfCandidates:Puppi'),
)
l1emulator += l1pfNeutralCandidatesPuppi
kt6L1PFJetsNeutralsPuppi = kt6L1PFJetsPuppi.clone(
    src = cms.InputTag('l1pfNeutralCandidatesPuppi')
)
l1emulator += kt6L1PFJetsNeutralsPuppi






