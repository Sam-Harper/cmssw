#!/usr/bin/env python3
import os
import sys
import multiprocessing

def execmd(cmd):
    return os.system(cmd)

if __name__ == '__main__':

    ###
    ### parameters
    ###
    minRunNumber = None
    maxRunNumber = None
    numEventsPerJob = -1

    numThreadsPerJobs = 20
    numStreamsPerJobs = 20

    eosDirs = [f'/eos/cms/store/data/Run2025C/EphemeralHLTPhysics{foo}/RAW/v1/000/393/240/00000' for foo in range(8)]

    hltMenu = '/dev/CMSSW_15_0_0/GRun/V97'

    hltLabel = sys.argv[1]

    def hltGetCmd(hltMenu, hltLabel):
        return f"""
https_proxy=http://cmsproxy.cms:3128/ \
hltGetConfiguration {hltMenu} \
  --process HLTX \
  --globaltag 150X_dataRun3_HLT_v1 \
  --data \
  --no-prescale \
  --output all \
  --max-events {numEventsPerJob} \
  --paths "*ScoutingPF*","*PFScouting*","-MC*" \
  > {hltLabel}.py

cat <<@EOF >> {hltLabel}.py

process.options.numberOfThreads = {numThreadsPerJobs}
process.options.numberOfStreams = {numStreamsPerJobs}
process.options.wantSummary = False

process.hltOutputScoutingPF.compressionAlgorithm = 'LZMA'
process.hltOutputScoutingPF.compressionLevel = 4

process.hltOutputScoutingPF.outputCommands += ['drop *_*_*_HLT']

streamPaths = [foo for foo in process.endpaths_() if foo.endswith('Output') and foo != 'ScoutingPFOutput']
for foo in streamPaths:
    process.__delattr__(foo)

for foo in ['HLTAnalyzerEndpath', 'MessageLogger']:
    if hasattr(process, foo):
        process.__delattr__(foo)

process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 500

#process.options.accelerators = ['cpu']
@EOF
"""

    hltCfgTypes = {}

    ## baseline
    config_baseline = f"from {hltLabel} import cms, process"
    hltCfgTypes[f'{hltLabel}_baseline'] = config_baseline

    ## CaloRecHits
    config_CaloRecHits = f"""from {hltLabel} import cms, process

process.hltScoutingCaloRecHitPackerECAL = cms.EDProducer("HLTScoutingCaloRecHitProducer",
  pfRecHits = cms.InputTag('hltParticleFlowRecHitECALUnseeded'),
  minEnergy = cms.double(-1),
  mantissaPrecision = cms.int32(10),
)

process.hltScoutingCaloRecHitPackerHBHE = cms.EDProducer("HLTScoutingCaloRecHitProducer",
  pfRecHits = cms.InputTag('hltParticleFlowRecHitHBHE'),
  minEnergy = cms.double(1),
  mantissaPrecision = cms.int32(10),
)

process.HLTPFScoutingPackingSequence.insert(0, process.hltScoutingCaloRecHitPackerECAL)
process.HLTPFScoutingPackingSequence.insert(1, process.hltScoutingCaloRecHitPackerHBHE)

process.hltOutputScoutingPF.outputCommands += [
    'keep *_hltScoutingCaloRecHitPackerECAL_*_*',
    'keep *_hltScoutingCaloRecHitPackerHBHE_*_*',
]
"""
    hltCfgTypes[f'{hltLabel}_CaloRecHits'] = config_CaloRecHits

    ## XcalRecHits
    config_XcalRecHits = f"""from {hltLabel} import cms, process

process.hltScoutingRecHitPacker = cms.EDProducer("HLTScoutingRecHitProducer",
  pfRecHitsECAL = cms.InputTag('hltParticleFlowRecHitECALUnseeded'),
  minEnergyEB = cms.double(-1),
  minEnergyEE = cms.double(-1),

  pfRecHitsHBHE = cms.InputTag('hltParticleFlowRecHitHBHE'),
  minEnergyHBHE = cms.double(1),

  mantissaPrecision = cms.int32(10),
)

process.HLTPFScoutingPackingSequence.insert(0, process.hltScoutingRecHitPacker)

process.hltOutputScoutingPF.outputCommands += [
    'keep *_hltScoutingRecHitPacker_EB_*',
    'keep *_hltScoutingRecHitPacker_EE_*',
    'keep *_hltScoutingRecHitPacker_HBHE_*',
]
"""
    hltCfgTypes[f'{hltLabel}_XcalRecHits'] = config_XcalRecHits

    print(f'Creating list of EDM input files on EOS ...')
    inputFileBlocks = []
    inputFiles = []
    for eosDir in eosDirs:
        execmd(f'eos ls {eosDir} > tmp.txt')
        inputFilesTmp = [fileName for fileName in open('tmp.txt').read().splitlines() if fileName.endswith('.root')]
        inputFiles += [f'root://eoscms.cern.ch/{eosDir}/{fileName}' for fileName in inputFilesTmp]
        os.remove('tmp.txt')
    inputFiles = sorted(list(set(inputFiles)))

    count = len(hltCfgTypes.keys())
    nRuns = len(inputFiles)
    print(f'... {nRuns} input files found')

    hltGetCmd = hltGetCmd(hltMenu, hltLabel)

    print(f'Downloading HLT menu ({hltMenu}) from ConfDB ...')
    execmd(hltGetCmd)

    print(f'Creating python configurations for {count} parallel jobs ', end='')
    print(f'({numThreadsPerJobs} threads and {numStreamsPerJobs} streams per job) ...')

    for hltCfgLabel in hltCfgTypes:
        with open(f'{hltCfgLabel}.py', 'w') as hltCfgFile:
            hltCfgFile.write(f'{hltCfgTypes[hltCfgLabel]}\n')

    for run_i in range(nRuns):
        if minRunNumber != None and run_i < minRunNumber:
            continue
        if maxRunNumber != None and run_i > maxRunNumber:
            continue

        runLabel = f'run{run_i:04d}'
        print(f'{runLabel} ...')

        jobCmds = []
        fileName = inputFiles[run_i]

        for hltCfgLabel in hltCfgTypes:
            hltLog = f'{hltCfgLabel}_{runLabel}.log'
            with open(f'{hltCfgLabel}.py', 'a') as hltCfgFile:
                hltCfgFile.write(f'process.hltOutputScoutingPF.fileName = "{hltCfgLabel}_{runLabel}.root"\n')
            jobCmds += [f'cmsRun {hltCfgLabel}.py inputFiles={fileName} &> {hltLog}']

        pool = multiprocessing.Pool(processes=count)
        pool.map(execmd, jobCmds)
