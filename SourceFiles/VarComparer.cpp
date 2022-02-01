/*!
 *  \file VarComparer.cpp
 *  \brief Source file for \ref VArComparer function implementation
 */

/*!
 *  \fn int VarComparer(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read) "";
 *  \brief Function used to read selected variables stored in selected trees
 *
 *  \param file2read name of the file which stores the trees
 *  \param trees2read vector of names of trees that the user wants to explore
 */

#include "HeaderFiles/VarComparer.h"

using namespace ROOT;

int VarComparer(vector<MarkedNames> file2read,
                vector<MarkedNames> trees2read,
                vector<VarToBePlotted> *vartobeplotted,
                bool debug)

{
    gStyle->SetPalette(kRainBow);

    // Defining histograms
    TCanvas canvas_comparison = TCanvas("canvas_comparison", "canvas_comparison", 1360, 768);
    ROOT::RDF::RResultPtr<TH1F> histang;

    vector<RDF::RNode> *dataframe_tobecompared = new vector<RDF::RNode>;
    for (vector<MarkedNames>::iterator fileiterator = file2read.begin(); fileiterator != file2read.end(); ++fileiterator)
    {
        // Opening a dataframeit with kinematics
        ROOT::RDataFrame BtoMu(fileiterator->treetype.Data(), fileiterator->name.Data());
        TString dataframeflag = TString::Format("%s_%s", fileiterator->datatype.Data(), fileiterator->treetype.Data());
        if (BtoMu.HasColumn("nsig_sw")==true)
        {
            RDF::RNode BtoMu_withflag = BtoMu.Define("Flag", [=]{ return (TString)dataframeflag; });
            dataframe_tobecompared->push_back(BtoMu_withflag);
        }
        else
        {
            RDF::RNode BtoMu_withflag = BtoMu.Define("Flag", [=]{ return (TString)dataframeflag; })
                                             .Define("nsig_sw", [=] {return (Double_t)1;});
            dataframe_tobecompared->push_back(BtoMu_withflag);
        }
    
    }
    for (vector<VarToBePlotted>::iterator varit = vartobeplotted->begin(); varit != vartobeplotted->end(); varit++)
    {
        TCanvas canvas_comparison = TCanvas("canvas_comparison", "canvas_comparison", 1360, 768);
        TPad pad_comparison = TPad("pad_comparison", "pad_comparison", 0.0, 0.3, 1, 1);
        TPad pad_ratio = TPad("pad_ratio", "pad_ratio", 0.0, 0.0, 1, 0.3);
        THStack *histvar_stack = new THStack();
        TH1F *histcompare_ratio = new TH1F();
        pad_comparison.SetTopMargin(0.05);
        pad_comparison.SetBottomMargin(0.);
        pad_comparison.SetBorderMode(0);
        pad_comparison.SetLeftMargin(0.1);
        pad_ratio.SetTopMargin(0.);
        pad_ratio.SetBottomMargin(0.2);
        pad_ratio.SetBorderMode(0);
        pad_ratio.SetLeftMargin(0.1);
        gStyle->SetOptTitle(kFALSE);
        pad_comparison.Draw();
        pad_ratio.Draw();
        TLegend legcomparer = varit->SetLegendPosAuto(varit->legpos, 2, varit->legxlength);
        for (vector<RDF::RNode>::iterator dataframeit = dataframe_tobecompared->begin(); dataframeit != dataframe_tobecompared->end(); dataframeit++)
        {
            auto tstringvec = dataframeit->Take<TString>("Flag");
            TString dataframeflag = tstringvec->at(0), dataframefilter;
            if (((string)dataframeflag).find("Data") != string::npos) {cout << "bella fra" << endl; TString dataframefilter = varit->cut;}
            if (((string)dataframeflag).find("MC") != string::npos) {cout << "bella framc" << endl; TString dataframefilter = varit->cutmc;}
            if (dataframeit->HasColumn(varit->varname.data())==false) 
            {
                RDF::RNode dataframe = dataframeit->Define(varit->varname.data(), varit->varexpression.data());
                //else dataframeit = *dataframeit;
                Int_t bins = 100;
                Double_t minlim = varit->xminlim, maxlim = varit->xmaxlim;
                Double_t bin_width = (maxlim-minlim)/bins;
                TH1F *histvarit = new TH1F();
                TString histnamebase = TString::Format("%s_%s", tstringvec->at(0).Data(), varit->varname.data());
                const char* histname = histnamebase.Data();
                cout << histname << endl;
                histvarit = (TH1F *)dataframe.Filter(dataframefilter.Data())
                                            .Fill<Double_t, Double_t>(TH1F(histname, histname, bins, minlim, maxlim), {varit->varname.data(), "nsig_sw"})->Clone();
                if (dataframeit==dataframe_tobecompared->begin())
                {
                    histcompare_ratio = (TH1F*)histvarit->Clone();
                }
                else
                {
                    histcompare_ratio->Divide(histcompare_ratio, histvarit);
                    canvas_comparison.cd(0);
                    pad_ratio.cd();
                    histcompare_ratio->SetMarkerColor(kBlack);
                    histcompare_ratio->SetLineColor(kBlack);
                    histcompare_ratio->SetFillColor(kBlack);
                    histcompare_ratio->SetMarkerSize(1.5);
                    histcompare_ratio->SetStats(false);
                    histcompare_ratio->Draw("SAME E1");
                    histcompare_ratio->GetYaxis()->SetNdivisions(5);
                    histcompare_ratio->GetXaxis()->SetTitle(varit->Xlabel());
                    histcompare_ratio->GetXaxis()->SetTitleSize(0.1);
                    histcompare_ratio->GetXaxis()->SetTitleOffset(0.8);
                    histcompare_ratio->GetXaxis()->SetLabelSize(0.09);
                    histcompare_ratio->GetYaxis()->SetTitle("Obs/Exp");
                    histcompare_ratio->GetYaxis()->SetTitleSize(0.1);
                    histcompare_ratio->GetYaxis()->SetTitleOffset(0.45);
                    histcompare_ratio->GetYaxis()->SetLabelSize(0.09);
                    pad_ratio.Modified();
                }

                // Drawing
                canvas_comparison.cd(0);
                pad_comparison.cd();
                histvarit->SetLineWidth(3);
                histvarit->SetStats(false);
                histvarit->GetYaxis()->ChangeLabel(1, -1, 0);
                histvarit->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, varit->vardimension.c_str()));
                histvarit->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                histvarit->GetYaxis()->SetTitleOffset(1);
                histvar_stack->Add((TH1F*)histvarit->DrawNormalized()->Clone());
                legcomparer.AddEntry(histname, histname, "L");
                /*if (dataframeit==dataframe_tobecompared->begin()) histvarit->DrawNormalized("HIST PFC PMC PLC");
                else histvarit->DrawNormalized("SAME HIST PFC PMC PLC");*/
    /*
                TPaveText pavtext_fitwithratio = varit->SetPaveTextPosManual(0, legcomparer.GetY1(), varit->legpos, 2, varit->legxlength);
                pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi) from fit: %.2f #pm %.2f", realvar_rjpsi->getVal(), realvar_rjpsi->getPropagatedError(*results_data)));
                pavtext_fitwithratio.SetTextSize(0.035);
                pavtext_fitwithratio.SetTextFont(42);
                pavtext_fitwithratio.DrawClone("SAME");
                gPad->SetLogy(0);*/
            }
            else
            {
                //else dataframeit = *dataframeit;
                Int_t bins = 100;
                Double_t minlim = varit->xminlim, maxlim = varit->xmaxlim;
                Double_t bin_width = (maxlim-minlim)/bins;
                TH1F *histvarit = new TH1F();
                TString histnamebase = TString::Format("%s_%s", tstringvec->at(0).Data(), varit->varname.data());
                const char* histname = histnamebase.Data();
                cout << histname << endl;
                histvarit = (TH1F *)dataframeit->Filter(dataframefilter.Data())
                                            .Fill<Double_t, Double_t>(TH1F(histname, histname, bins, minlim, maxlim), {varit->varname.data(), "nsig_sw"})->Clone();
                if (dataframeit==dataframe_tobecompared->begin())
                {
                    histcompare_ratio = (TH1F*)histvarit->Clone();
                }
                else
                {
                    histcompare_ratio->Divide(histcompare_ratio, histvarit);
                    canvas_comparison.cd(0);
                    pad_ratio.cd();
                    histcompare_ratio->SetMarkerColor(kBlack);
                    histcompare_ratio->SetLineColor(kBlack);
                    histcompare_ratio->SetFillColor(kBlack);
                    histcompare_ratio->SetMarkerSize(1.5);
                    histcompare_ratio->SetStats(false);
                    histcompare_ratio->Draw("SAME E1");
                    histcompare_ratio->GetYaxis()->SetNdivisions(5);
                    histcompare_ratio->GetXaxis()->SetTitle(varit->Xlabel());
                    histcompare_ratio->GetXaxis()->SetTitleSize(0.1);
                    histcompare_ratio->GetXaxis()->SetTitleOffset(0.8);
                    histcompare_ratio->GetXaxis()->SetLabelSize(0.09);
                    histcompare_ratio->GetYaxis()->SetTitle("Obs/Exp");
                    histcompare_ratio->GetYaxis()->SetTitleSize(0.1);
                    histcompare_ratio->GetYaxis()->SetTitleOffset(0.45);
                    histcompare_ratio->GetYaxis()->SetLabelSize(0.09);
                    pad_ratio.Modified();
                }

                // Drawing
                canvas_comparison.cd(0);
                pad_comparison.cd();
                histvarit->SetLineWidth(3);
                histvarit->SetStats(false);
                histvarit->GetYaxis()->ChangeLabel(1, -1, 0);
                histvarit->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, varit->vardimension.c_str()));
                histvarit->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                histvarit->GetYaxis()->SetTitleOffset(1);
                histvar_stack->Add((TH1F*)histvarit->DrawNormalized()->Clone());
                legcomparer.AddEntry(histname, histname, "L");
                /*if (dataframeit==dataframe_tobecompared->begin()) histvarit->DrawNormalized("HIST PFC PMC PLC");
                else histvarit->DrawNormalized("SAME HIST PFC PMC PLC");*/
    /*
                TPaveText pavtext_fitwithratio = varit->SetPaveTextPosManual(0, legcomparer.GetY1(), varit->legpos, 2, varit->legxlength);
                pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi) from fit: %.2f #pm %.2f", realvar_rjpsi->getVal(), realvar_rjpsi->getPropagatedError(*results_data)));
                pavtext_fitwithratio.SetTextSize(0.035);
                pavtext_fitwithratio.SetTextFont(42);
                pavtext_fitwithratio.DrawClone("SAME");
                gPad->SetLogy(0);*/
            }
        }
        canvas_comparison.cd(0);
        pad_comparison.cd();
        histvar_stack->Draw("SAME NOSTACK HIST PLC");
        legcomparer.Draw("SAME");
        canvas_comparison.Print(TString::Format("%s.png", varit->varname.data()).Data());
        if (histcompare_ratio!=NULL) histcompare_ratio->Clear();
        if (histvar_stack!=NULL) histvar_stack->Clear();
    }
    /*        vector<VarToBePlotted>::iterator plotvar = vartobeplotted->begin();
    if (debug==false) ROOT::DisableImplicitMT();*/
    return 0;
}