
import FWCore.ParameterSet.Config as cms

def readLowPtEleRegresFromDBFile(process,filename):
    from CondCore.CondDB.CondDB_cfi import CondDB
    CondDBReg = CondDB.clone(connect = 'sqlite_file:{}'.format(filename))
    process.lowPtEleRegres = cms.ESSource("PoolDBESSource",CondDBReg,
                                          DumpStat=cms.untracked.bool(False),
                                          toGet = cms.VPSet(
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string('lowPtElectron_EB_ECALOnly'),
                     tag = cms.string("lowPtElectron_EB_ECALOnly")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string('lowPtElectron_EB_ECALOnly_sigma'),
                     tag = cms.string("lowPtElectron_EB_ECALOnly_sigma")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string('lowPtElectron_EE_ECALOnly'),
                     tag = cms.string("lowPtElectron_EE_ECALOnly")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string('lowPtElectron_EE_ECALOnly_sigma'),
                     tag = cms.string("lowPtElectron_EE_ECALOnly_sigma")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string("lowPtElectron_EB_ECALTrk"),
                     tag = cms.string("lowPtElectron_EB_ECALTrk")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string("lowPtElectron_EB_ECALTrk_sigma"),
                     tag = cms.string("lowPtElectron_EB_ECALTrk_sigma")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string("lowPtElectron_EE_ECALTrk"),
                     tag = cms.string("lowPtElectron_EE_ECALTrk")),
            cms.PSet(record = cms.string("GBRDWrapperRcd"),
                     label = cms.untracked.string("lowPtElectron_EE_ECALTrk_sigma"),
                     tag = cms.string("lowPtElectron_EE_ECALTrk_sigma")),
            )
                                          )
    
    process.es_prefer_lowPtEleRegres = cms.ESPrefer("PoolDBESSource","lowPtEleRegres")
    return process
