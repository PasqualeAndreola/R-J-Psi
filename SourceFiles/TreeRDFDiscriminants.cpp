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
                         unordered_map<string, vector<string>> discriminantvariables,
                         string filter,
                         bool debugflag)
{
    //Enabling multi-threading
    ROOT::EnableImplicitMT();

    //Removing old output file
    TFile output("Discriminants.root","recreate");
    output.Close();

    for (vector<MarkedNames>::iterator fileiterator = file2read.begin(); fileiterator != file2read.end(); ++fileiterator)
    {
        // Opening the histogram collection
        TString outhistcol = TString::Format("OutputFiles/%s_Kinematics_Comparison.pdf", fileiterator->labeltxt.Data());

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
            auto BtoMu_discriminants = BtoMuData.Define("b_c_pisa_threemomentum", b_pisa_threemomentum, discriminantvariables["b_c_pisa_threemomentum"])
                                                .Define("Q2_gene", squaredtransferredmomentumgen, discriminantvariables["Q2_gene"])
                                                .Define("Q2_Pisa", squaredtransferredmomentumpisa, discriminantvariables["Q2_Pisa"])
                                                .Define("Q2_Jona", squaredtransferredmomentumjona, discriminantvariables["Q2_Jona"])                                             
                                                .Define("missingm2_gene", missingmasssquaredgen, discriminantvariables["missingm2_gene"])
                                                .Define("missingm2_Pisa", missingmasssquaredpisa, discriminantvariables["missingm2_Pisa"])
                                                .Define("missingm2_Jona", missingmasssquaredjona, discriminantvariables["missingm2_Jona"])
                                                .Define("missingpt_gene", missingptgen, discriminantvariables["missingpt_gene"])
                                                .Define("missingpt_Pisa", missingptpisa, discriminantvariables["missingpt_Pisa"])
                                                .Define("missingpt_Jona", missingptjona, discriminantvariables["missingpt_Jona"])
                                                .Define("eta_gene", eta, discriminantvariables["eta_gene"])
                                                .Define("eta_PVgeneSVreco", eta, discriminantvariables["eta_PVgeneSVreco"])
                                                .Define("eta_PVrecoSVgene", eta, discriminantvariables["eta_PVrecoSVgene"])
                                                .Define("eta_reco", eta, discriminantvariables["eta_reco"])
                                                .Define("eta_breco_beam", eta, discriminantvariables["eta_breco_beam"])
                                                .Define("eta_breco_pv", eta, discriminantvariables["eta_breco_pv"])
                                                .Define("eta_beamsvreco", eta, discriminantvariables["eta_beamsvreco"])
                                                .Define("eta_genePVSVreco", deltaeta, discriminantvariables["eta_genePVSVreco"])
                                                .Define("eta_genePVgeneSVreco", deltaeta, discriminantvariables["eta_genePVgeneSVreco"])
                                                .Define("eta_genePVrecoSVgene", deltaeta, discriminantvariables["eta_genePVrecoSVgene"])
                                                .Define("eta_3mumomenta_gene", eta_3mumomentum, discriminantvariables["eta_3mumomenta_gene"])
                                                .Define("eta_3mumomenta_reco", eta_3mumomentum, discriminantvariables["eta_3mumomenta_reco"])
                                                .Define("eta_BcPVgeneSVgene", deltaeta, discriminantvariables["eta_BcPVgeneSVgene"])
                                                .Define("eta_BcPVgeneSVreco", deltaeta, discriminantvariables["eta_BcPVgeneSVreco"])
                                                .Define("eta_BcPVrecoSVgene", deltaeta, discriminantvariables["eta_BcPVrecoSVgene"])
                                                .Define("eta_BcPVrecoSVreco", deltaeta, discriminantvariables["eta_BcPVrecoSVreco"])
                                                .Define("phi_gene", phi, discriminantvariables["phi_gene"])
                                                .Define("phi_PVgeneSVreco", phi, discriminantvariables["phi_PVgeneSVreco"])
                                                .Define("phi_PVrecoSVgene", phi, discriminantvariables["phi_PVrecoSVgene"])
                                                .Define("phi_reco", phi, discriminantvariables["phi_reco"])
                                                .Define("phi_breco_beam", phi, discriminantvariables["phi_breco_beam"])
                                                .Define("phi_breco_pv", phi, discriminantvariables["phi_breco_pv"])
                                                .Define("phi_beamsvreco", phi, discriminantvariables["phi_beamsvreco"])
                                                .Define("phi_genePVSVreco", deltaphi, discriminantvariables["phi_genePVSVreco"])
                                                .Define("phi_genePVgeneSVreco", deltaphi, discriminantvariables["phi_genePVgeneSVreco"])
                                                .Define("phi_genePVrecoSVgene", deltaphi, discriminantvariables["phi_genePVrecoSVgene"])
                                                .Define("phi_3mumomenta_gene", phi_3mumomentum, discriminantvariables["phi_3mumomenta_gene"])
                                                .Define("phi_3mumomenta_reco", phi_3mumomentum, discriminantvariables["phi_3mumomenta_reco"])
                                                .Define("phi_BcPVgeneSVgene", deltaphi, discriminantvariables["phi_BcPVgeneSVgene"])
                                                .Define("phi_BcPVgeneSVreco", deltaphi, discriminantvariables["phi_BcPVgeneSVreco"])
                                                .Define("phi_BcPVrecoSVgene", deltaphi, discriminantvariables["phi_BcPVrecoSVgene"])
                                                .Define("phi_BcPVrecoSVreco", deltaphi, discriminantvariables["phi_BcPVrecoSVreco"])
                                                .Define("angle_grandmother_pvsvgen", angle, discriminantvariables["angle_grandmother_pvsvgen"])
                                                .Define("angle_grandmother_PVrecoBreco", angle, discriminantvariables["angle_grandmother_PVrecoBreco"])
                                                .Define("angle_grandmother_BeamBreco", angle, discriminantvariables["angle_grandmother_BeamBreco"])
                                                .Define("angle_grandmother_PVrecoJpsireco", angle, discriminantvariables["angle_grandmother_PVrecoJpsireco"])
                                                .Define("angle_grandmother_BeamJpsireco", angle, discriminantvariables["angle_grandmother_BeamJpsireco"])
                                                .Define("angle_grandmother_3mureco", angle3mu, discriminantvariables["angle_grandmother_3mureco"])
                                                .Define("cos_angle_grandmother_pvsvgen", cosangle, discriminantvariables["cos_angle_grandmother_pvsvgen"])
                                                .Define("cos_angle_grandmother_PVrecoBreco", cosangle, discriminantvariables["cos_angle_grandmother_PVrecoBreco"])
                                                .Define("cos_angle_grandmother_BeamBreco", cosangle, discriminantvariables["cos_angle_grandmother_BeamBreco"])
                                                .Define("cos_angle_grandmother_PVrecoJpsireco", cosangle, discriminantvariables["cos_angle_grandmother_PVrecoJpsireco"])
                                                .Define("cos_angle_grandmother_BeamJpsireco", cosangle, discriminantvariables["cos_angle_grandmother_BeamJpsireco"])
                                                .Define("cos_angle_grandmother_3mureco", cosangle3mu, discriminantvariables["cos_angle_grandmother_3mureco"])
                                                .Filter(filter);

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
                branch2bewritten.push_back(i->first.data());
                i++;
            }
            for (vector<MarkedNames>::iterator variterator = var2read.begin(); variterator != var2read.end(); ++variterator)
            {
                branch2bewritten.push_back(variterator->name.Data());
                if ((variterator->geneflag)!= (-1)) branch2bewritten.push_back(variterator->genename.Data());
            }

            // Writing the new RDF with discriminants and their variables
            TString Tree2bewritten = TString::Format("%s_%s", fileiterator->labeltxt.Data(), treeiterator->name.Data());
            BtoMu_discriminants.Snapshot(Tree2bewritten.Data(), "Discriminants.root", branch2bewritten, snapopt);

            if (debugflag==false)
            {
                TFile ComparisonFile("Discriminants.root", "update");
                TTree *ReadTree = (TTree*)ComparisonFile.Get(Tree2bewritten.Data());
                ROOT::RDataFrame BtoMu(Tree2bewritten.Data(), "Discriminants.root");

                TCanvas *c0 = new TCanvas(TString::Format("c%s", fileiterator->name.Data()), TString::Format("c%s", fileiterator->name.Data()), 1366, 768);
                c0->Print(TString::Format("%s[", outhistcol.Data()));

                for (vector<MarkedNames>::iterator variterator = var2read.begin(); variterator != var2read.end(); ++variterator)
                {
                    Float_t recomin = ReadTree->GetMinimum(variterator->name.Data());
                    Float_t recomax = ReadTree->GetMaximum(variterator->name.Data());
                    Float_t genemin = ReadTree->GetMinimum(variterator->genename.Data());
                    Float_t genemax = ReadTree->GetMaximum(variterator->genename.Data());
                    Float_t totmin = min(recomin, genemin);
                    Float_t totmax = max(recomax, genemax);

                    // Filling histogram for generated variables
                    if ((variterator->geneflag) ==(-1) && (variterator->flag)==0)
                    {
                        histgene = BtoMu.Fill<Float_t>(TH1F(variterator->name.Data(), variterator->name.Data(), 50,
                                recomin - abs(recomin / 10), recomax + abs(recomax / 10)), {variterator->name.Data()});
                    } 

                    // Filling histogram for reco variables
                    else if ((variterator->geneflag) ==(-1) && (variterator->flag)==1)
                    {
                        histreco = BtoMu.Fill<Float_t>(TH1F(variterator->name.Data(), variterator->name.Data(), 50,
                                recomin - abs(recomin / 10), recomax + abs(recomax / 10)), {variterator->name.Data()});
                    }
                    else
                    {
                        // Filling histogram for generated variables
                        histgene = BtoMu.Fill<Float_t>(TH1F(variterator->genename.Data(), variterator->genename.Data(), 50,
                                    totmin - abs(totmin / 10), totmax + abs(totmax / 10)), {variterator->genename.Data()});

                        // Filling histogram for reco variables
                        histreco = BtoMu.Fill<Float_t>(TH1F(variterator->name.Data(), variterator->name.Data(), 50,
                                    totmin - abs(totmin / 10), totmax + abs(totmax / 10)), {variterator->name.Data()});

                        // Defining a column with the ratio reco/gene and drawing it as a histogram
                        auto BtoMuRatio = BtoMu.Define(TString::Format("%sover%s", variterator->name.Data(),variterator->genename.Data()),
                                                    [](Float_t x, Float_t y){return x/y;},
                                                    {TString::Format("%s", variterator->name.Data()).Data(),
                                                        TString::Format("%s", variterator->genename.Data()).Data()});
                        histratio = BtoMuRatio.Fill<Float_t>(TH1F
                                    (TString::Format("%sover%s", variterator->name.Data(),variterator->genename.Data()),
                                    TString::Format("%sover%s", variterator->name.Data(),variterator->genename.Data()),
                                    100, 0.9, 1.1), 
                                    {TString::Format("%sover%s", variterator->name.Data(), variterator->genename.Data()).Data()});

                        // Filling a 2D-Histogram with reco vs gene
                        scatter = BtoMuRatio.Fill<Float_t, Float_t>(TH2F
                                    (TString::Format("%s vs %s", variterator->name.Data(),variterator->genename.Data()),
                                    TString::Format("%s vs %s", variterator->name.Data(),variterator->genename.Data()),
                                    50, recomin, recomax, 50, genemin, genemax), 
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
                            histreco->SetTitle(TString::Format("Discriminant: %s", variterator->labeltxt.Data()));
                            histreco->GetXaxis()->SetTitle(TString::Format("%s[%s]", variterator->labeltxt.Data(), variterator->dime.Data()));
                        }
                        histreco->GetYaxis()->SetTitle("Normalized occurences [a.u.]");
                        histreco->SetLineColorAlpha(kRed, 1);
                        histreco->SetFillStyle(1001);
                        histreco->SetFillColor(kRed);
                        histreco->SetFillColorAlpha(kRed, 0.5);
                        histreco->SetMarkerStyle(kFullSquare);
                        histreco->SetMarkerColor(kRed);
                        histreco->SetMarkerSize(1.5);
                        histreco->DrawNormalized("HF P");
                        c0->Update();

                        //Adjusting the stats
                        TPaveStats *recostats = (TPaveStats *)c0->GetPrimitive("stats");
                        recostats->SetX1NDC(0.1);
                        recostats->SetX2NDC(0.28);
                        recostats->SetY1NDC(0.7);
                        recostats->SetY2NDC(0.8);
                        recostats->Draw("same");
                        c0->Modified();
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
                        histgene->GetYaxis()->SetTitle("Normalized occurences [a.u.]");
                        histgene->SetStats(kFALSE);
                        histgene->SetLineColorAlpha(kBlue, 1);
                        histgene->SetFillStyle(1001);
                        histgene->SetFillColor(kBlue);
                        histgene->SetFillColorAlpha(kBlue, 0.5);
                        histgene->SetMarkerStyle(kFullTriangleUp);
                        histgene->SetMarkerColor(kBlue);
                        histgene->SetMarkerSize(1.5);
                        histgene->DrawNormalized("SAME HF P");
                    }

                    //Adjusting the legend
                    auto *legend = new TLegend(0.1, 0.8, 0.28, 0.9);
                    legend->SetHeader(TString::Format("%s", fileiterator->name.Data()), "C");
                    if (variterator->flag==1) legend->AddEntry(variterator->name, variterator->name, "pf");
                    if (variterator->flag==0) legend->AddEntry(variterator->genename, variterator->genename, "pf");
                    legend->SetTextSize(0.025);
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
                            histratio->SetTitle(TString::Format("%s over %s(GEN)", variterator->labeltxt.Data(),  variterator->labeltxt.Data()));
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
                            scatter->SetTitle(TString::Format("2D histogram of %s and %s(GEN)", variterator->labeltxt.Data(),  variterator->labeltxt.Data()));
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
                ComparisonFile.Close();
                c0->Print(TString::Format("%s]", outhistcol.Data()));
                c0->Clear();
            }
        }
        inputfile.Close();
    }
    return 0;
}