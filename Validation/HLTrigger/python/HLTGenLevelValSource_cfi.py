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

etBinsStd=cms.vdouble(5,10,12.5,15,17.5,20,22.5,25,30,35,40,45,50,60,80,100,150,200,250,300,350,400)
etRangeCut= cms.PSet(
    rangeVar=cms.string("et"),
    allowedRanges=cms.vstring("0:10000"),
    )


# defining a test config
testconfig_Ele27WPTight = cms.PSet(
    trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    coll = cms.InputTag("gedGsfElectrons"),
    sampleTrigRequirements = cms.PSet(
        hltInputTag = cms.InputTag("TriggerResults","","HLT"),
        hltPaths = cms.vstring("HLT_Ele27_WPTight_Gsf_v*","HLT_Ele32_WPTight_Gsf_v*","HLT_Ele35_WPTight_Gsf_v*"
                               "HLT_Ele38_WPTight_Gsf_v*","HLT_Ele27_WPTight_Gsf_L1DoubleEG_v*","HLT_Ele32_WPTight_Gsf_L1DoubleEG_v*",
                               )
        ),
    #it is intended that these are the filters of the triggers listed for sampleTrigRequirements
    filters = cms.vstring("hltEle27WPTightGsfTrackIsoFilter",
                             "hltEle32WPTightGsfTrackIsoFilter"
                             "hltEle35noerWPTightGsfTrackIsoFilter"
                             "hltEle38noerWPTightGsfTrackIsoFilter"
                             "hltEle27L1DoubleEGWPTightGsfTrackIsoFilter",
                             "hltEle32L1DoubleEGWPTightGsfTrackIsoFilter" )
                             )


# hists to create
testHistConfigs = cms.VPSet(
    cms.PSet(
        histType=cms.string("1D"),
        vsVar=cms.string("et"),
        nameSuffex=cms.string("_EBvsEt"),
        binLowEdges=etBinsStd,
    ),
)

# filters to monitor
testFiltersToMonitor= cms.VPSet(
    # starting with single ele as simple test filter
    cms.PSet(
        folderName = cms.string("HLT/TEST/"),
        filterName = cms.string("hltEle27WPTightGsfTrackIsoFilter"),
        histTitle = cms.string(""),
    ),
)

# set up producers
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
HLTEleGenLevelValSource = DQMEDAnalyzer("HLTEleGenLevelValSource",
    collections = cms.VPSet(
        cms.PSet(
            testconfig_Ele27WPTight,
            histConfigs = testHistConfigs,
            baseHistName = cms.string("eleWPTightTag_"),
            filterConfigs = testFiltersToMonitor,
        ),
    )
)
