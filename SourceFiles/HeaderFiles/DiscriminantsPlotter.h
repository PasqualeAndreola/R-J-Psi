/*! 
 *  \file DiscriminantsPlotter.h 
 *  \brief Header file for \ref DiscriminantsPlotter function
 */

#ifndef DISCRIMINANTSPLOTTER_H
#define DISCRIMINANTSPLOTTER_H

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
#include "VarToBePlotted.h"
#include <TColor.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TFrame.h>
#include <TRatioPlot.h>
#include <TPaveText.h>

/*These namespaces can be useful*/
using namespace std;

int DiscriminantsPlotter(vector<MarkedNames> file2read, 
                         vector<MarkedNames> trees2read, 
                         vector<const char *> *var2bestored,
                         vector<VarToBePlotted> *vartobeplotted,
                         bool debug = false);

#endif