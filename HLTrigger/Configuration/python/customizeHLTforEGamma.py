import FWCore.ParameterSet.Config as cms

def customiseEGammaEventContent(process,extraContent=[]):
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
    
    egammaEvtContent.extend(extraContent)
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

def customiseEGammaInputContent(process):
    """
    customises the E/gamma input format.
    this is necessary if we have old products in the event as when we pack things up, 
    it may pick the old products and get confused.
    so set this to only the RAW format

    """
    egammaInputContent = [
         #basically RAW sim with some more HLT drops
        'drop *',
        'keep  FEDRawDataCollection_rawDataCollector_*_*',
        'keep  FEDRawDataCollection_source_*_*',
        'keep  FEDRawDataCollection_rawDataCollector_*_*',
        'keep  FEDRawDataCollection_source_*_*',
        'drop *_hlt*_*_*',
        'keep FEDRawDataCollection_rawDataCollector_*_*',
        'keep FEDRawDataCollection_source_*_*',
        'keep GlobalObjectMapRecord_hltGtStage2ObjectMap_*_*',
        'keep edmTriggerResults_*_*_*',
        'keep triggerTriggerEvent_*_*_*',
        'keep edmTriggerResults_*_*_*',
        'keep *_g4SimHits_*_*',
        'keep edmHepMCProduct_source_*_*',
        'keep *_allTrackMCMatch_*_*',
        'keep *_prunedTrackingParticles_*_*',
        'keep *_prunedDigiSimLinks_*_*',
        'keep StripDigiSimLinkedmDetSetVector_simMuonCSCDigis_*_*',
        'keep CSCDetIdCSCComparatorDigiMuonDigiCollection_simMuonCSCDigis_*_*',
        'keep DTLayerIdDTDigiSimLinkMuonDigiCollection_simMuonDTDigis_*_*',
        'keep RPCDigiSimLinkedmDetSetVector_simMuonRPCDigis_*_*',
        'keep *_simMuonCSCDigis_*_*',
        'keep *_simMuonRPCDigis_*_*',
        'keep *_simMuonGEMDigis_*_*',
        'keep EBSrFlagsSorted_simEcalDigis_*_*',
        'keep EESrFlagsSorted_simEcalDigis_*_*',
        'keep *_simHcalUnsuppressedDigis_*_*',
        'keep CrossingFramePlaybackInfoNew_*_*_*',
        'keep PileupSummaryInfos_*_*_*',
        'keep int6stdbitsetstdpairs_*_AffectedAPVList_*',
        'keep int_*_bunchSpacing_*',
        'keep *_genPUProtons_*_*',
        'keep *_mix_MergedTrackTruth_*',
        'keep LHERunInfoProduct_*_*_*',
        'keep LHEEventProduct_*_*_*',
        'keep GenRunInfoProduct_generator_*_*',
        'keep GenLumiInfoHeader_generator_*_*',
        'keep GenLumiInfoProduct_generator_*_*',
        'keep GenEventInfoProduct_generator_*_*',
        'keep edmHepMCProduct_generatorSmeared_*_*',
        'keep edmHepMCProduct_LHCTransport_*_*',
        'keep GenFilterInfo_*_*_*',
        'keep *_genParticles_*_*',
        'keep recoGenJets_ak*_*_*',
        'keep *_ak4GenJets_*_*',
        'keep *_ak8GenJets_*_*',
        'keep *_ak4GenJetsNoNu_*_*',
        'keep *_ak8GenJetsNoNu_*_*',
        'keep *_genParticle_*_*',
        'keep recoGenMETs_*_*_*',
    ]
    process.source.inputCommands = cms.untracked.vstring(egammaInputContent)
    process.source.dropDescendantsOfDroppedBranches = cms.untracked.bool(False)
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

    extraContent = [
        "keep *_hltAK4PFJets*_*_*",
        "keep *_hltAK8PFJets*_*_*",
        "keep *_hltPFHTForMC*_*_*",
        "keep *_hltIterL3MuonCandidates_*_*",
        "keep *_hltIter0L3MuonTrackSelectionHighPurity_*_*",
        "keep *_hltIter0L3MuonTrackSelectionHighPurity_*_*",
        "keep *_hltOnlineBeamsSpot_*_*",
        "keep *_hltParticleFlowClusterECALForMuonsMF_*_*",
        "keep *_hltParticleFlowClusterECALForMuonsMF_*_*",
      #  "keep *_hltParticleFlowClusterHCAL_*_*", (very space heavy)
    
    ]

    process = customiseEGammaEventContent(process,extraContent)
    
   
    return process

