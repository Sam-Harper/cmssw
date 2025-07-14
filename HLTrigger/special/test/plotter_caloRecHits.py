#####################################################################
# 2025 Scouting: including calo recHits in the scouting data format # 
# ----------------------------------------------------------------- #

#!/usr/bin/env python3

import ROOT
import os

# Set CMS style
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetTitleFontSize(0.04)
ROOT.gStyle.SetTitleOffset(1.3, "Y")
ROOT.gROOT.SetBatch(True)

# CMS label helper
def drawCMSLabel(canvas, lumi_text="2025 (13.6 TeV)"):
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextFont(42)
    latex.SetTextSize(0.045)
    latex.DrawLatex(0.15, 0.92, "#bf{CMS} #it{Preliminary}")
    latex.DrawLatex(0.67, 0.92, lumi_text)

# Output dir
output_dir = "/eos/user/e/elfontan/www/CMS_SCOUTING/2025/CALORecHits/"
os.makedirs(output_dir, exist_ok=True)

# Load input file
f = ROOT.TFile.Open("caloRecHit_histograms.root")
if not f or f.IsZombie():
    print("Error opening file.")
    exit(1)

# Iterate over histograms
for key in f.GetListOfKeys():
    obj = key.ReadObj()
    if not obj.InheritsFrom("TH1") and not obj.InheritsFrom("TH2"):
        continue

    name = obj.GetName()
    canvas = ROOT.TCanvas(f"c_{name}", f"{name}", 1000, 800)
    canvas.cd()
    canvas.SetLeftMargin(0.15)

    # Style
    if obj.InheritsFrom("TH1"):
        if (name == "h_energy_EB" or name == "h_energy_EE"):
            obj.SetLineColor(ROOT.kMagenta-7)
        elif (name == "h_energy_HB" or name == "h_energy_HE"):
            obj.SetLineColor(ROOT.kOrange-3)
        obj.SetLineWidth(3)
        obj.SetTitle("")
        obj.GetXaxis().SetTitleSize(0.045)
        obj.GetYaxis().SetTitleSize(0.045)
        obj.GetXaxis().SetLabelSize(0.04)
        obj.GetYaxis().SetLabelSize(0.04)
        obj.Draw("hist")
    elif obj.InheritsFrom("TH2"):
        obj.SetTitle("")
        obj.GetXaxis().SetTitleSize(0.045)
        obj.GetYaxis().SetTitleSize(0.045)
        obj.GetXaxis().SetLabelSize(0.04)
        obj.GetYaxis().SetLabelSize(0.04)
        obj.SetContour(100)
        obj.Draw("COLZ")

    # CMS label
    drawCMSLabel(canvas)

    # Optional legend
    if obj.InheritsFrom("TH1"):
        legend = ROOT.TLegend(0.53, 0.75, 0.88, 0.88)
        legend.SetHeader("New Scouting Dataformat", "C")
        legend.SetTextSize(0.04)
        legend.SetBorderSize(0)
        legend.SetFillColor(ROOT.kGray)
        legend.AddEntry(obj, name, "l")
        legend.Draw()

    # Save
    canvas.SaveAs(f"{output_dir}/{name}.pdf")
    canvas.SaveAs(f"{output_dir}/{name}.png")

f.Close()
