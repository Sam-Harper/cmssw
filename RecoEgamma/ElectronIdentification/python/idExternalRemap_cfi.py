import FWCore.ParameterSet.Config as cms

eidLoose = cms.EDProducer('ElectronIDExternalProducerRemapper',
    candidateProducer = cms.InputTag('gedGsfElectronsGSFixed'),
    newToOldObjectMap = cms.InputTag('gedGsfElectronsGSFixed'),
    idMap = cms.InputTag('eidLoose', '', cms.InputTag.skipCurrentProcess())
)

eidRobustHighEnergy = cms.EDProducer('ElectronIDExternalProducerRemapper',
    candidateProducer = cms.InputTag('gedGsfElectronsGSFixed'),
    newToOldObjectMap = cms.InputTag('gedGsfElectronsGSFixed'),
    idMap = cms.InputTag('eidRobustHighEnergy', '', cms.InputTag.skipCurrentProcess())
)

eidRobustLoose = cms.EDProducer('ElectronIDExternalProducerRemapper',
    candidateProducer = cms.InputTag('gedGsfElectronsGSFixed'),
    newToOldObjectMap = cms.InputTag('gedGsfElectronsGSFixed'),
    idMap = cms.InputTag('eidRobustLoose', '', cms.InputTag.skipCurrentProcess())
)

eidRobustTight = cms.EDProducer('ElectronIDExternalProducerRemapper',
    candidateProducer = cms.InputTag('gedGsfElectronsGSFixed'),
    newToOldObjectMap = cms.InputTag('gedGsfElectronsGSFixed'),
    idMap = cms.InputTag('eidRobustTight', '', cms.InputTag.skipCurrentProcess())
)

eidTight = cms.EDProducer('ElectronIDExternalProducerRemapper',
    candidateProducer = cms.InputTag('gedGsfElectronsGSFixed'),
    newToOldObjectMap = cms.InputTag('gedGsfElectronsGSFixed'),
    idMap = cms.InputTag('eidTight', '', cms.InputTag.skipCurrentProcess())
)

PhotonCutBasedIDLoose = cms.EDProducer('PhotonIDExternalProducerRemapper',
    candidateProducer = cms.InputTag('gedPhotonsGSFixed'),
    newToOldObjectMap = cms.InputTag('gedPhotonsGSFixed'),
    idMap = cms.InputTag('PhotonIDProdGED', 'PhotonCutBasedIDLoose', cms.InputTag.skipCurrentProcess())
)

PhotonCutBasedIDLooseEM = cms.EDProducer('PhotonIDExternalProducerRemapper',
    candidateProducer = cms.InputTag('gedPhotonsGSFixed'),
    newToOldObjectMap = cms.InputTag('gedPhotonsGSFixed'),
    idMap = cms.InputTag('PhotonIDProdGED', 'PhotonCutBasedIDLooseEM', cms.InputTag.skipCurrentProcess())
)

PhotonCutBasedIDTight = cms.EDProducer('PhotonIDExternalProducerRemapper',
    candidateProducer = cms.InputTag('gedPhotonsGSFixed'),
    newToOldObjectMap = cms.InputTag('gedPhotonsGSFixed'),
    idMap = cms.InputTag('PhotonIDProdGED', 'PhotonCutBasedIDTight', cms.InputTag.skipCurrentProcess())
)

ElectronIDExternalProducerRemapSequence = cms.Sequence(
    eidLoose + 
    eidRobustHighEnergy + 
    eidRobustLoose + 
    eidRobustTight + 
    eidTight
)

PhotonIDExternalProducerRemapSequence = cms.Sequence(
    PhotonCutBasedIDLoose +
    PhotonCutBasedIDLooseEM +
    PhotonCutBasedIDTight
)
