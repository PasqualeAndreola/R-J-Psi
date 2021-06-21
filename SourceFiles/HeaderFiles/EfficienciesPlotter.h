/*! 
 *  \file EfficienciesPlotter.h 
 *  \brief Header file for \ref EfficienciesPlotter function
 */

#ifndef EFFICIENCIESPLOTTER_H
#define EFFICIENCIESPLOTTER_H

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

/*These namespaces can be useful*/
using namespace std;

int EfficienciesPlotter(unordered_map<string, pair<string, string>> data_holder,
                        vector<DiscToBePlotted> *tobediscriminated,
                        bool debug = false);

#endif