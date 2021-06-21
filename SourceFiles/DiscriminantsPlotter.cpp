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
 */

#include "HeaderFiles/DiscriminantsPlotter.h"

int DiscriminantsPlotter(vector<MarkedNames> file2read,
                         vector<MarkedNames> trees2read,
                         vector<const char *> *var2bestored,
                         vector<VarToBePlotted> *vartobeplotted,
                         bool debug) 

{
    //Enabling multi-threading
    if (debug==false) ROOT::EnableImplicitMT();

    //Removing old output file
    TFile output("ManipulatedVariables.root","recreate");
    output.Close();

    //Defining histograms
    ROOT::RDF::RResultPtr<TH1F> histang;

    //Opening the new file where are stored new discriminants
    for (vector<MarkedNames>::iterator fileiterator = file2read.begin(); fileiterator != file2read.end(); ++fileiterator)
    {
        cout << fileiterator->labeltxt.Data();

        // Opening the histogram collection
        TString outhistpdf = TString::Format("OutputFiles/%s_Manipulated_Variables.pdf", fileiterator->labeltxt.Data());

        TCanvas *c0 = new TCanvas(TString::Format("ca%s", fileiterator->name.Data()), TString::Format("ca%s", fileiterator->name.Data()), 1366, 768);
        c0->Print(TString::Format("%s[", outhistpdf.Data()));

        for (vector<MarkedNames>::iterator treeiterator = trees2read.begin(); treeiterator != trees2read.end(); ++treeiterator)
        {
            //Opening a dataframe with kinematics
            ROOT::RDataFrame BtoMu(TString::Format("%s_%s", fileiterator->labeltxt.Data(), treeiterator->name.Data()), "Discriminants.root");
            auto b2munode = std::make_unique<ROOT::RDF::RNode>(BtoMu);

            //Looping on variables that the user wants to plot and to be stored
            vector<string> vartobestored;
            vector<VarToBePlotted>::iterator plotvar = vartobeplotted->begin();
            while (plotvar != vartobeplotted->end())
            {
                // Defining a column with the required variable
                if (BtoMu.HasColumn(plotvar->varname) == false)
                    b2munode = std::make_unique<ROOT::RDF::RNode>(b2munode->Define(plotvar->varname, plotvar->varexpression));
                vartobestored.push_back(plotvar->varname);
                plotvar++;
            }
            vector<const char *>::iterator storevar = var2bestored->begin();
            while (storevar != var2bestored->end())
            {
                // Defining a column with the required variable
                if (BtoMu.HasColumn(*storevar) == true)
                {
                    if (b2munode->HasColumn(*storevar) == false) b2munode = std::make_unique<ROOT::RDF::RNode>(b2munode->Define(*storevar, *storevar));
                    vartobestored.push_back(*storevar);
                }
                storevar++;                
            }

            // Modifying write options of the root data frame to overwrite other trees
            ROOT::RDF::RSnapshotOptions snapopt;
            snapopt.fMode = "UPDATE";
            snapopt.fOverwriteIfExists = "TRUE";
            b2munode->Snapshot(TString::Format("%s_%s", fileiterator->labeltxt.Data(), treeiterator->name.Data()), "ManipulatedVariables.root", vartobestored, snapopt);
            
            if (BtoMu.HasColumn("is_signal_channel")==true)
            {
                auto b2munode_signal = b2munode->Filter("is_signal_channel==1");
                auto b2munode_normal = b2munode->Filter("is_signal_channel==0");
                b2munode_signal.Snapshot(TString::Format("%s_%s_signal", fileiterator->labeltxt.Data(), treeiterator->name.Data()), "ManipulatedVariables.root", vartobestored, snapopt);
                b2munode_normal.Snapshot(TString::Format("%s_%s_normal", fileiterator->labeltxt.Data(), treeiterator->name.Data()), "ManipulatedVariables.root", vartobestored, snapopt);
            }

            TFile Manfile("ManipulatedVariables.root", "update");
            TTree *Readtree = (TTree *)Manfile.Get(TString::Format("%s_%s", fileiterator->labeltxt.Data(), treeiterator->name.Data())); 
            cout << ": sample with " << Readtree->GetEntries() << " entries" << endl << endl;

            //Disabling implicit multithreading for debugging
            if (debug == true) 
            {
                b2munode = std::make_unique<ROOT::RDF::RNode>(b2munode->Define("Pisa_phi", "(Float_t)b_c_pisa_threemomentum.Phi()"));
                auto d1 = b2munode->Display({"mu1_grandmother_phi", "Pisa_phi", "phi_breco_beam", "Bphi", "phi_3mumomenta_reco"}, 100);
                d1->Print();
                cout << endl;
            }

            plotvar = vartobeplotted->begin();
            while (plotvar != vartobeplotted->end())
            {
                // Opening the freshly written file
                TFile ManipulatedFile("ManipulatedVariables.root", "update");
                TTree *ReadTree = (TTree *)ManipulatedFile.Get(TString::Format("%s_%s", fileiterator->labeltxt.Data(), treeiterator->name.Data()));

                // Getting (min,max) to show real things
                Float_t min = ReadTree->GetMinimum(plotvar->varname.data());
                Float_t max = ReadTree->GetMaximum(plotvar->varname.data());

                if (plotvar->xminlim != 0 || plotvar->xmaxlim != 0)
                    histang = b2munode->Fill<Float_t>(TH1F(plotvar->varname.data(), plotvar->varname.data(),
                                                           50, plotvar->xminlim, plotvar->xmaxlim),
                                                      {plotvar->varname.data()});

                else
                    histang = b2munode->Fill<Float_t>(TH1F(plotvar->varname.data(), plotvar->varname.data(),
                                                           100, min, max),
                                                      {plotvar->varname.data()});

                if (debug == false) cout << "RMS of "<< plotvar->varname << " " << histang->GetRMS() << " +- " << histang->GetRMSError() << endl;

                if (plotvar->tobeprinted == true)
                {
                    // Plotting the histogram of these variables
                    c0->cd();
                    histang->SetTitle(plotvar->plottitle.data());
                    histang->GetXaxis()->SetTitle(plotvar->plotxlabel.data());
                    histang->GetYaxis()->SetTitle("Occurences");
                    histang->SetLineColorAlpha(kRed, 1);
                    histang->SetFillStyle(1001);
                    histang->SetFillColor(kRed);
                    histang->SetFillColorAlpha(kRed, 0.5);
                    histang->SetMarkerStyle(kFullSquare);
                    histang->SetMarkerColor(kRed);
                    histang->SetMarkerSize(1.5);
                    histang->Draw("HF P");
                    c0->SetLogy();
                    c0->Update();

                    //Adjusting the stats
                    TPaveStats *recostats = (TPaveStats *)c0->GetPrimitive("stats");
                    recostats->SetX1NDC(0.75);
                    recostats->SetX2NDC(0.93);
                    recostats->SetY1NDC(0.78);
                    recostats->SetY2NDC(0.9);
                    recostats->Draw("same");
                    recostats->SetOptStat(112111);
                    c0->Modified();
                    c0->Print(TString::Format("%s", outhistpdf.Data()));
                    c0->Print(TString::Format("%s%s_%s.png", plotvar->wheretoplot.data(), fileiterator->labeltxt.Data(), plotvar->varname.data()));
                    c0->Clear();
                    histang->Reset("ICESM");
                }

                // Drawing Comparison between variables
                if (plotvar->comparisonflag == true)
                {
                    ROOT::RDF::RResultPtr<TH1F> histcompare, histcompare_ratio;
                    ROOT::RDF::RResultPtr<TH2F> histcompare_scatter;
                    TCanvas canvas_comparison = TCanvas("canvas_comparison", "canvas_comparison", 1366, 768);
                    TCanvas canvas_comparison_ratio = TCanvas("canvas_comparison_ratio", "canvas_comparison_ratio", 1366, 768);
                    TCanvas canvas_comparison_scatter = TCanvas("canvas_comparison_scatter", "canvas_comparison_scatter", 1366, 768);
                    auto *legend = new TLegend(0.1, 0.8, 0.28, 0.9);
                    for (vector<VarToBePlotted>::iterator compit = vartobeplotted->begin(); compit < vartobeplotted->end(); compit++)
                    {
                        if ((plotvar->comparisonlabel) == (compit->comparisonlabel) && plotvar->comparisonlabel.compare("") != 0)
                        {
                            histcompare = b2munode->Fill<Float_t>(TH1F( TString::Format("%s_comparison", compit->varname.data()), 
                                                                        TString::Format("%s_comparison", compit->varname.data()),
                                                                        100, compit->xminlim, compit->xmaxlim), 
                                                                  {compit->varname.data()});
                            canvas_comparison.cd();
                            if (compit->comparisontitle.compare("")) histcompare->SetTitle(compit->comparisontitle.data());
                            if (compit->comparisonlabel.compare("")) histcompare->GetXaxis()->SetTitle(compit->comparisonlabel.data());
                            histcompare->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
                            histcompare->SetStats(false);
                            histcompare->SetFillStyle(1001);
                            histcompare->SetMarkerStyle(kFullSquare);
                            histcompare->SetMarkerSize(1.5);
                            if (compit == vartobeplotted->begin()) histcompare->DrawNormalized("HF PLC PMC");
                            else histcompare->DrawNormalized("SAME HF PLC PMC");
                            canvas_comparison.SetLogy();
                            canvas_comparison.Update();

                            //Adjusting the legend
                            legend->SetHeader(TString::Format("%s", fileiterator->labeltxt.Data()), "C");
                            legend->AddEntry(TString::Format("%s_comparison", compit->varname.data()), compit->varname.data(), "PLC PMC");
                            legend->SetTextSize(0.025);
                            legend->Draw("SAME");
                            TLegendEntry *header = (TLegendEntry *)legend->GetListOfPrimitives()->First();
                            header->SetTextSize(0.02);
                            canvas_comparison.Update();
                            //compit->comparisonflag = false;

                            if ( (compit->comparisonrationame.compare("")) != 0 && (b2munode->HasColumn(compit->comparisonrationame)) == false)
                            {
                                b2munode = std::make_unique<ROOT::RDF::RNode>(b2munode->Define(compit->comparisonrationame, compit->comparisonratioexpr));
                                histcompare_ratio = b2munode->Fill<Float_t>(TH1F(compit->comparisonrationame.data(), compit->comparisonrationame.data(), 100, 0, 2),
                                                                                 {compit->comparisonrationame.data()});                                
                                canvas_comparison_ratio.cd();
                                histcompare_ratio->SetTitle(compit->comparisonratiotitle.data());
                                histcompare_ratio->SetStats(kTRUE);
                                histcompare_ratio->SetLineColorAlpha(kBlue, 1);
                                histcompare_ratio->SetFillStyle(1001);
                                histcompare_ratio->SetFillColor(kBlue);
                                histcompare_ratio->SetFillColorAlpha(kBlue, 0.8);
                                histcompare_ratio->SetMarkerStyle(kFullSquare);
                                histcompare_ratio->SetMarkerColor(kBlue);
                                histcompare_ratio->SetMarkerSize(1);
                                histcompare_ratio->GetXaxis()->SetTitle(compit->comparisonratioxlabel.data());
                                histcompare_ratio->GetXaxis()->SetTitleOffset(1.4);
                                histcompare_ratio->GetXaxis()->SetTitleSize(0.028);
                                histcompare_ratio->GetYaxis()->SetTitle("Occurences");
                                histcompare_ratio->Draw("HF P");
                                canvas_comparison_ratio.Update();

                                //Adjusting the stats
                                TPaveStats *ratiostats = (TPaveStats *)histcompare_ratio->GetListOfFunctions()->FindObject("stats");
                                ratiostats->SetName(compit->comparisonrationame.data());
                                ratiostats->SetOptStat(112211);
                                ratiostats->SetX1NDC(0.1);
                                ratiostats->SetX2NDC(0.28);
                                ratiostats->SetY1NDC(0.66);
                                ratiostats->SetY2NDC(0.78);
                                ratiostats->Draw("same");

                                //Adjusting the legend
                                auto *legend_comparison_ratio = new TLegend(0.1, 0.78, 0.28, 0.9);
                                legend_comparison_ratio->Clear();
                                legend_comparison_ratio->SetX1NDC(0.1);
                                legend_comparison_ratio->SetX2NDC(0.28);
                                legend_comparison_ratio->SetY1NDC(0.78);
                                legend_comparison_ratio->SetY2NDC(0.9);
                                legend_comparison_ratio->SetHeader(TString::Format("%s", fileiterator->labeltxt.Data()), "C");
                                legend_comparison_ratio->AddEntry(compit->comparisonrationame.data(), compit->comparisonratioxlabel.data(), "pf");
                                legend_comparison_ratio->SetTextSize(0.02);
                                legend_comparison_ratio->Draw("same");
                                header = (TLegendEntry *)legend_comparison_ratio->GetListOfPrimitives()->First();
                                header->SetTextSize(0.02);
                                header->Draw("same");
                                canvas_comparison_ratio.Update();
                                canvas_comparison_ratio.Print(TString::Format("%s%s_%s.png", plotvar->wheretoplot.data(), fileiterator->labeltxt.Data(), compit->comparisonrationame.data()));;
                                histcompare_ratio->Reset("ICESM");
                                canvas_comparison_ratio.Clear();
                            }

                            if ( (compit->comparisonscattername.compare("")) != 0)
                            {
                                Float_t xmin = b2munode->Min(compit->comparisonscattervars[0]).GetValue();
                                Float_t xmax = b2munode->Max(compit->comparisonscattervars[0]).GetValue();
                                Float_t ymin = b2munode->Min(compit->comparisonscattervars[1]).GetValue();
                                Float_t ymax = b2munode->Max(compit->comparisonscattervars[1]).GetValue();
                                histcompare_scatter = b2munode->Fill<Float_t, Float_t>(TH2F(compit->comparisonscattername.data(), compit->comparisonscattername.data(), 
                                                                                            100, xmin, xmax,
                                                                                            100, ymin, ymax),
                                                                                            compit->comparisonscattervars);                                
                                canvas_comparison_scatter.cd();
                                histcompare_scatter->SetTitle(compit->comparisonscattertitle.data());
                                histcompare_scatter->SetStats(kTRUE);
                                histcompare_scatter->SetMarkerSize(1);
                                histcompare_scatter->GetXaxis()->SetTitle(compit->comparisonscatterxlabel.data());
                                histcompare_scatter->GetXaxis()->SetTitleOffset(1.2);
                                histcompare_scatter->GetYaxis()->SetTitle(compit->comparisonscatterylabel.data());
                                histcompare_scatter->Draw("colz");
                                canvas_comparison_scatter.Update();

                                //Adjusting the stats
                                TPaveStats *scatterstats = (TPaveStats *)histcompare_scatter->GetListOfFunctions()->FindObject("stats");
                                scatterstats->SetName(compit->comparisonscattername.data());
                                scatterstats->SetOptStat(112011);
                                scatterstats->SetX1NDC(0.1);
                                scatterstats->SetX2NDC(0.28);
                                scatterstats->SetY1NDC(0.66);
                                scatterstats->SetY2NDC(0.78);
                                scatterstats->Draw("same");
                                canvas_comparison_scatter.Modified();
                                canvas_comparison_scatter.Print(TString::Format("%s%s_%s.png", plotvar->wheretoplot.data(), fileiterator->labeltxt.Data(), compit->comparisonscattername.data()));
                                histcompare_scatter->Reset("ICESM");
                                canvas_comparison_scatter.Clear();
                            }
                        }
                    }
                    canvas_comparison.Print(TString::Format("%s%s_%s.png", plotvar->wheretoplot.data(), fileiterator->labeltxt.Data(), plotvar->comparisonlabel.data()));
                    histcompare->Reset("ICESM");
                    canvas_comparison.Clear();
                }

                plotvar++;
            }

            // Storing histograms to be comparedeta
            ROOT::RDF::RResultPtr<TH1F> hetagenepvsvreco, hetagenepvrecosvgene, hetagenepvgenesvreco;
            hetagenepvsvreco = b2munode->Fill<Float_t>(TH1F("hetagenepvsvreco", "hetagenepvsvreco", 100, -0.7, 0.7), {"eta_genePVSVreco"});
            hetagenepvrecosvgene = b2munode->Fill<Float_t>(TH1F("hetagenepvrecosvgene", "hetagenepvrecosvgene", 100, -0.7, 0.7), {"eta_genePVrecoSVgene"});
            hetagenepvgenesvreco = b2munode->Fill<Float_t>(TH1F("hetagenepvgenesvreco", "hetagenepvgenesvreco", 100, -0.7, 0.7), {"eta_genePVgeneSVreco"});

            c0->cd();
            c0->SetLogy();
            hetagenepvsvreco->SetTitle(TString::Format("%s: DeltaEta angular resolution", fileiterator->labeltxt.Data()));
            hetagenepvsvreco->GetYaxis()->SetTitle("Normalized occurences [a.u.]");
            hetagenepvsvreco->SetStats(kFALSE);
            hetagenepvsvreco->SetLineColorAlpha(kRed, 1);
            hetagenepvsvreco->SetFillStyle(1001);
            hetagenepvsvreco->SetFillColor(kRed);
            hetagenepvsvreco->SetFillColorAlpha(kRed, 0.5);
            hetagenepvsvreco->SetMarkerStyle(kFullSquare);
            hetagenepvsvreco->SetMarkerColor(kRed);
            hetagenepvsvreco->SetMarkerSize(1.5);
            hetagenepvsvreco->DrawNormalized("HF P");
            c0->Update();

            hetagenepvrecosvgene->GetYaxis()->SetTitle("Normalized occurences [a.u.]");
            hetagenepvrecosvgene->SetStats(kFALSE);
            hetagenepvrecosvgene->SetLineColorAlpha(kBlue, 1);
            hetagenepvrecosvgene->SetFillStyle(1001);
            hetagenepvrecosvgene->SetFillColor(kBlue);
            hetagenepvrecosvgene->SetFillColorAlpha(kBlue, 0.5);
            hetagenepvrecosvgene->SetMarkerStyle(kFullSquare);
            hetagenepvrecosvgene->SetMarkerColor(kBlue);
            hetagenepvrecosvgene->SetMarkerSize(1.5);
            hetagenepvrecosvgene->DrawNormalized("SAME HF P");
            c0->Update();

            hetagenepvgenesvreco->GetYaxis()->SetTitle("Normalized occurences [a.u.]");
            hetagenepvgenesvreco->SetStats(kFALSE);
            hetagenepvgenesvreco->SetLineColorAlpha(kGreen, 1);
            hetagenepvgenesvreco->SetFillStyle(1001);
            hetagenepvgenesvreco->SetFillColor(kGreen);
            hetagenepvgenesvreco->SetFillColorAlpha(kGreen, 0.5);
            hetagenepvgenesvreco->SetMarkerStyle(kFullSquare);
            hetagenepvgenesvreco->SetMarkerColor(kGreen);
            hetagenepvgenesvreco->SetMarkerSize(1.5);
            hetagenepvgenesvreco->DrawNormalized("SAME HF P");
            c0->Update();

            //Adjusting the legend
            auto *legend = new TLegend(0.1, 0.8, 0.28, 0.9);
            legend->SetHeader(TString::Format("%s", fileiterator->name.Data()), "C");
            legend->AddEntry("hetagenepvsvreco", "hetagenepvsvreco", "pf");
            legend->AddEntry("hetagenepvrecosvgene", "hetagenepvrecosvgene", "pf");
            legend->AddEntry("hetagenepvgenesvreco", "hetagenepvgenesvreco", "pf");
            legend->SetTextSize(0.025);
            legend->Draw("same");
            TLegendEntry *header = (TLegendEntry *)legend->GetListOfPrimitives()->First();
            header->SetTextSize(0.02);
            c0->Update();
            c0->Print(TString::Format("%s", outhistpdf.Data()));
            c0->Print(TString::Format("OutputFiles/PNGPlots/Angles/%ssuperimposedhistogrameta.png", fileiterator->labeltxt.Data()));
            c0->Clear();

            // Storing histograms to be comparedphi
            ROOT::RDF::RResultPtr<TH1F> hphigenepvsvreco, hphigenepvrecosvgene, hphigenepvgenesvreco;
            hphigenepvsvreco = b2munode->Fill<Float_t>(TH1F("hphigenepvsvreco", "hphigenepvsvreco", 100, -0.7, 0.7), {"phi_genePVSVreco"});
            hphigenepvrecosvgene = b2munode->Fill<Float_t>(TH1F("hphigenepvrecosvgene", "hphigenepvrecosvgene", 100, -0.7, 0.7), {"phi_genePVrecoSVgene"});
            hphigenepvgenesvreco = b2munode->Fill<Float_t>(TH1F("hphigenepvgenesvreco", "hphigenepvgenesvreco", 100, -0.7, 0.7), {"phi_genePVgeneSVreco"});

            hphigenepvsvreco->SetTitle(TString::Format("%s: DeltaPhi angular resolution", fileiterator->labeltxt.Data()));
            hphigenepvsvreco->GetYaxis()->SetTitle("Normalized occurences [a.u.]");
            hphigenepvsvreco->SetStats(kFALSE);
            hphigenepvsvreco->SetLineColorAlpha(kRed, 1);
            hphigenepvsvreco->SetFillStyle(1001);
            hphigenepvsvreco->SetFillColor(kRed);
            hphigenepvsvreco->SetFillColorAlpha(kRed, 0.5);
            hphigenepvsvreco->SetMarkerStyle(kFullSquare);
            hphigenepvsvreco->SetMarkerColor(kRed);
            hphigenepvsvreco->SetMarkerSize(1.5);
            hphigenepvsvreco->DrawNormalized("HF P");
            c0->Update();

            hphigenepvrecosvgene->GetYaxis()->SetTitle("Normalized occurences [a.u.]");
            hphigenepvrecosvgene->SetStats(kFALSE);
            hphigenepvrecosvgene->SetLineColorAlpha(kBlue, 1);
            hphigenepvrecosvgene->SetFillStyle(1001);
            hphigenepvrecosvgene->SetFillColor(kBlue);
            hphigenepvrecosvgene->SetFillColorAlpha(kBlue, 0.5);
            hphigenepvrecosvgene->SetMarkerStyle(kFullSquare);
            hphigenepvrecosvgene->SetMarkerColor(kBlue);
            hphigenepvrecosvgene->SetMarkerSize(1.5);
            hphigenepvrecosvgene->DrawNormalized("SAME HF P");
            c0->Update();

            hphigenepvgenesvreco->GetYaxis()->SetTitle("Normalized occurences [a.u.]");
            hphigenepvgenesvreco->SetStats(kFALSE);
            hphigenepvgenesvreco->SetLineColorAlpha(kGreen, 1);
            hphigenepvgenesvreco->SetFillStyle(1001);
            hphigenepvgenesvreco->SetFillColor(kGreen);
            hphigenepvgenesvreco->SetFillColorAlpha(kGreen, 0.5);
            hphigenepvgenesvreco->SetMarkerStyle(kFullSquare);
            hphigenepvgenesvreco->SetMarkerColor(kGreen);
            hphigenepvgenesvreco->SetMarkerSize(1.5);
            hphigenepvgenesvreco->DrawNormalized("SAME HF P");
            c0->Update();

            //Adjusting the legend
            auto *legend2 = new TLegend(0.1, 0.8, 0.28, 0.9);
            legend2->SetHeader(TString::Format("%s", fileiterator->name.Data()), "C");
            legend2->AddEntry("hphigenepvsvreco", "hphigenepvsvreco", "pf");
            legend2->AddEntry("hphigenepvrecosvgene", "hphigenepvrecosvgene", "pf");
            legend2->AddEntry("hphigenepvgenesvreco", "hphigenepvgenesvreco", "pf");
            legend2->SetTextSize(0.025);
            legend2->Draw("same");
            TLegendEntry *header2 = (TLegendEntry *)legend->GetListOfPrimitives()->First();
            header2->SetTextSize(0.02);
            c0->Update();
            c0->Print(TString::Format("%s", outhistpdf.Data()));
            c0->Print(TString::Format("OutputFiles/PNGPlots/Angles/%ssuperimposedhistogramphi.png", fileiterator->labeltxt.Data()));
            c0->Clear();

            // Scatter plot to understand why not zero eta_gene vs eta_pvgenesvgene
            ROOT::RDF::RResultPtr<TH2F> heta_genepvsvgen_scatter;
            heta_genepvsvgen_scatter = b2munode->Fill<Float_t, Float_t>(TH2F("heta_genepvsvgen_scatter","heta_genepvsvgen_scatter", 100, -3.5, 3.5, 100, -3.5, 3.5),
                                                                        {"eta_gene", "mu1_grandmother_eta"});
            heta_genepvsvgen_scatter->SetTitle("#eta(#vec{PV_{gene}}#vec{SV_{gene}}) - #eta_{grandmother}");
            heta_genepvsvgen_scatter->GetXaxis()->SetTitle("#eta(#vec{PV_{gene}}#vec{SV_{gene}})");
            heta_genepvsvgen_scatter->GetYaxis()->SetTitle("#eta_{grandmother}");
            heta_genepvsvgen_scatter->SetLineColorAlpha(kRed, 1);
            heta_genepvsvgen_scatter->SetFillStyle(1001);
            heta_genepvsvgen_scatter->SetFillColor(kRed);
            heta_genepvsvgen_scatter->SetFillColorAlpha(kRed, 0.5);
            heta_genepvsvgen_scatter->SetMarkerStyle(kFullSquare);
            heta_genepvsvgen_scatter->SetMarkerColor(kRed);
            heta_genepvsvgen_scatter->SetMarkerSize(.5);
            heta_genepvsvgen_scatter->Draw("colz");
            c0->SetLogy(0);
            c0->Update();

            //Adjusting the stats
            TPaveStats *heta_genepvsvgen_scatter_stats = (TPaveStats *)c0->GetPrimitive("stats");
            heta_genepvsvgen_scatter_stats->SetX1NDC(0.1);
            heta_genepvsvgen_scatter_stats->SetX2NDC(0.28);
            heta_genepvsvgen_scatter_stats->SetY1NDC(0.78);
            heta_genepvsvgen_scatter_stats->SetY2NDC(0.9);
            heta_genepvsvgen_scatter_stats->Draw("same");
            heta_genepvsvgen_scatter_stats->SetOptStat(112011);
            c0->Modified();
            c0->Print(TString::Format("OutputFiles/PNGPlots/Angles/%s_eta_genepvsvgen_scatter.png", fileiterator->labeltxt.Data()));
            c0->Clear();
            
            //Profile histogram
            b2munode = std::make_unique<ROOT::RDF::RNode>(b2munode->Define("abs_mu1_grandmother_eta", "abs(mu1_grandmother_eta)"));
            ROOT::RDF::RResultPtr<TProfile> hprofile_eta_res_grandmother_beambreco;
            hprofile_eta_res_grandmother_beambreco = b2munode->Fill<Float_t, Float_t>(TProfile("hprofile_eta_res_grandmother_beambreco", "hprofile_eta_res_grandmother_beambreco",
                                                                                                8, 0, 2.4, -0.1, 0.1), 
                                                                                                {"abs_mu1_grandmother_eta", "eta_resolution_grandmother_beambreco"});
            hprofile_eta_res_grandmother_beambreco->SetTitle("Different resolutions for various eta regions");
            hprofile_eta_res_grandmother_beambreco->GetXaxis()->SetTitle("|#eta_{Grandmother}|[]");
            hprofile_eta_res_grandmother_beambreco->GetYaxis()->SetTitle("Resolution");
            hprofile_eta_res_grandmother_beambreco->GetYaxis()->SetRangeUser(-0.006, 0.008);
            hprofile_eta_res_grandmother_beambreco->SetErrorOption("");
            hprofile_eta_res_grandmother_beambreco->SetStats(false);
            hprofile_eta_res_grandmother_beambreco->SetLineColorAlpha(kRed, 1);
            hprofile_eta_res_grandmother_beambreco->SetMarkerStyle(kFullSquare);
            hprofile_eta_res_grandmother_beambreco->SetMarkerColor(kRed);
            hprofile_eta_res_grandmother_beambreco->Draw();
            c0->SetLogy(0);
            c0->Update();

            ROOT::RDF::RResultPtr<TProfile> hprofile_eta_res_grandmother_3mu;
            hprofile_eta_res_grandmother_3mu = b2munode->Fill<Float_t, Float_t>(TProfile("hprofile_eta_res_grandmother_3mu", "hprofile_eta_res_grandmother_3mu", 
                                                                                         8, 0, 2.4, -0.1, 0.1), 
                                                                                         {"abs_mu1_grandmother_eta", "eta_resolution_grandmother_3mumomentareco"});
            hprofile_eta_res_grandmother_3mu->GetXaxis()->SetTitle("|#eta_{Grandmother}|[]");
            hprofile_eta_res_grandmother_3mu->GetYaxis()->SetTitle("Resolution");
            hprofile_eta_res_grandmother_3mu->GetYaxis()->SetRangeUser(-0.006, 0.008);
            hprofile_eta_res_grandmother_3mu->SetErrorOption("");
            hprofile_eta_res_grandmother_3mu->SetStats(false);
            hprofile_eta_res_grandmother_3mu->SetLineColorAlpha(kBlue, 1);
            hprofile_eta_res_grandmother_3mu->SetMarkerStyle(kFullSquare);
            hprofile_eta_res_grandmother_3mu->SetMarkerColor(kBlue);
            hprofile_eta_res_grandmother_3mu->Draw("same");
            c0->SetLogy(0);
            c0->Update();

            //Adjusting the legend
            auto *hprofile_legend = new TLegend(0.1, 0.75, 0.26, 0.9);
            hprofile_legend->SetHeader(TString::Format("%s", fileiterator->labeltxt.Data()), "C");
            hprofile_legend->AddEntry("hprofile_eta_res_grandmother_beambreco", "#eta_{Grandmother}-#eta_{BeamBreco}");
            hprofile_legend->AddEntry("hprofile_eta_res_grandmother_3mu", "#eta_{Grandmother}-#eta_{3#mureco}");
            //hprofile_legend->AddEntry("hetagenepvgenesvreco", "hetagenepvgenesvreco", "pf");
            hprofile_legend->SetTextSize(0.025);
            hprofile_legend->Draw("same");
            header = (TLegendEntry *)hprofile_legend->GetListOfPrimitives()->First();
            header->SetTextSize(0.018);
            c0->Update();

            c0->Print("OutputFiles/ProfileTrial_etaresolution.png");
            c0->Clear();
            for (int i = 1; i<=8; i++)
            {
                if (debug == false) cout << "Errore Eta beam-breco Bin : " << i << " " << hprofile_eta_res_grandmother_beambreco->GetBinError(i) << endl;
                if (debug == false) cout << "Errore Eta 3mureco Bin : " << i << " " << hprofile_eta_res_grandmother_3mu->GetBinError(i) << endl;
            }

            //Profile histogram for phi
            ROOT::RDF::RResultPtr<TProfile> hprofile_phi_res_grandmother_beambreco;
            hprofile_phi_res_grandmother_beambreco = b2munode->Fill<Float_t, Float_t>(TProfile("hprofile_phi_res_grandmother_beambreco", "hprofile_phi_res_grandmother_beambreco",
                                                                                                8, 0, 2.4, -0.1, 0.1), 
                                                                                                {"abs_mu1_grandmother_eta", "phi_resolution_grandmother_beambreco"});
            hprofile_phi_res_grandmother_beambreco->SetTitle("Different resolutions for various eta regions");
            hprofile_phi_res_grandmother_beambreco->GetXaxis()->SetTitle("|#eta_{Grandmother}|[]");
            hprofile_phi_res_grandmother_beambreco->GetYaxis()->SetTitle("Resolution");
            hprofile_phi_res_grandmother_beambreco->GetYaxis()->SetRangeUser(-0.006, 0.008);
            hprofile_phi_res_grandmother_beambreco->SetErrorOption("");
            hprofile_phi_res_grandmother_beambreco->SetStats(false);
            hprofile_phi_res_grandmother_beambreco->SetLineColorAlpha(kRed, 1);
            hprofile_phi_res_grandmother_beambreco->SetMarkerStyle(kFullSquare);
            hprofile_phi_res_grandmother_beambreco->SetMarkerColor(kRed);
            hprofile_phi_res_grandmother_beambreco->Draw();
            c0->SetLogy(0);
            c0->Update();

            ROOT::RDF::RResultPtr<TProfile> hprofile_phi_res_grandmother_3mu;
            hprofile_phi_res_grandmother_3mu = b2munode->Fill<Float_t, Float_t>(TProfile("hprofile_phi_res_grandmother_3mu", "hprofile_phi_res_grandmother_3mu", 
                                                                                         8, 0, 2.4, -0.1, 0.1), 
                                                                                         {"abs_mu1_grandmother_eta", "phi_resolution_grandmother_3mumomentareco"});
            hprofile_phi_res_grandmother_3mu->GetXaxis()->SetTitle("|#eta_{Grandmother}|[]");
            hprofile_phi_res_grandmother_3mu->GetYaxis()->SetTitle("Resolution");
            hprofile_phi_res_grandmother_3mu->GetYaxis()->SetRangeUser(-0.006, 0.008);
            hprofile_phi_res_grandmother_3mu->SetErrorOption("");
            hprofile_phi_res_grandmother_3mu->SetStats(false);
            hprofile_phi_res_grandmother_3mu->SetLineColorAlpha(kBlue, 1);
            hprofile_phi_res_grandmother_3mu->SetMarkerStyle(kFullSquare);
            hprofile_phi_res_grandmother_3mu->SetMarkerColor(kBlue);
            hprofile_phi_res_grandmother_3mu->Draw("same");
            c0->SetLogy(0);
            c0->Update();

            //Adjusting the legend
            hprofile_legend = new TLegend(0.1, 0.75, 0.26, 0.9);
            hprofile_legend->SetHeader(TString::Format("%s", fileiterator->labeltxt.Data()), "C");
            hprofile_legend->AddEntry("hprofile_phi_res_grandmother_beambreco", "#phi_{Grandmother}-#phi_{BeamBreco}");
            hprofile_legend->AddEntry("hprofile_phi_res_grandmother_3mu", "#phi_{Grandmother}-#phi_{3#mureco}");
            //hprofile_legend->AddEntry("hphigenepvgenesvreco", "hphigenepvgenesvreco", "pf");
            hprofile_legend->SetTextSize(0.025);
            hprofile_legend->Draw("same");
            header = (TLegendEntry *)hprofile_legend->GetListOfPrimitives()->First();
            header->SetTextSize(0.018);
            c0->Update();

            c0->Print("OutputFiles/ProfileTrial_phiresolution.png");
            c0->Clear();
            for (int i = 1; i<=8; i++)
            {
                if (debug == false) cout << "Errore Phi beam-breco Bin : " << i << " " << hprofile_phi_res_grandmother_beambreco->GetBinError(i) << endl;
                if (debug == false) cout << "Errore Phi 3mureco Bin : " << i << " " << hprofile_phi_res_grandmother_3mu->GetBinError(i) << endl;
            }

            //Profile histogram for cos(grandmother-vector)
            ROOT::RDF::RResultPtr<TProfile> hprofile_cos_angle_res_grandmother_beambreco;
            hprofile_cos_angle_res_grandmother_beambreco = b2munode->Fill<Float_t, Float_t>(TProfile("hprofile_cos_angle_res_grandmother_beambreco", "hprofile_cos_angle_res_grandmother_beambreco",
                                                                                                8, 0, 2.4, -1.1, 1.1), 
                                                                                                {"abs_mu1_grandmother_eta", "cos_angle_grandmother_BeamBreco"});
            hprofile_cos_angle_res_grandmother_beambreco->SetTitle("Different resolutions for various eta regions");
            hprofile_cos_angle_res_grandmother_beambreco->GetXaxis()->SetTitle("|#eta_{Grandmother}|[]");
            hprofile_cos_angle_res_grandmother_beambreco->GetYaxis()->SetTitle("Resolution");
            hprofile_cos_angle_res_grandmother_beambreco->GetYaxis()->SetRangeUser(0.985, 1.005);
            hprofile_cos_angle_res_grandmother_beambreco->SetErrorOption("");
            hprofile_cos_angle_res_grandmother_beambreco->SetStats(false);
            hprofile_cos_angle_res_grandmother_beambreco->SetLineColorAlpha(kRed, 1);
            hprofile_cos_angle_res_grandmother_beambreco->SetMarkerStyle(kFullSquare);
            hprofile_cos_angle_res_grandmother_beambreco->SetMarkerColor(kRed);
            hprofile_cos_angle_res_grandmother_beambreco->Draw();
            c0->SetLogy(0);
            c0->Update();

            ROOT::RDF::RResultPtr<TProfile> hprofile_cos_angle_res_grandmother_3mu;
            hprofile_cos_angle_res_grandmother_3mu = b2munode->Fill<Float_t, Float_t>(TProfile("hprofile_cos_angle_res_grandmother_3mu", "hprofile_cos_angle_res_grandmother_3mu", 
                                                                                         8, 0, 2.4, -1.1, 1.1), 
                                                                                         {"abs_mu1_grandmother_eta", "cos_angle_grandmother_3mureco"});
            hprofile_cos_angle_res_grandmother_3mu->GetXaxis()->SetTitle("|#eta_{Grandmother}|[]");
            hprofile_cos_angle_res_grandmother_3mu->GetYaxis()->SetTitle("Resolution");
            hprofile_cos_angle_res_grandmother_3mu->GetYaxis()->SetRangeUser(0.985, 1.005);
            hprofile_cos_angle_res_grandmother_3mu->SetErrorOption("");
            hprofile_cos_angle_res_grandmother_3mu->SetStats(false);
            hprofile_cos_angle_res_grandmother_3mu->SetLineColorAlpha(kBlue, 1);
            hprofile_cos_angle_res_grandmother_3mu->SetMarkerStyle(kFullSquare);
            hprofile_cos_angle_res_grandmother_3mu->SetMarkerColor(kBlue);
            hprofile_cos_angle_res_grandmother_3mu->Draw("same");
            c0->SetLogy(0);
            c0->Update();

            //Adjusting the legend
            hprofile_legend = new TLegend(0.1, 0.75, 0.42, 0.9);
            hprofile_legend->SetHeader(TString::Format("%s", fileiterator->labeltxt.Data()), "C");
            hprofile_legend->AddEntry("hprofile_cos_angle_res_grandmother_beambreco", "cos(#vec{(Grandmother,Mother)}#angle#vec{(BeamBreco)})");
            hprofile_legend->AddEntry("hprofile_cos_angle_res_grandmother_3mu", "cos(#vec{(Grandmother,Mother)}#angle#vec{(3#mureco)})");
            //hprofile_legend->AddEntry("hphigenepvgenesvreco", "hphigenepvgenesvreco", "pf");
            hprofile_legend->SetTextSize(0.025);
            hprofile_legend->Draw("same");
            header = (TLegendEntry *)hprofile_legend->GetListOfPrimitives()->First();
            header->SetTextSize(0.018);
            c0->Update();

            c0->Print("OutputFiles/ProfileTrial_cosalfaresolution.png");
            c0->Clear();
            for (int i = 1; i<=8; i++)
            {
                if (debug == false) cout << "Errore Cos(alfa) beam-breco Bin : " << i << " " << hprofile_cos_angle_res_grandmother_beambreco->GetBinError(i) << endl;
                if (debug == false) cout << "Errore Cos(alfa) 3mureco Bin : " << i << " " << hprofile_cos_angle_res_grandmother_3mu->GetBinError(i) << endl;
            }
            if (debug == false) cout << endl << endl;
        }
        c0->Print(TString::Format("%s]", outhistpdf.Data()));
    }

    if (debug==false) ROOT::DisableImplicitMT();
    return 0;
}