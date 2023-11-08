import FWCore.ParameterSet.Config as cms

eleExampleTriggerDQMSource = cms.EDProducer("EleExampleTriggerDQMSource",
                                            filterName=cms.string("hltEle32WPTightGsfTrackIsoFilter"),
                                            baseName=cms.string("elesExample"),
                                            objectSelectionVM = cms.InputTag("egmGsfElectronIDsForDQM:cutBasedElectronID-Summer16-80X-V1-tight"),
                                            objs = cms.InputTag("gedGsfElectrons"),
                                            sampleTrigRequirements = cms.PSet(
                                                hltInputTag = cms.InputTag("TriggerResults","","HLT"),
                                                hltPaths = cms.vstring("HLT_*"),
                                                stage2 = cms.bool(True),
        
                                            ),

                                            
)

muonExampleTriggerDQMSource = cms.EDProducer("MuonExampleTriggerDQMSource",
                                            filterName=cms.string("hltL3fL1sMu22Or25L1f0L2f10QL3Filtered50Q"),
                                            baseName=cms.string("musExample"),
                                            objectSelectionVM = cms.InputTag("egmDQMSelectedMuons"),
                                            objs = cms.InputTag("muons"),
                                            sampleTrigRequirements = cms.PSet(
                                                hltInputTag = cms.InputTag("TriggerResults","","HLT"),
                                                hltPaths = cms.vstring("HLT_*"),
                                                stage2 = cms.bool(True)
                                                 
                                            ),
                                            
                                            
)

from RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cff import egmGsfElectronIDs

egmGsfElectronIDsForDQM = egmGsfElectronIDs.clone()
egmGsfElectronIDsForDQM.physicsObjectsIDs = cms.VPSet()
egmGsfElectronIDsForDQM.physicsObjectSrc == cms.InputTag('gedGsfElectrons')
#note: be careful here to when selecting new ids that the vid tools dont do extra setup for them
#for example the HEEP cuts need an extra producer which vid tools automatically handles
from PhysicsTools.SelectorUtils.tools.vid_id_tools import setupVIDSelection
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
                 'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V1_cff']
for id_module_name in my_id_modules: 
    idmod= __import__(id_module_name, globals(), locals(), ['idName','cutFlow'])
    for name in dir(idmod):
        item = getattr(idmod,name)
        if hasattr(item,'idName') and hasattr(item,'cutFlow'):
            setupVIDSelection(egmGsfElectronIDsForDQM,item)


egmDQMSelectedMuons = cms.EDProducer("HLTDQMMuonSelector",
                                     objs=cms.InputTag("muons"),
                                     vertices=cms.InputTag("offlinePrimaryVertices"),
                                     selection=cms.string("pt > 20"),
                                     muonSelectionType=cms.string("tight")
                                     )
egmMuonIDSequenceForDQM = cms.Sequence(egmDQMSelectedMuons)


exampleTriggerDQMSeq = cms.Sequence(
    egmGsfElectronIDsForDQM +    
    egmMuonIDSequenceForDQM +
    eleExampleTriggerDQMSource +
    muonExampleTriggerDQMSource
)




    
    
