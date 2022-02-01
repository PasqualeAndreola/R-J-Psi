#include "HeaderFiles/VarFitter.h"

using namespace std;
using namespace RooFit;

int VarFitter(VarToBeFitted *it_tobefit,
              bool debug,
              string closuretest)
{
        // Silencing output
        RooFit::PrintLevel(-1);
        RooMinimizer::PrintLevel(RooMinimizer::None);

        // Fixing RJPsi
        Double_t RJPsi = 0.2;

        // Importing dataframes
        ROOT::RDataFrame BtoJpsiMuMu_is_JPsiTau("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_tau_merged.root");
        ROOT::RDataFrame BtoJpsiMuMu_is_JPsiMu("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_mu_merged.root");
        ROOT::RDataFrame BtoJpsiMuMu_is_Chic0("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_chic0_mu_merged.root");
        ROOT::RDataFrame BtoJpsiMuMu_is_Chic1("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_chic1_mu_merged.root");
        ROOT::RDataFrame BtoJpsiMuMu_is_Chic2("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_chic2_mu_merged.root");
        ROOT::RDataFrame BtoJpsiMuMu_is_JPsiHC("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_hc_merged.root");
        ROOT::RDataFrame BtoJpsiMuMu_is_Hc("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_hc_mu_merged.root");
        ROOT::RDataFrame BtoJpsiMuMu_is_Psi2stau("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_psi2s_tau_merged.root");
        ROOT::RDataFrame BtoJpsiMuMu_is_Psi2smu("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_psi2s_mu_merged.root");
        ROOT::RDataFrame HbToJpsiX("BTo3Mu", "InputFiles/HbToJPsiMuMu_ptmax_merged.root");
        ROOT::RDataFrame HbToJPsiMu("BTo3Mu", "InputFiles/HbToJPsiMuMu3MuFilter_ptmax_merged.root");
        ROOT::RDataFrame BtoMu_data("BTo3Mu", "InputFiles/data_ptmax_merged.root");
        ROOT::RDataFrame BtoMu_data_nn("BTo3Mu", "OutputFiles/TMVAResults/data_ptmax_merged.root");

        // Setting normalization
        Double_t hybrid_normalization = 0.62;     // Normalization of the hybrid sample
        Double_t hbtojpsix_normalization = 5.58;  // Normalization of the hb to jpsix sample
        Double_t hbtojpsimu_normalization = 8.53; // Normalization of the hb to jpsimu sample

        // Variable name
        const char *varfit_name = it_tobefit->vartobefitted;

        // Preselections
        TString preselection_mediumid_true = (it_tobefit->preselectioncut_mediumid_true);
        TString preselection_mediumid_true_even = (it_tobefit->preselectioncut_mediumid_true) + "&&(int(event) % 2) == 0";
        TString preselection_mediumid_true_odd = (it_tobefit->preselectioncut_mediumid_true) + "&&((int(event) % 2 )!= 0)";
        TString preselection_mediumid_false = it_tobefit->preselectioncut_mediumid_false;
        TString preselection_mediumid_false_even = (it_tobefit->preselectioncut_mediumid_false) + "&&(int(event) % 2) == 0";
        TString preselection_mediumid_false_odd = (it_tobefit->preselectioncut_mediumid_false) + "&&((int(event) % 2 )!= 0)";

        // Filters
        TString filter_mediumid_true = (it_tobefit->cut_mediumid_true);
        TString filter_mediumid_true_even = (it_tobefit->cut_mediumid_true) + "&&int(event) % 2 == 0";
        TString filter_mediumid_true_odd = (it_tobefit->cut_mediumid_true) + "&&((int(event) % 2 )!= 0)";
        TString filter_mediumid_false = it_tobefit->cut_mediumid_false;
        TString filter_mediumid_false_even = (it_tobefit->cut_mediumid_false) + "&&int(event) % 2 == 0";
        TString filter_mediumid_false_odd = (it_tobefit->cut_mediumid_false) + "&&((int(event) % 2 )!= 0)";

        // Fit variable parameters
        Double_t bins = it_tobefit->bins, minbin = it_tobefit->min_bin, maxbin = it_tobefit->max_bin, bin_width = (maxbin - minbin) / bins;

        // Counting events pre e post filtering
        Double_t nevents_jtau_prefilter_norange = BtoJpsiMuMu_is_JPsiTau.Count().GetValue();
        Double_t nevents_jmu_prefilter_norange = BtoJpsiMuMu_is_JPsiMu.Count().GetValue();
        Double_t nevents_jtau_postfilter_fitrange = BtoJpsiMuMu_is_JPsiTau.Filter(TString::Format("%s && %s>%f && %s<%f", filter_mediumid_true.Data(), varfit_name, minbin, varfit_name, maxbin).Data()).Count().GetValue();
        Double_t nevents_jmu_postfilter_fitrange = BtoJpsiMuMu_is_JPsiMu.Filter(TString::Format("%s && %s>%f && %s<%f", filter_mediumid_true.Data(), varfit_name, minbin, varfit_name, maxbin).Data()).Count().GetValue();
        Double_t signal_efficiency = nevents_jtau_postfilter_fitrange / nevents_jtau_prefilter_norange;
        Double_t normal_efficiency = nevents_jmu_postfilter_fitrange / nevents_jmu_prefilter_norange;

        // RooFit Parameters Initialization
        Int_t interpolation_order = 0;
        RooRealVar *varfit = new RooRealVar(varfit_name, varfit_name, minbin, maxbin, it_tobefit->vardimension.c_str());
        RooRealVar mu1pt = RooRealVar("mu1pt", "mu1pt", 0, 1000, "GeV");
        RooRealVar mu2pt = RooRealVar("mu2pt", "mu2pt", 0, 1000, "GeV");
        RooRealVar kpt = RooRealVar("kpt", "kpt", 0, 1000, "GeV");
        RooRealVar jpsi_pt = RooRealVar("jpsi_pt", "jpsi_pt", 0, 1000, "GeV");
        RooRealVar mu1eta = RooRealVar("mu1eta", "mu1eta", 0, 1000, "GeV");
        RooRealVar mu2eta = RooRealVar("mu2eta", "mu2eta", 0, 1000, "GeV");
        RooRealVar keta = RooRealVar("keta", "keta", 0, 1000, "GeV");
        RooRealVar bvtx_svprob = RooRealVar("bvtx_svprob", "bvtx_svprob", 0, 1000, "GeV");
        RooRealVar jpsivtx_svprob = RooRealVar("jpsivtx_svprob", "jpsivtx_svprob", 0, 1000, "GeV");
        RooRealVar mu1_mediumID = RooRealVar("mu1_mediumID", "mu1_mediumID", 0, 1000, "GeV");
        RooRealVar mu2_mediumID = RooRealVar("mu2_mediumID", "mu2_mediumID", 0, 1000, "GeV");
        RooRealVar k_mediumID = RooRealVar("k_mediumID", "k_mediumID", 0, 1000, "GeV");
        RooRealVar Bpt_reco = RooRealVar("Bpt_reco", "Bpt_reco", 0, 1000, "GeV");
        RooRealVar dr12 = RooRealVar("dr12", "dr12", 0, 1000, "GeV");
        RooRealVar dr13 = RooRealVar("dr13", "dr13", 0, 1000, "GeV");
        RooRealVar dr23 = RooRealVar("dr23", "dr23", 0, 1000, "GeV");
        RooRealVar DR_jpsimu = RooRealVar("DR_jpsimu", "DR_jpsimu", -100, 100, "");
        RooRealVar ip3d = RooRealVar("ip3d", "ip3d", -100, 100, "");
        RooRealVar mu1_dz = RooRealVar("mu1_dz", "mu1_dz", -1000, 1000, "GeV");
        RooRealVar mu2_dz = RooRealVar("mu2_dz", "mu2_dz", -1000, 1000, "GeV");
        RooRealVar k_dz = RooRealVar("k_dz", "k_dz", -1000, 1000, "GeV");
        RooRealVar mu1_dxy = RooRealVar("mu1_dxy", "mu1_dxy", -1000, 1000, "GeV");
        RooRealVar mu2_dxy = RooRealVar("mu2_dxy", "mu2_dxy", -1000, 1000, "GeV");
        RooRealVar k_dxy = RooRealVar("k_dxy", "k_dxy", -1000, 1000, "GeV");
        RooRealVar bvtx_cos2D = RooRealVar("bvtx_cos2D", "bvtx_cos2D", 0, 1000, "GeV");
        RooRealVar jpsi_mass = RooRealVar("jpsi_mass", "jpsi_mass", 0, 1000, "GeV");
        RooRealVar Bmass = RooRealVar("Bmass", "Bmass", 0, 1000, "GeV");
        RooRealVar jpsivtx_cos2D = RooRealVar("jpsivtx_cos2D", "jpsivtx_cos2D", 0, 1000, "GeV");
        RooRealVar mu1_isFromMuT = RooRealVar("mu1_isFromMuT", "mu1_isFromMuT", 0, 1000, "GeV");
        RooRealVar mu2_isFromMuT = RooRealVar("mu2_isFromMuT", "mu2_isFromMuT", 0, 1000, "GeV");
        RooRealVar mu1_isFromJpsi_MuT = RooRealVar("mu1_isFromJpsi_MuT", "mu1_isFromJpsi_MuT", 0, 1000, "GeV");
        RooRealVar mu2_isFromJpsi_MuT = RooRealVar("mu2_isFromJpsi_MuT", "mu2_isFromJpsi_MuT", 0, 1000, "GeV");
        RooRealVar k_isFromMuT = RooRealVar("k_isFromMuT", "k_isFromMuT", 0, 1000, "GeV");
        RooRealVar k_raw_db_corr_iso03_rel = RooRealVar("k_raw_db_corr_iso03_rel", "k_raw_db_corr_iso03_rel", -100, 100, "GeV");

        // List of the variables and parameters
        RooArgSet thevars = RooArgSet();
        thevars.add(*varfit);
        thevars.add(mu1pt);
        thevars.add(mu2pt);
        thevars.add(kpt);
        thevars.add(jpsi_pt);
        thevars.add(mu1eta);
        thevars.add(mu2eta);
        thevars.add(keta);
        thevars.add(bvtx_svprob);
        thevars.add(jpsivtx_svprob);
        thevars.add(mu1_mediumID);
        thevars.add(mu2_mediumID);
        thevars.add(k_mediumID);
        thevars.add(Bpt_reco);
        thevars.add(dr12);
        thevars.add(dr13);
        thevars.add(dr23);
        thevars.add(DR_jpsimu);
        thevars.add(ip3d);
        thevars.add(mu1_dz);
        thevars.add(mu2_dz);
        thevars.add(k_dz);
        thevars.add(mu1_dxy);
        thevars.add(mu2_dxy);
        thevars.add(k_dxy);
        thevars.add(bvtx_cos2D);
        thevars.add(jpsi_mass);
        thevars.add(Bmass);
        thevars.add(jpsivtx_cos2D);
        thevars.add(mu1_isFromMuT);
        thevars.add(mu2_isFromMuT);
        thevars.add(mu1_isFromJpsi_MuT);
        thevars.add(mu2_isFromJpsi_MuT);
        thevars.add(k_isFromMuT);
        thevars.add(k_raw_db_corr_iso03_rel);
        /*
                const char *data_files = "InputFiles/data_ptmax_merged.root";
                TTree *data_tree = (TTree *)(TFile::Open(data_files)->Get("BTo3Mu"));
                RooDataSet *fulldata = new RooDataSet("data", "data", data_tree->GetTree(), thevars, filter_mediumid_true.Data());
                RooDataHist *fulldata_binned = new RooDataHist("fulldata_binned", "fulldata_binned", RooArgSet(*varfit), *fulldata);
                TH1F *fulldata_binned_hist = (TH1F*)fulldata_binned->createHistogram("fulldata_binned_hist", *varfit, Binning(bins, minbin,maxbin));
        */
        if (closuretest.find("DATA") != string::npos)
        {
                TH1F *histogram_data_fake = (TH1F *)BtoMu_data_nn.Filter(filter_mediumid_false.Data()).Define("Weight", "FakeProba/(1-FakeProba)").Histo1D<Float_t, Double_t>({varfit_name, varfit_name, int(it_tobefit->bins), it_tobefit->min_bin, it_tobefit->max_bin}, it_tobefit->vartobefitted, "Weight")->Clone();
                histogram_data_fake->Sumw2();
                TH1F *histogram_fakes = (TH1F *)(histogram_data_fake->Clone());

                // Building variable distribution for various contributions
                // Data
                TH1F *histogram_data = (TH1F *)BtoMu_data_nn.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Data"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_data->Sumw2();

                // MC Signal and normalizaton
                TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "JPsiTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_JPsiTau->Sumw2();
                histogram_jpsimumu_is_JPsiTau->Scale(hybrid_normalization);
                TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "JPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_JPsiMu->Sumw2();
                histogram_jpsimumu_is_JPsiMu->Scale(hybrid_normalization);

                // MC Background: Build-up and sum of the components
                TH1F *histogram_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Chic0"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_chic0->Sumw2();
                TH1F *histogram_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Chic1"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_chic1->Sumw2();
                TH1F *histogram_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Chic2"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_chic2->Sumw2();
                TH1F *histogram_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "JPsiHC"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_JPsiHC->Sumw2();
                TH1F *histogram_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "HC"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_Hc->Sumw2();
                TH1F *histogram_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Psi2sTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_Psi2stau->Sumw2();
                TH1F *histogram_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(filter_mediumid_true.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Psi2sMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_Psi2smu->Sumw2();
                TH1F *histogram_hbtojpsix = (TH1F *)HbToJpsiX.Filter(filter_mediumid_true.Data()).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) != 13").Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "HbJPsiX"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_hbtojpsix->Sumw2();
                TH1F *histogram_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(filter_mediumid_true.Data()).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) == 13").Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "HbJPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_chic0->Scale(hybrid_normalization);
                histogram_jpsimumu_is_chic1->Scale(hybrid_normalization);
                histogram_jpsimumu_is_chic2->Scale(hybrid_normalization);
                histogram_jpsimumu_is_JPsiHC->Scale(hybrid_normalization);
                histogram_jpsimumu_is_Hc->Scale(hybrid_normalization);
                histogram_jpsimumu_is_Psi2stau->Scale(hybrid_normalization);
                histogram_jpsimumu_is_Psi2smu->Scale(hybrid_normalization);
                histogram_hbtojpsix->Scale(hbtojpsix_normalization);
                histogram_hbtojpsimu->Scale(hbtojpsimu_normalization);
                THStack *histstack_bkg_distribution = new THStack(TString::Format("%s%s", varfit_name, "HistStack"), TString::Format(varfit_name, "HistStack"));
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic0);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic1);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic2);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_JPsiHC);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_Hc);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_Psi2stau);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_Psi2smu);
                // histstack_bkg_distribution->Add(histogram_hbtojpsix);
                histstack_bkg_distribution->Add(histogram_hbtojpsimu);
                TH1F *histogram_jpsimu_is_bkg = (TH1F *)histstack_bkg_distribution->GetStack()->Last();
                THStack *histstack_mc_distribution = new THStack(TString::Format("%s%s", varfit_name, "MCStack"), TString::Format(varfit_name, "MCStack"));
                histstack_mc_distribution->Add(histogram_jpsimumu_is_JPsiTau);
                histstack_mc_distribution->Add(histogram_jpsimumu_is_JPsiMu);
                histstack_mc_distribution->Add(histogram_jpsimu_is_bkg);
                histstack_mc_distribution->Add(histogram_fakes);
                TH1F *histogram_mc = (TH1F *)histstack_mc_distribution->GetStack()->Last();

                // Building histograms for roofit
                RooDataHist *binned_sig_mc = new RooDataHist("binned_sig_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_JPsiTau);
                RooDataHist *binned_norm_mc = new RooDataHist("binned_norm_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_JPsiMu);
                RooDataHist *binned_bkg_chic0 = new RooDataHist("binned_bkg_chic0", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_chic0);
                RooDataHist *binned_bkg_chic1 = new RooDataHist("binned_bkg_chic1", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_chic1);
                RooDataHist *binned_bkg_chic2 = new RooDataHist("binned_bkg_chic2", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_chic2);
                RooDataHist *binned_bkg_JPsiHC = new RooDataHist("binned_bkg_JPsiHC", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_JPsiHC);
                RooDataHist *binned_bkg_Hc = new RooDataHist("binned_bkg_Hc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_Hc);
                RooDataHist *binned_bkg_Psi2sTau = new RooDataHist("binned_bkg_Psi2sTau", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_Psi2stau);
                RooDataHist *binned_bkg_Psi2sMu = new RooDataHist("binned_bkg_Psi2sMu", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_Psi2smu);
                RooDataHist *binned_bkg_hbjpsimu = new RooDataHist("binned_bkg_hbjpsimu", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_hbtojpsimu);
                RooDataHist *binned_bkg_mc = new RooDataHist("binned_bkg_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimu_is_bkg);
                RooDataHist *binned_fake_mc = new RooDataHist("binned_fake_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_fakes);
                RooDataHist *binned_mc = new RooDataHist("binned_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_mc);
                RooDataHist *binned_data = new RooDataHist("binned_data", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_data);

                /*oooooooooo ooooo ooooooooooooo        .oooooo.   ooooo   ooooo   .oooooo.   ooooo   .oooooo.   oooooooooooo  .oooooo..o
                `888'     `8 `888' 8'   888   `8       d8P'  `Y8b  `888'   `888'  d8P'  `Y8b  `888'  d8P'  `Y8b  `888'     `8 d8P'    `Y8
                888          888       888           888           888     888  888      888  888  888           888         Y88bo.
                888oooo8     888       888           888           888ooooo888  888      888  888  888           888oooo8     `"Y8888o.
                888    "     888       888           888           888     888  888      888  888  888           888    "         `"Y88b
                888          888       888           `88b    ooo   888     888  `88b    d88'  888  `88b    ooo   888       o oo     .d8P
                o888o        o888o     o888o           `Y8bood8P'  o888o   o888o  `Y8bood8P'  o888o  `Y8bood8P'  o888ooooood8 8""88888P' */
                string fittype = "4comp";
                RooDataHist *binned_tobefit = binned_data;
                TH1F *histogram_tobefit = histogram_data;

                // RooFit Variables taking part in the fit
                RooRealVar *realvar_rjpsi = new RooRealVar(TString::Format("RJPsi_of_%s", varfit_name), TString::Format("RJPsi_of_%s", varfit_name),
                                                           RJPsi, 0.01, 2);
                RooRealVar *normalization_amplitude = new RooRealVar(TString::Format("Normalizationch_normalization_of_%s", varfit_name), TString::Format("Normalization normalization of %s", varfit_name),
                                                                     histogram_jpsimumu_is_JPsiMu->Integral(), 1, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *signal_amplitude = new RooRealVar(TString::Format("Signal_normalization_of_%s", varfit_name), TString::Format("Signal normalization of %s", varfit_name),
                                                              histogram_jpsimumu_is_JPsiTau->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooProduct *signal_amplitude_fixed = new RooProduct("signal_amplitude_fixed", "signal_amplitude_fixed", RooArgList(*realvar_rjpsi, *normalization_amplitude));
                RooRealVar *chic0_amplitude = new RooRealVar(TString::Format("chic0_normalization_of_%s", varfit_name), TString::Format("chic0 normalization of %s", varfit_name),
                                                             histogram_jpsimumu_is_chic0->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *chic1_amplitude = new RooRealVar(TString::Format("chic1_normalization_of_%s", varfit_name), TString::Format("chic1 normalization of %s", varfit_name),
                                                             histogram_jpsimumu_is_chic1->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *chic2_amplitude = new RooRealVar(TString::Format("chic2_normalization_of_%s", varfit_name), TString::Format("chic2 normalization of %s", varfit_name),
                                                             histogram_jpsimumu_is_chic2->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *JPsiHC_amplitude = new RooRealVar(TString::Format("JPsiHC_normalization_of_%s", varfit_name), TString::Format("JPsiHC normalization of %s", varfit_name),
                                                              histogram_jpsimumu_is_JPsiHC->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *Hc_amplitude = new RooRealVar(TString::Format("Hc_normalization_of_%s", varfit_name), TString::Format("Hc normalization of %s", varfit_name),
                                                          histogram_jpsimumu_is_Hc->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *Psi2sTau_amplitude = new RooRealVar(TString::Format("Psi2sTau_normalization_of_%s", varfit_name), TString::Format("Psi2sTau normalization of %s", varfit_name),
                                                                histogram_jpsimumu_is_Psi2stau->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *Psi2sMu_amplitude = new RooRealVar(TString::Format("Psi2sMu_normalization_of_%s", varfit_name), TString::Format("Psi2sMu normalization of %s", varfit_name),
                                                               histogram_jpsimumu_is_Psi2smu->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *hbjpsimu_amplitude = new RooRealVar(TString::Format("hbjpsimu_normalization_of_%s", varfit_name), TString::Format("hbjpsimu normalization of %s", varfit_name),
                                                                histogram_hbtojpsimu->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *bkg_amplitude = new RooRealVar(TString::Format("Bkg_normalization_of_%s", varfit_name), TString::Format("Bkg normalization of %s", varfit_name),
                                                           1, 1, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *fake_amplitude = new RooRealVar(TString::Format("Fake_normalization_of_%s", varfit_name), TString::Format("Fake normalization of %s", varfit_name),
                                                            1, 1, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *total_amplitude = new RooRealVar(TString::Format("Total_normalization_of_%s", varfit_name), TString::Format("Total normalization of %s", varfit_name),
                                                             histogram_tobefit->Integral(), 0, 2 * histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                // Extracting pdfs from binned samples
                RooHistPdf *binned_pdf_signal = new RooHistPdf(TString::Format("%s_hist_signal_model", varfit_name), TString::Format("%s_hist_signal_model", varfit_name), RooArgList(*varfit), *binned_sig_mc, interpolation_order);
                RooHistPdf *binned_pdf_normalization = new RooHistPdf(TString::Format("%s_hist_normalization_model", varfit_name), TString::Format("%s_hist_normalization_model", varfit_name), RooArgList(*varfit), *binned_norm_mc, interpolation_order);
                RooHistPdf *binned_pdf_chic0 = new RooHistPdf(TString::Format("%s_hist_chic0_model", varfit_name), TString::Format("%s_hist_chic0_model", varfit_name), RooArgList(*varfit), *binned_bkg_chic0, interpolation_order);
                RooHistPdf *binned_pdf_chic1 = new RooHistPdf(TString::Format("%s_hist_chic1_model", varfit_name), TString::Format("%s_hist_chic1_model", varfit_name), RooArgList(*varfit), *binned_bkg_chic1, interpolation_order);
                RooHistPdf *binned_pdf_chic2 = new RooHistPdf(TString::Format("%s_hist_chic2_model", varfit_name), TString::Format("%s_hist_chic2_model", varfit_name), RooArgList(*varfit), *binned_bkg_chic2, interpolation_order);
                RooHistPdf *binned_pdf_JPsiHC = new RooHistPdf(TString::Format("%s_hist_JPsiHC_model", varfit_name), TString::Format("%s_hist_JPsiHC_model", varfit_name), RooArgList(*varfit), *binned_bkg_JPsiHC, interpolation_order);
                RooHistPdf *binned_pdf_Hc = new RooHistPdf(TString::Format("%s_hist_Hc_model", varfit_name), TString::Format("%s_hist_Hc_model", varfit_name), RooArgList(*varfit), *binned_bkg_Hc, interpolation_order);
                RooHistPdf *binned_pdf_Psi2sTau = new RooHistPdf(TString::Format("%s_hist_Psi2sTau_model", varfit_name), TString::Format("%s_hist_Psi2sTau_model", varfit_name), RooArgList(*varfit), *binned_bkg_Psi2sTau, interpolation_order);
                RooHistPdf *binned_pdf_Psi2sMu = new RooHistPdf(TString::Format("%s_hist_Psi2sMu_model", varfit_name), TString::Format("%s_hist_Psi2sMu_model", varfit_name), RooArgList(*varfit), *binned_bkg_Psi2sMu, interpolation_order);
                RooHistPdf *binned_pdf_hbjpsimu = new RooHistPdf(TString::Format("%s_hist_hbjpsimu_model", varfit_name), TString::Format("%s_hist_hbjpsimu_model", varfit_name), RooArgList(*varfit), *binned_bkg_hbjpsimu, interpolation_order);
                RooHistPdf *binned_pdf_background = new RooHistPdf(TString::Format("%s_hist_background_model", varfit_name), TString::Format("%s_hist_background_model", varfit_name), RooArgList(*varfit), *binned_bkg_mc, interpolation_order);
                RooHistPdf *binned_pdf_fake = new RooHistPdf(TString::Format("%s_hist_fake_model", varfit_name), TString::Format("%s_hist_fake_model", varfit_name), RooArgList(*varfit), *binned_fake_mc, interpolation_order);

                // Defining the fit model
                RooExtendPdf fit_binned_function_normalization = RooExtendPdf("fit_binned_function_normalization", "fit_binned_function_normalization",
                                                                              *binned_pdf_normalization, *varfit);
                RooExtendPdf fit_binned_function_signal = RooExtendPdf("fit_binned_function_signal", "fit_binned_function_signal",
                                                                       *binned_pdf_signal, *varfit);
                RooExtendPdf fit_binned_function_chic0 = RooExtendPdf("fit_binned_function_chic0", "fit_binned_function_chic0",
                                                                      *binned_pdf_chic0, *varfit);
                RooExtendPdf fit_binned_function_chic1 = RooExtendPdf("fit_binned_function_chic1", "fit_binned_function_chic1",
                                                                      *binned_pdf_chic1, *varfit);
                RooExtendPdf fit_binned_function_chic2 = RooExtendPdf("fit_binned_function_chic2", "fit_binned_function_chic2",
                                                                      *binned_pdf_chic2, *varfit);
                RooExtendPdf fit_binned_function_JPsiHC = RooExtendPdf("fit_binned_function_JPsiHC", "fit_binned_function_JPsiHC",
                                                                       *binned_pdf_JPsiHC, *varfit);
                RooExtendPdf fit_binned_function_Hc = RooExtendPdf("fit_binned_function_Hc", "fit_binned_function_Hc",
                                                                   *binned_pdf_Hc, *varfit);
                RooExtendPdf fit_binned_function_Psi2sTau = RooExtendPdf("fit_binned_function_Psi2sTau", "fit_binned_function_Psi2sTau",
                                                                         *binned_pdf_Psi2sTau, *varfit);
                RooExtendPdf fit_binned_function_Psi2sMu = RooExtendPdf("fit_binned_function_Psi2sMu", "fit_binned_function_Psi2sMu",
                                                                        *binned_pdf_Psi2sMu, *varfit);
                RooExtendPdf fit_binned_function_hbjpsimu = RooExtendPdf("fit_binned_function_hbjpsimu", "fit_binned_function_hbjpsimu",
                                                                         *binned_pdf_hbjpsimu, *varfit);
                RooExtendPdf fit_binned_function_background = RooExtendPdf("fit_binned_function_background", "fit_binned_function_background",
                                                                           *binned_pdf_background, *varfit);
                RooExtendPdf fit_binned_function_fake = RooExtendPdf("fit_binned_function_fake", "fit_binned_function_fake",
                                                                     *binned_pdf_fake, *varfit);

                RooFormulaVar binsig_formula = RooFormulaVar("binsig_plus_binnorm", "(@1*@0)", RooArgList(*realvar_rjpsi, fit_binned_function_signal));
                RooWrapperPdf fit_binsig("fit_binsig", "fit_binsig", binsig_formula);
                RooAddPdf fit_binsignorm_rjpsi = RooAddPdf("fit_binsignorm_rjpsi", "fit_binsignorm_rjpsi", RooArgList(fit_binsig, fit_binned_function_normalization), RooArgList(*signal_amplitude, *normalization_amplitude));

                RooFormulaVar binsigplusnorm_formula = RooFormulaVar("binsig_plus_binnorm", "@1*@0+@2", RooArgList(*realvar_rjpsi, fit_binned_function_signal, fit_binned_function_normalization));
                RooWrapperPdf fit_binsigplusnorm("fit_binsigplusnorm", "fit_binsigplusnorm", binsigplusnorm_formula);

                /*.oooo.          .oooooo.     .oooooo.   ooo        ooooo ooooooooo.
                .dP""Y88b        d8P'  `Y8b   d8P'  `Y8b  `88.       .888' `888   `Y88.
                ]8P'      888          888      888  888b     d'888   888   .d88'
                <88b.       888          888      888  8 Y88. .P  888   888ooo88P'
                `88b.      888          888      888  8  `888'   888   888
                o.   .88P       `88b    ooo  `88b    d88'  8    Y     888   888
                `8bd88P'         `Y8bood8P'   `Y8bood8P'  o8o        o888o o888o */

                if (fittype.find("3comp") != string::npos)
                {
                        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binsigplusnorm, fit_binned_function_background, fit_binned_function_fake), RooArgList(*normalization_amplitude, *bkg_amplitude, *fake_amplitude));
                        // RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binsignorm_rjpsi, fit_binned_function_background, fit_binned_function_fake), RooArgList(*total_amplitude, *bkg_amplitude, *fake_amplitude));

                        // RooAbsPdf *fit_model = RooClassFactory::makePdfInstance("RooMyPdf", "(@3*(@0*@1+@2))+(1-@3)*(@4+@5)",  RooArgSet(*realvar_rjpsi, fit_binned_function_signal, fit_binned_function_normalization, *normalization_amplitude, fit_binned_function_background, fit_binned_function_fake));
                        RooFormulaVar fitmodelformula = RooFormulaVar("fitmodelformula", "(@3*(@0*@1+@2))+(1-@3)*(@4+@5)", RooArgList(*realvar_rjpsi, fit_binned_function_signal, fit_binned_function_normalization, *normalization_amplitude, fit_binned_function_background, fit_binned_function_fake));
                        // RooWrapperPdf fit_model("fit_func", "fit_func", fitmodelformula);
                        // RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", fit_func, RooArgList(*total_amplitude));

                        // RooFitResult *results_data = fit_model.fitTo(*binned_tobefit, RooFit::Save(true), RooFit::Minos(true), Constrain(*realvar_rjpsi));

                        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*(fit_model.createNLL(*binned_tobefit, Extended(true), Offset(true) /*, IntegrateBins(0.01)*/)));
                        roofit_minimizer_interface->migrad();
                        roofit_minimizer_interface->minos();
                        roofit_minimizer_interface->hesse();
                        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

                        Double_t signamp = signal_amplitude->getVal(),
                                 normamp = normalization_amplitude->getVal(),
                                 rjpsierr = realvar_rjpsi->getPropagatedError(*results_data),
                                 normamperr = normalization_amplitude->getPropagatedError(*results_data),
                                 bkgamp = bkg_amplitude->getVal(),
                                 fakeamp = fake_amplitude->getVal(),
                                 totamp = normamp + bkgamp + fakeamp,
                                 rjpsi_plain = realvar_rjpsi->getVal(),
                                 rjpsi_inverted = 1 / realvar_rjpsi->getVal(),
                                 signaloneamp = (rjpsi_plain) * (1 / (rjpsi_plain + 1)) * (normamp),
                                 normaloneamp = (1 / (rjpsi_plain + 1)) * (normamp),
                                 signamperr = normamperr * (rjpsi_plain) * (1 / (rjpsi_plain + 1)) + normamp * rjpsierr / ((rjpsi_plain + 1) * (rjpsi_plain + 1));
                        cout << signamperr << endl;
                        cout << "Signamp: " << signamp << " Normamp: " << normamp << endl;
                        cout << "Signaloneamp: " << signaloneamp << " Normaloneamp: " << normaloneamp << endl;
                        cout << "R(J/Psi) from amplitudes: " << signaloneamp / normaloneamp << " +- " << signamperr / normaloneamp + signaloneamp * normamperr / (normaloneamp * normaloneamp) << endl;

                        Double_t tau_eff = histogram_jpsimumu_is_JPsiTau->Integral() / (BtoJpsiMuMu_is_JPsiTau.Count().GetValue()),
                                 mu_eff = histogram_jpsimumu_is_JPsiMu->Integral() / (BtoJpsiMuMu_is_JPsiMu.Count().GetValue());
                        Double_t rjpsi = histogram_jpsimumu_is_JPsiTau->Integral() / histogram_jpsimumu_is_JPsiMu->Integral();
                        Double_t rjpsi_error = (histogram_jpsimumu_is_JPsiTau->GetRMS() / histogram_jpsimumu_is_JPsiTau->Integral() + histogram_jpsimumu_is_JPsiMu->GetRMS() / histogram_jpsimumu_is_JPsiMu->Integral()) * rjpsi;
                        cout << "Signal events pre cut: " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() << endl;
                        cout << "Signal events post cut: " << histogram_jpsimumu_is_JPsiTau->Integral() << " Normalization events post cut: " << histogram_jpsimumu_is_JPsiMu->Integral() << endl;
                        cout << "Signal events efficiency: " << tau_eff << " Normalization events efficiency: " << mu_eff << endl;
                        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
                        RooAbsReal *sigregion = fit_binned_function_signal.createIntegral(*varfit, NormSet(*normalization_amplitude));         // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        RooAbsReal *normregion = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude)); // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        Double_t nsigevents = sigregion->getVal() * (signamp);
                        Double_t nnormevents = normregion->getVal() * (normamp);
                        Double_t nsigeventserror = sigregion->getPropagatedError(*results_data) * (nsigevents);
                        Double_t nnormeventserror = normregion->getPropagatedError(*results_data) * (nnormevents);
                        cout << "SignEvents for signorm: " << nsigevents << " NormEvents for signorm: " << nnormevents << endl;
                        cout << "SignEvents error: " << nsigeventserror << " NormEvents error: " << nnormeventserror << endl;

                        // Plotting
                        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"), 1920, 1080);
                        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
                        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

                        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
                        fit_model.fillHistogram(histogram_fitmodel, *varfit, totamp);
                        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
                        fit_binned_function_normalization.fillHistogram(histogram_fitnormalization, *varfit, normaloneamp);
                        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
                        fit_binned_function_signal.fillHistogram(histogram_fitsignal, *varfit, signaloneamp);
                        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
                        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkgamp);
                        TH1F *histogram_fitfake = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), bins, minbin, maxbin);
                        fit_binned_function_fake.fillHistogram(histogram_fitfake, *varfit, fakeamp);
                        THStack *inclusive_histstack = new THStack(TString::Format("%s%s", varfit_name, "inchiststack"), TString::Format("%s%s", varfit_name, "inchiststack"));

                        // Drawing
                        inclusive_canvas->cd(0);
                        inclusive_canvas->Clear();
                        inclusive_canvas->Draw();
                        inclusive_pad_distribution->SetTopMargin(0.05);
                        inclusive_pad_distribution->SetBottomMargin(0.);
                        inclusive_pad_distribution->SetBorderMode(0);
                        inclusive_pad_distribution->SetLeftMargin(0.1);
                        inclusive_pad_residuals->SetTopMargin(0.);
                        inclusive_pad_residuals->SetBottomMargin(0.2);
                        inclusive_pad_residuals->SetBorderMode(0);
                        inclusive_pad_residuals->SetLeftMargin(0.1);
                        gStyle->SetOptTitle(kFALSE);
                        inclusive_pad_distribution->Draw();
                        inclusive_pad_residuals->Draw();
                        gStyle->SetPalette(kRainBow);
                        inclusive_pad_distribution->cd();
                        // inclusive_histstack->Add(histogram_fitmodel);
                        histogram_fitnormalization->SetLineWidth(3);
                        histogram_fitnormalization->SetStats(false);
                        inclusive_histstack->Add(histogram_fitnormalization);
                        histogram_fitsignal->SetLineWidth(3);
                        histogram_fitsignal->SetStats(false);
                        inclusive_histstack->Add(histogram_fitsignal);
                        histogram_fitbackground->SetLineWidth(3);
                        histogram_fitbackground->SetStats(false);
                        inclusive_histstack->Add(histogram_fitbackground);
                        histogram_fitfake->SetLineWidth(3);
                        histogram_fitfake->SetStats(false);
                        inclusive_histstack->Add(histogram_fitfake);
                        inclusive_histstack->Draw("HIST PFC PMC PLC");
                        inclusive_histstack->GetHistogram()->GetYaxis()->ChangeLabel(1, -1, 0);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, it_tobefit->vardimension.c_str()));
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleOffset(1);
                        histogram_fitmodel->SetMarkerColor(kGray);
                        histogram_fitmodel->SetLineColor(kGray);
                        histogram_fitmodel->SetLineWidth(3);
                        histogram_fitmodel->SetStats(false);
                        // histogram_fitmodel->Draw("SAME");
                        histogram_tobefit->SetMarkerStyle(kFullCircle);
                        histogram_tobefit->SetMarkerSize(1.5);
                        histogram_tobefit->SetMarkerColor(kBlack);
                        histogram_tobefit->SetLineColor(kBlack);
                        histogram_tobefit->SetFillColor(kBlack);
                        histogram_tobefit->SetStats(false);
                        histogram_tobefit->Draw("SAME ME");
                        TLegend leg_fitwithratio = it_tobefit->SetLegendPosAuto(it_tobefit->legpos, 6, it_tobefit->legxlength);
                        leg_fitwithratio.SetTextSize(0.035);
                        /*leg_fitwithratio.SetBorderSize(0);
                        leg_fitwithratio.SetFillColor(0);
                        leg_fitwithratio.SetFillStyle(0);*/

                        // RooFit
                        leg_fitwithratio.AddEntry("Data", "Data");
                        leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "F");
                        leg_fitwithratio.AddEntry(histogram_fitfake->GetName(), "Fakes", "F");
                        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "B_{c}#rightarrowJ/#Psi#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "B_{c}#rightarrowJ/#Psi#mu", "F");
                        leg_fitwithratio.DrawClone("SAME");

                        TPaveText pavtext_fitwithratio = it_tobefit->SetPaveTextPosManual(0, leg_fitwithratio.GetY1(), it_tobefit->legpos, 2, it_tobefit->legxlength);
                        pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi) from fit: %.2f #pm %.2f", realvar_rjpsi->getVal(), realvar_rjpsi->getPropagatedError(*results_data)));
                        pavtext_fitwithratio.SetTextSize(0.035);
                        pavtext_fitwithratio.SetTextFont(42);
                        pavtext_fitwithratio.DrawClone("SAME");
                        gPad->SetLogy(0);

                        inclusive_pad_residuals->cd();
                        TH1F *inclusive_distributions_ratio = new TH1F();
                        inclusive_distributions_ratio = (TH1F *)histogram_tobefit->Clone();
                        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
                        inclusive_distributions_ratio->SetStats(false);
                        inclusive_distributions_ratio->Draw("M");
                        // inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
                        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
                        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->varname.data(), it_tobefit->vardimension.c_str()));
                        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.8);
                        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
                        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
                        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.45);
                        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
                        inclusive_pad_residuals->Modified();

                        inclusive_canvas->Print(TString::Format("OutputFiles/PNGPlots/FitResults/FitRatioData_Plot_%s_%dbins_3comp.png", varfit_name, int(it_tobefit->bins)).Data());
                        inclusive_canvas->Clear();
                        results_data->printMultiline(cout, 20);
                }

         /*       .o          .oooooo.     .oooooo.   ooo        ooooo ooooooooo.
                .d88         d8P'  `Y8b   d8P'  `Y8b  `88.       .888' `888   `Y88.
             .d'888        888          888      888  888b     d'888   888   .d88'
           .d'  888        888          888      888  8 Y88. .P  888   888ooo88P'
           88ooo888oo      888          888      888  8  `888'   888   888
                888        `88b    ooo  `88b    d88'  8    Y     888   888
                o888o        `Y8bood8P'   `Y8bood8P'  o8o        o888o o888o*/

                if (fittype.find("4comp") != string::npos)
                {
                        RooFormulaVar sumCoeff("sumCoeff", "@1+@2+@3+@0 + (1-@1-@2-@3-@0)", RooArgList(*signal_amplitude, *normalization_amplitude, *bkg_amplitude, *fake_amplitude));
                        RooGaussian constraint("constraint", "constraint", RooConst(1), sumCoeff, RooConst(0.01));
                        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binned_function_signal, fit_binned_function_normalization, fit_binned_function_background, fit_binned_function_fake),
                                                        RooArgList(*signal_amplitude, *normalization_amplitude, *bkg_amplitude, *fake_amplitude));

                        RooAbsReal *nll = fit_model.createNLL(*binned_tobefit, IntegrateBins(0.01), Extended(true), Offset(true), ExternalConstraints(constraint));
                        RooMinimizer *roofit_minimizer_interface0 = new RooMinimizer(*nll);
                        roofit_minimizer_interface0->migrad();
                        roofit_minimizer_interface0->minos();
                        roofit_minimizer_interface0->hesse();

                        RooRealVar bkg_br_mean = RooRealVar("bkg_br_mean", "bkg_br_mean", bkg_amplitude->getVal(), -0.01, bkg_amplitude->getVal()*2);
                        RooRealVar bkg_br_sigma = RooRealVar("bkg_br_sigma", "bkg_br_sigma", 0.3*bkg_amplitude->getVal(), -0.01, bkg_amplitude->getVal());
                        RooGaussian BR("BRconstraint", "BRconstraint", *bkg_amplitude, bkg_br_mean, bkg_br_sigma);
                        RooProdPdf fit_model_smeared("fit_model_smeared", "fit_model_smeared", RooArgList(fit_model, BR));
                        RooAbsReal *nll_smeared = fit_model_smeared.createNLL(*binned_tobefit, IntegrateBins(0.01), Extended(true), Offset(true), ExternalConstraints(BR));
                        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*nll_smeared);
                        roofit_minimizer_interface->migrad();
                        roofit_minimizer_interface->minos();
                        roofit_minimizer_interface->hesse();

                        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

                        Double_t signamp = signal_amplitude->getVal(),
                                 normamp = normalization_amplitude->getVal(),
                                 rjpsiamp = signamp / normamp,
                                 signamperr = signal_amplitude->getError(),
                                 normamperr = normalization_amplitude->getError(),
                                 rjpsiamperr = rjpsiamp * (signamperr / signamp + normamperr / normamp),
                                 bkgamp = bkg_amplitude->getVal(),
                                 fakeamp = fake_amplitude->getVal(),
                                 totamp = signamp + normamp + bkgamp + fakeamp,
                                 rjpsi_plain = realvar_rjpsi->getVal(),
                                 rjpsi_inverted = 1 / realvar_rjpsi->getVal(),
                                 signaloneamp = (rjpsi_plain) * (1 / (rjpsi_plain + 1)) * (normamp),
                                 normaloneamp = (1 / (rjpsi_plain + 1)) * (normamp);
                        cout << "Signamp: " << signamp << " +- " << signamperr << " Normamp: " << normamp << " +- " << normamperr << endl;
                        cout << "R(J/Psi) from amplitudes: " << signamp / normamp << " +- " << signamp / normamp * (signamperr / signamp + normamperr / normamp) << endl;
                        cout << "Signaloneamp: " << signaloneamp << " Normaloneamp: " << normaloneamp << endl;

                        varfit->setRange("Var_Range", minbin, maxbin);
                        RooAbsReal *varfit_region = varfit->createIntegral(*varfit, NormSet(*varfit), Range("Var_Range"));
                        Double_t nevents_signal = varfit_region->getVal() * signal_amplitude->getVal();
                        Double_t nevents_normalization = varfit_region->getVal() * normalization_amplitude->getVal();
                        cout << "Number of events from the signal region: " << nevents_signal << endl;
                        cout << "Number of events from the normalization region: " << nevents_normalization << endl;

                        Double_t tau_eff = histogram_jpsimumu_is_JPsiTau->Integral() / (BtoJpsiMuMu_is_JPsiTau.Count().GetValue()),
                                 mu_eff = histogram_jpsimumu_is_JPsiMu->Integral() / (BtoJpsiMuMu_is_JPsiMu.Count().GetValue());
                        Double_t rjpsi = histogram_jpsimumu_is_JPsiTau->Integral() / histogram_jpsimumu_is_JPsiMu->Integral();
                        Double_t rjpsi_error = (histogram_jpsimumu_is_JPsiTau->GetRMS() / histogram_jpsimumu_is_JPsiTau->Integral() + histogram_jpsimumu_is_JPsiMu->GetRMS() / histogram_jpsimumu_is_JPsiMu->Integral()) * rjpsi;
                        cout << "Signal events pre cut: " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() << endl;
                        cout << "Signal events post cut: " << histogram_jpsimumu_is_JPsiTau->Integral() << " Normalization events post cut: " << histogram_jpsimumu_is_JPsiMu->Integral() << endl;
                        cout << "Signal events efficiency: " << tau_eff << " Normalization events efficiency: " << mu_eff << endl;
                        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
                        RooAbsReal *sigregion = fit_binned_function_signal.createIntegral(*varfit, NormSet(*normalization_amplitude));         // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        RooAbsReal *normregion = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude)); // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        Double_t nsigevents = sigregion->getVal() * (signamp);
                        Double_t nnormevents = normregion->getVal() * (normamp);
                        Double_t nsigeventserror = sigregion->getPropagatedError(*results_data) * (nsigevents);
                        Double_t nnormeventserror = normregion->getPropagatedError(*results_data) * (nnormevents);
                        cout << "SignEvents for signorm: " << nsigevents << " NormEvents for signorm: " << nnormevents << endl;
                        cout << "SignEvents error: " << nsigeventserror << " NormEvents error: " << nnormeventserror << endl;

                        // Plotting
                        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"), 1920, 1080);
                        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
                        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

                        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
                        fit_model.fillHistogram(histogram_fitmodel, *varfit, totamp);
                        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
                        fit_binned_function_normalization.fillHistogram(histogram_fitnormalization, *varfit, normamp);
                        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
                        fit_binned_function_signal.fillHistogram(histogram_fitsignal, *varfit, signamp);
                        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
                        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkgamp);
                        TH1F *histogram_fitfake = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), bins, minbin, maxbin);
                        fit_binned_function_fake.fillHistogram(histogram_fitfake, *varfit, fakeamp);
                        THStack *inclusive_histstack = new THStack(TString::Format("%s%s", varfit_name, "inchiststack"), TString::Format("%s%s", varfit_name, "inchiststack"));

                        // Drawing
                        inclusive_canvas->cd(0);
                        inclusive_canvas->Clear();
                        inclusive_canvas->Draw();
                        inclusive_pad_distribution->SetTopMargin(0.05);
                        inclusive_pad_distribution->SetBottomMargin(0.);
                        inclusive_pad_distribution->SetBorderMode(0);
                        inclusive_pad_distribution->SetLeftMargin(0.1);
                        inclusive_pad_residuals->SetTopMargin(0.);
                        inclusive_pad_residuals->SetBottomMargin(0.2);
                        inclusive_pad_residuals->SetBorderMode(0);
                        inclusive_pad_residuals->SetLeftMargin(0.1);
                        gStyle->SetOptTitle(kFALSE);
                        inclusive_pad_distribution->Draw();
                        inclusive_pad_residuals->Draw();
                        gStyle->SetPalette(kRainBow);
                        inclusive_pad_distribution->cd();
                        // inclusive_histstack->Add(histogram_fitmodel);
                        histogram_fitnormalization->SetLineWidth(3);
                        histogram_fitnormalization->SetStats(false);
                        inclusive_histstack->Add(histogram_fitnormalization);
                        histogram_fitsignal->SetLineWidth(3);
                        histogram_fitsignal->SetStats(false);
                        inclusive_histstack->Add(histogram_fitsignal);
                        histogram_fitbackground->SetLineWidth(3);
                        histogram_fitbackground->SetStats(false);
                        inclusive_histstack->Add(histogram_fitbackground);
                        histogram_fitfake->SetLineWidth(3);
                        histogram_fitfake->SetStats(false);
                        inclusive_histstack->Add(histogram_fitfake);
                        inclusive_histstack->Draw("HIST PFC PMC PLC");
                        inclusive_histstack->GetHistogram()->GetYaxis()->ChangeLabel(1, -1, 0);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, it_tobefit->vardimension.c_str()));
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleOffset(1);
                        histogram_fitmodel->SetMarkerColor(kGray);
                        histogram_fitmodel->SetLineColor(kGray);
                        histogram_fitmodel->SetLineWidth(3);
                        histogram_fitmodel->SetStats(false);
                        // histogram_fitmodel->Draw("SAME");
                        histogram_tobefit->SetMarkerStyle(kFullCircle);
                        histogram_tobefit->SetMarkerSize(1.5);
                        histogram_tobefit->SetMarkerColor(kBlack);
                        histogram_tobefit->SetLineColor(kBlack);
                        histogram_tobefit->SetFillColor(kBlack);
                        histogram_tobefit->SetStats(false);
                        histogram_tobefit->Draw("SAME ME");
                        TLegend leg_fitwithratio = it_tobefit->SetLegendPosAuto(it_tobefit->legpos, 6, it_tobefit->legxlength);
                        leg_fitwithratio.SetTextSize(0.035);
                        /*leg_fitwithratio.SetBorderSize(0);
                        leg_fitwithratio.SetFillColor(0);
                        leg_fitwithratio.SetFillStyle(0);*/

                        // RooFit
                        leg_fitwithratio.AddEntry("Data", "Data");
                        leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "F");
                        leg_fitwithratio.AddEntry(histogram_fitfake->GetName(), "Fakes", "F");
                        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "B_{c}#rightarrowJ/#Psi#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "B_{c}#rightarrowJ/#Psi#mu", "F");
                        leg_fitwithratio.DrawClone("SAME");

                        TPaveText pavtext_fitwithratio = it_tobefit->SetPaveTextPosManual(0, leg_fitwithratio.GetY1(), it_tobefit->legpos, 4, it_tobefit->legxlength);
                        pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                        pavtext_fitwithratio.AddText(TString::Format("B_{c}#rightarrowJ/#Psi#tau events: %.0f#pm%.0f", signamp, signamperr));
                        pavtext_fitwithratio.AddText(TString::Format("B_{c}#rightarrowJ/#Psi#mu events: %.0f#pm%.0f", normamp, normamperr));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi): %.2f #pm %.2f", rjpsiamp * normal_efficiency / signal_efficiency, rjpsiamperr * normal_efficiency / signal_efficiency + rjpsiamp * 0.05));
                        pavtext_fitwithratio.SetTextSize(0.035);
                        pavtext_fitwithratio.SetTextFont(42);
                        pavtext_fitwithratio.DrawClone("SAME");
                        gPad->SetLogy(0);

                        inclusive_pad_residuals->cd();
                        TH1F *inclusive_distributions_ratio = new TH1F();
                        inclusive_distributions_ratio = (TH1F *)histogram_tobefit->Clone();
                        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
                        inclusive_distributions_ratio->SetStats(false);
                        inclusive_distributions_ratio->Draw("M");
                        // inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
                        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
                        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->varname.data(), it_tobefit->vardimension.c_str()));
                        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.8);
                        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
                        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
                        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.45);
                        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
                        inclusive_pad_residuals->Modified();

                        inclusive_canvas->Print(TString::Format("OutputFiles/PNGPlots/FitResults/FitRatioData_Plot_%s_%dbins_4comp.png", varfit_name, int(it_tobefit->bins)).Data());
                        inclusive_canvas->Clear();
                        results_data->printMultiline(cout, 20);
                        RooPlot *frame_data = varfit->frame();
                        frame_data->SetTitle("");
                        binned_data->plotOn(frame_data, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack));
                        fit_model.plotOn(frame_data, Name("data_signal_fit_function_Norm"), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
                        Double_t chi2_datafit = frame_data->chiSquare();
                        cout << endl
                             << endl
                             << endl
                             << endl
                             << endl
                             << varfit_name << " Variable in " << bins << " bins" << endl;
                        cout << "RJPsi:" << rjpsiamp * normal_efficiency / signal_efficiency << "+-" << rjpsiamperr * normal_efficiency / signal_efficiency + rjpsiamp * 0.05 << endl;
                        cout << "Chi2: " << chi2_datafit << endl
                             << endl
                             << endl
                             << endl
                             << endl;
                }

                /*.o    .oooo.          .oooooo.     .oooooo.   ooo        ooooo ooooooooo.
                o888   d8P'`Y8b        d8P'  `Y8b   d8P'  `Y8b  `88.       .888' `888   `Y88.
                888  888    888      888          888      888  888b     d'888   888   .d88'
                888  888    888      888          888      888  8 Y88. .P  888   888ooo88P'
                888  888    888      888          888      888  8  `888'   888   888
                888  `88b  d88'      `88b    ooo  `88b    d88'  8    Y     888   888
                o888o  `Y8bd8P'        `Y8bood8P'   `Y8bood8P'  o8o        o888o o888o */

                if (fittype.find("10comp") != string::npos)
                {
                        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binsigplusnorm, fit_binned_function_fake, fit_binned_function_chic0, fit_binned_function_chic1, fit_binned_function_chic2, fit_binned_function_JPsiHC, fit_binned_function_Hc, fit_binned_function_Psi2sTau, fit_binned_function_Psi2sMu, fit_binned_function_hbjpsimu),
                                                        RooArgList(*normalization_amplitude, *fake_amplitude,
                                                                   *chic0_amplitude, *chic1_amplitude, *chic2_amplitude, *JPsiHC_amplitude,
                                                                   *Hc_amplitude, *Psi2sTau_amplitude, *Psi2sMu_amplitude, *hbjpsimu_amplitude));

                        // RooFitResult *results_data = fit_model.fitTo(*binned_tobefit, RooFit::Save(true), RooFit::Minos(true), Constrain(*realvar_rjpsi));

                        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*(fit_model.createNLL(*binned_tobefit, Extended(true), Offset(true), Constrain(*realvar_rjpsi))));
                        roofit_minimizer_interface->migrad();
                        roofit_minimizer_interface->minos();
                        roofit_minimizer_interface->hesse();
                        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

                        Double_t signamp = signal_amplitude->getVal(),
                                 normamp = normalization_amplitude->getVal(),
                                 chic0amp = chic0_amplitude->getVal(),
                                 chic1amp = chic1_amplitude->getVal(),
                                 chic2amp = chic2_amplitude->getVal(),
                                 JPsiHCamp = JPsiHC_amplitude->getVal(),
                                 Hcamp = Hc_amplitude->getVal(),
                                 Psi2sTauamp = Psi2sTau_amplitude->getVal(),
                                 Psi2sMuamp = Psi2sMu_amplitude->getVal(),
                                 hbjpsimuamp = hbjpsimu_amplitude->getVal(),
                                 bkgamp = bkg_amplitude->getVal(),
                                 fakeamp = fake_amplitude->getVal(),
                                 totamp = normamp + bkgamp + fakeamp,
                                 rjpsi_plain = realvar_rjpsi->getVal(),
                                 rjpsi_inverted = 1 / realvar_rjpsi->getVal(),
                                 signaloneamp = (rjpsi_plain) * (1 / (rjpsi_plain + 1)) * (normamp),
                                 normaloneamp = (1 / (rjpsi_plain + 1)) * (normamp);
                        cout << "Signamp: " << signamp << " Normamp: " << normamp << endl;
                        cout << "Signaloneamp: " << signaloneamp << " Normaloneamp: " << normaloneamp << endl;

                        Double_t tau_eff = histogram_jpsimumu_is_JPsiTau->Integral() / (BtoJpsiMuMu_is_JPsiTau.Count().GetValue()),
                                 mu_eff = histogram_jpsimumu_is_JPsiMu->Integral() / (BtoJpsiMuMu_is_JPsiMu.Count().GetValue());
                        Double_t rjpsi = histogram_jpsimumu_is_JPsiTau->Integral() / histogram_jpsimumu_is_JPsiMu->Integral();
                        Double_t rjpsi_error = (histogram_jpsimumu_is_JPsiTau->GetRMS() / histogram_jpsimumu_is_JPsiTau->Integral() + histogram_jpsimumu_is_JPsiMu->GetRMS() / histogram_jpsimumu_is_JPsiMu->Integral()) * rjpsi;
                        cout << "Signal events pre cut: " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() << endl;
                        cout << "Signal events post cut: " << histogram_jpsimumu_is_JPsiTau->Integral() << " Normalization events post cut: " << histogram_jpsimumu_is_JPsiMu->Integral() << endl;
                        cout << "Signal events efficiency: " << tau_eff << " Normalization events efficiency: " << mu_eff << endl;
                        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
                        RooAbsReal *sigregion = fit_binned_function_signal.createIntegral(*varfit, NormSet(*normalization_amplitude));         // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        RooAbsReal *normregion = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude)); // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        Double_t nsigevents = sigregion->getVal() * (signamp);
                        Double_t nnormevents = normregion->getVal() * (normamp);
                        Double_t nsigeventserror = sigregion->getPropagatedError(*results_data) * (nsigevents);
                        Double_t nnormeventserror = normregion->getPropagatedError(*results_data) * (nnormevents);
                        cout << "SignEvents for signorm: " << nsigevents << " NormEvents for signorm: " << nnormevents << endl;
                        cout << "SignEvents error: " << nsigeventserror << " NormEvents error: " << nnormeventserror << endl;

                        // Plotting
                        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"), 1920, 1080);
                        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
                        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

                        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
                        fit_model.fillHistogram(histogram_fitmodel, *varfit, totamp);
                        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
                        fit_binned_function_normalization.fillHistogram(histogram_fitnormalization, *varfit, normaloneamp);
                        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
                        fit_binned_function_signal.fillHistogram(histogram_fitsignal, *varfit, signaloneamp);
                        TH1F *histogram_fitchic0 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic0"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic0"), bins, minbin, maxbin);
                        fit_binned_function_chic0.fillHistogram(histogram_fitchic0, *varfit, chic0amp);
                        TH1F *histogram_fitchic1 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic1"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic1"), bins, minbin, maxbin);
                        fit_binned_function_chic1.fillHistogram(histogram_fitchic1, *varfit, chic1amp);
                        TH1F *histogram_fitchic2 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic2"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic2"), bins, minbin, maxbin);
                        fit_binned_function_chic2.fillHistogram(histogram_fitchic2, *varfit, chic2amp);
                        TH1F *histogram_fitJPsiHC = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_JPsiHC"), TString::Format("%s%s", varfit_name, "fit_binned_function_JPsiHC"), bins, minbin, maxbin);
                        fit_binned_function_JPsiHC.fillHistogram(histogram_fitJPsiHC, *varfit, JPsiHCamp);
                        TH1F *histogram_fitHc = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Hc"), TString::Format("%s%s", varfit_name, "fit_binned_function_Hc"), bins, minbin, maxbin);
                        fit_binned_function_Hc.fillHistogram(histogram_fitHc, *varfit, Hcamp);
                        TH1F *histogram_fitPsi2sTau = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sTau"), TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sTau"), bins, minbin, maxbin);
                        fit_binned_function_Psi2sTau.fillHistogram(histogram_fitPsi2sTau, *varfit, Psi2sTauamp);
                        TH1F *histogram_fitPsi2sMu = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sMu"), TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sMu"), bins, minbin, maxbin);
                        fit_binned_function_Psi2sMu.fillHistogram(histogram_fitPsi2sMu, *varfit, Psi2sMuamp);
                        TH1F *histogram_fithbjpsimu = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_hbjpsimu"), TString::Format("%s%s", varfit_name, "fit_binned_function_hbjpsimu"), bins, minbin, maxbin);
                        fit_binned_function_hbjpsimu.fillHistogram(histogram_fithbjpsimu, *varfit, hbjpsimuamp);
                        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
                        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkgamp);
                        TH1F *histogram_fitfake = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), bins, minbin, maxbin);
                        fit_binned_function_fake.fillHistogram(histogram_fitfake, *varfit, fakeamp);
                        THStack *inclusive_histstack = new THStack(TString::Format("%s%s", varfit_name, "inchiststack"), TString::Format("%s%s", varfit_name, "inchiststack"));

                        // Drawing
                        inclusive_canvas->cd(0);
                        inclusive_canvas->Clear();
                        inclusive_canvas->Draw();
                        inclusive_pad_distribution->SetTopMargin(0.05);
                        inclusive_pad_distribution->SetBottomMargin(0.);
                        inclusive_pad_distribution->SetBorderMode(0);
                        inclusive_pad_distribution->SetLeftMargin(0.1);
                        inclusive_pad_residuals->SetTopMargin(0.);
                        inclusive_pad_residuals->SetBottomMargin(0.2);
                        inclusive_pad_residuals->SetBorderMode(0);
                        inclusive_pad_residuals->SetLeftMargin(0.1);
                        gStyle->SetOptTitle(kFALSE);
                        inclusive_pad_distribution->Draw();
                        inclusive_pad_residuals->Draw();
                        gStyle->SetPalette(kRainBow);
                        inclusive_pad_distribution->cd();
                        histogram_fitmodel->SetMarkerColor(kBlue);
                        histogram_fitmodel->SetLineColor(kBlue);
                        histogram_fitmodel->SetLineWidth(3);
                        histogram_fitmodel->SetStats(false);
                        // inclusive_histstack->Add(histogram_fitmodel);
                        histogram_fitnormalization->SetLineWidth(3);
                        histogram_fitnormalization->SetStats(false);
                        inclusive_histstack->Add(histogram_fitnormalization);
                        histogram_fitsignal->SetLineWidth(3);
                        histogram_fitsignal->SetStats(false);
                        inclusive_histstack->Add(histogram_fitsignal);
                        histogram_fitchic0->SetLineWidth(3);
                        histogram_fitchic0->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic0);
                        histogram_fitchic1->SetLineWidth(3);
                        histogram_fitchic1->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic1);
                        histogram_fitchic2->SetLineWidth(3);
                        histogram_fitchic2->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic2);
                        histogram_fitJPsiHC->SetLineWidth(3);
                        histogram_fitJPsiHC->SetStats(false);
                        inclusive_histstack->Add(histogram_fitJPsiHC);
                        histogram_fitHc->SetLineWidth(3);
                        histogram_fitHc->SetStats(false);
                        inclusive_histstack->Add(histogram_fitHc);
                        histogram_fitPsi2sTau->SetLineWidth(3);
                        histogram_fitPsi2sTau->SetStats(false);
                        inclusive_histstack->Add(histogram_fitPsi2sTau);
                        histogram_fitPsi2sMu->SetLineWidth(3);
                        histogram_fitPsi2sMu->SetStats(false);
                        inclusive_histstack->Add(histogram_fitPsi2sMu);
                        histogram_fithbjpsimu->SetLineWidth(3);
                        histogram_fithbjpsimu->SetStats(false);
                        inclusive_histstack->Add(histogram_fithbjpsimu);
                        // histogram_fitbackground->SetLineWidth(3);
                        // histogram_fitbackground->SetStats(false);
                        // inclusive_histstack->Add(histogram_fitbackground);
                        histogram_fitfake->SetLineWidth(3);
                        histogram_fitfake->SetStats(false);
                        inclusive_histstack->Add(histogram_fitfake);
                        inclusive_histstack->Draw("HIST PFC PMC PLC");
                        inclusive_histstack->GetHistogram()->GetYaxis()->ChangeLabel(1, -1, 0);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, it_tobefit->vardimension.c_str()));
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleOffset(1);
                        histogram_tobefit->SetMarkerStyle(kFullCircle);
                        histogram_tobefit->SetMarkerSize(1.5);
                        histogram_tobefit->SetMarkerColor(kBlack);
                        histogram_tobefit->SetLineColor(kBlack);
                        histogram_tobefit->SetFillColor(kBlack);
                        histogram_tobefit->SetStats(false);
                        histogram_tobefit->Draw("SAME ME");
                        // histogram_tobefit->GetYaxis()->ChangeLabel(1, -1, 0);
                        TLegend leg_fitwithratio = it_tobefit->SetLegendPosAuto(it_tobefit->legpos, 6, it_tobefit->legxlength);
                        leg_fitwithratio.SetNColumns(3);
                        leg_fitwithratio.SetTextSize(0.035);
                        /*leg_fitwithratio.SetBorderSize(0);
                        leg_fitwithratio.SetFillColor(0);
                        leg_fitwithratio.SetFillStyle(0);*/

                        // RooFit
                        leg_fitwithratio.AddEntry("Data", "Data");
                        leg_fitwithratio.AddEntry(histogram_fitchic0->GetName(), "B_{c}#rightarrow#chi_{c0}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitchic1->GetName(), "B_{c}#rightarrow#chi_{c1}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitchic2->GetName(), "B_{c}#rightarrow#chi_{c2}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitJPsiHC->GetName(), "B_{c}#rightarrowJ/#PsiH_{c}", "F");
                        leg_fitwithratio.AddEntry(histogram_fitHc->GetName(), "B_{c}#rightarrowh_{c}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitPsi2sTau->GetName(), "B_{c}#rightarrow#Psi(2S)#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitPsi2sMu->GetName(), "B_{c}#rightarrow#Psi(2S)#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fithbjpsimu->GetName(), "J/#Psi+#mu", "F");
                        // leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "F");
                        leg_fitwithratio.AddEntry(histogram_fitfake->GetName(), "Fakes", "F");
                        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "B_{c}#rightarrowJ/#Psi#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "B_{c}#rightarrowJ/#Psi#mu", "F");
                        leg_fitwithratio.DrawClone("SAME");

                        TPaveText pavtext_fitwithratio = it_tobefit->SetPaveTextPosManual(0, leg_fitwithratio.GetY1(), it_tobefit->legpos, 2, it_tobefit->legxlength);
                        pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi) from fit: %.2f #pm %.2f", realvar_rjpsi->getVal(), realvar_rjpsi->getPropagatedError(*results_data)));
                        pavtext_fitwithratio.SetTextSize(0.035);
                        pavtext_fitwithratio.SetTextFont(42);
                        pavtext_fitwithratio.DrawClone("SAME");
                        gPad->SetLogy(0);

                        inclusive_pad_residuals->cd();
                        TH1F *inclusive_distributions_ratio = new TH1F();
                        inclusive_distributions_ratio = (TH1F *)histogram_tobefit->Clone();
                        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
                        inclusive_distributions_ratio->SetStats(false);
                        inclusive_distributions_ratio->Draw("M");
                        // inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
                        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
                        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->varname.data(), it_tobefit->vardimension.c_str()));
                        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.8);
                        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
                        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
                        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.45);
                        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
                        inclusive_pad_residuals->Modified();

                        inclusive_canvas->Print(TString::Format("OutputFiles/PNGPlots/FitResults/FitRatioData_Plot_%s_%dbins_10comp.png", varfit_name, int(it_tobefit->bins)).Data());
                        inclusive_canvas->Clear();
                }

                /*.o    .o         .oooooo.     .oooooo.   ooo        ooooo ooooooooo.
                o888  o888        d8P'  `Y8b   d8P'  `Y8b  `88.       .888' `888   `Y88.
                888   888       888          888      888  888b     d'888   888   .d88'
                888   888       888          888      888  8 Y88. .P  888   888ooo88P'
                888   888       888          888      888  8  `888'   888   888
                888   888       `88b    ooo  `88b    d88'  8    Y     888   888
                o888o o888o       `Y8bood8P'   `Y8bood8P'  o8o        o888o o888o*/

                if (fittype.find("11comp") != string::npos)
                {
                        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binned_function_normalization, fit_binned_function_signal, fit_binned_function_fake, fit_binned_function_chic0, fit_binned_function_chic1, fit_binned_function_chic2, fit_binned_function_JPsiHC, fit_binned_function_Hc, fit_binned_function_Psi2sTau, fit_binned_function_Psi2sMu, fit_binned_function_hbjpsimu),
                                                        RooArgList(*normalization_amplitude, *signal_amplitude, *fake_amplitude,
                                                                   *chic0_amplitude, *chic1_amplitude, *chic2_amplitude, *JPsiHC_amplitude,
                                                                   *Hc_amplitude, *Psi2sTau_amplitude, *Psi2sMu_amplitude, *hbjpsimu_amplitude));

                        // RooFitResult *results_data = fit_model.fitTo(*binned_tobefit, RooFit::Save(true), RooFit::Minos(true), Constrain(*realvar_rjpsi));

                        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*(fit_model.createNLL(*binned_tobefit, Extended(true), Offset(true), IntegrateBins(100), Constrain(*realvar_rjpsi))));
                        roofit_minimizer_interface->migrad();
                        roofit_minimizer_interface->minos();
                        roofit_minimizer_interface->hesse();
                        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

                        Double_t signamp = signal_amplitude->getVal(),
                                 normamp = normalization_amplitude->getVal(),
                                 rjpsiamp = signamp / normamp,
                                 signamperr = signal_amplitude->getError(),
                                 normamperr = normalization_amplitude->getError(),
                                 rjpsiamperr = rjpsiamp * (signamperr / signamp + normamperr / normamp),
                                 chic0amp = chic0_amplitude->getVal(),
                                 chic1amp = chic1_amplitude->getVal(),
                                 chic2amp = chic2_amplitude->getVal(),
                                 JPsiHCamp = JPsiHC_amplitude->getVal(),
                                 Hcamp = Hc_amplitude->getVal(),
                                 Psi2sTauamp = Psi2sTau_amplitude->getVal(),
                                 Psi2sMuamp = Psi2sMu_amplitude->getVal(),
                                 hbjpsimuamp = hbjpsimu_amplitude->getVal(),
                                 bkgamp = bkg_amplitude->getVal(),
                                 fakeamp = fake_amplitude->getVal(),
                                 totamp = signamp + normamp + bkgamp + fakeamp + chic0amp + chic1amp + chic2amp + JPsiHCamp + Hcamp + Psi2sTauamp + Psi2sMuamp + hbjpsimuamp,
                                 rjpsi_plain = realvar_rjpsi->getVal(),
                                 rjpsi_inverted = 1 / realvar_rjpsi->getVal(),
                                 signaloneamp = (rjpsi_plain) * (1 / (rjpsi_plain + 1)) * (normamp),
                                 normaloneamp = (1 / (rjpsi_plain + 1)) * (normamp);
                        cout << "Signamp: " << signamp << " +- " << signamperr << " Normamp: " << normamp << " +- " << normamperr << endl;
                        cout << "R(J/Psi) from amplitudes: " << signamp / normamp << " +- " << signamp / normamp * (signamperr / signamp + normamperr / normamp) << endl;
                        cout << "Signaloneamp: " << signaloneamp << " Normaloneamp: " << normaloneamp << endl;

                        Double_t tau_eff = histogram_jpsimumu_is_JPsiTau->Integral() / (BtoJpsiMuMu_is_JPsiTau.Count().GetValue()),
                                 mu_eff = histogram_jpsimumu_is_JPsiMu->Integral() / (BtoJpsiMuMu_is_JPsiMu.Count().GetValue());
                        Double_t rjpsi = histogram_jpsimumu_is_JPsiTau->Integral() / histogram_jpsimumu_is_JPsiMu->Integral();
                        Double_t rjpsi_error = (histogram_jpsimumu_is_JPsiTau->GetRMS() / histogram_jpsimumu_is_JPsiTau->Integral() + histogram_jpsimumu_is_JPsiMu->GetRMS() / histogram_jpsimumu_is_JPsiMu->Integral()) * rjpsi;
                        cout << "Signal events pre cut: " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() / hybrid_normalization << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() / hybrid_normalization << endl;
                        cout << "Signal events pre cut (scaled): " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() << " Normalization events pre cut (scaled): " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() << endl;
                        cout << "Signal events preselection cut: " << BtoJpsiMuMu_is_JPsiTau.Filter(preselection_mediumid_true.Data()).Count().GetValue() / hybrid_normalization;
                        cout << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Filter(preselection_mediumid_true.Data()).Count().GetValue() / hybrid_normalization << endl;
                        cout << "Signal events post cut: " << histogram_jpsimumu_is_JPsiTau->Integral() << " Normalization events post cut: " << histogram_jpsimumu_is_JPsiMu->Integral() << endl;
                        cout << "Signal events efficiency: " << tau_eff << " Normalization events efficiency: " << mu_eff << endl;
                        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
                        RooAbsReal *sigregion = fit_binned_function_signal.createIntegral(*varfit, NormSet(*normalization_amplitude));         // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        RooAbsReal *normregion = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude)); // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        Double_t nsigevents = sigregion->getVal() * (signamp);
                        Double_t nnormevents = normregion->getVal() * (normamp);
                        Double_t nsigeventserror = sigregion->getPropagatedError(*results_data) * (nsigevents);
                        Double_t nnormeventserror = normregion->getPropagatedError(*results_data) * (nnormevents);
                        cout << "SignEvents for signorm: " << nsigevents << " NormEvents for signorm: " << nnormevents << endl;
                        cout << "SignEvents error: " << nsigeventserror << " NormEvents error: " << nnormeventserror << endl;

                        // Plotting
                        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"), 1920, 1080);
                        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
                        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

                        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
                        fit_model.fillHistogram(histogram_fitmodel, *varfit, totamp);
                        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
                        fit_binned_function_normalization.fillHistogram(histogram_fitnormalization, *varfit, normamp);
                        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
                        fit_binned_function_signal.fillHistogram(histogram_fitsignal, *varfit, signamp);
                        TH1F *histogram_fitchic0 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic0"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic0"), bins, minbin, maxbin);
                        fit_binned_function_chic0.fillHistogram(histogram_fitchic0, *varfit, chic0amp);
                        TH1F *histogram_fitchic1 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic1"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic1"), bins, minbin, maxbin);
                        fit_binned_function_chic1.fillHistogram(histogram_fitchic1, *varfit, chic1amp);
                        TH1F *histogram_fitchic2 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic2"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic2"), bins, minbin, maxbin);
                        fit_binned_function_chic2.fillHistogram(histogram_fitchic2, *varfit, chic2amp);
                        TH1F *histogram_fitJPsiHC = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_JPsiHC"), TString::Format("%s%s", varfit_name, "fit_binned_function_JPsiHC"), bins, minbin, maxbin);
                        fit_binned_function_JPsiHC.fillHistogram(histogram_fitJPsiHC, *varfit, JPsiHCamp);
                        TH1F *histogram_fitHc = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Hc"), TString::Format("%s%s", varfit_name, "fit_binned_function_Hc"), bins, minbin, maxbin);
                        fit_binned_function_Hc.fillHistogram(histogram_fitHc, *varfit, Hcamp);
                        TH1F *histogram_fitPsi2sTau = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sTau"), TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sTau"), bins, minbin, maxbin);
                        fit_binned_function_Psi2sTau.fillHistogram(histogram_fitPsi2sTau, *varfit, Psi2sTauamp);
                        TH1F *histogram_fitPsi2sMu = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sMu"), TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sMu"), bins, minbin, maxbin);
                        fit_binned_function_Psi2sMu.fillHistogram(histogram_fitPsi2sMu, *varfit, Psi2sMuamp);
                        TH1F *histogram_fithbjpsimu = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_hbjpsimu"), TString::Format("%s%s", varfit_name, "fit_binned_function_hbjpsimu"), bins, minbin, maxbin);
                        fit_binned_function_hbjpsimu.fillHistogram(histogram_fithbjpsimu, *varfit, hbjpsimuamp);
                        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
                        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkgamp);
                        TH1F *histogram_fitfake = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), bins, minbin, maxbin);
                        fit_binned_function_fake.fillHistogram(histogram_fitfake, *varfit, fakeamp);
                        THStack *inclusive_histstack = new THStack(TString::Format("%s%s", varfit_name, "inchiststack"), TString::Format("%s%s", varfit_name, "inchiststack"));

                        // Drawing
                        inclusive_canvas->cd(0);
                        inclusive_canvas->Clear();
                        inclusive_canvas->Draw();
                        inclusive_pad_distribution->SetTopMargin(0.05);
                        inclusive_pad_distribution->SetBottomMargin(0.);
                        inclusive_pad_distribution->SetBorderMode(0);
                        inclusive_pad_distribution->SetLeftMargin(0.1);
                        inclusive_pad_residuals->SetTopMargin(0.);
                        inclusive_pad_residuals->SetBottomMargin(0.2);
                        inclusive_pad_residuals->SetBorderMode(0);
                        inclusive_pad_residuals->SetLeftMargin(0.1);
                        gStyle->SetOptTitle(kFALSE);
                        inclusive_pad_distribution->Draw();
                        inclusive_pad_residuals->Draw();
                        gStyle->SetPalette(kRainBow);
                        inclusive_pad_distribution->cd();
                        histogram_fitmodel->SetMarkerColor(kBlue);
                        histogram_fitmodel->SetLineColor(kBlue);
                        histogram_fitmodel->SetLineWidth(3);
                        histogram_fitmodel->SetStats(false);
                        // inclusive_histstack->Add(histogram_fitmodel);
                        histogram_fitnormalization->SetLineWidth(3);
                        histogram_fitnormalization->SetStats(false);
                        inclusive_histstack->Add(histogram_fitnormalization);
                        histogram_fitsignal->SetLineWidth(3);
                        histogram_fitsignal->SetStats(false);
                        inclusive_histstack->Add(histogram_fitsignal);
                        histogram_fitchic0->SetLineWidth(3);
                        histogram_fitchic0->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic0);
                        histogram_fitchic1->SetLineWidth(3);
                        histogram_fitchic1->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic1);
                        histogram_fitchic2->SetLineWidth(3);
                        histogram_fitchic2->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic2);
                        histogram_fitJPsiHC->SetLineWidth(3);
                        histogram_fitJPsiHC->SetStats(false);
                        inclusive_histstack->Add(histogram_fitJPsiHC);
                        histogram_fitHc->SetLineWidth(3);
                        histogram_fitHc->SetStats(false);
                        inclusive_histstack->Add(histogram_fitHc);
                        histogram_fitPsi2sTau->SetLineWidth(3);
                        histogram_fitPsi2sTau->SetStats(false);
                        inclusive_histstack->Add(histogram_fitPsi2sTau);
                        histogram_fitPsi2sMu->SetLineWidth(3);
                        histogram_fitPsi2sMu->SetStats(false);
                        inclusive_histstack->Add(histogram_fitPsi2sMu);
                        histogram_fithbjpsimu->SetLineWidth(3);
                        histogram_fithbjpsimu->SetStats(false);
                        inclusive_histstack->Add(histogram_fithbjpsimu);
                        // histogram_fitbackground->SetLineWidth(3);
                        // histogram_fitbackground->SetStats(false);
                        // inclusive_histstack->Add(histogram_fitbackground);
                        histogram_fitfake->SetLineWidth(3);
                        histogram_fitfake->SetStats(false);
                        inclusive_histstack->Add(histogram_fitfake);
                        inclusive_histstack->Draw("HIST PFC PMC PLC");
                        inclusive_histstack->GetHistogram()->GetYaxis()->ChangeLabel(1, -1, 0);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, it_tobefit->vardimension.c_str()));
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleOffset(1);
                        histogram_tobefit->SetMarkerStyle(kFullCircle);
                        histogram_tobefit->SetMarkerSize(1.5);
                        histogram_tobefit->SetMarkerColor(kBlack);
                        histogram_tobefit->SetLineColor(kBlack);
                        histogram_tobefit->SetFillColor(kBlack);
                        histogram_tobefit->SetStats(false);
                        histogram_tobefit->Draw("SAME ME");
                        // histogram_tobefit->GetYaxis()->ChangeLabel(1, -1, 0);
                        TLegend leg_fitwithratio = it_tobefit->SetLegendPosAuto(it_tobefit->legpos, 6, it_tobefit->legxlength);
                        leg_fitwithratio.SetNColumns(3);
                        leg_fitwithratio.SetTextSize(0.035);
                        /*leg_fitwithratio.SetBorderSize(0);
                        leg_fitwithratio.SetFillColor(0);
                        leg_fitwithratio.SetFillStyle(0);*/

                        // RooFit
                        leg_fitwithratio.AddEntry("Data", "Data");
                        leg_fitwithratio.AddEntry(histogram_fitchic0->GetName(), "B_{c}#rightarrow#chi_{c0}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitchic1->GetName(), "B_{c}#rightarrow#chi_{c1}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitchic2->GetName(), "B_{c}#rightarrow#chi_{c2}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitJPsiHC->GetName(), "B_{c}#rightarrowJ/#PsiH_{c}", "F");
                        leg_fitwithratio.AddEntry(histogram_fitHc->GetName(), "B_{c}#rightarrowh_{c}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitPsi2sTau->GetName(), "B_{c}#rightarrow#Psi(2S)#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitPsi2sMu->GetName(), "B_{c}#rightarrow#Psi(2S)#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fithbjpsimu->GetName(), "J/#Psi+#mu", "F");
                        // leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "F");
                        leg_fitwithratio.AddEntry(histogram_fitfake->GetName(), "Fakes", "F");
                        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "B_{c}#rightarrowJ/#Psi#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "B_{c}#rightarrowJ/#Psi#mu", "F");
                        leg_fitwithratio.DrawClone("SAME");

                        TPaveText pavtext_fitwithratio = it_tobefit->SetPaveTextPosManual(0, leg_fitwithratio.GetY1(), it_tobefit->legpos, 5, it_tobefit->legxlength);
                        pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                        pavtext_fitwithratio.AddText(TString::Format("B_{c}#rightarrowJ/#Psi#tau events: %.0f#pm%.0f", signamp, signamperr));
                        pavtext_fitwithratio.AddText(TString::Format("B_{c}#rightarrowJ/#Psi#mu events: %.0f#pm%.0f", normamp, normamperr));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi): %.2f #pm %.2f", rjpsiamp, rjpsiamperr));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi) scaled for efficiencies: %.2f #pm %.2f", rjpsiamp * mu_eff / tau_eff, rjpsiamperr * mu_eff / tau_eff + rjpsi * 0.05));
                        pavtext_fitwithratio.SetTextSize(0.035);
                        pavtext_fitwithratio.SetTextFont(42);
                        pavtext_fitwithratio.DrawClone("SAME");
                        gPad->SetLogy(0);

                        inclusive_pad_residuals->cd();
                        TH1F *inclusive_distributions_ratio = new TH1F();
                        inclusive_distributions_ratio = (TH1F *)histogram_tobefit->Clone();
                        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
                        inclusive_distributions_ratio->SetStats(false);
                        inclusive_distributions_ratio->Draw("M");
                        // inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
                        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
                        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->varname.data(), it_tobefit->vardimension.c_str()));
                        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.8);
                        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
                        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
                        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.45);
                        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
                        inclusive_pad_residuals->Modified();

                        inclusive_canvas->Print(TString::Format("OutputFiles/PNGPlots/FitResults/FitRatioData_Plot_%s_%dbins_11comp.png", varfit_name, int(it_tobefit->bins)).Data());
                        inclusive_canvas->Clear();
                        results_data->printMultiline(cout, 20);
                }
        }

        if (closuretest.find("CT") != string::npos)
        {

                // Setting normalization
                hybrid_normalization = 1;     // Normalization of the hybrid sample
                hbtojpsix_normalization = 1;  // Normalization of the hb to jpsix sample
                hbtojpsimu_normalization = 1; // Normalization of the hb to jpsimu sample

                // MC Signal and normalizaton
                TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "JPsiTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_JPsiTau->Sumw2();
                histogram_jpsimumu_is_JPsiTau->Scale(hybrid_normalization);
                TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "JPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_JPsiMu->Sumw2();
                histogram_jpsimumu_is_JPsiMu->Scale(hybrid_normalization);

                // MC Background: Build-up and sum of the components
                TH1F *histogram_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Chic0"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_chic0->Sumw2();
                TH1F *histogram_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Chic1"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_chic1->Sumw2();
                TH1F *histogram_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Chic2"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_chic2->Sumw2();
                TH1F *histogram_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "JPsiHC"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_JPsiHC->Sumw2();
                TH1F *histogram_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "HC"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_Hc->Sumw2();
                TH1F *histogram_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Psi2sTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_Psi2stau->Sumw2();
                TH1F *histogram_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Psi2sMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_Psi2smu->Sumw2();
                TH1F *histogram_hbtojpsix = (TH1F *)HbToJpsiX.Filter(filter_mediumid_true_even.Data()).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) != 13").Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "HbJPsiX"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_hbtojpsix->Sumw2();
                TH1F *histogram_fakes = (TH1F *)HbToJpsiX.Filter(filter_mediumid_true_even.Data()).Filter("abs(k_genpdgId) != 13").Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Fakes"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_fakes->Sumw2();
                TH1F *histogram_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(filter_mediumid_true_even.Data()).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) == 13").Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "HbJPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_jpsimumu_is_chic0->Scale(hybrid_normalization);
                histogram_jpsimumu_is_chic1->Scale(hybrid_normalization);
                histogram_jpsimumu_is_chic2->Scale(hybrid_normalization);
                histogram_jpsimumu_is_JPsiHC->Scale(hybrid_normalization);
                histogram_jpsimumu_is_Hc->Scale(hybrid_normalization);
                histogram_jpsimumu_is_Psi2stau->Scale(hybrid_normalization);
                histogram_jpsimumu_is_Psi2smu->Scale(hybrid_normalization);
                histogram_hbtojpsix->Scale(hbtojpsix_normalization);
                histogram_hbtojpsimu->Scale(hbtojpsimu_normalization);
                THStack *histstack_bkg_distribution = new THStack(TString::Format("%s%s", varfit_name, "HistStack"), TString::Format(varfit_name, "HistStack"));
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic0);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic1);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic2);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_JPsiHC);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_Hc);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_Psi2stau);
                histstack_bkg_distribution->Add(histogram_jpsimumu_is_Psi2smu);
                histstack_bkg_distribution->Add(histogram_hbtojpsix);
                histstack_bkg_distribution->Add(histogram_hbtojpsimu);
                TH1F *histogram_jpsimu_is_bkg = (TH1F *)histstack_bkg_distribution->GetStack()->Last();
                THStack *histstack_mc_distribution = new THStack(TString::Format("%s%s", varfit_name, "MCStack"), TString::Format(varfit_name, "MCStack"));
                histstack_mc_distribution->Add(histogram_jpsimumu_is_JPsiTau);
                histstack_mc_distribution->Add(histogram_jpsimumu_is_JPsiMu);
                histstack_mc_distribution->Add(histogram_jpsimu_is_bkg);
                TH1F *histogram_mc = (TH1F *)histstack_mc_distribution->GetStack()->Last();

                // MC Signal and normalizaton
                TH1F *histogram_asdata_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "JPsiTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_JPsiTau->Sumw2();
                histogram_asdata_jpsimumu_is_JPsiTau->Scale(hybrid_normalization);
                TH1F *histogram_asdata_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "JPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_JPsiMu->Sumw2();
                histogram_asdata_jpsimumu_is_JPsiMu->Scale(hybrid_normalization);

                // MC Background: Build-up and sum of the components
                TH1F *histogram_asdata_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "Chic0"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_chic0->Sumw2();
                TH1F *histogram_asdata_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "Chic1"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_chic1->Sumw2();
                TH1F *histogram_asdata_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "Chic2"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_chic2->Sumw2();
                TH1F *histogram_asdata_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "JPsiHC"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_JPsiHC->Sumw2();
                TH1F *histogram_asdata_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "HC"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_Hc->Sumw2();
                TH1F *histogram_asdata_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "Psi2sTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_Psi2stau->Sumw2();
                TH1F *histogram_asdata_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "Psi2sMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_Psi2smu->Sumw2();
                TH1F *histogram_asdata_hbtojpsix = (TH1F *)HbToJpsiX.Filter(filter_mediumid_true_odd.Data()).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) != 13").Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "HbJPsiX"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_hbtojpsix->Sumw2();
                TH1F *histogram_asdata_fakes = (TH1F *)HbToJpsiX.Filter(filter_mediumid_true_odd.Data()).Filter("abs(k_genpdgId) != 13").Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Fakes"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_fakes->Sumw2();
                TH1F *histogram_asdata_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(filter_mediumid_true_odd.Data()).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) == 13").Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "HbJPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
                histogram_asdata_jpsimumu_is_chic0->Scale(hybrid_normalization);
                histogram_asdata_jpsimumu_is_chic1->Scale(hybrid_normalization);
                histogram_asdata_jpsimumu_is_chic2->Scale(hybrid_normalization);
                histogram_asdata_jpsimumu_is_JPsiHC->Scale(hybrid_normalization);
                histogram_asdata_jpsimumu_is_Hc->Scale(hybrid_normalization);
                histogram_asdata_jpsimumu_is_Psi2stau->Scale(hybrid_normalization);
                histogram_asdata_jpsimumu_is_Psi2smu->Scale(hybrid_normalization);
                histogram_asdata_hbtojpsix->Scale(hbtojpsix_normalization);
                histogram_asdata_fakes->Scale(hbtojpsix_normalization);
                histogram_asdata_hbtojpsimu->Scale(hbtojpsimu_normalization);
                THStack *histstack_asdata_bkg_distribution = new THStack(TString::Format("AsData%s%s", varfit_name, "HistStack"), TString::Format(varfit_name, "HistStack"));
                histstack_asdata_bkg_distribution->Add(histogram_asdata_jpsimumu_is_chic0);
                histstack_asdata_bkg_distribution->Add(histogram_asdata_jpsimumu_is_chic1);
                histstack_asdata_bkg_distribution->Add(histogram_asdata_jpsimumu_is_chic2);
                histstack_asdata_bkg_distribution->Add(histogram_asdata_jpsimumu_is_JPsiHC);
                histstack_asdata_bkg_distribution->Add(histogram_asdata_jpsimumu_is_Hc);
                histstack_asdata_bkg_distribution->Add(histogram_asdata_jpsimumu_is_Psi2stau);
                histstack_asdata_bkg_distribution->Add(histogram_asdata_jpsimumu_is_Psi2smu);
                histstack_asdata_bkg_distribution->Add(histogram_asdata_hbtojpsix);
                histstack_asdata_bkg_distribution->Add(histogram_asdata_hbtojpsimu);
                TH1F *histogram_asdata_jpsimu_is_bkg = (TH1F *)histstack_bkg_distribution->GetStack()->Last();
                THStack *histstack_asdata_mc_distribution = new THStack(TString::Format("AsData%s%s", varfit_name, "MCStack"), TString::Format(varfit_name, "MCStack"));
                histstack_asdata_mc_distribution->Add(histogram_asdata_jpsimumu_is_JPsiTau);
                histstack_asdata_mc_distribution->Add(histogram_asdata_jpsimumu_is_JPsiMu);
                histstack_asdata_mc_distribution->Add(histogram_asdata_jpsimu_is_bkg);
                histstack_asdata_mc_distribution->Add(histogram_asdata_fakes);
                TH1F *histogram_asdata_mc = (TH1F *)histstack_asdata_mc_distribution->GetStack()->Last();
                histogram_asdata_mc->Sumw2();

                // Building histograms for roofit
                RooDataHist *binned_sig_mc = new RooDataHist("binned_sig_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_JPsiTau);
                RooDataHist *binned_norm_mc = new RooDataHist("binned_norm_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_JPsiMu);
                RooDataHist *binned_bkg_chic0 = new RooDataHist("binned_bkg_chic0", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_chic0);
                RooDataHist *binned_bkg_chic1 = new RooDataHist("binned_bkg_chic1", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_chic1);
                RooDataHist *binned_bkg_chic2 = new RooDataHist("binned_bkg_chic2", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_chic2);
                RooDataHist *binned_bkg_JPsiHC = new RooDataHist("binned_bkg_JPsiHC", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_JPsiHC);
                RooDataHist *binned_bkg_Hc = new RooDataHist("binned_bkg_Hc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_Hc);
                RooDataHist *binned_bkg_Psi2sTau = new RooDataHist("binned_bkg_Psi2sTau", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_Psi2stau);
                RooDataHist *binned_bkg_Psi2sMu = new RooDataHist("binned_bkg_Psi2sMu", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_Psi2smu);
                RooDataHist *binned_bkg_hbjpsimu = new RooDataHist("binned_bkg_hbjpsimu", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_hbtojpsimu);
                RooDataHist *binned_bkg_mc = new RooDataHist("binned_bkg_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimu_is_bkg);
                RooDataHist *binned_fake_mc = new RooDataHist("binned_fake_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_fakes);
                RooDataHist *binned_mc = new RooDataHist("binned_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_mc);
                RooDataHist *binned_mc_asdata = new RooDataHist("binned_mc_asdata", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_asdata_mc);

                /*oooooooooo ooooo ooooooooooooo        .oooooo.   ooooo   ooooo   .oooooo.   ooooo   .oooooo.   oooooooooooo  .oooooo..o
                `888'     `8 `888' 8'   888   `8       d8P'  `Y8b  `888'   `888'  d8P'  `Y8b  `888'  d8P'  `Y8b  `888'     `8 d8P'    `Y8
                888          888       888           888           888     888  888      888  888  888           888         Y88bo.
                888oooo8     888       888           888           888ooooo888  888      888  888  888           888oooo8     `"Y8888o.
                888    "     888       888           888           888     888  888      888  888  888           888    "         `"Y88b
                888          888       888           `88b    ooo   888     888  `88b    d88'  888  `88b    ooo   888       o oo     .d8P
                o888o        o888o     o888o           `Y8bood8P'  o888o   o888o  `Y8bood8P'  o888o  `Y8bood8P'  o888ooooood8 8""88888P' */
                string fittype = "4comp";
                RooDataHist *binned_tobefit = binned_mc_asdata;
                TH1F *histogram_tobefit = (TH1F *)histogram_asdata_mc->Clone();

                // RooFit Variables taking part in the fit
                RooRealVar *realvar_rjpsi = new RooRealVar(TString::Format("RJPsi_of_%s", varfit_name), TString::Format("RJPsi_of_%s", varfit_name),
                                                           RJPsi, 0.01, 1);
                RooRealVar *normalization_amplitude = new RooRealVar(TString::Format("Normalizationch_normalization_of_%s", varfit_name), TString::Format("Signal normalization of %s", varfit_name),
                                                                     7800, 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *signal_amplitude = new RooRealVar(TString::Format("Signal_normalization_of_%s", varfit_name), TString::Format("Signal normalization of %s", varfit_name),
                                                              770, 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooProduct *signal_amplitude_fixed = new RooProduct("signal_amplitude_fixed", "signal_amplitude_fixed", RooArgList(*realvar_rjpsi, *normalization_amplitude));
                RooRealVar *chic0_amplitude = new RooRealVar(TString::Format("chic0_normalization_of_%s", varfit_name), TString::Format("chic0 normalization of %s", varfit_name),
                                                             histogram_jpsimumu_is_chic0->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *chic1_amplitude = new RooRealVar(TString::Format("chic1_normalization_of_%s", varfit_name), TString::Format("chic1 normalization of %s", varfit_name),
                                                             histogram_jpsimumu_is_chic1->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *chic2_amplitude = new RooRealVar(TString::Format("chic2_normalization_of_%s", varfit_name), TString::Format("chic2 normalization of %s", varfit_name),
                                                             histogram_jpsimumu_is_chic2->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *JPsiHC_amplitude = new RooRealVar(TString::Format("JPsiHC_normalization_of_%s", varfit_name), TString::Format("JPsiHC normalization of %s", varfit_name),
                                                              histogram_jpsimumu_is_JPsiHC->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *Hc_amplitude = new RooRealVar(TString::Format("Hc_normalization_of_%s", varfit_name), TString::Format("Hc normalization of %s", varfit_name),
                                                          histogram_jpsimumu_is_Hc->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *Psi2sTau_amplitude = new RooRealVar(TString::Format("Psi2sTau_normalization_of_%s", varfit_name), TString::Format("Psi2sTau normalization of %s", varfit_name),
                                                                histogram_jpsimumu_is_Psi2stau->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *Psi2sMu_amplitude = new RooRealVar(TString::Format("Psi2sMu_normalization_of_%s", varfit_name), TString::Format("Psi2sMu normalization of %s", varfit_name),
                                                               histogram_jpsimumu_is_Psi2smu->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *hbjpsimu_amplitude = new RooRealVar(TString::Format("hbjpsimu_normalization_of_%s", varfit_name), TString::Format("hbjpsimu normalization of %s", varfit_name),
                                                                histogram_hbtojpsimu->Integral(), 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *bkg_amplitude = new RooRealVar(TString::Format("Bkg_normalization_of_%s", varfit_name), TString::Format("Bkg normalization of %s", varfit_name),
                                                           1, 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *fake_amplitude = new RooRealVar(TString::Format("Fake_normalization_of_%s", varfit_name), TString::Format("Fake normalization of %s", varfit_name),
                                                            1, 0, histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                RooRealVar *total_amplitude = new RooRealVar(TString::Format("Total_normalization_of_%s", varfit_name), TString::Format("Total normalization of %s", varfit_name),
                                                             histogram_tobefit->Integral(), 0, 2 * histogram_tobefit->Integral(), it_tobefit->vardimension.c_str());
                // Extracting pdfs from binned samples
                RooHistPdf *binned_pdf_signal = new RooHistPdf(TString::Format("%s_hist_signal_model", varfit_name), TString::Format("%s_hist_signal_model", varfit_name), RooArgList(*varfit), *binned_sig_mc, interpolation_order);
                RooHistPdf *binned_pdf_normalization = new RooHistPdf(TString::Format("%s_hist_normalization_model", varfit_name), TString::Format("%s_hist_normalization_model", varfit_name), RooArgList(*varfit), *binned_norm_mc, interpolation_order);
                RooHistPdf *binned_pdf_chic0 = new RooHistPdf(TString::Format("%s_hist_chic0_model", varfit_name), TString::Format("%s_hist_chic0_model", varfit_name), RooArgList(*varfit), *binned_bkg_chic0, interpolation_order);
                RooHistPdf *binned_pdf_chic1 = new RooHistPdf(TString::Format("%s_hist_chic1_model", varfit_name), TString::Format("%s_hist_chic1_model", varfit_name), RooArgList(*varfit), *binned_bkg_chic1, interpolation_order);
                RooHistPdf *binned_pdf_chic2 = new RooHistPdf(TString::Format("%s_hist_chic2_model", varfit_name), TString::Format("%s_hist_chic2_model", varfit_name), RooArgList(*varfit), *binned_bkg_chic2, interpolation_order);
                RooHistPdf *binned_pdf_JPsiHC = new RooHistPdf(TString::Format("%s_hist_JPsiHC_model", varfit_name), TString::Format("%s_hist_JPsiHC_model", varfit_name), RooArgList(*varfit), *binned_bkg_JPsiHC, interpolation_order);
                RooHistPdf *binned_pdf_Hc = new RooHistPdf(TString::Format("%s_hist_Hc_model", varfit_name), TString::Format("%s_hist_Hc_model", varfit_name), RooArgList(*varfit), *binned_bkg_Hc, interpolation_order);
                RooHistPdf *binned_pdf_Psi2sTau = new RooHistPdf(TString::Format("%s_hist_Psi2sTau_model", varfit_name), TString::Format("%s_hist_Psi2sTau_model", varfit_name), RooArgList(*varfit), *binned_bkg_Psi2sTau, interpolation_order);
                RooHistPdf *binned_pdf_Psi2sMu = new RooHistPdf(TString::Format("%s_hist_Psi2sMu_model", varfit_name), TString::Format("%s_hist_Psi2sMu_model", varfit_name), RooArgList(*varfit), *binned_bkg_Psi2sMu, interpolation_order);
                RooHistPdf *binned_pdf_hbjpsimu = new RooHistPdf(TString::Format("%s_hist_hbjpsimu_model", varfit_name), TString::Format("%s_hist_hbjpsimu_model", varfit_name), RooArgList(*varfit), *binned_bkg_hbjpsimu, interpolation_order);
                RooHistPdf *binned_pdf_background = new RooHistPdf(TString::Format("%s_hist_background_model", varfit_name), TString::Format("%s_hist_background_model", varfit_name), RooArgList(*varfit), *binned_bkg_mc, interpolation_order);
                RooHistPdf *binned_pdf_fake = new RooHistPdf(TString::Format("%s_hist_fake_model", varfit_name), TString::Format("%s_hist_fake_model", varfit_name), RooArgList(*varfit), *binned_fake_mc, interpolation_order);

                // Defining the fit model
                RooExtendPdf fit_binned_function_normalization = RooExtendPdf("fit_binned_function_normalization", "fit_binned_function_normalization",
                                                                              *binned_pdf_normalization, *varfit);
                RooExtendPdf fit_binned_function_signal = RooExtendPdf("fit_binned_function_signal", "fit_binned_function_signal",
                                                                       *binned_pdf_signal, *varfit);
                RooExtendPdf fit_binned_function_chic0 = RooExtendPdf("fit_binned_function_chic0", "fit_binned_function_chic0",
                                                                      *binned_pdf_chic0, *varfit);
                RooExtendPdf fit_binned_function_chic1 = RooExtendPdf("fit_binned_function_chic1", "fit_binned_function_chic1",
                                                                      *binned_pdf_chic1, *varfit);
                RooExtendPdf fit_binned_function_chic2 = RooExtendPdf("fit_binned_function_chic2", "fit_binned_function_chic2",
                                                                      *binned_pdf_chic2, *varfit);
                RooExtendPdf fit_binned_function_JPsiHC = RooExtendPdf("fit_binned_function_JPsiHC", "fit_binned_function_JPsiHC",
                                                                       *binned_pdf_JPsiHC, *varfit);
                RooExtendPdf fit_binned_function_Hc = RooExtendPdf("fit_binned_function_Hc", "fit_binned_function_Hc",
                                                                   *binned_pdf_Hc, *varfit);
                RooExtendPdf fit_binned_function_Psi2sTau = RooExtendPdf("fit_binned_function_Psi2sTau", "fit_binned_function_Psi2sTau",
                                                                         *binned_pdf_Psi2sTau, *varfit);
                RooExtendPdf fit_binned_function_Psi2sMu = RooExtendPdf("fit_binned_function_Psi2sMu", "fit_binned_function_Psi2sMu",
                                                                        *binned_pdf_Psi2sMu, *varfit);
                RooExtendPdf fit_binned_function_hbjpsimu = RooExtendPdf("fit_binned_function_hbjpsimu", "fit_binned_function_hbjpsimu",
                                                                         *binned_pdf_hbjpsimu, *varfit);
                RooExtendPdf fit_binned_function_background = RooExtendPdf("fit_binned_function_background", "fit_binned_function_background",
                                                                           *binned_pdf_background, *varfit);
                RooExtendPdf fit_binned_function_fake = RooExtendPdf("fit_binned_function_fake", "fit_binned_function_fake",
                                                                     *binned_pdf_fake, *varfit);

                RooFormulaVar binsig_formula = RooFormulaVar("binsig_plus_binnorm", "(@1*@0)", RooArgList(*realvar_rjpsi, fit_binned_function_signal));
                RooWrapperPdf fit_binsig("fit_binsig", "fit_binsig", binsig_formula);
                RooAddPdf fit_binsignorm_rjpsi = RooAddPdf("fit_binsignorm_rjpsi", "fit_binsignorm_rjpsi", RooArgList(fit_binsig, fit_binned_function_normalization), RooArgList(*signal_amplitude, *normalization_amplitude));

                RooFormulaVar binsigplusnorm_formula = RooFormulaVar("binsig_plus_binnorm", "@0*@1+@2", RooArgList(*realvar_rjpsi, fit_binned_function_signal, fit_binned_function_normalization));
                RooWrapperPdf fit_binsigplusnorm("fit_binsigplusnorm", "fit_binsigplusnorm", binsigplusnorm_formula);

                /*.oooo.          .oooooo.     .oooooo.   ooo        ooooo ooooooooo.
                .dP""Y88b        d8P'  `Y8b   d8P'  `Y8b  `88.       .888' `888   `Y88.
                ]8P'      888          888      888  888b     d'888   888   .d88'
                <88b.       888          888      888  8 Y88. .P  888   888ooo88P'
                `88b.      888          888      888  8  `888'   888   888
                o.   .88P       `88b    ooo  `88b    d88'  8    Y     888   888
                `8bd88P'         `Y8bood8P'   `Y8bood8P'  o8o        o888o o888o */

                if (fittype.find("3comp") != string::npos)
                {
                        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binsigplusnorm, fit_binned_function_background, fit_binned_function_fake), RooArgList(*normalization_amplitude, *bkg_amplitude, *fake_amplitude));
                        // RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binsignorm_rjpsi, fit_binned_function_background, fit_binned_function_fake), RooArgList(*total_amplitude, *bkg_amplitude, *fake_amplitude));

                        // RooAbsPdf *fit_model = RooClassFactory::makePdfInstance("RooMyPdf", "(@3*(@0*@1+@2))+(1-@3)*(@4+@5)",  RooArgSet(*realvar_rjpsi, fit_binned_function_signal, fit_binned_function_normalization, *normalization_amplitude, fit_binned_function_background, fit_binned_function_fake));
                        RooFormulaVar fitmodelformula = RooFormulaVar("fitmodelformula", "(@3*(@0*@1+@2))+(1-@3)*(@4+@5)", RooArgList(*realvar_rjpsi, fit_binned_function_signal, fit_binned_function_normalization, *normalization_amplitude, fit_binned_function_background, fit_binned_function_fake));
                        // RooWrapperPdf fit_model("fit_func", "fit_func", fitmodelformula);
                        // RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", fit_func, RooArgList(*total_amplitude));

                        // RooFitResult *results_data = fit_model.fitTo(*binned_tobefit, RooFit::Save(true), RooFit::Minos(true), Constrain(*realvar_rjpsi));

                        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*(fit_model.createNLL(*binned_tobefit, Extended(true), Offset(true) /*, IntegrateBins(0.01)*/)));
                        roofit_minimizer_interface->migrad();
                        roofit_minimizer_interface->minos();
                        roofit_minimizer_interface->hesse();
                        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

                        Double_t signamp = signal_amplitude->getVal(),
                                 normamp = normalization_amplitude->getVal(),
                                 rjpsierr = realvar_rjpsi->getPropagatedError(*results_data),
                                 normamperr = normalization_amplitude->getPropagatedError(*results_data),
                                 bkgamp = bkg_amplitude->getVal(),
                                 fakeamp = fake_amplitude->getVal(),
                                 totamp = normamp + bkgamp + fakeamp,
                                 rjpsi_plain = realvar_rjpsi->getVal(*normalization_amplitude),
                                 rjpsi_inverted = 1 / realvar_rjpsi->getVal(),
                                 signaloneamp = (rjpsi_plain) * (1 / (rjpsi_plain + 1)) * (normamp),
                                 normaloneamp = (1 / (rjpsi_plain + 1)) * (normamp),
                                 signamperr = normamperr * (rjpsi_plain) * (1 / (rjpsi_plain + 1)) + normamp * rjpsierr / ((rjpsi_plain + 1) * (rjpsi_plain + 1));
                        cout << signamperr << endl;
                        cout << "Signamp: " << signamp << " Normamp: " << normamp << endl;
                        cout << "Signaloneamp: " << signaloneamp << " Normaloneamp: " << normaloneamp << endl;
                        cout << "R(J/Psi) from amplitudes: " << signaloneamp / normaloneamp << " +- " << signamperr / normaloneamp + signaloneamp * normamperr / (normaloneamp * normaloneamp) << endl;

                        Double_t tau_eff = histogram_jpsimumu_is_JPsiTau->Integral() / (BtoJpsiMuMu_is_JPsiTau.Count().GetValue()),
                                 mu_eff = histogram_jpsimumu_is_JPsiMu->Integral() / (BtoJpsiMuMu_is_JPsiMu.Count().GetValue());
                        Double_t rjpsi = histogram_jpsimumu_is_JPsiTau->Integral() / histogram_jpsimumu_is_JPsiMu->Integral();
                        Double_t rjpsi_error = (histogram_jpsimumu_is_JPsiTau->GetRMS() / histogram_jpsimumu_is_JPsiTau->Integral() + histogram_jpsimumu_is_JPsiMu->GetRMS() / histogram_jpsimumu_is_JPsiMu->Integral()) * rjpsi;
                        cout << "Signal events pre cut: " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() << endl;
                        cout << "Signal events post cut: " << histogram_jpsimumu_is_JPsiTau->GetEntries() << " Normalization events post cut: " << histogram_jpsimumu_is_JPsiMu->GetEntries() << endl;
                        cout << "Signal events efficiency: " << tau_eff << " Normalization events efficiency: " << mu_eff << endl;
                        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
                        RooAbsReal *sigregion = fit_binned_function_signal.createIntegral(*varfit, NormSet(*normalization_amplitude));         // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        RooAbsReal *normregion = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude)); // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        Double_t nsigevents = sigregion->getVal() * (signamp);
                        Double_t nnormevents = normregion->getVal() * (normamp);
                        Double_t nsigeventserror = sigregion->getPropagatedError(*results_data) * (nsigevents);
                        Double_t nnormeventserror = normregion->getPropagatedError(*results_data) * (nnormevents);
                        cout << "SignEvents for signorm: " << nsigevents << " NormEvents for signorm: " << nnormevents << endl;
                        cout << "SignEvents error: " << nsigeventserror << " NormEvents error: " << nnormeventserror << endl;
                        cout << "rjpsi_plain: " << rjpsi_plain << endl;

                        // Plotting
                        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"), 1920, 1080);
                        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
                        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

                        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
                        fit_model.fillHistogram(histogram_fitmodel, *varfit, totamp);
                        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
                        fit_binned_function_normalization.fillHistogram(histogram_fitnormalization, *varfit, normaloneamp);
                        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
                        fit_binned_function_signal.fillHistogram(histogram_fitsignal, *varfit, signaloneamp);
                        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
                        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkgamp);
                        TH1F *histogram_fitfake = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), bins, minbin, maxbin);
                        fit_binned_function_fake.fillHistogram(histogram_fitfake, *varfit, fakeamp);
                        THStack *inclusive_histstack = new THStack(TString::Format("%s%s", varfit_name, "inchiststack"), TString::Format("%s%s", varfit_name, "inchiststack"));

                        // Drawing
                        inclusive_canvas->cd(0);
                        inclusive_canvas->Clear();
                        inclusive_canvas->Draw();
                        inclusive_pad_distribution->SetTopMargin(0.05);
                        inclusive_pad_distribution->SetBottomMargin(0.);
                        inclusive_pad_distribution->SetBorderMode(0);
                        inclusive_pad_distribution->SetLeftMargin(0.1);
                        inclusive_pad_residuals->SetTopMargin(0.);
                        inclusive_pad_residuals->SetBottomMargin(0.2);
                        inclusive_pad_residuals->SetBorderMode(0);
                        inclusive_pad_residuals->SetLeftMargin(0.1);
                        gStyle->SetOptTitle(kFALSE);
                        inclusive_pad_distribution->Draw();
                        inclusive_pad_residuals->Draw();
                        gStyle->SetPalette(kRainBow);
                        inclusive_pad_distribution->cd();
                        // inclusive_histstack->Add(histogram_fitmodel);
                        histogram_fitnormalization->SetLineWidth(3);
                        histogram_fitnormalization->SetStats(false);
                        inclusive_histstack->Add(histogram_fitnormalization);
                        histogram_fitsignal->SetLineWidth(3);
                        histogram_fitsignal->SetStats(false);
                        inclusive_histstack->Add(histogram_fitsignal);
                        histogram_fitbackground->SetLineWidth(3);
                        histogram_fitbackground->SetStats(false);
                        inclusive_histstack->Add(histogram_fitbackground);
                        histogram_fitfake->SetLineWidth(3);
                        histogram_fitfake->SetStats(false);
                        inclusive_histstack->Add(histogram_fitfake);
                        inclusive_histstack->Draw("HIST PFC PMC PLC");
                        inclusive_histstack->GetHistogram()->GetYaxis()->ChangeLabel(1, -1, 0);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, it_tobefit->vardimension.c_str()));
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleOffset(1);
                        histogram_fitmodel->SetMarkerColor(kGray);
                        histogram_fitmodel->SetLineColor(kGray);
                        histogram_fitmodel->SetLineWidth(3);
                        histogram_fitmodel->SetStats(false);
                        // histogram_fitmodel->Draw("SAME");
                        histogram_tobefit->SetMarkerStyle(kFullCircle);
                        histogram_tobefit->SetMarkerSize(1.5);
                        histogram_tobefit->SetMarkerColor(kBlack);
                        histogram_tobefit->SetLineColor(kBlack);
                        histogram_tobefit->SetFillColor(kBlack);
                        histogram_tobefit->SetStats(false);
                        histogram_tobefit->Draw("SAME ME");
                        TLegend leg_fitwithratio = it_tobefit->SetLegendPosAuto(it_tobefit->legpos, 6, it_tobefit->legxlength);
                        leg_fitwithratio.SetTextSize(0.035);
                        /*leg_fitwithratio.SetBorderSize(0);
                        leg_fitwithratio.SetFillColor(0);
                        leg_fitwithratio.SetFillStyle(0);*/

                        // RooFit
                        leg_fitwithratio.AddEntry("Data", "Data");
                        leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "F");
                        leg_fitwithratio.AddEntry(histogram_fitfake->GetName(), "Fakes", "F");
                        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "B_{c}#rightarrowJ/#Psi#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "B_{c}#rightarrowJ/#Psi#mu", "F");
                        leg_fitwithratio.DrawClone("SAME");

                        TPaveText pavtext_fitwithratio = it_tobefit->SetPaveTextPosManual(0, leg_fitwithratio.GetY1(), it_tobefit->legpos, 2, it_tobefit->legxlength);
                        pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi) from fit: %.2f #pm %.2f", realvar_rjpsi->getVal(), realvar_rjpsi->getPropagatedError(*results_data)));
                        pavtext_fitwithratio.SetTextSize(0.035);
                        pavtext_fitwithratio.SetTextFont(42);
                        pavtext_fitwithratio.DrawClone("SAME");
                        gPad->SetLogy(0);

                        inclusive_pad_residuals->cd();
                        TH1F *inclusive_distributions_ratio = new TH1F();
                        inclusive_distributions_ratio = (TH1F *)histogram_tobefit->Clone();
                        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
                        inclusive_distributions_ratio->SetStats(false);
                        inclusive_distributions_ratio->Draw("M");
                        // inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
                        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
                        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->varname.data(), it_tobefit->vardimension.c_str()));
                        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.8);
                        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
                        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
                        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.45);
                        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
                        inclusive_pad_residuals->Modified();

                        inclusive_canvas->Print(TString::Format("OutputFiles/PNGPlots/FitResults/FitRatioClosureTest_Plot_%s_%dbins_3comp.png", varfit_name, int(it_tobefit->bins)).Data());
                        inclusive_canvas->Clear();
                }

                /*    .o          .oooooo.     .oooooo.   ooo        ooooo ooooooooo.
                .d88         d8P'  `Y8b   d8P'  `Y8b  `88.       .888' `888   `Y88.
                .d'888        888          888      888  888b     d'888   888   .d88'
                .d'  888        888          888      888  8 Y88. .P  888   888ooo88P'
                88ooo888oo      888          888      888  8  `888'   888   888
                888        `88b    ooo  `88b    d88'  8    Y     888   888
                o888o        `Y8bood8P'   `Y8bood8P'  o8o        o888o o888o*/

                if (fittype.find("4comp") != string::npos)
                {
                        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binned_function_signal, fit_binned_function_normalization, fit_binned_function_background, fit_binned_function_fake),
                                                        RooArgList(*signal_amplitude, *normalization_amplitude, *bkg_amplitude, *fake_amplitude));

                        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*(fit_model.createNLL(*binned_tobefit, Extended(true), Offset(true), Constrain(*realvar_rjpsi))));
                        roofit_minimizer_interface->migrad();
                        roofit_minimizer_interface->minos();
                        roofit_minimizer_interface->hesse();
                        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

                        Double_t signamp = signal_amplitude->getVal(),
                                 normamp = normalization_amplitude->getVal(),
                                 rjpsiamp = signamp / normamp,
                                 signamperr = signal_amplitude->getError(),
                                 normamperr = normalization_amplitude->getError(),
                                 rjpsiamperr = rjpsiamp * (signamperr / signamp + normamperr / normamp),
                                 bkgamp = bkg_amplitude->getVal(),
                                 fakeamp = fake_amplitude->getVal(),
                                 totamp = signamp + normamp + bkgamp + fakeamp,
                                 rjpsi_plain = realvar_rjpsi->getVal(),
                                 rjpsi_inverted = 1 / realvar_rjpsi->getVal(),
                                 signaloneamp = (rjpsi_plain) * (1 / (rjpsi_plain + 1)) * (normamp),
                                 normaloneamp = (1 / (rjpsi_plain + 1)) * (normamp);
                        cout << "Signamp: " << signamp << " +- " << signamperr << " Normamp: " << normamp << " +- " << normamperr << endl;
                        cout << "R(J/Psi) from amplitudes: " << signamp / normamp << " +- " << signamp / normamp * (signamperr / signamp + normamperr / normamp) << endl;
                        cout << "Signaloneamp: " << signaloneamp << " Normaloneamp: " << normaloneamp << endl;

                        Double_t tau_eff = histogram_jpsimumu_is_JPsiTau->Integral() / (BtoJpsiMuMu_is_JPsiTau.Count().GetValue()),
                                 mu_eff = histogram_jpsimumu_is_JPsiMu->Integral() / (BtoJpsiMuMu_is_JPsiMu.Count().GetValue());
                        Double_t rjpsi = histogram_jpsimumu_is_JPsiTau->Integral() / histogram_jpsimumu_is_JPsiMu->Integral();
                        Double_t rjpsi_error = (histogram_jpsimumu_is_JPsiTau->GetRMS() / histogram_jpsimumu_is_JPsiTau->Integral() + histogram_jpsimumu_is_JPsiMu->GetRMS() / histogram_jpsimumu_is_JPsiMu->Integral()) * rjpsi;
                        cout << "Signal events pre cut: " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() << endl;
                        cout << "Signal events post cut: " << histogram_jpsimumu_is_JPsiTau->Integral() << " Normalization events post cut: " << histogram_jpsimumu_is_JPsiMu->Integral() << endl;
                        cout << "Signal events efficiency: " << tau_eff << " Normalization events efficiency: " << mu_eff << endl;
                        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
                        RooAbsReal *sigregion = fit_binned_function_signal.createIntegral(*varfit, NormSet(*normalization_amplitude));         // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        RooAbsReal *normregion = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude)); // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        Double_t nsigevents = sigregion->getVal() * (signamp);
                        Double_t nnormevents = normregion->getVal() * (normamp);
                        Double_t nsigeventserror = sigregion->getPropagatedError(*results_data) * (nsigevents);
                        Double_t nnormeventserror = normregion->getPropagatedError(*results_data) * (nnormevents);
                        cout << "SignEvents for signorm: " << nsigevents << " NormEvents for signorm: " << nnormevents << endl;
                        cout << "SignEvents error: " << nsigeventserror << " NormEvents error: " << nnormeventserror << endl;

                        // Plotting
                        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"), 1920, 1080);
                        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
                        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

                        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
                        fit_model.fillHistogram(histogram_fitmodel, *varfit, totamp);
                        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
                        fit_binned_function_normalization.fillHistogram(histogram_fitnormalization, *varfit, normamp);
                        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
                        fit_binned_function_signal.fillHistogram(histogram_fitsignal, *varfit, signamp);
                        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
                        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkgamp);
                        TH1F *histogram_fitfake = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), bins, minbin, maxbin);
                        fit_binned_function_fake.fillHistogram(histogram_fitfake, *varfit, fakeamp);
                        THStack *inclusive_histstack = new THStack(TString::Format("%s%s", varfit_name, "inchiststack"), TString::Format("%s%s", varfit_name, "inchiststack"));

                        // Drawing
                        inclusive_canvas->cd(0);
                        inclusive_canvas->Clear();
                        inclusive_canvas->Draw();
                        inclusive_pad_distribution->SetTopMargin(0.05);
                        inclusive_pad_distribution->SetBottomMargin(0.);
                        inclusive_pad_distribution->SetBorderMode(0);
                        inclusive_pad_distribution->SetLeftMargin(0.1);
                        inclusive_pad_residuals->SetTopMargin(0.);
                        inclusive_pad_residuals->SetBottomMargin(0.2);
                        inclusive_pad_residuals->SetBorderMode(0);
                        inclusive_pad_residuals->SetLeftMargin(0.1);
                        gStyle->SetOptTitle(kFALSE);
                        inclusive_pad_distribution->Draw();
                        inclusive_pad_residuals->Draw();
                        gStyle->SetPalette(kRainBow);
                        inclusive_pad_distribution->cd();
                        // inclusive_histstack->Add(histogram_fitmodel);
                        histogram_fitnormalization->SetLineWidth(3);
                        histogram_fitnormalization->SetStats(false);
                        inclusive_histstack->Add(histogram_fitnormalization);
                        histogram_fitsignal->SetLineWidth(3);
                        histogram_fitsignal->SetStats(false);
                        inclusive_histstack->Add(histogram_fitsignal);
                        histogram_fitbackground->SetLineWidth(3);
                        histogram_fitbackground->SetStats(false);
                        inclusive_histstack->Add(histogram_fitbackground);
                        histogram_fitfake->SetLineWidth(3);
                        histogram_fitfake->SetStats(false);
                        inclusive_histstack->Add(histogram_fitfake);
                        inclusive_histstack->Draw("HIST PFC PMC PLC");
                        inclusive_histstack->GetHistogram()->GetYaxis()->ChangeLabel(1, -1, 0);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, it_tobefit->vardimension.c_str()));
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleOffset(1);
                        histogram_fitmodel->SetMarkerColor(kGray);
                        histogram_fitmodel->SetLineColor(kGray);
                        histogram_fitmodel->SetLineWidth(3);
                        histogram_fitmodel->SetStats(false);
                        // histogram_fitmodel->Draw("SAME");
                        histogram_tobefit->SetMarkerStyle(kFullCircle);
                        histogram_tobefit->SetMarkerSize(1.5);
                        histogram_tobefit->SetMarkerColor(kBlack);
                        histogram_tobefit->SetLineColor(kBlack);
                        histogram_tobefit->SetFillColor(kBlack);
                        histogram_tobefit->SetStats(false);
                        histogram_tobefit->Draw("SAME ME");
                        TLegend leg_fitwithratio = it_tobefit->SetLegendPosAuto(it_tobefit->legpos, 6, it_tobefit->legxlength);
                        leg_fitwithratio.SetTextSize(0.035);
                        /*leg_fitwithratio.SetBorderSize(0);
                        leg_fitwithratio.SetFillColor(0);
                        leg_fitwithratio.SetFillStyle(0);*/

                        // RooFit
                        leg_fitwithratio.AddEntry("Data", "Data");
                        leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "F");
                        leg_fitwithratio.AddEntry(histogram_fitfake->GetName(), "Fakes", "F");
                        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "B_{c}#rightarrowJ/#Psi#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "B_{c}#rightarrowJ/#Psi#mu", "F");
                        leg_fitwithratio.DrawClone("SAME");

                        TPaveText pavtext_fitwithratio = it_tobefit->SetPaveTextPosManual(0, leg_fitwithratio.GetY1(), it_tobefit->legpos, 4, it_tobefit->legxlength);
                        pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                        pavtext_fitwithratio.AddText(TString::Format("B_{c}#rightarrowJ/#Psi#tau events: %.0f#pm%.0f", signamp, signamperr));
                        pavtext_fitwithratio.AddText(TString::Format("B_{c}#rightarrowJ/#Psi#mu events: %.0f#pm%.0f", normamp, normamperr));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi): %.2f #pm %.2f", rjpsiamp * normal_efficiency / signal_efficiency, rjpsiamperr * normal_efficiency / signal_efficiency + rjpsiamp * 0.05));
                        pavtext_fitwithratio.SetTextSize(0.035);
                        pavtext_fitwithratio.SetTextFont(42);
                        pavtext_fitwithratio.DrawClone("SAME");
                        gPad->SetLogy(0);

                        inclusive_pad_residuals->cd();
                        TH1F *inclusive_distributions_ratio = new TH1F();
                        inclusive_distributions_ratio = (TH1F *)histogram_tobefit->Clone();
                        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
                        inclusive_distributions_ratio->SetStats(false);
                        inclusive_distributions_ratio->Draw("M");
                        // inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
                        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
                        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->varname.data(), it_tobefit->vardimension.c_str()));
                        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.8);
                        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
                        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
                        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.45);
                        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
                        inclusive_pad_residuals->Modified();

                        inclusive_canvas->Print(TString::Format("OutputFiles/PNGPlots/FitResults/FitRatioClosureTest_Plot_%s_%dbins_4comp.png", varfit_name, int(it_tobefit->bins)).Data());
                        inclusive_canvas->Clear();
                        results_data->printMultiline(cout, 20);
                        RooPlot *frame_data = varfit->frame();
                        frame_data->SetTitle("");
                        binned_mc_asdata->plotOn(frame_data, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack));
                        fit_model.plotOn(frame_data, Name("data_signal_fit_function_Norm"), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
                        Double_t chi2_datafit = frame_data->chiSquare();
                        cout << endl
                             << endl
                             << endl
                             << endl
                             << endl
                             << varfit_name << " Variable in " << bins << " bins" << endl;
                        cout << "RJPsi:" << rjpsiamp * normal_efficiency / signal_efficiency << "+-" << rjpsiamperr * normal_efficiency / signal_efficiency + rjpsiamp * 0.05 << endl;
                        cout << "Chi2: " << chi2_datafit << endl
                             << endl
                             << endl
                             << endl
                             << endl;
                }

                /*.o    .oooo.          .oooooo.     .oooooo.   ooo        ooooo ooooooooo.
                o888   d8P'`Y8b        d8P'  `Y8b   d8P'  `Y8b  `88.       .888' `888   `Y88.
                888  888    888      888          888      888  888b     d'888   888   .d88'
                888  888    888      888          888      888  8 Y88. .P  888   888ooo88P'
                888  888    888      888          888      888  8  `888'   888   888
                888  `88b  d88'      `88b    ooo  `88b    d88'  8    Y     888   888
                o888o  `Y8bd8P'        `Y8bood8P'   `Y8bood8P'  o8o        o888o o888o */

                if (fittype.find("10comp") != string::npos)
                {
                        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binsigplusnorm, fit_binned_function_fake, fit_binned_function_chic0, fit_binned_function_chic1, fit_binned_function_chic2, fit_binned_function_JPsiHC, fit_binned_function_Hc, fit_binned_function_Psi2sTau, fit_binned_function_Psi2sMu, fit_binned_function_hbjpsimu),
                                                        RooArgList(*normalization_amplitude, *fake_amplitude,
                                                                   *chic0_amplitude, *chic1_amplitude, *chic2_amplitude, *JPsiHC_amplitude,
                                                                   *Hc_amplitude, *Psi2sTau_amplitude, *Psi2sMu_amplitude, *hbjpsimu_amplitude));

                        // RooFitResult *results_data = fit_model.fitTo(*binned_tobefit, RooFit::Save(true), RooFit::Minos(true), Constrain(*realvar_rjpsi));

                        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*(fit_model.createNLL(*binned_tobefit, Extended(true), Offset(true), Constrain(*realvar_rjpsi))));
                        roofit_minimizer_interface->migrad();
                        roofit_minimizer_interface->minos();
                        roofit_minimizer_interface->hesse();
                        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

                        Double_t signamp = signal_amplitude->getVal(),
                                 normamp = normalization_amplitude->getVal(),
                                 chic0amp = chic0_amplitude->getVal(),
                                 chic1amp = chic1_amplitude->getVal(),
                                 chic2amp = chic2_amplitude->getVal(),
                                 JPsiHCamp = JPsiHC_amplitude->getVal(),
                                 Hcamp = Hc_amplitude->getVal(),
                                 Psi2sTauamp = Psi2sTau_amplitude->getVal(),
                                 Psi2sMuamp = Psi2sMu_amplitude->getVal(),
                                 hbjpsimuamp = hbjpsimu_amplitude->getVal(),
                                 bkgamp = bkg_amplitude->getVal(),
                                 fakeamp = fake_amplitude->getVal(),
                                 totamp = normamp + bkgamp + fakeamp,
                                 rjpsi_plain = realvar_rjpsi->getVal(),
                                 rjpsi_inverted = 1 / realvar_rjpsi->getVal(),
                                 signaloneamp = (rjpsi_plain) * (1 / (rjpsi_plain + 1)) * (normamp),
                                 normaloneamp = (1 / (rjpsi_plain + 1)) * (normamp);
                        cout << "Signamp: " << signamp << " Normamp: " << normamp << endl;
                        cout << "Signaloneamp: " << signaloneamp << " Normaloneamp: " << normaloneamp << endl;

                        Double_t tau_eff = histogram_jpsimumu_is_JPsiTau->Integral() / (BtoJpsiMuMu_is_JPsiTau.Count().GetValue()),
                                 mu_eff = histogram_jpsimumu_is_JPsiMu->Integral() / (BtoJpsiMuMu_is_JPsiMu.Count().GetValue());
                        Double_t rjpsi = histogram_jpsimumu_is_JPsiTau->Integral() / histogram_jpsimumu_is_JPsiMu->Integral();
                        Double_t rjpsi_error = (histogram_jpsimumu_is_JPsiTau->GetRMS() / histogram_jpsimumu_is_JPsiTau->Integral() + histogram_jpsimumu_is_JPsiMu->GetRMS() / histogram_jpsimumu_is_JPsiMu->Integral()) * rjpsi;
                        cout << "Signal events pre cut: " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() << endl;
                        cout << "Signal events post cut: " << histogram_jpsimumu_is_JPsiTau->Integral() << " Normalization events post cut: " << histogram_jpsimumu_is_JPsiMu->Integral() << endl;
                        cout << "Signal events efficiency: " << tau_eff << " Normalization events efficiency: " << mu_eff << endl;
                        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
                        RooAbsReal *sigregion = fit_binned_function_signal.createIntegral(*varfit, NormSet(*normalization_amplitude));         // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        RooAbsReal *normregion = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude)); // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        Double_t nsigevents = sigregion->getVal() * (signamp);
                        Double_t nnormevents = normregion->getVal() * (normamp);
                        Double_t nsigeventserror = sigregion->getPropagatedError(*results_data) * (nsigevents);
                        Double_t nnormeventserror = normregion->getPropagatedError(*results_data) * (nnormevents);
                        cout << "SignEvents for signorm: " << nsigevents << " NormEvents for signorm: " << nnormevents << endl;
                        cout << "SignEvents error: " << nsigeventserror << " NormEvents error: " << nnormeventserror << endl;

                        // Plotting
                        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"), 1920, 1080);
                        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
                        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

                        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
                        fit_model.fillHistogram(histogram_fitmodel, *varfit, totamp);
                        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
                        fit_binned_function_normalization.fillHistogram(histogram_fitnormalization, *varfit, normaloneamp);
                        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
                        fit_binned_function_signal.fillHistogram(histogram_fitsignal, *varfit, signaloneamp);
                        TH1F *histogram_fitchic0 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic0"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic0"), bins, minbin, maxbin);
                        fit_binned_function_chic0.fillHistogram(histogram_fitchic0, *varfit, chic0amp);
                        TH1F *histogram_fitchic1 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic1"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic1"), bins, minbin, maxbin);
                        fit_binned_function_chic1.fillHistogram(histogram_fitchic1, *varfit, chic1amp);
                        TH1F *histogram_fitchic2 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic2"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic2"), bins, minbin, maxbin);
                        fit_binned_function_chic2.fillHistogram(histogram_fitchic2, *varfit, chic2amp);
                        TH1F *histogram_fitJPsiHC = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_JPsiHC"), TString::Format("%s%s", varfit_name, "fit_binned_function_JPsiHC"), bins, minbin, maxbin);
                        fit_binned_function_JPsiHC.fillHistogram(histogram_fitJPsiHC, *varfit, JPsiHCamp);
                        TH1F *histogram_fitHc = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Hc"), TString::Format("%s%s", varfit_name, "fit_binned_function_Hc"), bins, minbin, maxbin);
                        fit_binned_function_Hc.fillHistogram(histogram_fitHc, *varfit, Hcamp);
                        TH1F *histogram_fitPsi2sTau = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sTau"), TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sTau"), bins, minbin, maxbin);
                        fit_binned_function_Psi2sTau.fillHistogram(histogram_fitPsi2sTau, *varfit, Psi2sTauamp);
                        TH1F *histogram_fitPsi2sMu = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sMu"), TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sMu"), bins, minbin, maxbin);
                        fit_binned_function_Psi2sMu.fillHistogram(histogram_fitPsi2sMu, *varfit, Psi2sMuamp);
                        TH1F *histogram_fithbjpsimu = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_hbjpsimu"), TString::Format("%s%s", varfit_name, "fit_binned_function_hbjpsimu"), bins, minbin, maxbin);
                        fit_binned_function_hbjpsimu.fillHistogram(histogram_fithbjpsimu, *varfit, hbjpsimuamp);
                        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
                        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkgamp);
                        TH1F *histogram_fitfake = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), bins, minbin, maxbin);
                        fit_binned_function_fake.fillHistogram(histogram_fitfake, *varfit, fakeamp);
                        THStack *inclusive_histstack = new THStack(TString::Format("%s%s", varfit_name, "inchiststack"), TString::Format("%s%s", varfit_name, "inchiststack"));

                        // Drawing
                        inclusive_canvas->cd(0);
                        inclusive_canvas->Clear();
                        inclusive_canvas->Draw();
                        inclusive_pad_distribution->SetTopMargin(0.05);
                        inclusive_pad_distribution->SetBottomMargin(0.);
                        inclusive_pad_distribution->SetBorderMode(0);
                        inclusive_pad_distribution->SetLeftMargin(0.1);
                        inclusive_pad_residuals->SetTopMargin(0.);
                        inclusive_pad_residuals->SetBottomMargin(0.2);
                        inclusive_pad_residuals->SetBorderMode(0);
                        inclusive_pad_residuals->SetLeftMargin(0.1);
                        gStyle->SetOptTitle(kFALSE);
                        inclusive_pad_distribution->Draw();
                        inclusive_pad_residuals->Draw();
                        gStyle->SetPalette(kRainBow);
                        inclusive_pad_distribution->cd();
                        histogram_fitmodel->SetMarkerColor(kBlue);
                        histogram_fitmodel->SetLineColor(kBlue);
                        histogram_fitmodel->SetLineWidth(3);
                        histogram_fitmodel->SetStats(false);
                        // inclusive_histstack->Add(histogram_fitmodel);
                        histogram_fitnormalization->SetLineWidth(3);
                        histogram_fitnormalization->SetStats(false);
                        inclusive_histstack->Add(histogram_fitnormalization);
                        histogram_fitsignal->SetLineWidth(3);
                        histogram_fitsignal->SetStats(false);
                        inclusive_histstack->Add(histogram_fitsignal);
                        histogram_fitchic0->SetLineWidth(3);
                        histogram_fitchic0->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic0);
                        histogram_fitchic1->SetLineWidth(3);
                        histogram_fitchic1->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic1);
                        histogram_fitchic2->SetLineWidth(3);
                        histogram_fitchic2->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic2);
                        histogram_fitJPsiHC->SetLineWidth(3);
                        histogram_fitJPsiHC->SetStats(false);
                        inclusive_histstack->Add(histogram_fitJPsiHC);
                        histogram_fitHc->SetLineWidth(3);
                        histogram_fitHc->SetStats(false);
                        inclusive_histstack->Add(histogram_fitHc);
                        histogram_fitPsi2sTau->SetLineWidth(3);
                        histogram_fitPsi2sTau->SetStats(false);
                        inclusive_histstack->Add(histogram_fitPsi2sTau);
                        histogram_fitPsi2sMu->SetLineWidth(3);
                        histogram_fitPsi2sMu->SetStats(false);
                        inclusive_histstack->Add(histogram_fitPsi2sMu);
                        histogram_fithbjpsimu->SetLineWidth(3);
                        histogram_fithbjpsimu->SetStats(false);
                        inclusive_histstack->Add(histogram_fithbjpsimu);
                        // histogram_fitbackground->SetLineWidth(3);
                        // histogram_fitbackground->SetStats(false);
                        // inclusive_histstack->Add(histogram_fitbackground);
                        histogram_fitfake->SetLineWidth(3);
                        histogram_fitfake->SetStats(false);
                        inclusive_histstack->Add(histogram_fitfake);
                        inclusive_histstack->Draw("HIST PFC PMC PLC");
                        inclusive_histstack->GetHistogram()->GetYaxis()->ChangeLabel(1, -1, 0);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, it_tobefit->vardimension.c_str()));
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleOffset(1);
                        histogram_tobefit->SetMarkerStyle(kFullCircle);
                        histogram_tobefit->SetMarkerSize(1.5);
                        histogram_tobefit->SetMarkerColor(kBlack);
                        histogram_tobefit->SetLineColor(kBlack);
                        histogram_tobefit->SetFillColor(kBlack);
                        histogram_tobefit->SetStats(false);
                        histogram_tobefit->Draw("SAME ME");
                        // histogram_tobefit->GetYaxis()->ChangeLabel(1, -1, 0);
                        TLegend leg_fitwithratio = it_tobefit->SetLegendPosAuto(it_tobefit->legpos, 6, it_tobefit->legxlength);
                        leg_fitwithratio.SetNColumns(3);
                        leg_fitwithratio.SetTextSize(0.035);
                        /*leg_fitwithratio.SetBorderSize(0);
                        leg_fitwithratio.SetFillColor(0);
                        leg_fitwithratio.SetFillStyle(0);*/

                        // RooFit
                        leg_fitwithratio.AddEntry("Data", "Data");
                        leg_fitwithratio.AddEntry(histogram_fitchic0->GetName(), "B_{c}#rightarrow#chi_{c0}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitchic1->GetName(), "B_{c}#rightarrow#chi_{c1}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitchic2->GetName(), "B_{c}#rightarrow#chi_{c2}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitJPsiHC->GetName(), "B_{c}#rightarrowJ/#PsiH_{c}", "F");
                        leg_fitwithratio.AddEntry(histogram_fitHc->GetName(), "B_{c}#rightarrowh_{c}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitPsi2sTau->GetName(), "B_{c}#rightarrow#Psi(2S)#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitPsi2sMu->GetName(), "B_{c}#rightarrow#Psi(2S)#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fithbjpsimu->GetName(), "J/#Psi+#mu", "F");
                        // leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "F");
                        leg_fitwithratio.AddEntry(histogram_fitfake->GetName(), "Fakes", "F");
                        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "B_{c}#rightarrowJ/#Psi#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "B_{c}#rightarrowJ/#Psi#mu", "F");
                        leg_fitwithratio.DrawClone("SAME");

                        TPaveText pavtext_fitwithratio = it_tobefit->SetPaveTextPosManual(0, leg_fitwithratio.GetY1(), it_tobefit->legpos, 2, it_tobefit->legxlength);
                        pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi) from fit: %.2f #pm %.2f", realvar_rjpsi->getVal(), realvar_rjpsi->getPropagatedError(*results_data)));
                        pavtext_fitwithratio.SetTextSize(0.035);
                        pavtext_fitwithratio.SetTextFont(42);
                        pavtext_fitwithratio.DrawClone("SAME");
                        gPad->SetLogy(0);

                        inclusive_pad_residuals->cd();
                        TH1F *inclusive_distributions_ratio = new TH1F();
                        inclusive_distributions_ratio = (TH1F *)histogram_tobefit->Clone();
                        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
                        inclusive_distributions_ratio->SetStats(false);
                        inclusive_distributions_ratio->Draw("M");
                        // inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
                        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
                        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->varname.data(), it_tobefit->vardimension.c_str()));
                        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.8);
                        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
                        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
                        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.45);
                        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
                        inclusive_pad_residuals->Modified();

                        inclusive_canvas->Print(TString::Format("OutputFiles/PNGPlots/FitResults/FitRatioClosureTest_Plot_%s_%dbins_10comp.png", varfit_name, int(it_tobefit->bins)).Data());
                        inclusive_canvas->Clear();
                }

                /*.o    .o         .oooooo.     .oooooo.   ooo        ooooo ooooooooo.
                o888  o888        d8P'  `Y8b   d8P'  `Y8b  `88.       .888' `888   `Y88.
                888   888       888          888      888  888b     d'888   888   .d88'
                888   888       888          888      888  8 Y88. .P  888   888ooo88P'
                888   888       888          888      888  8  `888'   888   888
                888   888       `88b    ooo  `88b    d88'  8    Y     888   888
                o888o o888o       `Y8bood8P'   `Y8bood8P'  o8o        o888o o888o*/

                if (fittype.find("11comp") != string::npos)
                {
                        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model", RooArgList(fit_binned_function_normalization, fit_binned_function_signal, fit_binned_function_fake, fit_binned_function_chic0, fit_binned_function_chic1, fit_binned_function_chic2, fit_binned_function_JPsiHC, fit_binned_function_Hc, fit_binned_function_Psi2sTau, fit_binned_function_Psi2sMu, fit_binned_function_hbjpsimu),
                                                        RooArgList(*normalization_amplitude, *signal_amplitude, *fake_amplitude,
                                                                   *chic0_amplitude, *chic1_amplitude, *chic2_amplitude, *JPsiHC_amplitude,
                                                                   *Hc_amplitude, *Psi2sTau_amplitude, *Psi2sMu_amplitude, *hbjpsimu_amplitude));

                        // RooFitResult *results_data = fit_model.fitTo(*binned_tobefit, RooFit::Save(true), RooFit::Minos(true), Constrain(*realvar_rjpsi));

                        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*(fit_model.createNLL(*binned_tobefit, Extended(true), Offset(true), IntegrateBins(100), Constrain(*realvar_rjpsi))));
                        roofit_minimizer_interface->migrad();
                        roofit_minimizer_interface->minos();
                        roofit_minimizer_interface->hesse();
                        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

                        Double_t signamp = signal_amplitude->getVal(),
                                 normamp = normalization_amplitude->getVal(),
                                 rjpsiamp = signamp / normamp,
                                 signamperr = signal_amplitude->getError(),
                                 normamperr = normalization_amplitude->getError(),
                                 rjpsiamperr = rjpsiamp * (signamperr / signamp + normamperr / normamp),
                                 chic0amp = chic0_amplitude->getVal(),
                                 chic1amp = chic1_amplitude->getVal(),
                                 chic2amp = chic2_amplitude->getVal(),
                                 JPsiHCamp = JPsiHC_amplitude->getVal(),
                                 Hcamp = Hc_amplitude->getVal(),
                                 Psi2sTauamp = Psi2sTau_amplitude->getVal(),
                                 Psi2sMuamp = Psi2sMu_amplitude->getVal(),
                                 hbjpsimuamp = hbjpsimu_amplitude->getVal(),
                                 bkgamp = bkg_amplitude->getVal(),
                                 fakeamp = fake_amplitude->getVal(),
                                 totamp = signamp + normamp + bkgamp + fakeamp + chic0amp + chic1amp + chic2amp + JPsiHCamp + Hcamp + Psi2sTauamp + Psi2sMuamp + hbjpsimuamp,
                                 rjpsi_plain = realvar_rjpsi->getVal(),
                                 rjpsi_inverted = 1 / realvar_rjpsi->getVal(),
                                 signaloneamp = (rjpsi_plain) * (1 / (rjpsi_plain + 1)) * (normamp),
                                 normaloneamp = (1 / (rjpsi_plain + 1)) * (normamp);
                        cout << "Signamp: " << signamp << " +- " << signamperr << " Normamp: " << normamp << " +- " << normamperr << endl;
                        cout << "R(J/Psi) from amplitudes: " << signamp / normamp << " +- " << signamp / normamp * (signamperr / signamp + normamperr / normamp) << endl;
                        cout << "Signaloneamp: " << signaloneamp << " Normaloneamp: " << normaloneamp << endl;

                        Double_t tau_eff = histogram_jpsimumu_is_JPsiTau->Integral() / (BtoJpsiMuMu_is_JPsiTau.Count().GetValue()),
                                 mu_eff = histogram_jpsimumu_is_JPsiMu->Integral() / (BtoJpsiMuMu_is_JPsiMu.Count().GetValue());
                        Double_t rjpsi = histogram_jpsimumu_is_JPsiTau->Integral() / histogram_jpsimumu_is_JPsiMu->Integral();
                        Double_t rjpsi_error = (histogram_jpsimumu_is_JPsiTau->GetRMS() / histogram_jpsimumu_is_JPsiTau->Integral() + histogram_jpsimumu_is_JPsiMu->GetRMS() / histogram_jpsimumu_is_JPsiMu->Integral()) * rjpsi;
                        cout << "Signal events pre cut: " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() / hybrid_normalization << " Normalization events pre cut: " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() / hybrid_normalization << endl;
                        cout << "Signal events pre cut (scaled): " << BtoJpsiMuMu_is_JPsiTau.Count().GetValue() << " Normalization events pre cut (scaled): " << BtoJpsiMuMu_is_JPsiMu.Count().GetValue() << endl;
                        cout << "Signal events post cut: " << histogram_jpsimumu_is_JPsiTau->Integral() << " Normalization events post cut: " << histogram_jpsimumu_is_JPsiMu->Integral() << endl;
                        cout << "Signal events efficiency: " << tau_eff << " Normalization events efficiency: " << mu_eff << endl;
                        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
                        RooAbsReal *sigregion = fit_binned_function_signal.createIntegral(*varfit, NormSet(*normalization_amplitude));         // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        RooAbsReal *normregion = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude)); // The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
                        Double_t nsigevents = sigregion->getVal() * (signamp);
                        Double_t nnormevents = normregion->getVal() * (normamp);
                        Double_t nsigeventserror = sigregion->getPropagatedError(*results_data) * (nsigevents);
                        Double_t nnormeventserror = normregion->getPropagatedError(*results_data) * (nnormevents);
                        cout << "SignEvents for signorm: " << nsigevents << " NormEvents for signorm: " << nnormevents << endl;
                        cout << "SignEvents error: " << nsigeventserror << " NormEvents error: " << nnormeventserror << endl;

                        // Plotting
                        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"), 1920, 1080);
                        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
                        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

                        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
                        fit_model.fillHistogram(histogram_fitmodel, *varfit, totamp);
                        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
                        fit_binned_function_normalization.fillHistogram(histogram_fitnormalization, *varfit, normamp);
                        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
                        fit_binned_function_signal.fillHistogram(histogram_fitsignal, *varfit, signamp);
                        TH1F *histogram_fitchic0 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic0"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic0"), bins, minbin, maxbin);
                        fit_binned_function_chic0.fillHistogram(histogram_fitchic0, *varfit, chic0amp);
                        TH1F *histogram_fitchic1 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic1"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic1"), bins, minbin, maxbin);
                        fit_binned_function_chic1.fillHistogram(histogram_fitchic1, *varfit, chic1amp);
                        TH1F *histogram_fitchic2 = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_chic2"), TString::Format("%s%s", varfit_name, "fit_binned_function_chic2"), bins, minbin, maxbin);
                        fit_binned_function_chic2.fillHistogram(histogram_fitchic2, *varfit, chic2amp);
                        TH1F *histogram_fitJPsiHC = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_JPsiHC"), TString::Format("%s%s", varfit_name, "fit_binned_function_JPsiHC"), bins, minbin, maxbin);
                        fit_binned_function_JPsiHC.fillHistogram(histogram_fitJPsiHC, *varfit, JPsiHCamp);
                        TH1F *histogram_fitHc = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Hc"), TString::Format("%s%s", varfit_name, "fit_binned_function_Hc"), bins, minbin, maxbin);
                        fit_binned_function_Hc.fillHistogram(histogram_fitHc, *varfit, Hcamp);
                        TH1F *histogram_fitPsi2sTau = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sTau"), TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sTau"), bins, minbin, maxbin);
                        fit_binned_function_Psi2sTau.fillHistogram(histogram_fitPsi2sTau, *varfit, Psi2sTauamp);
                        TH1F *histogram_fitPsi2sMu = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sMu"), TString::Format("%s%s", varfit_name, "fit_binned_function_Psi2sMu"), bins, minbin, maxbin);
                        fit_binned_function_Psi2sMu.fillHistogram(histogram_fitPsi2sMu, *varfit, Psi2sMuamp);
                        TH1F *histogram_fithbjpsimu = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_hbjpsimu"), TString::Format("%s%s", varfit_name, "fit_binned_function_hbjpsimu"), bins, minbin, maxbin);
                        fit_binned_function_hbjpsimu.fillHistogram(histogram_fithbjpsimu, *varfit, hbjpsimuamp);
                        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
                        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkgamp);
                        TH1F *histogram_fitfake = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), TString::Format("%s%s", varfit_name, "fit_binned_function_fake"), bins, minbin, maxbin);
                        fit_binned_function_fake.fillHistogram(histogram_fitfake, *varfit, fakeamp);
                        THStack *inclusive_histstack = new THStack(TString::Format("%s%s", varfit_name, "inchiststack"), TString::Format("%s%s", varfit_name, "inchiststack"));

                        // Drawing
                        inclusive_canvas->cd(0);
                        inclusive_canvas->Clear();
                        inclusive_canvas->Draw();
                        inclusive_pad_distribution->SetTopMargin(0.05);
                        inclusive_pad_distribution->SetBottomMargin(0.);
                        inclusive_pad_distribution->SetBorderMode(0);
                        inclusive_pad_distribution->SetLeftMargin(0.1);
                        inclusive_pad_residuals->SetTopMargin(0.);
                        inclusive_pad_residuals->SetBottomMargin(0.2);
                        inclusive_pad_residuals->SetBorderMode(0);
                        inclusive_pad_residuals->SetLeftMargin(0.1);
                        gStyle->SetOptTitle(kFALSE);
                        inclusive_pad_distribution->Draw();
                        inclusive_pad_residuals->Draw();
                        gStyle->SetPalette(kRainBow);
                        inclusive_pad_distribution->cd();
                        histogram_fitmodel->SetMarkerColor(kBlue);
                        histogram_fitmodel->SetLineColor(kBlue);
                        histogram_fitmodel->SetLineWidth(3);
                        histogram_fitmodel->SetStats(false);
                        // inclusive_histstack->Add(histogram_fitmodel);
                        histogram_fitnormalization->SetLineWidth(3);
                        histogram_fitnormalization->SetStats(false);
                        inclusive_histstack->Add(histogram_fitnormalization);
                        histogram_fitsignal->SetLineWidth(3);
                        histogram_fitsignal->SetStats(false);
                        inclusive_histstack->Add(histogram_fitsignal);
                        histogram_fitchic0->SetLineWidth(3);
                        histogram_fitchic0->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic0);
                        histogram_fitchic1->SetLineWidth(3);
                        histogram_fitchic1->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic1);
                        histogram_fitchic2->SetLineWidth(3);
                        histogram_fitchic2->SetStats(false);
                        inclusive_histstack->Add(histogram_fitchic2);
                        histogram_fitJPsiHC->SetLineWidth(3);
                        histogram_fitJPsiHC->SetStats(false);
                        inclusive_histstack->Add(histogram_fitJPsiHC);
                        histogram_fitHc->SetLineWidth(3);
                        histogram_fitHc->SetStats(false);
                        inclusive_histstack->Add(histogram_fitHc);
                        histogram_fitPsi2sTau->SetLineWidth(3);
                        histogram_fitPsi2sTau->SetStats(false);
                        inclusive_histstack->Add(histogram_fitPsi2sTau);
                        histogram_fitPsi2sMu->SetLineWidth(3);
                        histogram_fitPsi2sMu->SetStats(false);
                        inclusive_histstack->Add(histogram_fitPsi2sMu);
                        histogram_fithbjpsimu->SetLineWidth(3);
                        histogram_fithbjpsimu->SetStats(false);
                        inclusive_histstack->Add(histogram_fithbjpsimu);
                        // histogram_fitbackground->SetLineWidth(3);
                        // histogram_fitbackground->SetStats(false);
                        // inclusive_histstack->Add(histogram_fitbackground);
                        histogram_fitfake->SetLineWidth(3);
                        histogram_fitfake->SetStats(false);
                        inclusive_histstack->Add(histogram_fitfake);
                        inclusive_histstack->Draw("HIST PFC PMC PLC");
                        inclusive_histstack->GetHistogram()->GetYaxis()->ChangeLabel(1, -1, 0);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitle(TString::Format("Events/(%.3f %s)", bin_width, it_tobefit->vardimension.c_str()));
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleSize(0.1 * 3 / 7);
                        inclusive_histstack->GetHistogram()->GetYaxis()->SetTitleOffset(1);
                        histogram_tobefit->SetMarkerStyle(kFullCircle);
                        histogram_tobefit->SetMarkerSize(1.5);
                        histogram_tobefit->SetMarkerColor(kBlack);
                        histogram_tobefit->SetLineColor(kBlack);
                        histogram_tobefit->SetFillColor(kBlack);
                        histogram_tobefit->SetStats(false);
                        histogram_tobefit->Draw("SAME ME");
                        // histogram_tobefit->GetYaxis()->ChangeLabel(1, -1, 0);
                        TLegend leg_fitwithratio = it_tobefit->SetLegendPosAuto(it_tobefit->legpos, 6, it_tobefit->legxlength);
                        leg_fitwithratio.SetNColumns(3);
                        leg_fitwithratio.SetTextSize(0.035);
                        /*leg_fitwithratio.SetBorderSize(0);
                        leg_fitwithratio.SetFillColor(0);
                        leg_fitwithratio.SetFillStyle(0);*/

                        // RooFit
                        leg_fitwithratio.AddEntry("Data", "Data");
                        leg_fitwithratio.AddEntry(histogram_fitchic0->GetName(), "B_{c}#rightarrow#chi_{c0}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitchic1->GetName(), "B_{c}#rightarrow#chi_{c1}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitchic2->GetName(), "B_{c}#rightarrow#chi_{c2}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitJPsiHC->GetName(), "B_{c}#rightarrowJ/#PsiH_{c}", "F");
                        leg_fitwithratio.AddEntry(histogram_fitHc->GetName(), "B_{c}#rightarrowh_{c}#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fitPsi2sTau->GetName(), "B_{c}#rightarrow#Psi(2S)#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitPsi2sMu->GetName(), "B_{c}#rightarrow#Psi(2S)#mu", "F");
                        leg_fitwithratio.AddEntry(histogram_fithbjpsimu->GetName(), "J/#Psi+#mu", "F");
                        // leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "F");
                        leg_fitwithratio.AddEntry(histogram_fitfake->GetName(), "Fakes", "F");
                        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "B_{c}#rightarrowJ/#Psi#tau", "F");
                        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "B_{c}#rightarrowJ/#Psi#mu", "F");
                        leg_fitwithratio.DrawClone("SAME");

                        TPaveText pavtext_fitwithratio = it_tobefit->SetPaveTextPosManual(0, leg_fitwithratio.GetY1(), it_tobefit->legpos, 5, it_tobefit->legxlength);
                        pavtext_fitwithratio.AddText(TString::Format("Candidate events: %.0f", histogram_tobefit->Integral()));
                        pavtext_fitwithratio.AddText(TString::Format("B_{c}#rightarrowJ/#Psi#tau events: %.0f#pm%.0f", signamp, signamperr));
                        pavtext_fitwithratio.AddText(TString::Format("B_{c}#rightarrowJ/#Psi#mu events: %.0f#pm%.0f", normamp, normamperr));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi): %.2f #pm %.2f", rjpsiamp, rjpsiamperr));
                        pavtext_fitwithratio.AddText(TString::Format("R(J/#Psi) scaled for efficiencies: %.2f #pm %.2f", rjpsiamp * mu_eff / tau_eff, rjpsiamperr * mu_eff / tau_eff + rjpsi * 0.05));
                        pavtext_fitwithratio.SetTextSize(0.035);
                        pavtext_fitwithratio.SetTextFont(42);
                        pavtext_fitwithratio.DrawClone("SAME");
                        gPad->SetLogy(0);

                        inclusive_pad_residuals->cd();
                        TH1F *inclusive_distributions_ratio = new TH1F();
                        inclusive_distributions_ratio = (TH1F *)histogram_tobefit->Clone();
                        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
                        inclusive_distributions_ratio->SetStats(false);
                        inclusive_distributions_ratio->Draw("M");
                        // inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
                        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
                        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->varname.data(), it_tobefit->vardimension.c_str()));
                        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.8);
                        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
                        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
                        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
                        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.45);
                        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
                        inclusive_pad_residuals->Modified();

                        inclusive_canvas->Print(TString::Format("OutputFiles/PNGPlots/FitResults/FitRatioClosureTest_Plot_%s_%dbins_11comp.png", varfit_name, int(it_tobefit->bins)).Data());
                        inclusive_canvas->Clear();
                        results_data->printMultiline(cout, 20);
                }
        }

        cout << "Signal events pre cut fit range: " << nevents_jtau_prefilter_norange << endl;
        cout << "Normalization events pre cut fit range: " << nevents_jmu_prefilter_norange << endl;
        cout << "Signal events post cut fit range: " << nevents_jtau_postfilter_fitrange << endl;
        cout << "Normalization events post cut fit range: " << nevents_jmu_postfilter_fitrange << endl;

        return 0;
}