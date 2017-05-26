import FWCore.ParameterSet.Config as cms

from DQMOffline.Trigger.HLTTagAndProbeOfflineSource_cfi import *

egammaMonitorHLT = cms.Sequence(
    egHLTDQMSelection*
    egHLTDQMOfflineTPSource
)
