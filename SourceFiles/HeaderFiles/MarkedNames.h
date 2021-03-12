/*!
 *  \file MarkedNames.h
 *  \brief Header file for \ref MarkedNames class.
 * 
 *  \class This class is needed to associate a flag to each file
 */

#ifndef MARKEDNAMES_H
#define MARKEDNAMES_H

class MarkedNames
{
public:
    TString name;
    int flag;
    TString genename = "";
    int geneflag = -1;
    MarkedNames(TString rnm, int rfl)
    {
        name = rnm;
        flag = rfl;
    };
    MarkedNames(TString rnm, int rfl, TString gnm, int gfl)
    {
        if (rfl == 1)
        {
            name = rnm;
            flag = rfl;
            genename = gnm;
            geneflag = gfl;
        }
        else if (rfl == 0)
        {
            name = gnm;
            flag = gfl;
            genename = rnm;
            geneflag = rfl;
        }
    };
};

#endif