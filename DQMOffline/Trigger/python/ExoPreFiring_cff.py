import FWCore.ParameterSet.Config as cms

from DQMOffline.Trigger.ExoPreFiring_cfi import exoPreFireBaseParam

exoJetPreFireMonitor = cms.EDProducer("EXOJetPreFireMonitor",
                                      **exoPreFireBaseParam.clone(
                                          baseName="jetsNotPrefireFirstBunchInTrain",
                                          objs = "ak4PFJets").parameters_()
)
exoElePreFireMonitor = cms.EDProducer("EXOElePreFireMonitor",
                                      **exoPreFireBaseParam.clone(
                                          baseName="elesNotPrefireFirstBunchInTrain",
                                          objectSelectionVM = "egmGsfElectronIDsForDQM:cutBasedElectronID-RunIIIWinter22-V1-tight",
                                          objs = "gedGsfElectrons").parameters_()
)

exoMuonPreFireMonitor = cms.EDProducer("EXOMuonPreFireMonitor",
                                      **exoPreFireBaseParam.clone(
                                          baseName="muonsNotPrefireFirstBunchInTrain",
                                          objectSelectionVM = "egmDQMSelectedMuons",
                                          objs = "muons").parameters_()
)
exoPhoPreFireMonitor = cms.EDProducer("EXOPhoPreFireMonitor",
                                      **exoPreFireBaseParam.clone(
                                          baseName="phosNotPrefireFirstBunchInTrain",
                                          objectSelectionVM = "egmPhotonIDsForDQM:cutBasedPhotonID-RunIIIWinter22-122X-V1-loose",
                                          objs = "gedPhotons").parameters_()
)
exoMetPreFireMonitor = cms.EDProducer("EXOPFMETPreFireMonitor",
                                      **exoPreFireBaseParam.clone(
                                          baseName="metNotPrefireFirstBunchInTrain",
                                          objs = "pfMet").parameters_()
)


from RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cff import egmGsfElectronIDs

egmGsfElectronIDsForDQM = egmGsfElectronIDs.clone()
egmGsfElectronIDsForDQM.physicsObjectsIDs = cms.VPSet()
egmGsfElectronIDsForDQM.physicsObjectSrc == cms.InputTag('gedGsfElectrons')
#note: be careful here to when selecting new ids that the vid tools dont do extra setup for them
#for example the HEEP cuts need an extra producer which vid tools automatically handles
from PhysicsTools.SelectorUtils.tools.vid_id_tools import setupVIDSelection
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Winter22_122X_V1_cff']
for id_module_name in my_id_modules: 
    idmod= __import__(id_module_name, globals(), locals(), ['idName','cutFlow'])
    for name in dir(idmod):
        item = getattr(idmod,name)
        if hasattr(item,'idName') and hasattr(item,'cutFlow'):
            setupVIDSelection(egmGsfElectronIDsForDQM,item)


from RecoEgamma.PhotonIdentification.photonIDValueMapProducer_cff import photonIDValueMapProducer
from RecoEgamma.PhotonIdentification.egmPhotonIDs_cfi import egmPhotonIDs
photonIDValueMapProducerForDQM = photonIDValueMapProducer.clone(
    src="gedPhotons",
    vertices="offlinePrimaryVertices",
    ebReducedRecHitCollection="reducedEcalRecHitsEB",
    eeReducedRecHitCollection="reducedEcalRecHitsEE",
    esReducedRecHitCollection="reducedEcalRecHitsES",
    pfCandidates="particleFlow",
    isAOD=True,
)
egmPhotonIDsForDQM = egmPhotonIDs.clone()
#note: be careful here to when selecting new ids that the vid tools dont do extra setup for them
#for example the HEEP cuts need an extra producer which vid tools automatically handles
from PhysicsTools.SelectorUtils.tools.vid_id_tools import setupVIDSelection
my_id_modules = ['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_RunIIIWinter22_122X_V1_cff']
for id_module_name in my_id_modules: 
    idmod= __import__(id_module_name, globals(), locals(), ['idName','cutFlow'])
    for name in dir(idmod):
        item = getattr(idmod,name)
        if hasattr(item,'idName') and hasattr(item,'cutFlow'):
            setupVIDSelection(egmPhotonIDsForDQM,item)
egmPhotonIDSequenceForDQM = cms.Sequence(#photonIDValueMapProducerForDQM*
                                         egmPhotonIDsForDQM)

egmDQMSelectedMuons = cms.EDProducer("HLTDQMMuonSelector",
                                     objs=cms.InputTag("muons"),
                                     vertices=cms.InputTag("offlinePrimaryVertices"),
                                     selection=cms.string("pt > 20"),
                                     muonSelectionType=cms.string("tight")
                                     )
egmMuonIDSequenceForDQM = cms.Sequence(egmDQMSelectedMuons)

exoPreFireMonitorSeq = cms.Sequence(
    egmGsfElectronIDsForDQM + 
    egmPhotonIDSequenceForDQM + 
    egmMuonIDSequenceForDQM +
    exoJetPreFireMonitor +
    exoElePreFireMonitor +
    exoMuonPreFireMonitor +
    exoPhoPreFireMonitor +
    exoMetPreFireMonitor)

