import FWCore.ParameterSet.Config as cms

# this is a config to test the HLT validation plugin
# to learn how this works, a few "test" histograms are made for egamma

# defining a few variable sets
ecalBarrelEtaCut=cms.PSet(
    rangeVar=cms.string("scEta"),
    allowedRanges=cms.vstring("-1.4442:1.4442")
)
ecalBarrelAndEndcapEtaCut = cms.PSet(
    rangeVar=cms.string("scEta"),
    allowedRanges=cms.vstring("-1.4442:1.4442","-2.5:-1.556","1.556:2.5"),
)


# defining TnP ele27
tagAndProbeConfigEle27WPTight = cms.PSet(
    trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    tagColl = cms.InputTag("gedGsfElectrons"),
    probeColl = cms.InputTag("gedGsfElectrons"),
    tagVIDCuts = cms.InputTag("egmGsfElectronIDsForDQM:cutBasedElectronID-Summer16-80X-V1-tight"),
    probeVIDCuts = cms.InputTag("egmGsfElectronIDsForDQM:cutBasedElectronID-Summer16-80X-V1-tight"),
    sampleTrigRequirements = cms.PSet(
        hltInputTag = cms.InputTag("TriggerResults","","HLT"),
        hltPaths = cms.vstring("HLT_Ele27_WPTight_Gsf_v*","HLT_Ele32_WPTight_Gsf_v*","HLT_Ele35_WPTight_Gsf_v*"
                               "HLT_Ele38_WPTight_Gsf_v*",
                               "HLT_Ele27_WPTight_Gsf_L1DoubleEG_v*","HLT_Ele32_WPTight_Gsf_L1DoubleEG_v*",
                               )
        ),
    #it is intended that these are the filters of the triggers listed for sampleTrigRequirements
    tagFilters = cms.vstring("hltEle27WPTightGsfTrackIsoFilter",
                             "hltEle32WPTightGsfTrackIsoFilter"
                             "hltEle35noerWPTightGsfTrackIsoFilter"
                             "hltEle38noerWPTightGsfTrackIsoFilter"
                             "hltEle27L1DoubleEGWPTightGsfTrackIsoFilter",
                             "hltEle32L1DoubleEGWPTightGsfTrackIsoFilter" ),
    tagFiltersORed = cms.bool(True),
    tagRangeCuts = cms.VPSet(ecalBarrelEtaCut),
    probeFilters = cms.vstring(),
    probeFiltersORed = cms.bool(False),
    probeRangeCuts = cms.VPSet(ecalBarrelAndEndcapEtaCut),
    minTagProbeDR = cms.double(0),
    minMass = cms.double(70.0),
    maxMass = cms.double(110.0),
    requireOpSign = cms.bool(False),
)


# hists to create
testHistConfigs = cms.VPSet(
    cms.PSet(
        histType=cms.string("1D"),
        vsVar=cms.string("et"),
        nameSuffex=cms.string("_EBvsEt"),
        rangeCuts=cms.VPSet(ecalBarrelEtaCut),
        binLowEdges=etBinsStd,
    ),
)

# filters to monitor
testFiltersToMonitor= cms.VPSet(
    # starting with single ele as simple test filter
    cms.PSet(
        folderName = cms.string("HLT/EGM/TagAndProbeEffs/HLT_Ele27_WPTight_Gsf"),
        rangeCuts = cms.VPSet(etRangeCut.clone(allowedRanges=cms.vstring("30:99999")),),
        filterName = cms.string("hltEle27WPTightGsfTrackIsoFilter"),
        histTitle = cms.string(""),
        tagExtraFilter = cms.string(""),
    ),
)

# set up producers
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
HLTGenLevelValSource = DQMEDAnalyzer("HLTGenLevelValSource",
    tagAndProbeCollections = cms.VPSet(
        cms.PSet(
            tagAndProbeConfigEle27WPTight,
            histConfigs = testHistConfigs,
            baseHistName = cms.string("eleWPTightTag_"),
            filterConfigs = testFiltersToMonitor,
        ),
    )
)
