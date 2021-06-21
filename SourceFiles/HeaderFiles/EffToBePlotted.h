/*!
 *  \file EffToBePlotted.h
 *  \brief Header file for \ref EffToBePlotted class.
 * 
 *  \class This class is needed to associate a flag to each file
 */

#ifndef EFFTOBEPLOTTED_H
#define EFFTOBEPLOTTED_H

using namespace std;

class EffToBePlotted
{
public:
    string effname;
    string effvariable;
    
    Float_t bins = 100;
    Float_t min_cut = 0;
    Float_t max_cut = 1;

    bool tobeprinted = false;
    string plottitle = "";
    string plotxlabel = "";
    double xminlim = 0;
    double xmaxlim = 0;

    bool comparisonflag = false;
    string comparisonname = "";
    string comparisonlabel = "";
    string comparisontitle = "";

    string comparisonrationame = "";
    string comparisonratioexpr = "";
    string comparisonratiotitle = "";
    string comparisonratioxlabel = "";

    string comparisonscattername = "";
    vector<string> comparisonscattereffs = {""};
    string comparisonscattertitle = "";
    string comparisonscatterxlabel = "";
    string comparisonscatterylabel = "";

    string wheretoplot = "";
};

#endif