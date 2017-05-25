import FWCore.ParameterSet.Config as cms


etBinsLow=cms.PSet(
    vsVar=cms.string("et"),
    nameSuffex=cms.string("_vsEt"),
    binLowEdges=cms.vdouble(5,10,15,20,25,30,35,40,45,50,60,80,100),
    )
scEtaBins=cms.PSet(
    vsVar=cms.string("scEta"),
    nameSuffex=cms.string("_vsSCEta"),
    binLowEdges=cms.vdouble(-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.8,-1.566,-1.4442,-1.3,-1.,-0.5,0,0.5,1,1.3,1.4442,1.566,1.8,2.0,2.1,2.2,2.3,2.4,2.5)
    )

etRangeCut= cms.PSet(
    rangeVar=cms.string("et"),
    allowedRanges=cms.vstring("0:10000"),
    )
ecalBarrelEtaCut=cms.PSet(
    rangeVar=cms.string("scEta"),
    allowedRanges=cms.vstring("-1.4442:1.4442")
    )
ecalEndcapEtaCut=cms.PSet(
    rangeVar=cms.string("scEta"),
    allowedRanges=cms.vstring("-2.5:-1.556","1.556:2.5")
    )
hcalPosEtaCut= cms.PSet(
    rangeVar=cms.string("scEta"),
    allowedRanges=cms.vstring("1.3:1.4442","1.556:2.5"),
    )
hcalNegEtaCut= cms.PSet(
    rangeVar=cms.string("scEta"),
    allowedRanges=cms.vstring("-2.5:-1.556","-1.4442:-1.3"),
    )
hcalPhi17Cut = cms.PSet(
    rangeVar=cms.string("phi"),
    allowedRanges=cms.vstring("-0.87:-0.52"),
    )



tagAndProbeConfigEle27WPTight = cms.PSet(
    trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLTX"),
    objColl = cms.InputTag("gedGsfElectrons"),
    trigResults = cms.InputTag("TriggerResults","","HLTX"),
    tagVIDCuts = cms.InputTag("egmGsfElectronIDs2","cutBasedElectronID-Spring15-25ns-V1-standalone-tight"),
    probeVIDCuts = cms.InputTag("egmGsfElectronIDs2","cutBasedElectronID-Spring15-25ns-V1-standalone-tight"),
    tagTrigger = cms.string("HLT_Ele27_WPTight_Gsf_v*"),
    tagFilters = cms.vstring("hltEle27WPTightGsfTrackIsoFilter"),
    tagRangeCuts = cms.VPSet(
        cms.PSet(
            rangeVar=cms.string("scEta"),
            allowedRanges=cms.vstring("-1.4442:1.4442"),
            ),
        
        ),
    probeFilters = cms.vstring(),
    probeRangeCuts = cms.VPSet(
        cms.PSet(
            rangeVar=cms.string("scEta"),
            allowedRanges=cms.vstring("-1.4442:1.4442","-2.5:-1.556","1.556:2.5"),
            ),
        
        ),
    minMass = cms.double(70.0),
    maxMass = cms.double(110.0),
    requireOpSign = cms.bool(False),
    ) 


tagAndProbeConfigEle27WPTightHEP17 = tagAndProbeConfigEle27WPTight.clone( 
    probeRangeCuts = cms.VPSet(
        hcalPosEtaCut,
        hcalPhi17Cut,
))
tagAndProbeConfigEle27WPTightHEM17 = tagAndProbeConfigEle27WPTight.clone( 
    probeRangeCuts = cms.VPSet(
        hcalNegEtaCut,
        hcalPhi17Cut,
))
    

egammaStdHistConfigs = cms.VPSet(
    cms.PSet(
        histType=cms.string("1D"),
        vsVar=cms.string("et"),
        nameSuffex=cms.string("_EB_vsEt"),
        rangeCuts=cms.VPSet(ecalBarrelEtaCut),
        binLowEdges=cms.vdouble(5,10,15,20,25,30,35,40,45,50,60,80,100,150,200,250,300,350,400),
        ),
    cms.PSet(
        histType=cms.string("1D"),
        vsVar=cms.string("et"),
        nameSuffex=cms.string("_EE_vsEt"),
        rangeCuts=cms.VPSet(ecalEndcapEtaCut),
        binLowEdges=cms.vdouble(5,10,15,20,25,30,35,40,45,50,60,80,100,150,200,250,300,350,400),
        ),
    cms.PSet(
        histType=cms.string("1D"),
        vsVar=cms.string("scEta"),
        nameSuffex=cms.string("_vsSCEta"),
        rangeCuts=cms.VPSet(),
        binLowEdges=cms.vdouble(-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.8,-1.566,-1.4442,-1.3,-1.,-0.5,0,0.5,1,1.3,1.4442,1.566,1.8,2.0,2.1,2.2,2.3,2.4,2.5)
        ),
    cms.PSet(
        histType=cms.string("1D"),
        vsVar=cms.string("phi"),
        nameSuffex=cms.string("_EB_vsPhi"),
        rangeCuts=cms.VPSet(ecalBarrelEtaCut),
        binLowEdges=cms.vdouble(-3.15,-2.5,-2.0,-1.5,-1.0,-0.5,0,0.5,1.0,1.5,2.0,2.5,3.15)
        ),
    cms.PSet(
        histType=cms.string("1D"),
        vsVar=cms.string("phi"),
        nameSuffex=cms.string("_EE_vsPhi"),
        rangeCuts=cms.VPSet(ecalEndcapEtaCut),
        binLowEdges=cms.vdouble(-3.15,-2.5,-2.0,-1.5,-1.0,-0.5,0,0.5,1.0,1.5,2.0,2.5,3.15)
        ),
    cms.PSet(
        histType=cms.string("2D"),
        xVar=cms.string("scEta"),
        yVar=cms.string("phi"),
        nameSuffex=cms.string("_vsSCEtaPhi"), 
        rangeCuts=cms.VPSet(),
        xBinLowEdges=cms.vdouble(-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.8,-1.566,-1.4442,-1.3,-1.,-0.5,0,0.5,1,1.3,1.4442,1.566,1.8,2.0,2.1,2.2,2.3,2.4,2.5),
        yBinLowEdges=cms.vdouble(-3.15,-2.5,-2.0,-1.5,-1.0,-0.87,-0.52,0,0.52,0.87,1.0,1.5,2.0,2.5,3.15)
        ),
    
    )

