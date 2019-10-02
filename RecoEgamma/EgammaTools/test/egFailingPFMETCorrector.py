

import FWCore.ParameterSet.Config as cms
import os
import sys
# set up process
process = cms.Process("EGAMMA")

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('analysis') 
options.register('era','2017-Nov17ReReco',options.multiplicity.singleton,options.varType.string," ")
options.register('isMC',False,options.multiplicity.singleton,options.varType.bool," ")
options.parseArguments()

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)
# set the number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),  
                          )

process.corrEgammaBadPF = cms.EDProducer("EGMiniAODType1METCorrProducer",
                                         eles=cms.InputTag("slimmedElectrons"),
                                         jets=cms.InputTag("slimmedJets"),
                                         correctorCfg = cms.PSet(
        minJetPt = cms.double(15),
        maxJetEMFrac = cms.double(0.9)
        )
)


process.slimmedMetsEGFixed = cms.EDProducer("CorrectedPatMETProducer",
    src = cms.InputTag("slimmedMETs"),
    srcCorrections = cms.VInputTag(cms.InputTag("corrEgammaBadPF"))
)


process.p = cms.Path( process.corrEgammaBadPF*process.slimmedMetsEGFixed )


process.egammaOutput = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(4),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('AODSIM'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
    fileName = cms.untracked.string(options.outputFile.replace(".root","_EDM.root")),
    outputCommands = cms.untracked.vstring('drop *',
                                           "keep *_*_*_RECO",
                                           "keep *_*_*_PAT",
                                           'keep *_*_*_HLT',
                                           'keep *_slimmedElectrons*_*_*',
                                           'keep *_slimmedPhotons*_*_*',
                                           'keep *')
                                        )

process.outPath = cms.EndPath(process.egammaOutput)


