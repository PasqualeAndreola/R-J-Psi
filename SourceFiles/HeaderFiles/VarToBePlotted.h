/*!
 *  \file VarToBePlotted.h
 *  \brief Header file for \ref VarToBePlotted class.
 * 
 *  \class This class is needed to associate a flag to each file
 */

#ifndef VARTOBEPLOTTED_H
#define VARTOBEPLOTTED_H

using namespace std;

class VarToBePlotted
{
public:
    string varname;
    string varexpression;

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
    vector<string> comparisonscattervars = {""};
    string comparisonscattertitle = "";
    string comparisonscatterxlabel = "";
    string comparisonscatterylabel = "";

    string wheretoplot = "";
};

#endif