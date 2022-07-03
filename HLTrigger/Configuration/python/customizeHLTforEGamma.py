import FWCore.ParameterSet.Config as cms

def customiseEGammaEventContent(process):
    """
    this loads the hltEgammaHLTExtra module and adds it to all
    EndPaths containing a PoolOutputModule and adds the Egamma event 
    content to that output modules event content
    if no suitable output module exists, it adds one
    """
    process.load("RecoEgamma.EgammaHLTProducers.hltEgammaHLTExtra_cfi")
    egammaEvtContent = [
        'keep *_hltGtStage2ObjectMap_*_*',
        'keep edmTriggerResults_*_*_*',
        'keep triggerTriggerEvent_*_*_*',
        'keep recoRecoEcalCandidates*_*_*_*',
        'keep recoSuperClusters_*_*_*',
        'keep recoCaloClusters_*_*_*',
        'keep *_genParticles_*_*',
        'keep *_addPileupInfo_*_*',
        'keep *_externalLHEProducer_*_*',
        'keep *_generator_*_*',
        'keep *_hltEgammaGsfTracks*_*_*',
        'keep recoElectronSeeds_*_*_*',
        'keep *_hltEgammaHLTExtra_*_*',
        'keep *_hltNrInputEvents_*_*',
        'keep *_hltGtStage2Digis_*_*'
    ]
    addedEvtContent = False
    for outmodname in process.outputModules_():
        outmod = process.outputModules_()[outmodname]
        if outmod.type_()=='PoolOutputModule':
            outmod.outputCommands.extend(egammaEvtContent)
            addedEvtContent = True

            

    if not addedEvtContent:
         process.egOutMod = cms.OutputModule( "PoolOutputModule",
                                              fileName = cms.untracked.string( "output.root" ),
                                              outputCommands = cms.untracked.vstring('drop *')                                              
                                           )   
         process.egOutMod.outputCommands.extend(egammaEvtContent)
         process.hltEgHLTOut = cms.FinalPath(process.egOutMod)
         if hasattr(process,"schedule") and process.schedule:
             process.schedule.append(process.hltEgHLTOut)
    
    process.EgHLTExtraPath = cms.Path(process.hltEgammaHLTExtra)
    if hasattr(process,"schedule") and process.schedule:
             process.schedule.append(process.EgHLTExtraPath)

    return process

def customiseEGammaMenuDev(process):
    """
    Customise the HLT for E/gamma menu development
    It adds the E/gamma event content and deletes the DQM output
    """

    process.load( "DQMServices.Core.DQMStore_cfi" ) 
    for attrToDel in ["dqmOutput","DQMOutput"]:
        if hasattr(process,attrToDel):
            delattr(process,attrToDel)

    process = customiseEGammaEventContent(process)
   
    return process