egammaTestFiltersToMonitor = cms.VPSet(
    cms.PSet(
        folderName = cms.string("HLT/TagAndProbeEffs/HLT_Ele5_WPLoose_Gsf"),
        rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("5:99999")),),
        filterName = cms.string("hltEle5CaloIdLGsfTrkIdVLDPhiFilter"),
        histTitle = cms.string("hltEle5CaloIdLGsfTrkIdVLDPhiFilter"),
        tagExtraFilter = cms.string(""),
        ),
    )
egammaStdFiltersToMonitor= cms.VPSet(
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_DoubleEle33_CaloIdL_MW"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("40:99999")),),
         filterName = cms.string("hltEle33CaloIdLMWPMS2Filter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_DoubleEle33_CaloIdL_MW"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("40:99999")),),
         filterName = cms.string("hltDiEle33CaloIdLMWPMS2UnseededFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string("hltEle33CaloIdLMWPMS2Filter"),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Photon300_NoHE"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("320:99999")),),
         filterName = cms.string("hltEG300erFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_DoublePhoton70"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("80:99999")),),
         filterName = cms.string("hltEG70HEFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_DoublePhoton70"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("80:99999")),),
         filterName = cms.string("hltDiEG70HEUnseededFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string("hltEG70HEFilter"),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_DoublePhoton85"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("90:99999")),),
         filterName = cms.string("hltEG85HEFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_DoublePhoton85"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("80:99999")),),
         filterName = cms.string("hltDiEG85HEUnseededFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string("hltEG85HEFilter"),
         ),

     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_DiSC30_18_EIso_AND_HE_Mass70"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("35:99999")),),
         filterName = cms.string("hltEG30EIso15HE30EcalIsoLastFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_DiSC30_18_EIso_AND_HE_Mass70"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("35:99999")),),
         filterName = cms.string("hltEG18EIso15HE30EcalIsoUnseededFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string("hltEG30EIso15HE30EcalIsoLastFilter"),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("26:99999")),),
         filterName = cms.string("hltEle23Ele12CaloIdLTrackIdLIsoVLTrackIsoLeg1Filter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
      cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("15:99999")),),
         filterName = cms.string("hltEle23Ele12CaloIdLTrackIdLIsoVLTrackIsoLeg2Filter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Ele27_WPTight_Gsf"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("30:99999")),),
         filterName = cms.string("hltEle27WPTightGsfTrackIsoFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
      cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Ele32_WPTight_Gsf"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("35:99999")),),
         filterName = cms.string("hltEle32noerWPTightGsfTrackIsoFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Ele38_WPTight_Gsf"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("42:99999")),),
         filterName = cms.string("hltEle38noerWPTightGsfTrackIsoFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Ele40_WPTight_Gsf"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("45:99999")),),
         filterName = cms.string("hltEle40noerWPTightGsfTrackIsoFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Photon33"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("35:99999")),),
         filterName = cms.string("hltEG33L1EG26HEFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),
     cms.PSet(
         folderName = cms.string("HLT/EGTagAndProbeEffs/HLT_Photon5"),
         rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("35:99999")),),
         filterName = cms.string("hltEG33L1EG26HEFilter"),
         histTitle = cms.string(""),
         tagExtraFilter = cms.string(""),
         ),

     )
  
 

hltEgTPOfflineSource = cms.EDAnalyzer("HLTTagAndProbeOfflineSource",
                                      histCollections = cms.VPSet(
        cms.PSet( 
            tagAndProbeConfigEle27WPTight,
            histConfigs = egammaStdHistConfigs,
            baseHistName = cms.string("ele27Tag_"),
            histCollConfigs = egammaStdFiltersToMonitor,
        ),
        cms.PSet(
            tagAndProbeConfigEle27WPTightHEM17,
            histConfigs = egammaStdHistConfigs,
            baseHistName = cms.string("ele27Tag_HEM17_"),
             histCollConfigs = egammaStdFiltersToMonitor,
        ),
        cms.PSet(
            tagAndProbeConfigEle27WPTightHEP17,
            histConfigs = egammaStdHistConfigs,
            baseHistName = cms.string("ele27Tag_HEP17_"),
            histCollConfigs = egammaStdFiltersToMonitor,
        ),
           
        )
                                      )

