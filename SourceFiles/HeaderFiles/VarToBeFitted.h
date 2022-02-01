/*!
 *  \file VarToBeFitted.h
 *  \brief Header file for \ref VarToBeFitted class.
 * 
 *  \class This class is needed to associate a flag to each file
 */

#ifndef VARTOBEFITTED_H
#define VARTOBEFITTED_H

#include <string>
#include <TROOT.h>
#include <TString.h>
#include <TLegend.h>
#include <TPad.h>
#include <TPaveText.h>

using namespace std;

class VarToBeFitted
{
public:
    char const *vartobefitted = "";
    string varname = "";
    string vardimension = "";
    string cut_mediumid_true = "";
    string cut_mediumid_false = "";
    string preselectioncut_mediumid_true = "";
    string preselectioncut_mediumid_false = "";
    
    Float_t bins = 100;
    Float_t min_bin = 0;
    Float_t max_bin = 1;

    string legpos = "TL";
    Float_t legend_position_x0 = 0.60;
    Float_t legend_position_x1 = 0.9;
    Float_t legend_position_y0 = 0.66;
    Float_t legend_position_y1 = 0.9;

    Float_t legxlength = 0.2;
    TString Xlabel() {return TString::Format("%s[%s]", varname.data(), vardimension.data());};
    TLegend SetLegendPos(Float_t x0, Float_t x1, Float_t y0, Float_t y1) 
    {
        return TLegend(x0, y0, x1, y1);
    };   
    TLegend SetLegendPosManual(Float_t start_x, Float_t start_y, string position = "TL", int nlegentries=1, Float_t xlength=0.2, Float_t yentrywidth=0.04) 
    {
        Float_t x0 = 0, x1 = 0, y0 = 0, y1 = 0;
        if (position.compare("TL") == 0)
        {
            x0 = gPad->GetLeftMargin()+start_x; x1 = x0 + xlength;
            y1 = start_y; y0 = y1 - yentrywidth*nlegentries;
        }        
        else if (position.compare("TR") == 0)
        {
            x1 = 1-gPad->GetRightMargin()-start_x; x0 = x1 - xlength;
            y1 = start_y; y0 = y1 - yentrywidth*nlegentries;
        }

        return TLegend(x0, y0, x1, y1);
    };
    static TLegend SetLegendPosAuto(string position = "TL", int nlegentries=1, Float_t xlength=0.2, Float_t yentrywidth=0.04) 
    {
        Float_t x0 = 0, x1 = 0, y0 = 0, y1 = 0;
        if (position.compare("TL") == 0)
        {
            x0 = gPad->GetLeftMargin(); x1 = x0 + xlength;
            y1 = 1-gPad->GetTopMargin(); y0 = y1 - yentrywidth*nlegentries;
        }        
        else if (position.compare("TR") == 0)
        {
            x1 = 1-gPad->GetRightMargin(); x0 = x1 - xlength;
            y1 = 1-gPad->GetTopMargin(); y0 = y1 - yentrywidth*nlegentries;
        }

        return TLegend(x0, y0, x1, y1);
    };
    TPaveText SetPaveTextPosManual(Float_t start_x, Float_t start_y, string position = "TL", int ntextentries=1, Float_t xlength=0.2, Float_t yentrywidth=0.04) 
    {
        Float_t x0 = 0, x1 = 0, y0 = 0, y1 = 0;
        if (position.compare("TL") == 0)
        {
            x0 = gPad->GetLeftMargin()+start_x; x1 = x0 + xlength;
            y1 = start_y; y0 = y1 - yentrywidth*ntextentries;
        }        
        else if (position.compare("TR") == 0)
        {
            x1 = 1-gPad->GetRightMargin()-start_x; x0 = x1 - xlength;
            y1 = start_y; y0 = y1 - yentrywidth*ntextentries;
        }
        TPaveText pavetext(x0, y0, x1, y1, "NDC");
        pavetext.SetBorderSize(1);
        pavetext.SetFillColor(kWhite);
        return pavetext;
    };
};

#endif