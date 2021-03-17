/*!
 *  \file TreeReader.cpp
 *  \brief Source file for \ref TreeReader function implementation
 */

/*!
 *  \fn int TreeReader(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read) "";
 *  \brief Function used to read selected variables stored in selected trees
 * 
 *  \param file2read name of the file which stores the trees
 *  \param trees2read vector of names of trees that the user wants to explore
 *  \param var2read vector of names of variables that the user wants to read
 */

#include "HeaderFiles/TreeReader.h"

int TreeReader(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read)
{
    // Initializing branch addresses
    Float_t var, genevar;

    for (vector<MarkedNames>::iterator fileiterator = file2read.begin(); fileiterator != file2read.end(); ++fileiterator)
    {
        // Opening the histogram collection
        TString outhistcol = TString::Format("OutputFiles/%s_Kinematics_Comparison.pdf", fileiterator->name.Data());
        TCanvas *c0 = new TCanvas("c0", "c0", 1366, 768);
        c0->Print(TString::Format("%s[", outhistcol.Data()));

        // Reading the input file
        TFile inputfile(TString::Format("InputFiles/%s", fileiterator->name.Data()), "update");
        for (vector<MarkedNames>::iterator treeiterator = trees2read.begin(); treeiterator != trees2read.end(); ++treeiterator)
        {
            TTree *ReadTree = (TTree *)inputfile.Get(TString::Format("%s", treeiterator->name.Data()));
            for (vector<MarkedNames>::iterator variterator = var2read.begin(); variterator != var2read.end(); ++variterator)
            {
                //Initializing histograms
                Float_t recomin = ReadTree->GetMinimum(variterator->name.Data());
                Float_t recomax = ReadTree->GetMaximum(variterator->name.Data());
                Float_t genemin = ReadTree->GetMinimum(variterator->genename.Data());
                Float_t genemax = ReadTree->GetMaximum(variterator->genename.Data());
                TH1F *histreco = new TH1F("reco", "reco", 100, recomin - abs(recomin / 10), recomax + abs(recomax / 10));
                TH1F *histgene = new TH1F("gene", "gene", 100, genemin - abs(genemin / 10), genemax + abs(genemax / 10));
                TH1F *histratio = new TH1F("ratio", "ratio", 100, -0.1, 2.1);
                TH2F *scatter = new TH2F("scatter", "scatter", 100, recomin, recomax, 100, genemin, genemax);

                if (variterator->geneflag == -1)
                {
                    TBranch *ReadBranch = ReadTree->GetBranch(variterator->name.Data());
                    ReadBranch->SetAddress(&var);
                    unsigned long int nentries = (unsigned long int)ReadTree->GetEntries();
                    for (unsigned long int i = 0; i < nentries; i++)
                    {
                        ReadBranch->GetEntry(i);
                        if (variterator->flag == 1)
                            histreco->Fill(var);
                        if (variterator->flag == 0)
                            histgene->Fill(var);
                    }
                }
                else
                {
                    TBranch *ReadBranch = ReadTree->GetBranch(variterator->name.Data());
                    ReadBranch->SetAddress(&var);
                    TBranch *ReadGeneBranch = ReadTree->GetBranch(variterator->genename.Data());
                    ReadGeneBranch->SetAddress(&genevar);
                    unsigned long int nentries = (unsigned long int)ReadTree->GetEntries();
                    for (unsigned long int i = 0; i < nentries; i++)
                    {
                        ReadBranch->GetEntry(i);
                        ReadGeneBranch->GetEntry(i);
                        histreco->Fill(var);
                        histgene->Fill(genevar);
                        histratio->Fill(var / genevar);
                        scatter->Fill(var, genevar);
                    }
                }

                //Histogram on the same plot
                c0->cd();
                histreco->SetTitle(TString::Format("Kinematic comparison: %s vs %s", variterator->name.Data(), variterator->genename.Data()));
                histreco->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->name.Data(), variterator->dime.Data()));
                histreco->GetYaxis()->SetTitle("Occurences");
                histreco->SetStats(kFALSE);
                histreco->SetLineColorAlpha(kRed, 1);
                histreco->SetFillStyle(1001);
                histreco->SetFillColor(kRed);
                histreco->SetFillColorAlpha(kRed, 0.8);
                histreco->SetMarkerStyle(kFullSquare);
                histreco->SetMarkerColor(kRed);
                histreco->SetMarkerSize(1);
                histreco->Draw("HF P");
                histgene->SetStats(kFALSE);
                histgene->SetLineColorAlpha(kBlue, 1);
                histgene->SetFillStyle(1001);
                histgene->SetFillColor(kBlue);
                histgene->SetFillColorAlpha(kBlue, 0.8);
                histgene->SetMarkerStyle(kFullTriangleUp);
                histgene->SetMarkerColor(kBlue);
                histgene->SetMarkerSize(1);
                histgene->Draw("SAME HF P");

                //Adjusting the legend
                auto legend = new TLegend(0.80, 0.85, 0.98, 0.95);
                legend->SetHeader(TString::Format("%s", fileiterator->name.Data()), "C");
                legend->AddEntry(histreco, variterator->name, "pf");
                legend->AddEntry(histgene, variterator->genename, "pf");
                legend->SetTextSize(0.03);
                legend->Draw("same");
                TLegendEntry *header = (TLegendEntry *)legend->GetListOfPrimitives()->First();
                header->SetTextSize(0.02);
                c0->Update();
                c0->Print(TString::Format("%s", outhistcol.Data()));

                //Histogram of ratio
                if ((histratio->GetEntries()) > 0)
                {
                    c0->Clear();
                    histratio->SetTitle(TString::Format("%s over %s", variterator->name.Data(), variterator->genename.Data()));
                    histratio->SetStats(kTRUE);
                    histratio->SetLineColorAlpha(kBlue, 1);
                    histratio->SetFillStyle(1001);
                    histratio->SetFillColor(kBlue);
                    histratio->SetFillColorAlpha(kBlue, 0.8);
                    histratio->SetMarkerStyle(kFullSquare);
                    histratio->SetMarkerColor(kBlue);
                    histratio->SetMarkerSize(1);
                    histratio->GetXaxis()->SetTitle(TString::Format("#frac{%s}{%s}", variterator->name.Data(), variterator->genename.Data()));
                    histratio->GetXaxis()->SetTitleOffset(1.4);
                    histratio->GetXaxis()->SetTitleSize(0.03);
                    histratio->GetYaxis()->SetTitle("Occurences");
                    histratio->Draw("HF P");
                    c0->Update();

                    //Adjusting the stats
                    TPaveStats *ratiostats = (TPaveStats *)histratio->GetListOfFunctions()->FindObject("stats");
                    ratiostats->SetName(TString::Format("%s over %s", variterator->name.Data(), variterator->genename.Data()));
                    ratiostats->SetX1NDC(0.1);
                    ratiostats->SetX2NDC(0.28);
                    ratiostats->SetY1NDC(0.68);
                    ratiostats->SetY2NDC(0.8);
                    ratiostats->Draw("same");

                    //Adjusting the legend
                    legend->Clear();
                    legend->SetX1NDC(0.1);
                    legend->SetX2NDC(0.28);
                    legend->SetY1NDC(0.8);
                    legend->SetY2NDC(0.9);
                    legend->SetHeader(TString::Format("%s", fileiterator->name.Data()), "C");
                    legend->AddEntry(histratio, TString::Format("#frac{%s}{%s}", variterator->name.Data(), variterator->genename.Data()), "pf");
                    legend->SetTextSize(0.02);
                    legend->Draw("same");
                    header = (TLegendEntry *)legend->GetListOfPrimitives()->First();
                    header->SetTextSize(0.02);
                    header->Draw("same");
                    c0->Update();
                    c0->Print(TString::Format("%s", outhistcol.Data()));
                }

                //Scatter plot
                if ((scatter->GetEntries()) > 0)
                {
                    c0->Clear();
                    scatter->SetTitle(TString::Format("2D histogram of %s and %s", variterator->name.Data(), variterator->genename.Data()));
                    scatter->SetStats(kTRUE);
                    scatter->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->name.Data(), variterator->dime.Data()));
                    scatter->GetYaxis()->SetTitle(TString::Format("%s[%s]", variterator->genename.Data(), variterator->dime.Data()));
                    scatter->Draw("colz");
                    c0->Update();
                    TAxis *contourpalette = (TAxis *)scatter->GetZaxis();
                    contourpalette->SetMaxDigits(3);

                    //Adjusting the stats
                    TPaveStats *scatterstats = (TPaveStats *)scatter->GetListOfFunctions()->FindObject("stats");
                    scatterstats->SetName(TString::Format("%s over %s", variterator->name.Data(), variterator->genename.Data()));
                    scatterstats->SetX1NDC(0.1);
                    scatterstats->SetX2NDC(0.28);
                    scatterstats->SetY1NDC(0.68);
                    scatterstats->SetY2NDC(0.85);
                    scatterstats->Draw("same");

                    //Adjusting the legend
                    legend->Clear();
                    legend->SetX1NDC(0.1);
                    legend->SetX2NDC(0.28);
                    legend->SetY1NDC(0.85);
                    legend->SetY2NDC(0.9);
                    legend->SetHeader(TString::Format("%s", fileiterator->name.Data()), "C");
                    legend->Draw("same");
                    header = (TLegendEntry *)legend->GetListOfPrimitives()->First();
                    header->SetTextSize(0.02);
                    header->Draw("same");
                    c0->Update();
                    c0->Print(TString::Format("%s", outhistcol.Data()));
                }
                histreco->Delete();
                histgene->Delete();
                histratio->Delete();
                scatter->Delete();
            }
        }
        c0->Print(TString::Format("%s]", outhistcol.Data()));
    }
    return 0;
}