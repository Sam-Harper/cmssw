# L1TauProducerPhase2


cmsrel CMSSW_11_1_7

cd CMSSW_11_1_7/src

cmsenv

git cms-init

git cms-merge-topic -u cms-l1t-offline:l1t-phase2-v3.3.9

scram b -j 8


# To Reconstruct L1 HPS Tau


git clone https://github.com/sandeepbhowmik1/L1TauProducerPhase2 $CMSSW_BASE/src/L1Trigger/Phase2L1Taus 

git clone https://github.com/sandeepbhowmik1/L1TauProducerPhase2-DataFormats $CMSSW_BASE/src/DataFormats/Phase2L1Taus

scram b -j 8



