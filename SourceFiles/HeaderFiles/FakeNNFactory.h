/*! 
 *  \file FakeNNFactory.h 
 *  \brief Header file for \ref FakeNNFactory function
 */

#ifndef FAKENNFACTORY_H
#define FAKENNFACTORY_H

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
#include <TMVA/DataLoader.h>
#include <TMVA/Factory.h>
#include <TMVA/Config.h>
#include <TMVA/Configurable.h>
#include <TMVA/TMVAGui.h>
#include "TMVAMethod.h"
#include "VarToBeFitted.h"

/*These namespaces can be useful*/
using namespace std;

int FakeNNFactory(TString preselection_filter, 
                  vector<string> *fake_factory_names,
                  const char *pass_id,
                  vector<string> features,
                  vector<VarToBeFitted> *vartobefit,
                  vector<TMVAMethod> tmvamethods);

#endif