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
    char const * discriminantlabel = "";
    char const * discriminantdimension = "";
    char const * output_directory_png;

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

};

#endif