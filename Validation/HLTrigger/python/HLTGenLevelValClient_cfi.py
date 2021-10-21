import FWCore.ParameterSet.Config as cms

# filters to monitor
testFiltersToMonitor= cms.PSet(
    cms.PSet(
        filterNames = cms.vstring("hltEle27WPTightGsfTrackIsoFilter"),
    ),
)

from DQMServices.Core.DQMEDHarvester import DQMEDHarvester
HLTGenLevelValClient = DQMEDHarvester("HLTGenLevelValClient",
                        testFiltersToMonitor,
                        DQMDirName=cms.string("HLT/TEST/"),
                        hltTag = cms.string("HLT"),
                        runClientEndLumiBlock=cms.bool(False),
                        runClientEndRun=cms.bool(True),
                        runClientEndJob=cms.bool(False),
                        effVars=cms.vstring('dEtaIn','dPhiIn','hOverE','sigmaIEtaIEta', 'hltIsolEm','hltIsolHad','hltIsoltrksEle'),
)
