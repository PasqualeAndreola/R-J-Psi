/*! 
 *  \file NormalizationJPsi_X.h 
 *  \brief Header file for \ref JPsiMu_Normalization function
 */

#ifndef NORMALIZATIONJPSIMU_H
#define NORMALIZATIONJPSIMU_H

#include "VarToBeFitted.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <TROOT.h>
#include "ROOT/RDataFrame.hxx"
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <THStack.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TPaveStats.h>
#include "MarkedNames.h"
#include <TLorentzVector.h>
#include "DiscToBePlotted.h"
#include "EffToBePlotted.h"
#include <TColor.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TFrame.h>
#include <TRatioPlot.h>
#include <TPaveText.h>
#include <TMultiGraph.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooWorkspace.h>
#include <RooAbsPdf.h>
#include <RooPlot.h>
#include <RooFit.h>
#include <RooHistPdf.h>
#include <RooAddPdf.h>
#include <RooExtendPdf.h>
#include <RooProduct.h>
#include <RooProdPdf.h>
#include <RooDataSet.h>
#include <RooMinimizer.h>
#include <RooChebychev.h>
#include <RooArgusBG.h>
#include <RooFFTConvPdf.h>
#include <RooGaussian.h>
#include"TArrow.h"

/*These namespaces can be useful*/
using namespace std;

int NormalizationJPsiMu(TString cuts, bool debug = false);

#endif