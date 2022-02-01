/*!
 *  \file DiscToBePlotted.h
 *  \brief Header file for \ref DiscToBePlotted class.
 * 
 *  \class This class is needed to associate a flag to each file
 */

#ifndef DISCTOBEPLOTTED_H
#define DISCTOBEPLOTTED_H

using namespace std;

class DiscToBePlotted
{
public:
    char const * discriminantvariable;
    char const * discriminantname;
    char const * discriminantprettyname;
    char const * discriminantlabel = "";
    char const * discriminantdimension = "";
    char const * output_directory_png;
    string varname = "";
    string vardimension = "";

    Float_t discriminantbins = 100;
    Float_t discriminantmin = 0;
    Float_t discriminantmax = 1;

    bool flag_signormplot = true;
    char const * signormplot_title = "";
    char const * signormplot_xtitle = "";
    char const * signormplot_ytitle = "Normalized Occurences [a.u.]";
    int distributionlogscale = 0;
    TLegend *distributionplotlegend = new TLegend();


    bool flag_signormeffplot = true;
    char const * signormeffplot_title = "";
    char const * signormeffplot_xtitle = "";
    char const * signormeffplot_ytitle = "Efficiency";

    bool flag_signormbkgplot = true;
    char const * signormbkgplot_title = "";
    char const * signormbkgplot_xtitle = "";
    char const * signormbkgplot_ytitle = "Normalized Occurences [a.u.]";

    bool flag_signormbkgeffplot = true;
    char const * signormbkgeffplot_title = "";
    char const * signormbkgeffplot_xtitle = "";
    char const * signormbkgeffplot_ytitle = "Normalized Occurences [a.u.]";

    bool flag_ratioplot = true;
    char const * ratioplot_title = "";
    char const * ratioplot_xtitle = "";
    char const * ratioplot_ytitle = "Normalized Occurences [a.u.]";

    bool flag_rocplot = true;

    string legpos = "TL";
    Float_t legxlength = 0.2;
    TString Xlabel() {return TString::Format("%s[%s]", varname.data(), vardimension.data());};
    TLegend SetLegendPos(Float_t x0, Float_t x1, Float_t y0, Float_t y1) 
    {
        return TLegend(x0, y0, x1, y1);
    };   
    TLegend SetLegendPosAuto(string position = "TL", int nlegentries=1, Float_t xlength=0.2, Float_t yentrywidth=0.04) 
    {
        Float_t x0 = 0, x1 = 0, y0 = 0, y1 = 0;
        if (position.compare("TL") == 0)
        {
            x0 = gPad->GetLeftMargin(); x1 = x0 + xlength;
            y1 = 0.9; y0 = y1 - yentrywidth*nlegentries;
        }        
        else if (position.compare("TR") == 0)
        {
            x1 = 1-gPad->GetRightMargin(); x0 = x1 - xlength;
            y1 = 0.9; y0 = y1 - yentrywidth*nlegentries;
        }

        return TLegend(x0, y0, x1, y1);
    };
};

#endif