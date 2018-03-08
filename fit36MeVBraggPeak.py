#!/usr/bin/env python

import numpy as np
import ROOT
from array import array
import time

import sys


##
## Bragg Peak data
##

perspexDepth = array('d',[0.100,\
8.703,\
10.543,\
9.495,\
9.660,\
9.745,\
10.478,\
9.999,\
10.112,\
10.323,\
10.381,\
#10.407,\
5.810,\
4.003,\
10.466,\
10.436,\
10.225,\
0.100
])

perspexDepthError = array('d',[0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
#0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
0.001,\
])

chargeRatio = array('d',[0.1042581662,\
0.1873343717,\
0.2863028523,\
0.2594580435,\
0.2791194513,\
0.2893973453,\
0.4308533026,\
0.3472775278,\
0.4013973552,\
0.5276881039,\
0.5332576455,\
#0.5356591345,\
0.1227917805,\
0.1058701033,\
0.43703083,\
0.5020912608,\
0.4712739847,\
0.103348396
])

chargeRatioError = array('d', [0.0002620268,\
0.0005656895,\
0.0023932511,\
0.0026989089,\
0.0004236028,\
0.0010478589,\
0.0024718841,\
0.000452892,\
0.000735451,\
0.0074241379,\
0.0031672444,\
#0.0036804467,\
0.0004238249,\
0.0005135132,\
0.0021910097,\
0.006846084,\
0.0029634399,\
0.0003266762
])

chargeRatio = np.array(chargeRatio)
chargeRatio = chargeRatio / np.max(chargeRatio)
nPoints = len(perspexDepth)
perspexDepth = np.array(perspexDepth)
perspexDepth -= 0.1

data = ROOT.TGraphErrors(nPoints, perspexDepth, chargeRatio, perspexDepthError, chargeRatioError)

##
## Get the simulated bragg peak histogram for QGSP_BIC_EMY
##
fin_bic = ROOT.TFile('rootfiles/run_slc6_gcc4.8.1_geant4.10.1.p02.root')
bp_bic = fin_bic.Get("absorber.dEdx.Marcus")
#bp_bic.UseCurrentStyle()
bp_bic.SetStats(0)
bp_bic.GetXaxis().SetRangeUser(0,11)

# Now scale the plot to fit the data
# array to hold scale factors
scale_factors_bic = np.zeros(shape=(nPoints))

# loop through all the data points and calculate the scale factor for each
for b in np.arange(0,nPoints):
	simvalue = bp_bic.GetBinContent(bp_bic.FindBin(perspexDepth[b]))
	print b,' ',simvalue
	scale_factors_bic[b] = chargeRatio[b] / simvalue

print 'mean scale factor = ', np.mean(scale_factors_bic)
	
scale_factor_bic = np.mean(scale_factors_bic)	
	

#scale the simulation to match the data
bp_bic.Scale(scale_factor_bic)

minchi2_bic = 5e06
minscalefactor_bic = 0

for sf in np.arange(0.01,2,0.01):
	
	h = bp_bic.Clone("h")
	h.Scale(1/float(sf))
	chi2 = 0.0
	
	for b in np.arange(0,nPoints):
		binvalue = h.GetBinContent(h.FindBin(perspexDepth[b]))
		chi2 += np.power(chargeRatio[b]-binvalue,2) / binvalue

	print sf,' ',chi2
	if chi2 < minchi2_bic:
		minchi2_bic = chi2
		minscalefactor_bic = sf

print minchi2_bic, ' occurs with a scale factor of', minscalefactor_bic

bp_bic.Scale(1/minscalefactor_bic)


##
## FINALLY DRAW THE PLOT
##
canvas = ROOT.TCanvas("canvas","canvas")
canvas.cd()
bp_bic.SetTitle("Dose deposited as a function of depth in PMMA")
bp_bic.GetXaxis().SetTitle("Depth in PMMA [mm]")
bp_bic.GetYaxis().SetTitle("Normalised Dose")
bp_bic.SetLineColor(3)
bp_bic.SetLineStyle(2)
bp_bic.SetLineWidth(2)
bp_bic.Draw("C")
#bp_bert.Draw("C")
data.SetMarkerStyle(2)
data.SetMarkerSize(1)
data.SetMarkerColor(2)
data.SetLineColor(2)
data.Draw("p")

legend = ROOT.TLegend(0.25,0.70,0.7,0.90)
legend.SetBorderSize(0)
legend.SetFillColor(canvas.GetFillColor())
legend.SetHeader("36.4 #pm 0.15 MeV")
legend.AddEntry(data, "Data", "p")
legend.AddEntry(bp_bic, "QGSP_BIC_EMY", "l")
legend.Draw()
canvas.Update()

canvas.Print("plots/fittedBraggPeak36MeV.eps")
canvas.Print("plots/fittedBraggPeak36MeV.pdf")
canvas.Print("plots/fittedBraggPeak36MeV.png")


time.sleep(60)
