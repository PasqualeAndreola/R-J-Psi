/*!
 *  \file TreeRDFDiscriminants.cpp
 *  \brief Source file for \ref TreeRDFDiscriminants function implementation
 */

/*!
 *  \fn int TreeReader(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read) "";
 *  \brief Function used to read selected variables stored in selected trees
 * 
 *  \param file2read name of the file which stores the trees
 *  \param trees2read vector of names of trees that the user wants to explore
 *  \param var2read vector of names of variables that the user wants to read
 */

#include "HeaderFiles/TreeRDFDiscriminants.h"
#include "HeaderFiles/Discriminants.h"

int TreeRDFDiscriminants(vector<MarkedNames> file2read, 
                         vector<MarkedNames> trees2read, 
                         vector<MarkedNames> var2read, 
                         unordered_map<string, vector<string>> discriminantvariables)
{
    //Enabling multi-threading
    ROOT::EnableImplicitMT();

    //Removing old output file
    TFile output("newfile.root","recreate");
    output.Close();

    for (vector<MarkedNames>::iterator fileiterator = file2read.begin(); fileiterator != file2read.end(); ++fileiterator)
    {
        // Opening the histogram collection
        TString outhistcol = TString::Format("OutputFiles/%s_Kinematics_Comparison.pdf", fileiterator->name.Data());
        TCanvas *c0 = new TCanvas(TString::Format("c%s", fileiterator->name.Data()), TString::Format("c%s", fileiterator->name.Data()), 1366, 768);
        c0->Print(TString::Format("%s[", outhistcol.Data()));

        // Defining histograms I would fill from a RootDataFrame
        ROOT::RDF::RResultPtr<TH1F> histreco;
        ROOT::RDF::RResultPtr<TH1F> histgene;
        ROOT::RDF::RResultPtr<TH1F> histratio;
        ROOT::RDF::RResultPtr<TH2F> scatter;

        // Reading the input file
        TFile inputfile(TString::Format("InputFiles/%s", fileiterator->name.Data()), "update");
        for (vector<MarkedNames>::iterator treeiterator = trees2read.begin(); treeiterator != trees2read.end(); ++treeiterator)
        {
            // Initializing a RootDataFrame from the input tree
            ROOT::RDataFrame BtoMuData(TString::Format("%s", treeiterator->name.Data()), TString::Format("InputFiles/%s", fileiterator->name.Data()));

            // Defining the new discriminants as new columns from their lambda expression
            auto BtoMu_discriminants = BtoMuData.Define("Q2", squaredtransferredmomentum, discriminantvariables["Q2"])
                                                .Define("missingm2", missingmasssquared, discriminantvariables["missingm2"])
                                                .Define("missingpt", missingpt, discriminantvariables["missingpt"]);

            //auto BtoMu_discriminants_gene = BtoMuData.Define("Q2gene", squaredtransferredmomentum, discriminantvariables[0])
            //                                         .Define("missingm2gene", missingmasssquared, discriminantvariables[1]);

            // Modifying write options of the root data frame to overwrite other trees
            ROOT::RDF::RSnapshotOptions snapopt;
            snapopt.fMode = "UPDATE";
            snapopt.fOverwriteIfExists = "TRUE";

            // Adding every variable used in the various discriminants as a branch to be written in the tree
            vector<string> branch2bewritten;
            unordered_map<string, vector<string>>::iterator i = discriminantvariables.begin(); 
            while (i != discriminantvariables.end())
            {
                copy(i->second.begin(), i->second.end(), back_inserter(branch2bewritten));
                i++;
            }
            for (vector<MarkedNames>::iterator variterator = var2read.begin(); variterator != var2read.end(); ++variterator)
            {
                branch2bewritten.push_back(variterator->name.Data());
                if ((variterator->geneflag)!= (-1)) branch2bewritten.push_back(variterator->genename.Data());
            }

            // Adding the various discriminants as a branch to be written
            if (fileiterator == file2read.begin())
            {
                branch2bewritten.push_back("Q2");
                var2read.push_back(MarkedNames("Q2", 1, "GeV", "Q^{2}"));
                branch2bewritten.push_back("missingm2");
                var2read.push_back(MarkedNames("missingm2", 1, "GeV", "m_{miss}^{2}"));
                branch2bewritten.push_back("missingpt");
                var2read.push_back(MarkedNames("missingpt", 1, "GeV", "pt_{T}^{miss}"));
                //branch2bewritten.push_back("Q2gene");
                //branch2bewritten.push_back("missingm2gene");
            }

            // Writing the new RDF with discriminants and their variables
            TString Tree2bewritten = TString::Format("%s_%s", fileiterator->dime.Data(), treeiterator->name.Data());
            BtoMu_discriminants.Snapshot(Tree2bewritten.Data(), "newfile.root", branch2bewritten, snapopt);

            TFile inputfile2("newfile.root", "update");
            TTree *ReadTree = (TTree*)inputfile2.Get(Tree2bewritten.Data());
            ROOT::RDataFrame BtoMu(Tree2bewritten.Data(), "newfile.root");

            for (vector<MarkedNames>::iterator variterator = var2read.begin(); variterator != var2read.end(); ++variterator)
            {
                Float_t recomin = ReadTree->GetMinimum(variterator->name.Data());
                Float_t recomax = ReadTree->GetMaximum(variterator->name.Data());
                Float_t genemin = ReadTree->GetMinimum(variterator->genename.Data());
                Float_t genemax = ReadTree->GetMaximum(variterator->genename.Data());
                
                // Filling histogram for generated variables
                if ((variterator->geneflag) ==(-1) && (variterator->flag)==0)
                {
                    histgene = BtoMu.Fill<Float_t>(TH1F(variterator->name.Data(), variterator->name.Data(), 100,
                               recomin - abs(recomin / 10), recomax + abs(recomax / 10)), {variterator->name.Data()});
                } 

                // Filling histogram for reco variables
                else if ((variterator->geneflag) ==(-1) && (variterator->flag)==1)
                {
                    histreco = BtoMu.Fill<Float_t>(TH1F(variterator->name.Data(), variterator->name.Data(), 100,
                               recomin - abs(recomin / 10), recomax + abs(recomax / 10)), {variterator->name.Data()});
                }
                else
                {
                    // Filling histogram for generated variables
                    histgene = BtoMu.Fill<Float_t>(TH1F(variterator->genename.Data(), variterator->genename.Data(), 100,
                                genemin - abs(genemin / 10), genemax + abs(genemax / 10)), {variterator->genename.Data()});

                    // Filling histogram for reco variables
                    histreco = BtoMu.Fill<Float_t>(TH1F(variterator->name.Data(), variterator->name.Data(), 100,
                                recomin - abs(recomin / 10), recomax + abs(recomax / 10)), {variterator->name.Data()});

                    // Defining a column with the ratio reco/gene and drawing it as a histogram
                    auto BtoMuRatio = BtoMu.Define(TString::Format("%sover%s", variterator->name.Data(),variterator->genename.Data()),
                                                   [](Float_t x, Float_t y){return x/y;},
                                                   {TString::Format("%s", variterator->name.Data()).Data(),
                                                    TString::Format("%s", variterator->genename.Data()).Data()});
                    histratio = BtoMuRatio.Fill<Float_t>(TH1F
                                (TString::Format("%sover%s", variterator->name.Data(),variterator->genename.Data()),
                                 TString::Format("%sover%s", variterator->name.Data(),variterator->genename.Data()),
                                 100, -0.1, 2.1), 
                                 {TString::Format("%sover%s", variterator->name.Data(), variterator->genename.Data()).Data()});

                    // Filling a 2D-Histogram with reco vs gene
                    scatter = BtoMuRatio.Fill<Float_t, Float_t>(TH2F
                                (TString::Format("%s vs %s", variterator->name.Data(),variterator->genename.Data()),
                                 TString::Format("%s vs %s", variterator->name.Data(),variterator->genename.Data()),
                                 100, recomin, recomax, 100, genemin, genemax), 
                                 {TString::Format("%s", variterator->name.Data()).Data(),
                                  TString::Format("%s", variterator->genename.Data()).Data()});      
                }

                //Histogram on the same plot
                c0->cd();
                if (variterator->flag==1 || variterator->geneflag!=-1)
                {
                    histreco->SetTitle(TString::Format("Comparison: %s vs %s", variterator->name.Data(), variterator->genename.Data()));
                    histreco->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->name.Data(), variterator->dime.Data()));
                    if (variterator->labeltxt.IsNull() == 0)
                    {
                        histreco->SetTitle(TString::Format("Comparison: %s vs %s(GEN)", variterator->labeltxt.Data(),  variterator->labeltxt.Data()));
                        histreco->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->labeltxt.Data(), variterator->dime.Data()));
                    }
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
                }
                if (variterator->flag==0 || variterator->geneflag!=-1)
                {
                    histgene->SetTitle(TString::Format("Comparison: %s vs %s", variterator->name.Data(), variterator->genename.Data()));
                    histgene->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->name.Data(), variterator->dime.Data()));
                    if (variterator->labeltxt.IsNull() == 0)
                    {
                        histgene->SetTitle(TString::Format("Comparison: %s vs %s(GEN)", variterator->labeltxt.Data(),  variterator->labeltxt.Data()));
                        histgene->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->labeltxt.Data(), variterator->dime.Data()));
                    }
                    histgene->GetYaxis()->SetTitle("Occurences");
                    histgene->SetStats(kFALSE);
                    histgene->SetLineColorAlpha(kBlue, 1);
                    histgene->SetFillStyle(1001);
                    histgene->SetFillColor(kBlue);
                    histgene->SetFillColorAlpha(kBlue, 0.8);
                    histgene->SetMarkerStyle(kFullTriangleUp);
                    histgene->SetMarkerColor(kBlue);
                    histgene->SetMarkerSize(1);
                    histgene->Draw("SAME HF P");
                }

                //Adjusting the legend
                auto *legend = new TLegend(0.80, 0.85, 0.98, 0.95);
                legend->SetHeader(TString::Format("%s", fileiterator->name.Data()), "C");
                legend->AddEntry(variterator->name, variterator->name, "pf");
                legend->AddEntry(variterator->genename, variterator->genename, "pf");
                legend->SetTextSize(0.03);
                legend->Draw("same");
                TLegendEntry *header = (TLegendEntry *)legend->GetListOfPrimitives()->First();
                header->SetTextSize(0.02);
                c0->Update();
                c0->Print(TString::Format("%s", outhistcol.Data()));

                //Histogram of ratio
                if (histratio != 0 && histratio->GetEntries()>0)
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
                    if (variterator->labeltxt.IsNull() == 0)
                    {
                        histratio->SetTitle(TString::Format("#frac{%s}{%s(GEN)}", variterator->labeltxt.Data(),  variterator->labeltxt.Data()));
                        histratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->labeltxt.Data(), variterator->dime.Data()));
                    }
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
                    legend->AddEntry(TString::Format("%sover%s", variterator->name.Data(),variterator->genename.Data()), TString::Format("#frac{%s}{%s}", variterator->name.Data(), variterator->genename.Data()), "pf");
                    legend->SetTextSize(0.02);
                    legend->Draw("same");
                    header = (TLegendEntry *)legend->GetListOfPrimitives()->First();
                    header->SetTextSize(0.02);
                    header->Draw("same");
                    c0->Update();
                    c0->Print(TString::Format("%s", outhistcol.Data()));
                    histratio->Reset("ICESM");
                }

                //Scatter plot
                if (scatter != 0 && scatter->GetEntries()>0)
                {
                    c0->Clear();
                    scatter->SetTitle(TString::Format("2D histogram of %s and %s", variterator->name.Data(), variterator->genename.Data()));
                    scatter->SetStats(kTRUE);
                    scatter->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->name.Data(), variterator->dime.Data()));
                    scatter->GetYaxis()->SetTitle(TString::Format("%s[%s]", variterator->genename.Data(), variterator->dime.Data()));
                    if (variterator->labeltxt.IsNull() == 0)
                    {
                        scatter->SetTitle(TString::Format("2D histogram of %s and %s", variterator->labeltxt.Data(),  variterator->labeltxt.Data()));
                        scatter->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->labeltxt.Data(), variterator->dime.Data()));
                        scatter->GetYaxis()->SetTitle(TString::Format("%s(GEN)[%s]", variterator->labeltxt.Data(), variterator->dime.Data()));
                    }
                    scatter->Draw("colz");
                    c0->Update();
                    TAxis *contourpalette = (TAxis *)scatter->GetZaxis();
                    contourpalette->SetMaxDigits(3);

                    //Adjusting the stats
                    TPaveStats *scatterstats = (TPaveStats *)scatter->GetListOfFunctions()->FindObject("stats");
                    scatterstats->SetName(TString::Format("%s vs %s", variterator->name.Data(), variterator->genename.Data()));
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
                    scatter->Reset("ICESM");
                }
                c0->Clear();
            }
        }
        c0->Print(TString::Format("%s]", outhistcol.Data()));
        c0->Clear();
    }
    return 0;
}