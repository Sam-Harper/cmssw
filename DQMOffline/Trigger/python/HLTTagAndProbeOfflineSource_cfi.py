import FWCore.ParameterSet.Config as cms

hltEgTPOfflineSource = cms.EDAnalyzer("HLTTagAndProbeOfflineSource",
                                      histCollections = cms.VPSet(
        cms.PSet( 
            trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
            objColl = cms.InputTag("gedGsfElectrons"),
            trigResults = cms.InputTag("TriggerResults","","HLT"),
            tagVIDCuts = cms.InputTag("egmGsfElectronIDs2","cutBasedElectronID-Spring15-25ns-V1-standalone-tight"),
            probeVIDCuts = cms.InputTag("egmGsfElectronIDs2","cutBasedElectronID-Spring15-25ns-V1-standalone-tight"),
            tagTrigger = cms.string("MC_Ele5_WPLoose_Gsf_v*"),
            tagFilters = cms.vstring("hltEle5WPLooseGsfTrackIsoFilter"),
            tagRangeCuts = cms.PSet(
                cuts = cms.VPSet(
                    cms.PSet(
                        rangeVar=cms.string("scEta"),
                        allowedRanges=cms.vstring("-1.4442:1.4442"),
                        ),
                    )
                ),
           # probeFilters = cms.vstring("hltEle5WPLooseGsfTrackIsoFilter"),
            probeFilters = cms.vstring(),
            probeRangeCuts = cms.PSet(
                cuts = cms.VPSet()
                ),
            minMass = cms.double(70.0),
            maxMass = cms.double(110.0),
            requireOpSign = cms.bool(False),
            
            
            histConfigs = cms.VPSet(
                cms.PSet(
                    vsVar=cms.string("et"),
                    nameSuffex=cms.string("_vsEt"),
                    binLowEdges=cms.vdouble(5,10,15,20,25,30,35,40,45,50,60,80,100),
                    ),
                cms.PSet(
                    vsVar=cms.string("scEta"),
                    nameSuffex=cms.string("_vsSCEta"),
                    binLowEdges=cms.vdouble(-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.8,-1.566,-1.4442,-1.,-0.5,0,0.5,1,1.4442,1.566,1.8,2.0,2.1,2.2,2.3,2.4,2.5)
                    ),
                ),
                
            
            filtersToMonitor = cms.vstring("hltEle5CaloIdLGsfTrkIdVLDPhiFilter"),
            baseHistName = cms.string("Test_hltEle5CaloIdLGsfTrkIdVLDPhiFilter"),
            ),
        )
            
        )

