import FWCore.ParameterSet.Config as cms

packedCandsForTkIso = cms.EDProducer("PATPackedCandsForTkIso",
                                     pfCands = cms.InputTag("particleFlow"),  
                                     tracks = cms.InputTag("generalTracks"),
                                     vertices = cms.InputTag("offlinePrimaryVertices"),
                                     vertAsso = cms.InputTag("primaryVertexAssociation","original"),
                                     vertAssoQual = cms.InputTag("primaryVertexAssociation","original"),
                                     secondaryVertices = cms.VInputTag(
                                        cms.InputTag("inclusiveCandidateSecondaryVertices"),
                                        cms.InputTag("inclusiveCandidateSecondaryVerticesCvsL"),
                                     ),
                                     minPtToHaveStoredTrk = cms.double(0.95),
                                     minHits = cms.int32(8),
                                     minPixelHits = cms.int32(1)
                                     )
