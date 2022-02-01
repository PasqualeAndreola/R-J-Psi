/*! 
 *  \file JPsiReweighting.h 
 *  \brief Header file for \ref JPsiReweighting function
 */

#ifndef JPSIREWEIGHTING_H
#define JPSIREWEIGHTING_H

#include <iostream>
#include <fstream>
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
#include "VarToBePlotted.h"
#include <TColor.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TFrame.h>
#include <TRatioPlot.h>
#include <TPaveText.h>
#include <TMVA/DataLoader.h>
#include <TMVA/RReader.hxx>
#include <TMVA/Factory.h>
#include <TMVA/Config.h>
#include <TMVA/Configurable.h>
#include <TMVA/TMVAGui.h>
#include "TMVAMethod.h"
#include <TMVA/Reader.h>
#include "VarToBeFitted.h"
#include <TLine.h>
#include "RooAbsData.h"
#include "TChain.h"
#include "RooRealVar.h"
#include "RooArgusBG.h"
#include "RooGaussian.h"
#include "RooFFTConvPdf.h"
#include "RooChebychev.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooFit.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooStats/SPlot.h"
#include "TIterator.h"
#include "VarToBeFitted.h"
#include "MarkedNames.h"
#include "RooDataHist.h"
#include "VarComparer.h"

// Namespaces
using namespace std;
using namespace RooFit;

// Header of the function
int JPsiReweighting(vector<MarkedNames> datanames, vector<MarkedNames> mcnames, vector<VarToBePlotted>* variable2becompared, bool debugfit=false);

#endif