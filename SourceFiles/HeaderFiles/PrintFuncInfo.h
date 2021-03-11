/*!
 *  \file PrintFuncInfo.h
 *  \brief Header file for \ref PrintFuncInfo function.
 */

#ifndef PRINTFUNCINFO_H
#define PRINTFUNCINFO_H

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <TString.h>

/*These namespaces can be useful*/
using namespace std;

void PrintFuncInfo(vector<TString> string2print);
void PrintFuncInfo(std::ostream& stream, vector<TString> string2print);

#endif