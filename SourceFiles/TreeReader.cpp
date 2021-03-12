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

    // Initializing histograms
    TH1F *histreco = new TH1F("reco", "reco", 150, 0, 150);
    TH1F *histgene = new TH1F("gene", "gene", 150, 0, 150);
    TCanvas *c0 = new TCanvas("c0", "c0", 1366, 768);

    for (vector<MarkedNames>::iterator fileiterator = file2read.begin(); fileiterator != file2read.end(); ++fileiterator)
    {
        // Opening the histogram collection
        TString outhistcol = TString::Format("OutputFiles/%s_Kinematics_Comparison.pdf", fileiterator->name.Data());
        c0->Print(TString::Format("%s[", outhistcol.Data()));
        
        // Reading the input file
        TFile inputfile(TString::Format("InputFiles/%s", fileiterator->name.Data()), "read");
        for (vector<MarkedNames>::iterator treeiterator = trees2read.begin(); treeiterator != trees2read.end(); ++treeiterator)
        {
            TTree *ReadTree = (TTree *)inputfile.Get(TString::Format("%s", treeiterator->name.Data()));
            for (vector<MarkedNames>::iterator variterator = var2read.begin(); variterator != var2read.end(); ++variterator)
            {
                if (variterator->geneflag == -1)
                {
                    TBranch *ReadBranch = ReadTree->GetBranch(variterator->name.Data());
                    ReadBranch->SetAddress(&var);
                    unsigned long int nentries = (unsigned long int)ReadTree->GetEntries();
                    for (unsigned long int i = 0; i<nentries; i++)
                    {
                        ReadBranch->GetEntry(i);
                        if (variterator->flag == 1) histreco->Fill(var);
                        if (variterator->flag == 0) histgene->Fill(var);
                    }
                }
                else
                {
                    TBranch *ReadBranch = ReadTree->GetBranch(variterator->name.Data());
                    ReadBranch->SetAddress(&var);
                    TBranch *ReadGeneBranch = ReadTree->GetBranch(variterator->genename.Data());
                    ReadGeneBranch->SetAddress(&genevar);                    
                    unsigned long int nentries = (unsigned long int)ReadTree->GetEntries();
                    for (unsigned long int i = 0; i<nentries; i++)
                    {
                        ReadBranch->GetEntry(i);
                        ReadGeneBranch->GetEntry(i);
                        histreco->Fill(var);
                        histgene->Fill(genevar);
                    }
                }

                THStack *kinehiststack = new THStack("LeadingpT_total", "LeadingpT_total");
                histreco->SetLineColor(kRed);
                histreco->SetFillColor(kRed);
                histreco->SetStats(kFALSE);
                histgene->SetLineColor(kBlue);
                histgene->SetFillColorAlpha(kBlue, 0.3);
                histgene->SetStats(kFALSE);
                kinehiststack->Add(histreco);
                kinehiststack->Add(histgene);
                kinehiststack->Draw("NOSTACK");
                kinehiststack->SetTitle(TString::Format("Kinematic comparison: %s vs %s", variterator->name.Data(), variterator->genename.Data()));
                kinehiststack->GetXaxis()->SetTitle("");
                kinehiststack->GetYaxis()->SetTitle("Occurences");

                //Adjusting the legend
                auto legend = new TLegend(0.65, 0.8, 0.9, 0.9);
                legend->AddEntry(histreco, variterator->name, "f");
                legend->AddEntry(histgene, variterator->genename, "f");
                legend->Draw("same");

                //Adding the file name as a text
                TPaveText *text = new TPaveText(0.65, 0.74, 0.89, 0.79, "NB NDC");
                text->AddText(TString::Format("%s", fileiterator->name.Data()));
                text->SetFillColor(kWhite);
                text->Draw("same");

                c0->Update();
                c0->Print(TString::Format("%s", outhistcol.Data()));
                histreco->Reset();
                histgene->Reset();
            }
        }
        c0->Print(TString::Format("%s]", outhistcol.Data()));
    }
    return 0;
}