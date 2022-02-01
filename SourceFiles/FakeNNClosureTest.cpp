/*!
 *  \file FakeNNClosureTest.cpp
 *  \brief Source file for \ref FakeNNClosureTest function implementation
 */

/*!
 *  \fn int TreeReader(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read) "";
 *  \brief Function used to read selected variables stored in selected trees
 * 
 *  \param file2read name of the file which stores the trees
 *  \param trees2read vector of names of trees that the user wants to explore
 */

#include "HeaderFiles/FakeNNClosureTest.h"

int FakeNNClosureTest(vector<string> *fake_factory_names,
                      vector<string> features,
                      vector<Float_t> Input_feature_var,
                      unordered_map<string, Float_t> MVA_feature_var,
                      vector<VarToBeFitted> *vartobefit,
                      vector<TMVAMethod> tmvamethods)
{
    TFile *eventinputfile = TFile::Open(fake_factory_names->at(1).data(), "read");
    TFile *eventoutputfile = TFile::Open(TString::Format("OutputFiles/TMVAResults/ClosureTest/ClosureTest.root"), "recreate");
    TList *list = new TList();
    TTree *PassFakesTree = (TTree *)eventinputfile->Get(fake_factory_names->at(5).data());
    TTree *FailFakesTree = (TTree *)eventinputfile->Get(fake_factory_names->at(6).data());
    Double_t passentries = PassFakesTree->GetEntries(), failentries = FailFakesTree->GetEntries();
    cout << passentries << endl;
    cout << failentries << endl;
    Double_t fractionsignal = passentries / (passentries + failentries);
    cout << fractionsignal << endl;
    list->Add(PassFakesTree);
    list->Add(FailFakesTree);
    TTree *Event = TTree::MergeTrees(list);
    Double_t FakeWeight = 0, FakeProba = 0, FakeRarity = 0;
    TBranch *Branch_FakeWeight = Event->Branch("FakeWeight", &FakeWeight, "FakeWeight/D");
    TBranch *Branch_FakeProba = Event->Branch("FakeProba", &FakeProba, "FakeProba/D");
    TBranch *Branch_FakeRarity = Event->Branch("FakeRarity", &FakeRarity, "FakeRarity/D");
    int event_entries = Event->GetEntries();
    for (vector<string>::iterator mvavar = features.begin(); mvavar < features.end(); mvavar++)
        Event->SetBranchAddress(mvavar->data(), &(Input_feature_var[distance(features.begin(), mvavar)]));

    //Creating a dataframe which holds only names of variables with a variance greater than the threshold set up before
    TString loader_with_variance_threshold_filename = TString::Format("%s", fake_factory_names->at(2).data());
    TString loader_with_variance_threshold_treename = TString::Format("%s/TrainTree", "vt_transformed_dataset");
    ROOT::RDataFrame loader_with_variance_threshold_dataframe(loader_with_variance_threshold_treename, loader_with_variance_threshold_filename);

    for (vector<TMVAMethod>::iterator tmvamethit = tmvamethods.begin(); tmvamethit < tmvamethods.end(); tmvamethit++)
    {
        cout << endl
             << endl
             << endl
             << endl;
        Event->SetName(TString::Format("FakeClosureTest_%s", tmvamethit->tmvamethodname));

        //Looking for any weight of the specified method in the folder containing the training results
        TString weightfilename = TString::Format("TMVAResults/%s/Weights/TMVAClassification_%s.weights.xml", "vt_transformed_dataset", tmvamethit->tmvamethodname);

        //Defining the reader that should do the MVA evaluation
        TMVA::Reader *reader = new TMVA::Reader(tmvamethit->tmvamethodname);
        for (vector<string>::iterator mvavar = features.begin(); mvavar < features.end(); mvavar++)
        {
            if (loader_with_variance_threshold_dataframe.HasColumn(mvavar->data()))
            {
                reader->AddVariable(mvavar->data(), &(MVA_feature_var[mvavar->data()]));
            }
        }
        reader->BookMVA(tmvamethit->tmvamethodname, weightfilename);

        //Checking that input tree is read from the right file, because histograms are written to a different file
        eventinputfile->cd();
        for (int readerindex = 0; readerindex < event_entries; readerindex++)
        {
            Event->GetEntry(readerindex);
            for (vector<string>::iterator mvavar = features.begin(); mvavar < features.end(); mvavar++)
            {
                MVA_feature_var[mvavar->data()] = (Float_t)Input_feature_var[distance(features.begin(), mvavar)];
            }
            FakeWeight = reader->EvaluateMVA(tmvamethit->tmvamethodname);
            FakeProba = reader->GetProba(tmvamethit->tmvamethodname);
            FakeRarity = reader->GetRarity(tmvamethit->tmvamethodname);
            Branch_FakeWeight->Fill();
            Branch_FakeProba->Fill();
            Branch_FakeRarity->Fill();
        }

        //Freeing memory allocated for the reader
        delete reader;
        eventoutputfile->cd();
        Event->Write();
    }
    delete list;
    eventinputfile->Close();
    eventoutputfile->Close();

    vector<string> nn_weighted_variables;
    for (vector<TMVAMethod>::iterator tmvamethitct = tmvamethods.begin(); tmvamethitct < tmvamethods.end(); tmvamethitct++)
    {
      TString weightfilename = TString::Format("TMVAResults/%s/Weights/TMVAClassification_%s.weights.xml", "vt_transformed_dataset", tmvamethitct->tmvamethodname);
      TMVA::Experimental::RReader model(weightfilename.Data());
      auto variables = model.GetVariableNames();
      //auto prediction = model.Compute();

      ROOT::RDataFrame NN_Closure_original(TString::Format("FakeClosureTest_%s", tmvamethitct->tmvamethodname), "OutputFiles/TMVAResults/ClosureTest/ClosureTest.root");
      auto NN_Closure = NN_Closure_original.Define("Weight", "FakeProba/(1-FakeProba)");
      TString filter = "Target==0";
      //        filter+= "Target==0" ;
      for (vector<VarToBeFitted>::iterator nn_varit = vartobefit->begin(); nn_varit < vartobefit->end(); nn_varit++)
      {
        TString failvarname = TString::Format("FailEvents_%s_%s", tmvamethitct->tmvamethodname, nn_varit->vartobefitted);
        TString passvarname = TString::Format("PassEvents_%s_%s", tmvamethitct->tmvamethodname, nn_varit->vartobefitted);
        TString weightedvarname = TString::Format("WeightedEvents_%s_%s", tmvamethitct->tmvamethodname, nn_varit->vartobefitted);
        TString closure_canvas_name = TString::Format("histogram_closure_%s_%s", nn_varit->vartobefitted, tmvamethitct->tmvamethodname);
        TString closure_canvas_filename = TString::Format("OutputFiles/TMVAResults/ClosureTest/histogram_closure_%s_%s.png", nn_varit->vartobefitted, tmvamethitct->tmvamethodname);
        TString closure_canvas_norm_name = TString::Format("%s_norm", closure_canvas_name.Data());
        TString closure_canvas_norm_filename = TString::Format("OutputFiles/TMVAResults/ClosureTest/Normalized/histogram_closure_%s_%s_norm.png", nn_varit->vartobefitted, tmvamethitct->tmvamethodname);
        TString closure_canvas_norm_ratio_name = TString::Format("%s_ratio", closure_canvas_norm_name.Data());
        TString closure_canvas_norm_ratio_filename = TString::Format("OutputFiles/TMVAResults/ClosureTest/Normalized/Pass_NNFail_Ratio/histogram_closure_%s_%s_norm_ratio.png", nn_varit->vartobefitted, tmvamethitct->tmvamethodname);
        TString closurestackname = TString::Format("Closurestackname_%s_%s", tmvamethitct->tmvamethodname, nn_varit->vartobefitted);

        TH1F *histogram_closure_fail = (TH1F*)NN_Closure.Filter("Target==0").Histo1D<Float_t>({failvarname, failvarname, int(nn_varit->bins), nn_varit->min_bin, nn_varit->max_bin}, nn_varit->vartobefitted)->Clone();
        TH1F *histogram_closure_pass = (TH1F*)NN_Closure.Filter("Target==1").Histo1D<Float_t>({passvarname, passvarname, int(nn_varit->bins), nn_varit->min_bin, nn_varit->max_bin}, nn_varit->vartobefitted)->Clone();
        TH1F *histogram_closure_weighted = (TH1F *)NN_Closure.Filter(filter.Data()).Histo1D<Float_t, Double_t>({weightedvarname, weightedvarname, int(nn_varit->bins), nn_varit->min_bin, nn_varit->max_bin}, nn_varit->vartobefitted, "Weight")->Clone();
        //TH1F *histogram_closure_weighted = (TH1F *)NN_Closure.Filter(filter.Data()).Histo1D<Float_t>({weightedvarname, weightedvarname, int(nn_varit->bins), nn_varit->min_bin, nn_varit->max_bin}, nn_varit->vartobefitted)->Clone();

        histogram_closure_weighted->SetTitle(TString::Format("%s distribution for pass/fail region", nn_varit->varname.data()));
        histogram_closure_weighted->GetXaxis()->SetTitle(nn_varit->Xlabel());
        histogram_closure_weighted->GetYaxis()->SetTitle("Norm. Occ. [a.u.]");

        TCanvas *closure_canvas = new TCanvas(closure_canvas_name, closure_canvas_name, 1920, 1080);
        THStack *closure_histstack = new THStack(closurestackname, closurestackname);
        histogram_closure_weighted->SetMarkerColor(kBlue);
        histogram_closure_weighted->SetLineColor(kBlue);
        histogram_closure_weighted->SetLineWidth(3);
        histogram_closure_weighted->SetStats(kFALSE);
        closure_histstack->Add(histogram_closure_weighted);
        histogram_closure_pass->SetMarkerColor(kRed);
        histogram_closure_pass->SetLineColor(kRed);
        histogram_closure_pass->SetLineWidth(3);
        histogram_closure_pass->SetStats(kFALSE);
        closure_histstack->Add(histogram_closure_pass);
        histogram_closure_fail->SetMarkerColor(kYellow + 1);
        histogram_closure_fail->SetLineColor(kYellow + 1);
        histogram_closure_fail->SetLineWidth(3);
        histogram_closure_fail->SetStats(kFALSE);
        closure_histstack->Add(histogram_closure_fail);
        closure_histstack->Draw("NOSTACK HIST");
        closure_histstack->SetNameTitle(TString::Format("%s distribution for pass/fail region", nn_varit->varname.data()), TString::Format("%s distribution for pass/fail region", nn_varit->varname.data()));
        closure_histstack->GetHistogram()->GetXaxis()->SetTitle(nn_varit->Xlabel());
        closure_histstack->GetHistogram()->GetYaxis()->SetTitle("Occurences");

        TLegend legend = nn_varit->SetLegendPos(0.1, 0.3, 0.75, 0.9);
        legend.AddEntry(histogram_closure_fail, "Fail Events");
        legend.AddEntry(histogram_closure_weighted, "Fail Events (with NN weights)");
        legend.AddEntry(histogram_closure_pass, "Pass Events");
        legend.AddEntry((TObject *)0, TString::Format("Fail region entries: %.0f", histogram_closure_fail->GetEntries()), "");
        legend.AddEntry((TObject *)0, TString::Format("Events passing the NN cut: %.0f", histogram_closure_weighted->GetEntries()), "");
        legend.AddEntry((TObject *)0, TString::Format("Pass region entries: %.0f", histogram_closure_pass->GetEntries()), "");
        legend.SetTextSize(0.019);
        legend.SetBorderSize(1);
        legend.SetFillColor(kWhite);
        legend.Draw("SAME");
        closure_canvas->Print(closure_canvas_filename);

        //Drawing histogram normalized
        TH1F *histogram_closure_fail_norm = (TH1F*)histogram_closure_fail->Clone(TString::Format("%s_norm",histogram_closure_fail->GetName()));
        TH1F *histogram_closure_pass_norm = (TH1F*)histogram_closure_pass->Clone(TString::Format("%s_norm",histogram_closure_pass->GetName()));
        TH1F *histogram_closure_weighted_norm = (TH1F*)histogram_closure_weighted->Clone(TString::Format("%s_norm",histogram_closure_weighted->GetName()));

        TCanvas *closure_norm_canvas = new TCanvas(closure_canvas_norm_name, closure_canvas_norm_name, 1920, 1080);
        THStack *closure_norm_histstack = new THStack(closurestackname, closurestackname);
        THStack *closure_norm_histstack_ratio = new THStack(TString::Format("%s_ratio", closurestackname.Data()).Data(), TString::Format("%s_ratio", closurestackname.Data()));
        histogram_closure_weighted_norm->Scale(1./histogram_closure_weighted->Integral());
        closure_norm_histstack->Add(histogram_closure_weighted_norm);
        closure_norm_histstack_ratio->Add(histogram_closure_weighted_norm);
        histogram_closure_pass_norm->Scale(1./histogram_closure_pass->Integral());
        closure_norm_histstack->Add(histogram_closure_pass_norm);
        closure_norm_histstack_ratio->Add(histogram_closure_pass_norm);
        histogram_closure_fail_norm->Scale(1./histogram_closure_fail->Integral());
        closure_norm_histstack->Add(histogram_closure_fail_norm);
        closure_norm_histstack->Draw("NOSTACK HIST");
        closure_norm_histstack->SetNameTitle(TString::Format("%s normalized distribution for pass/fail region", nn_varit->varname.data()), TString::Format("%s normalized distribution for pass/fail region", nn_varit->varname.data()));
        closure_norm_histstack->GetHistogram()->GetXaxis()->SetTitle(nn_varit->Xlabel());
        closure_norm_histstack->GetHistogram()->GetYaxis()->SetTitle("Norm. Occ. [a.u.]");

        TLegend legend_norm = nn_varit->SetLegendPos(0.1, 0.33, 0.75, 0.9);
        legend_norm.AddEntry(histogram_closure_fail_norm, "Fail Events");
        legend_norm.AddEntry(histogram_closure_weighted_norm, "Fail Events (with NN weights)");
        legend_norm.AddEntry(histogram_closure_pass_norm, "Pass Events");
        legend_norm.SetTextSize(0.02);
        legend_norm.SetBorderSize(1);
        legend_norm.SetFillColor(kWhite);

        //Kolmogorov test
        TH1F *histogram_closure_ratio_fail_pass = (TH1F*)histogram_closure_fail_norm->Clone();
        TH1F *histogram_closure_ratio_pass_pass = (TH1F*)histogram_closure_pass_norm->Clone();
        TH1F *histogram_closure_ratio_weighted_pass = (TH1F*)histogram_closure_weighted_norm->Clone();
        histogram_closure_ratio_fail_pass->Divide(histogram_closure_fail_norm, histogram_closure_pass_norm);
        histogram_closure_ratio_pass_pass->Divide(histogram_closure_pass_norm, histogram_closure_pass_norm);
        histogram_closure_ratio_weighted_pass->Divide(histogram_closure_weighted_norm, histogram_closure_pass_norm);
        Double_t kolmogorov_fail_pass = histogram_closure_ratio_fail_pass->KolmogorovTest(histogram_closure_ratio_pass_pass);
        Double_t kolmogorov_weight_pass = histogram_closure_ratio_weighted_pass->KolmogorovTest(histogram_closure_ratio_pass_pass);
        cout << "Kolmogorov Fail-Pass" << kolmogorov_fail_pass << endl;
        cout << "Kolmogorov Weight-Pass" << kolmogorov_weight_pass << endl;
        legend_norm.AddEntry((TObject *)0, TString::Format("Kolmogorov test Fail-Pass: %.3f", kolmogorov_fail_pass), "");
        legend_norm.AddEntry((TObject *)0, TString::Format("Kolmogorov test Faill(NN)-Pass: %.3f", kolmogorov_weight_pass), "");
        legend_norm.Draw("SAME");      
        closure_norm_canvas->Print(closure_canvas_norm_filename);

        //Plot with ratio
        TCanvas *closure_norm_ratio_canvas = new TCanvas(closure_canvas_norm_ratio_name.Data(), closure_canvas_norm_ratio_name.Data(), 1920, 1080);
        TPad *closure_norm_pad = new TPad(TString::Format("%s%s", closure_canvas_norm_ratio_name.Data(), nn_varit->varname.data()), TString::Format("%s%s", closure_canvas_norm_ratio_name.Data(), nn_varit->varname.data()), 0.0, 0.3, 1, 1);
        TPad *closure_norm_ratio_pad = new TPad(TString::Format("%s_ratio_%s", closure_canvas_norm_ratio_name.Data(), nn_varit->varname.data()), TString::Format("%s_ratio_%s", closure_canvas_norm_ratio_name.Data(), nn_varit->varname.data()), 0.0, 0.0, 1, 0.3);
        closure_norm_ratio_canvas->cd(0);
        closure_norm_ratio_canvas->Clear();
        closure_norm_ratio_canvas->Draw();
        closure_norm_pad->SetTopMargin(0.1);
        closure_norm_pad->SetBottomMargin(0.015);
        closure_norm_pad->SetBorderMode(0);
        closure_norm_pad->SetLeftMargin(0.1);
        closure_norm_ratio_pad->SetTopMargin(0.00);
        closure_norm_ratio_pad->SetBottomMargin(0.2);
        closure_norm_ratio_pad->SetBorderMode(0);
        closure_norm_ratio_pad->SetLeftMargin(0.1);
        gStyle->SetOptTitle(kFALSE);
        closure_norm_pad->Draw();
        closure_norm_ratio_pad->Draw();

        closure_norm_pad->cd();
        closure_norm_histstack_ratio->Draw("NOSTACK HIST");
        closure_norm_histstack_ratio->SetNameTitle(TString::Format("%s normalized distribution for pass/fail region", nn_varit->varname.data()), TString::Format("%s normalized distribution for pass/fail region", nn_varit->varname.data()));
        closure_norm_histstack_ratio->GetHistogram()->GetXaxis()->SetTitle(nn_varit->Xlabel());
        closure_norm_histstack_ratio->GetHistogram()->GetYaxis()->SetTitle("Norm. Occ. [a.u.]");
        closure_norm_histstack_ratio->GetHistogram()->GetXaxis()->SetLabelSize(0);
        closure_norm_histstack_ratio->SetMaximum(closure_norm_histstack_ratio->GetHistogram()->GetMaximum()*1.1);
        closure_norm_histstack_ratio->Draw("NOSTACK HIST");
        //closure_norm_histstack_ratio->GetYaxis()->SetTitleSize(0.1);
        //closure_norm_histstack_ratio->GetYaxis()->SetTitleOffset(0.4);

        TLegend leg_fitwithratio = nn_varit->SetLegendPosAuto(nn_varit->legpos, 3);
        leg_fitwithratio.SetTextFont(42);
        leg_fitwithratio.AddEntry(histogram_closure_weighted_norm, "Fail Events (with NN weights)");
        leg_fitwithratio.AddEntry(histogram_closure_pass_norm, "Pass Events");
        leg_fitwithratio.AddEntry((TObject *)0, TString::Format("Kolmogorov test Faill(NN)-Pass: %.3f", kolmogorov_weight_pass), "");
        leg_fitwithratio.SetTextSize(0.02);
        leg_fitwithratio.SetBorderSize(1);
        leg_fitwithratio.SetFillColor(kWhite);
        leg_fitwithratio.Draw("SAME");        
        gPad->SetLogy(0);

        closure_norm_ratio_pad->cd();
        TH1F *closure_pass_failnn_ratio = new TH1F();
        closure_pass_failnn_ratio = (TH1F *)histogram_closure_pass_norm->Clone();
        closure_pass_failnn_ratio->Divide(closure_pass_failnn_ratio, histogram_closure_weighted_norm);
        closure_pass_failnn_ratio->SetStats(false);
        closure_pass_failnn_ratio->SetStats(false);

        //Fitting residuals with a line
        closure_pass_failnn_ratio->Fit("pol0");
        TF1 *linearfit = closure_pass_failnn_ratio->GetFunction("pol0");
        linearfit->SetName("linearfit");
        linearfit->SetLineColor(kGreen+1);
        linearfit->SetLineWidth(4);
        closure_pass_failnn_ratio->Draw("M");

        //Adding fit results to legend
        closure_norm_pad->cd();
        Float_t legendpositionx1ndc = leg_fitwithratio.GetX1(), legendpositionx2ndc = leg_fitwithratio.GetX2(),
                legendpositiony1ndc = leg_fitwithratio.GetY1()-0.04*4, legendpositiony2ndc = leg_fitwithratio.GetY1();
        TPaveText *pt = new TPaveText(legendpositionx1ndc, legendpositiony1ndc, legendpositionx2ndc, legendpositiony2ndc, "NDC");
        //pt->SetTextAlign(11);
        pt->SetTextFont(42);
        pt->SetTextSize(0.02);
        pt->SetBorderSize(1);
        pt->SetFillColor(kWhite);
        pt->AddText("Function fitting the ratio: " + linearfit->GetExpFormula());
        pt->AddText((TString::Format("%s: %.3f#pm%.3f", linearfit->GetParName(0), linearfit->GetParameter(0), linearfit->GetParError(0))));
        pt->AddText((TString::Format("#Chi^{2}: %.3f", linearfit->GetChisquare())));
        pt->AddText((TString::Format("NDOF: %d", linearfit->GetNDF())));
        pt->Draw();
        closure_norm_pad->Modified();

        //closure_pass_failnn_ratio->SetAxisRange(0, 2, "Y");
        closure_norm_ratio_pad->cd();
        closure_pass_failnn_ratio->GetXaxis()->SetTitle(nn_varit->Xlabel());
        closure_pass_failnn_ratio->GetXaxis()->SetTitleSize(0.1);
        closure_pass_failnn_ratio->GetXaxis()->SetTitleOffset(0.7);
        closure_pass_failnn_ratio->GetXaxis()->SetLabelSize(0.09);
        closure_pass_failnn_ratio->GetYaxis()->SetTitle("Pass/Fail(NN)");
        closure_pass_failnn_ratio->GetYaxis()->SetTitleSize(0.1);
        closure_pass_failnn_ratio->GetYaxis()->SetTitleOffset(0.4);
        closure_pass_failnn_ratio->GetYaxis()->SetNdivisions(5);
        closure_pass_failnn_ratio->GetYaxis()->SetLabelSize(0.09);
        closure_norm_ratio_pad->Modified();

        TLine *ideal_ratio_line = new TLine(nn_varit->min_bin,1.0,nn_varit->max_bin,1.0);
        ideal_ratio_line->SetLineStyle(kDashed);
        ideal_ratio_line->SetLineColor(kGray);
        ideal_ratio_line->Draw();

        closure_norm_ratio_canvas->Print(closure_canvas_norm_ratio_filename);
        closure_norm_ratio_canvas->Clear();
      }
    }
    return 0;
}