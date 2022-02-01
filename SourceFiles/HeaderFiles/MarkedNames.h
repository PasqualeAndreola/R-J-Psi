/*!
 *  \file MarkedNames.h
 *  \brief Header file for \ref MarkedNames class.
 * 
 *  \class This class is needed to associate a flag to each file
 */

#ifndef MARKEDNAMES_H
#define MARKEDNAMES_H

using namespace std;

class MarkedNames
{
public:
    TString name;
    int flag;
    TString genename = "";
    int geneflag = -1;
    TString dime;
    TString labeltxt;
    TString datatype = "";
    TString treetype = "";
    vector<const char *> columns = {""};
    MarkedNames()
    {
        name = "";
    };
    MarkedNames(TString rnm, int rfl, TString measure = TString(), TString labeltext = TString(), vector<const char *> columnsrequired = {""})
    {
        name = rnm;
        flag = rfl;
        dime = measure;
        labeltxt = labeltext;
        columns = columnsrequired;
    };
    MarkedNames(TString rnm, int rfl, TString gnm, int gfl, TString measure = TString(), TString labeltext = TString(), vector<const char *> columnsrequired = {""})
    {
        if (rfl == 1)
        {
            name = rnm;
            flag = rfl;
            genename = gnm;
            geneflag = gfl;
            dime = measure;
            labeltxt = labeltext;
            columns = columnsrequired;
        }
        else if (rfl == 0)
        {
            name = gnm;
            flag = gfl;
            genename = rnm;
            geneflag = rfl;
            dime = measure;
            labeltxt = labeltext;
            columns = columnsrequired;
        }
    };
};

#endif