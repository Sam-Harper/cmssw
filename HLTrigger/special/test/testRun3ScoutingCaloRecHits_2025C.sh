#!/bin/bashs

# LARGE SINGLE FILE for TESTING:
#/store/data/Run2025C/HLTPhysics/RAW/v1/000/392/175/00000/a13c68ba-66a5-418c-99cd-541e1c2365ce.root
inputFiles=($(ls /eos/cms/store/data/Run2025C/HLTPhysics/RAW/v1/000/392/175/*/*.root))

printf -v joined '%s,' "${inputFiles[@]:0:1}"
inputFilesStr="${joined%,}"
inputFilesStr=${inputFilesStr//\/eos\/cms/}

hltGetConfiguration /dev/CMSSW_15_0_0/GRun \
  --globaltag 150X_dataRun3_HLT_v1 \
  --data \
  --no-prescale \
  --output all \
  --max-events -1\
  --paths "*ScoutingPF*","*PFScouting*","-MC*" \
  --input "${inputFilesStr}" \
  > hlt_baseline.py

cat <<@EOF >> hlt_baseline.py

process.hltOutputScoutingPF.fileName = 'out_reHLT_Baseline_2025C.root'

process.hltOutputScoutingPF.compressionAlgorithm = 'LZMA'
process.hltOutputScoutingPF.compressionLevel = 4

process.options.wantSummary = False
process.options.numberOfThreads = 1
process.options.numberOfStreams = 0

del process.MessageLogger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 500

streamPaths = [foo for foo in process.endpaths_() if foo.endswith('Output') and foo != 'ScoutingPFOutput']
for foo in streamPaths:
    process.__delattr__(foo)
@EOF

cp hlt_baseline.py hlt_caloRecHits.py
cat <<@EOF >> hlt_caloRecHits.py

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

process.hltOutputScoutingPF.fileName = "out_reHLT_CaloRecHits_2025C.root"

@EOF

echo "=================================="
echo " baseline (no changes)"
echo "=================================="
#cmsRun hlt_baseline.py 2>&1 | tee hlt1.log

echo "=================================="
echo " caloRecHits (target)"
echo "=================================="
cmsRun hlt_caloRecHits.py 2>&1 | tee caloRecHits.log
