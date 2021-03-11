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

/*These namespaces can be useful*/
using namespace std;

int TreeReader(TString file2read, vector<TString> trees2read, vector<TString> var2read);

#endif