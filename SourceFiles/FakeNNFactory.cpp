/*!
 *  \file FakeNNFactory.cpp
 *  \brief Source file for \ref FakeNNFactory function implementation
 */

/*!
 *  \fn int TreeReader(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read) "";
 *  \brief Function used to read selected variables stored in selected trees
 * 
 *  \param file2read name of the file which stores the trees
 *  \param trees2read vector of names of trees that the user wants to explore
 */

#include "HeaderFiles/FakeNNFactory.h"

int FakeNNFactory(TString preselection_filter, 
                  vector<string> *fake_factory_names,
                  const char *pass_id,
                  vector<string> features,
                  vector<VarToBeFitted> *vartobefit,
                  vector<TMVAMethod> tmvamethods)
{
    //preselection and not-true muon request
    cout << fake_factory_names->at(0).data() << endl;
    ROOT::RDataFrame mc("BTo3Mu", fake_factory_names->at(0).data());
    ROOT::RDF::RNode  mc_filtered = ROOT::RDF::RNode(mc.Filter(preselection_filter.Data()));
    ROOT::RDF::RNode  passing_mc_tmp = mc_filtered.Filter(pass_id);
    ROOT::RDF::RNode failing_mc_tmp = mc_filtered.Filter(TString::Format("!(%s)", pass_id).Data());

    cout << mc_filtered.Count().GetValue() << endl;
    cout << passing_mc_tmp.Count().GetValue() << endl;
    cout << failing_mc_tmp.Count().GetValue() << endl;

    for (vector<TMVAMethod>::iterator tmvamethit = tmvamethods.begin(); tmvamethit < tmvamethods.end(); tmvamethit++)
    {
      for (vector<VarToBeFitted>::iterator nn_varit = vartobefit->begin(); nn_varit < vartobefit->end(); nn_varit++)
      {
        TString failvarname = TString::Format("FailEvents_%s_%s", tmvamethit->tmvamethodname, nn_varit->vartobefitted);
        TString passvarname = TString::Format("PassEvents_%s_%s", tmvamethit->tmvamethodname, nn_varit->vartobefitted);
        TString weightedvarname = TString::Format("WeightedEvents_%s_%s", tmvamethit->tmvamethodname, nn_varit->vartobefitted);
        TString failpass_canvas_name = TString::Format("histogram__%s_%s", nn_varit->vartobefitted, tmvamethit->tmvamethodname);
        TString failpass_canvas_filename = TString::Format("OutputFiles/TMVAResults/histogram_failpass_%s_%s.png", nn_varit->vartobefitted, tmvamethit->tmvamethodname);
        TString failpass_canvas_norm_name = TString::Format("%s_norm", failpass_canvas_name.Data());
        TString failpassstackname = TString::Format("Failpassstackname_%s_%s", tmvamethit->tmvamethodname, nn_varit->vartobefitted);
        
        TH1F *histogram_pass = (TH1F*)mc_filtered.Filter(pass_id).Histo1D<Float_t>({failvarname, failvarname, int(nn_varit->bins), nn_varit->min_bin, nn_varit->max_bin}, nn_varit->vartobefitted)->Clone();
        TH1F *histogram_fail = (TH1F*)mc_filtered.Filter(TString::Format("!(%s)", pass_id).Data()).Histo1D<Float_t>({passvarname, passvarname, int(nn_varit->bins), nn_varit->min_bin, nn_varit->max_bin}, nn_varit->vartobefitted)->Clone();
        THStack *failpass_histstack = new THStack(failpassstackname, failpassstackname);

        TCanvas *failpass_canvas = new TCanvas(failpass_canvas_name, failpass_canvas_name, 1080, 1080);
        failpass_canvas->cd();
        failpass_canvas->Clear();
        gPad->SetRightMargin(0.05);
        gPad->SetLeftMargin(0.15);
        histogram_fail->SetMarkerColor(kRed);
        histogram_fail->SetLineColor(kBlue);
        histogram_fail->SetLineWidth(3);
        histogram_fail->SetStats(kFALSE);
        histogram_fail = (TH1F*)histogram_fail->DrawNormalized()->Clone();
        failpass_histstack->Add(histogram_fail);
        histogram_pass->SetMarkerColor(kRed);
        histogram_pass->SetLineColor(kRed);
        histogram_pass->SetLineWidth(3);
        histogram_pass->SetStats(kFALSE);
        histogram_pass = (TH1F*)histogram_pass->DrawNormalized()->Clone();
        failpass_histstack->Add(histogram_pass);
        failpass_histstack->Draw("NOSTACK HIST");
        failpass_histstack->SetNameTitle(TString::Format("%s distribution for pass/fail region", nn_varit->varname.data()), TString::Format("%s distribution for pass/fail region", nn_varit->varname.data()));
        failpass_histstack->GetHistogram()->GetXaxis()->SetTitle(nn_varit->Xlabel());
        failpass_histstack->GetHistogram()->GetYaxis()->SetTitle("Normalized occurences");

        TLegend legend = nn_varit->SetLegendPosAuto(nn_varit->legpos, 3, nn_varit->legxlength);
        legend.AddEntry(histogram_fail, "Fail Events");
        legend.AddEntry(histogram_pass, "Pass Events");
        legend.AddEntry((TObject *)0, TString::Format("Total entries: %.0llu", mc_filtered.Count().GetValue()), "");
        legend.AddEntry((TObject *)0, TString::Format("Fail region entries: %.0f", histogram_fail->GetEntries()), "");
        legend.AddEntry((TObject *)0, TString::Format("Pass region entries: %.0f", histogram_pass->GetEntries()), "");
        legend.SetTextSize(0.019);
        legend.Draw("SAME");
        failpass_canvas->Print(failpass_canvas_filename);
      }
    }

    Float_t closure_test_threshold = 1;
    ROOT::RDF::RNode  passing_mc = passing_mc_tmp.Range(0, int(passing_mc_tmp.Count().GetValue() * closure_test_threshold));
    ROOT::RDF::RNode  failing_mc = failing_mc_tmp.Range(0, int(failing_mc_tmp.Count().GetValue() * closure_test_threshold));

    //samples for the closure test
    ROOT::RDF::RNode  passing_mc_ct = passing_mc_tmp.Range(int(passing_mc_tmp.Count().GetValue() * closure_test_threshold), 0).Define("Target", "return 1");
    ROOT::RDF::RNode  failing_mc_ct = failing_mc_tmp.Range(int(failing_mc_tmp.Count().GetValue() * closure_test_threshold), 0).Define("Target", "return 0");

    cout << passing_mc.Count().GetValue() << endl;
    cout << failing_mc.Count().GetValue() << endl;
    cout << passing_mc_ct.Count().GetValue() << endl;
    cout << failing_mc_ct.Count().GetValue() << endl;

    cout << "" << endl;
    cout << "Model" << endl;
    cout << "" << endl;

    ROOT::RDF::RSnapshotOptions tmva_mc_snapop;
    tmva_mc_snapop.fMode = "RECREATE";
    passing_mc.Snapshot(fake_factory_names->at(3).data(), fake_factory_names->at(1).data(), features, tmva_mc_snapop);
    tmva_mc_snapop.fMode = "UPDATE";
    failing_mc.Snapshot(fake_factory_names->at(4).data(), fake_factory_names->at(1).data(), features, tmva_mc_snapop);
    vector<string> features_ct = features;
    features_ct.push_back("Target");
    for (vector<VarToBeFitted>::iterator varit = vartobefit->begin(); varit < vartobefit->end(); varit++)
      if (passing_mc_ct.HasColumn(varit->vartobefitted) == true)
        features_ct.push_back(varit->vartobefitted);
    passing_mc_ct.Snapshot(fake_factory_names->at(5).data(), fake_factory_names->at(1).data(), features_ct, tmva_mc_snapop);
    failing_mc_ct.Snapshot(fake_factory_names->at(6).data(), fake_factory_names->at(1).data(), features_ct, tmva_mc_snapop);
    TFile *inputfile = TFile::Open(fake_factory_names->at(1).data(), "read");
    TFile *outputfile = TFile::Open(fake_factory_names->at(2).data(), "recreate");
    TTree *signal = (TTree *)inputfile->Get(fake_factory_names->at(3).data());
    TTree *background = (TTree *)inputfile->Get(fake_factory_names->at(4).data());
    Double_t signalweight = 1.0, backgroundweight = 1.0;
    TMVA::gConfig().GetIONames().fWeightFileDirPrefix = "TMVAResults";
    TMVA::gConfig().GetIONames().fWeightFileDir = "Weights";
    TMVA::DataLoader *loader = new TMVA::DataLoader("");
    TMVA::Factory *factory = new TMVA::Factory("TMVAClassification", outputfile, "AnalysisType=Classification");
    for (vector<string>::iterator mvavar = features.begin(); mvavar < features.end(); mvavar++)
    {
      loader->AddVariable(mvavar->data(), mvavar->data(), "", 'F');
    }
    loader->AddSignalTree(signal, signalweight);
    loader->AddBackgroundTree(background, backgroundweight);

    Float_t train_test_treshold = 0.9;
    long unsigned int ntrainsign = signal->GetEntries()*train_test_treshold; /*!< Number of events used to train signal identification in the sample */
    long unsigned int ntrainback = background->GetEntries()*train_test_treshold; /*!< Number of events used to train background identification in the sample */
    long unsigned int ntestsign = signal->GetEntries()*(1-train_test_treshold);  /*!< Number of events used to test signal identification in the sample */
    long unsigned int ntestback = background->GetEntries()*(1-train_test_treshold);  /*!< Number of events used to test background identification in the sample */
    TString dataString = TString::Format("nTrain_Signal=%lu", ntrainsign);
    dataString.Append(TString::Format(":nTrain_Background=%lu", ntrainback));
    dataString.Append(TString::Format(":nTest_Signal=%lu", ntestsign));
    dataString.Append(TString::Format(":nTest_Background=%lu", ntestback));
    dataString.Append(":SplitMode=Random:NormMode=NumEvents:!V");
    loader->PrepareTrainingAndTestTree("", "", dataString);
    TMVA::DataLoader *loader_with_variance_threshold = loader->VarTransform("VT(0.0)");
    //loader_with_variance_threshold->SetName("FakeReweighting");
    cout << loader_with_variance_threshold->GetName() << endl;

    for (vector<TMVAMethod>::iterator tmvamethit = tmvamethods.begin(); tmvamethit < tmvamethods.end(); tmvamethit++)
    {
      factory->BookMethod(loader_with_variance_threshold, tmvamethit->tmvamethodtype, tmvamethit->tmvamethodname, tmvamethit->tmvamethodconfiguration);
    }
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();
    inputfile->Close();
    outputfile->Close();
    if(!gROOT->IsBatch()) TMVA::TMVAGui(fake_factory_names->at(2).data());

    return 0;
}