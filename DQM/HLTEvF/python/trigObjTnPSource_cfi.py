import FWCore.ParameterSet.Config as cms

_ecalBarrelEtaCut = cms.PSet(
    rangeVar = cms.string("eta"),
    allowedRanges=cms.vstring("-1.4442:1.4442")
    )

_ecalEtaCut = cms.PSet(
    rangeVar = cms.string("eta"),
    allowedRanges=cms.vstring("-2.5:-1.556","-1.4442:1.4442","1.556:2.5")
    )


trigObjTnPSource = cms.EDProducer('TrigObjTnPSource',
  triggerEvent = cms.InputTag('hltTriggerSummaryAOD','','HLT'),
  histColls = cms.VPSet(cms.PSet(
    tagCuts = cms.VPSet(_ecalBarrelEtaCut),
    probeCuts = cms.VPSet(_ecalEtaCut),
    tagFilters = cms.PSet(
      filterSets = cms.VPSet(
        cms.PSet( 
           filters = cms.vstring(
             "hltEle32WPTightGsfTrackIsoFilter"
           ),
           isAND = cms.bool(False)
        ),
      ),
      isAND = cms.bool(False)
    ),
    collName = cms.string("stdTag"),
    folderName = cms.string("HLT/EGM/TrigObjTnP"),
    evtTrigSel = cms.PSet(
      hltInputTag = cms.InputTag('TriggerResults','','HLT'),
      hltPaths = cms.vstring("HLT_Ele32_WPTight_Gsf_v*")
    ),
    histBins = cms.PSet(
      pt = cms.vdouble(10,20,30,60,100),
      eta = cms.vdouble(-2.5,-1.5,0,1.5,2.5),
      phi = cms.vdouble(-3.14,-1.57,0,1.57,3.14),
      mass = cms.vdouble(i for i in range(60,120+1))
    ),
    probeFilters = cms.vstring("hltEG32L1SingleEGOrEtFilter",
                               "hltEle32WPTightClusterShapeFilter",
                               "hltEle32WPTightHEFilter",
                               "hltEle32WPTightEcalIsoFilter",
                               "hltEle32WPTightHcalIsoFilter",
                               "hltEle32WPTightPixelMatchFilter",
                               "hltEle32WPTightPMS2Filter",
                               "hltEle32WPTightGsfOneOEMinusOneOPFilter",
                               "hltEle32WPTightGsfMissingHitsFilter",
                               "hltEle32WPTightGsfDetaFilter",
                               "hltEle32WPTightGsfDphiFilter",
                               "hltEle32WPTightGsfTrackIsoFilter")           
  ))
)
