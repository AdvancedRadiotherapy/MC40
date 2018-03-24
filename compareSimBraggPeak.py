import ROOT

def getHistogram(fname):
    fin = ROOT.TFile(fname)
    histo = fin.Get("absorber.dEdx.Marcus;1")
    return histo

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

fname1 = "rootfiles/run_ubuntu16.04_Win10_gcc5.4.0_geant4.10.2.p03.root" # name of your output ROOT file from simulation
fname0 = "rootfiles/run_slc6_gcc4.8.1_geant4.10.1.p02.root" # name of my default ROOT file to compare with

fin0 = ROOT.TFile(fname0)
histo0 = fin0.Get("absorber.dEdx.Marcus;1")
histo0.Scale(1.0/histo0.GetEntries())

fin1 = ROOT.TFile(fname1)
histo1 = fin1.Get("absorber.dEdx.Marcus;1")
histo1.Scale(1.0/histo1.GetEntries())
histo1.SetLineColor(2)
histo1.GetYaxis().SetLabelFont(16)
histo1.GetXaxis().SetLabelFont(16)

ratio = histo0.Clone("ratio")
ratio.Divide(histo1)
ratio.GetYaxis().SetTitle("histo0/histo1")
ratio.GetYaxis().SetLabelFont(16)
ratio.GetXaxis().SetLabelFont(16)

canvas = ROOT.TCanvas("canvas", "canvas", 800,600)
pad1 = ROOT.TPad("pad1", "The pad 80% of the height",0.0,0.3,1.0,1.0)
pad1.SetBottomMargin(0)
pad2 = ROOT.TPad("pad2", "The pad 20% of the height",0.0,0.0,1.0,0.3)
pad2.SetTopMargin(0)
pad2.SetBottomMargin(0.2)

pad1.Draw()
pad2.Draw()

pad1.cd()
histo0.Draw()
histo1.Draw("same")

pad2.cd()
ratio.Draw()

import time
time.sleep(30)
