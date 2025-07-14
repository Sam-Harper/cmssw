#####################################################################
# 2025 Scouting: including calo recHits in the scouting data format # 
# ----------------------------------------------------------------- #

#!/usr/bin/env python3

import ROOT
import sys

# Loading CMSSW infrastructure: FWLite libraries and CMSSW dictionaries to handel the Detector IDs
# ------------------------------------------------------------------------------------------------ 

ROOT.gSystem.Load("libFWCoreFWLite")
ROOT.FWLiteEnabler.enable()
from DataFormats.FWLite import Events, Handle

ROOT.gSystem.Load("libDataFormatsDetId")
ROOT.gSystem.Load("libDataFormatsHcalDetId")
ROOT.gSystem.Load("libDataFormatsEcalDetId")

# Input and output file
# ---------------------
input_file = "out_reHLT_CaloRecHits_2025C.root"
output_file = ROOT.TFile("caloRecHit_wEnergyDetId_histograms.root", "RECREATE")

# Create the events object and define the handle and label for the collection
# ---------------------------------------------------------------------------
events = Events(input_file)

handle_HBHE = Handle("std::vector<Run3ScoutingCaloRecHit>")
label_HBHE = ("hltScoutingCaloRecHitPackerHBHE", "", "HLTX")
handle_ECAL = Handle("std::vector<Run3ScoutingCaloRecHit>")
label_ECAL = ("hltScoutingCaloRecHitPackerECAL", "", "HLTX")

# Create histograms
# -----------------
h_energy_HB = ROOT.TH1F("h_energy_HB", "HB RecHit Energy;Energy [GeV];Entries", 100, 0, 10)
h_energy_HE = ROOT.TH1F("h_energy_HE", "HE RecHit Energy;Energy [GeV];Entries", 100, 0, 10)
h_energy_EB = ROOT.TH1F("h_energy_EB", "EB RecHit Energy;Energy [GeV];Entries", 100, 0, 10)
h_energy_EE = ROOT.TH1F("h_energy_EE", "EE RecHit Energy;Energy [GeV];Entries", 100, 0, 10)

h_map_HB = ROOT.TH2F("h_map_HB", "HB iEta-iPhi Map;iEta;iPhi", 83, -41.5, 41.5, 72, 0.5, 72.5)
h_map_HE = ROOT.TH2F("h_map_HE", "HE iEta-iPhi Map;iEta;iPhi", 83, -41.5, 41.5, 72, 0.5, 72.5)
h_map_EB = ROOT.TH2F("h_map_EB", "EB iEta-iPhi Map;iEta;iPhi", 171, -85.5, 85.5, 360, 0.5, 360.5)
h_map_EE_plus = ROOT.TH2F("h_map_EE_plus", "EE+ iX-iY Map;iX;iY", 101, 0.5, 100.5, 101, 0.5, 100.5)
h_map_EE_minus = ROOT.TH2F("h_map_EE_minus", "EE- iX-iY Map;iX;iY", 101, 0.5, 100.5, 101, 0.5, 100.5)

# Loop through events
# -------------------
for i, event in enumerate(events):
    event.getByLabel(label_HBHE, handle_HBHE)
    rechits_HBHE = handle_HBHE.product()
    event.getByLabel(label_ECAL, handle_ECAL)
    rechits_ECAL = handle_ECAL.product()

    #print(f"Event {i}: Number of CaloRecHits (HBHE) = {len(rechits_HBHE)}")
    #print(f"Event {i}: Number of CaloRecHits (ECAL) = {len(rechits_ECAL)}")

    ###################
    # HCAL: HB and HE #
    ###################
    for hit in rechits_HBHE:
        energy = hit.energy()
        detid = ROOT.HcalDetId(hit.detId())
        subdet = detid.subdet()
        ieta = detid.ieta()
        iphi = detid.iphi()

        if subdet == 1:  # HB
            h_energy_HB.Fill(energy)
            h_map_HB.Fill(ieta, iphi)
        elif subdet == 2:  # HE
            h_energy_HE.Fill(energy)
            h_map_HE.Fill(ieta, iphi)

    ###################
    # ECAL: EB and EE #
    ###################
    for hit in rechits_ECAL:
        #if (not (hit.flags()==0 or hit.flags()==1)):
        energy = hit.energy()
        raw_detid = hit.detId()
        detid = ROOT.DetId(raw_detid)

        if detid.det() != ROOT.DetId.Ecal:
            continue

        subdet = detid.subdetId()
        if subdet == 1:  # EB
            if (hit.energy()>2 and (not (hit.flags()==1))):
                print("Flags?? YES = ", hit.flags())
            ebid = ROOT.EBDetId(raw_detid)
            ieta = ebid.ieta()
            iphi = ebid.iphi()
            h_energy_EB.Fill(energy)
            h_map_EB.Fill(ieta, iphi)
        elif subdet == 2:  # EE
            eeid = ROOT.EEDetId(raw_detid)
            ix = eeid.ix()
            iy = eeid.iy()
            zside = eeid.zside()
            h_energy_EE.Fill(energy)
            if zside > 0:
                h_map_EE_plus.Fill(ix, iy)
            else:
                h_map_EE_minus.Fill(ix, iy)

    # Uncomment to test on fewer events
    # if i >= 100:
    #     break

# Save histograms
# ---------------
output_file.cd()
for hist in [h_energy_HB, h_energy_HE, h_energy_EB, h_energy_EE,
             h_map_HB, h_map_HE, h_map_EB, h_map_EE_plus, h_map_EE_minus]:
    hist.Write()

output_file.Close()
print("Histograms saved to caloRecHit_wEnergyDetId_histograms.root")
