/*! 
 *  \file FakeNNClosureTest.h 
 *  \brief Header file for \ref FakeNNClosureTest function
 */

#ifndef FAKENNCLOSURETEST_H
#define FAKENNCLOSURETEST_H

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

/*These namespaces can be useful*/
using namespace std;

int FakeNNClosureTest(vector<string> *fake_factory_names,
                      vector<string> features,
                      vector<Float_t> Input_feature_var,
                      unordered_map<string, Float_t> MVA_feature_var,
                      vector<VarToBeFitted> *vartobefit,
                      vector<TMVAMethod> tmvamethods);

#endif