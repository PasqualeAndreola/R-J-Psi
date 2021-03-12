/*! 
 *  \file TreeReader.h 
 *  \brief Header file for \ref TreeReader function
 */

#ifndef TREEREADER_H
#define TREEREADER_H

#include <iostream>
#include <vector>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <THStack.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include "MarkedNames.h"

/*These namespaces can be useful*/
using namespace std;

int TreeReader(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read);

#endif