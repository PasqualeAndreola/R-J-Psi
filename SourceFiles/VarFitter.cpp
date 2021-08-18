#include "HeaderFiles/VarFitter.h"

using namespace std;
using namespace RooFit;

int VarFitter(vector<VarToBeFitted> *tobefitted,
              bool debug)
{
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

    // Setting normalization
    Double_t hybrid_normalization = 0.62;     // Normalization of the hybrid sample
    Double_t hbtojpsix_normalization = 11.8;  // Normalization of the hb to jpsix sample
    Double_t hbtojpsimu_normalization = 9.05; // Normalization of the hb to jpsimu sample

    for (vector<VarToBeFitted>::iterator it_tobefit = tobefitted->begin();
         it_tobefit < tobefitted->end();
         it_tobefit++)
    {
        // Variable name
        const char *varfit_name = it_tobefit->vartobefitted;

        // Filters
        const char *evenevents = "int(event) % 2 == 0", *oddevents = "int(event) % 2 != 0";
        TString filter_mediumid_true = it_tobefit->cut_mediumid_true;
        TString filter_mediumid_false = it_tobefit->cut_mediumid_false;
        TString filter_mediumid_true_even = TString::Format("%s && %s", filter_mediumid_true.Data(), evenevents);
        TString filter_mediumid_false_even = TString::Format("%s && %s", filter_mediumid_false.Data(), evenevents);
        TString filter_mediumid_true_odd = TString::Format("%s && %s", filter_mediumid_true.Data(), oddevents);
        TString filter_mediumid_false_odd = TString::Format("%s && %s", filter_mediumid_false.Data(), oddevents);

        // Binning
        Double_t bins = it_tobefit->bins, minbin = it_tobefit->min_bin, maxbin = it_tobefit->max_bin, bin_width = (maxbin - minbin) / bins;
        Int_t interpolation_order = 0;

        /*
    oooooooooooo           oooo                  
    `888'     `8           `888                  
    888          .oooo.    888  oooo   .ooooo.  
    888oooo8    `P  )88b   888 .8P'   d88' `88b 
    888    "     .oP"888   888888.    888ooo888 
    888         d8(  888   888 `88b.  888    .o 
    o888o        `Y888""8o o888o o888o `Y8bod8P'
    */
        TH1F *histogram_data_fake = (TH1F *)BtoMu_data.Filter(it_tobefit->cut_mediumid_false).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FData"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_data_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_JPsiTau_fake = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(it_tobefit->cut_mediumid_true).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FJPsiTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_JPsiTau_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_JPsiMu_fake = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(it_tobefit->cut_mediumid_true).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FJPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_JPsiMu_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_chic0_fake = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(it_tobefit->cut_mediumid_false).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FChic0"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_chic0_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_chic1_fake = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(it_tobefit->cut_mediumid_false).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FChic1"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_chic1_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_chic2_fake = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(it_tobefit->cut_mediumid_false).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FChic2"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_chic2_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_JPsiHC_fake = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(it_tobefit->cut_mediumid_false).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FJPsiHC"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_JPsiHC_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_Hc_fake = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(it_tobefit->cut_mediumid_false).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FHC"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_Hc_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_Psi2stau_fake = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(it_tobefit->cut_mediumid_false).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FPsi2sTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_Psi2stau_fake->Sumw2();
        TH1F *histogram_jpsimumu_is_Psi2smu_fake = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(it_tobefit->cut_mediumid_false).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FPsi2sMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_Psi2smu_fake->Sumw2();
        TH1F *histogram_hbtojpsix_fake = (TH1F *)HbToJpsiX.Filter(it_tobefit->cut_mediumid_false).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) != 13").Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FHbJPsiX"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_hbtojpsix_fake->Sumw2();
        TH1F *histogram_hbtojpsimu_fake = (TH1F *)HbToJPsiMu.Filter(it_tobefit->cut_mediumid_false).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) == 13").Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "FHbJPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();

        histogram_jpsimumu_is_JPsiTau_fake->Scale(hybrid_normalization);
        histogram_jpsimumu_is_JPsiMu_fake->Scale(hybrid_normalization);
        histogram_jpsimumu_is_chic0_fake->Scale(hybrid_normalization);
        histogram_jpsimumu_is_chic1_fake->Scale(hybrid_normalization);
        histogram_jpsimumu_is_chic2_fake->Scale(hybrid_normalization);
        histogram_jpsimumu_is_JPsiHC_fake->Scale(hybrid_normalization);
        histogram_jpsimumu_is_Hc_fake->Scale(hybrid_normalization);
        histogram_jpsimumu_is_Psi2stau_fake->Scale(hybrid_normalization);
        histogram_jpsimumu_is_Psi2smu_fake->Scale(hybrid_normalization);
        histogram_hbtojpsix_fake->Scale(hbtojpsix_normalization);
        histogram_hbtojpsimu_fake->Scale(hbtojpsimu_normalization);
        histogram_data_fake->Scale(100);

        THStack *histstack_bkg_distribution_fake = new THStack(TString::Format("%s%s", varfit_name, "FHistStack"), TString::Format(varfit_name, "FHistStack"));
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_JPsiTau_fake);
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_JPsiMu_fake);
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_chic0_fake);
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_chic1_fake);
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_chic2_fake);
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_JPsiHC_fake);
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_Hc_fake);
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_Psi2stau_fake);
        histstack_bkg_distribution_fake->Add(histogram_jpsimumu_is_Psi2smu_fake);
        histstack_bkg_distribution_fake->Add(histogram_hbtojpsix_fake);
        histstack_bkg_distribution_fake->Add(histogram_hbtojpsimu_fake);
        TH1F *histogram_fakes = (TH1F *)(*histogram_data_fake - *(TH1F *)(histstack_bkg_distribution_fake->GetStack()->Last())).Clone();

        // Building variable distribution for various contributions
        // Data
        TH1F *histogram_data = (TH1F *)BtoMu_data.Filter(it_tobefit->cut_mediumid_true).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "Data"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_data->Sumw2();

        // MC Signal and normalizaton
        TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "JPsiTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_JPsiTau->Sumw2();
        TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(filter_mediumid_true_even.Data()).Fill<Float_t>(TH1F(TString::Format("%s%s", varfit_name, "JPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_jpsimumu_is_JPsiMu->Sumw2();

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
        //histstack_mc_distribution->Add(histogram_fakes);
        TH1F *histogram_mc = (TH1F *)histstack_mc_distribution->GetStack()->Last();

        // MC Signal and normalizaton
        TH1F *histogram_asdata_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "JPsiTau"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_asdata_jpsimumu_is_JPsiTau->Sumw2();
        TH1F *histogram_asdata_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(filter_mediumid_true_odd.Data()).Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "JPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_asdata_jpsimumu_is_JPsiMu->Sumw2();

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
        TH1F *histogram_asdata_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(filter_mediumid_true_odd.Data()).Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541").Filter("abs(k_genpdgId) == 13").Fill<Float_t>(TH1F(TString::Format("AsData%s%s", varfit_name, "HbJPsiMu"), "y", bins, minbin, maxbin), {it_tobefit->vartobefitted})->Clone();
        histogram_asdata_jpsimumu_is_chic0->Scale(hybrid_normalization);
        histogram_asdata_jpsimumu_is_chic1->Scale(hybrid_normalization);
        histogram_asdata_jpsimumu_is_chic2->Scale(hybrid_normalization);
        histogram_asdata_jpsimumu_is_JPsiHC->Scale(hybrid_normalization);
        histogram_asdata_jpsimumu_is_Hc->Scale(hybrid_normalization);
        histogram_asdata_jpsimumu_is_Psi2stau->Scale(hybrid_normalization);
        histogram_asdata_jpsimumu_is_Psi2smu->Scale(hybrid_normalization);
        histogram_asdata_hbtojpsix->Scale(hbtojpsix_normalization);
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
        //histstack_mc_distribution->Add(histogram_asdata_fakes);
        TH1F *histogram_asdata_mc = (TH1F *)histstack_asdata_mc_distribution->GetStack()->Last();
        histogram_asdata_mc->Sumw2();

        /*
        ooooooooo.                       oooooooooooo  o8o      .   
        `888   `Y88.                     `888'     `8  `"'    .o8   
        888   .d88'  .ooooo.   .ooooo.   888         oooo  .o888oo 
        888ooo88P'  d88' `88b d88' `88b  888oooo8    `888    888   
        888`88b.    888   888 888   888  888    "     888    888   
        888  `88b.  888   888 888   888  888          888    888 . 
        o888o  o888o `Y8bod8P' `Y8bod8P' o888o        o888o   "888"
        */

        // RooFit Parameters Initialization
        RooRealVar *varfit = new RooRealVar(TString::Format("%s", varfit_name), TString::Format("%s", varfit_name),
                                            minbin, maxbin, it_tobefit->vardimension.c_str());
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

        // RooFit Variables taking part in the fit
        RooRealVar *realvar_rjpsi = new RooRealVar(TString::Format("RJPsi_of_%s", varfit_name), TString::Format("RJPsi_of_%s", varfit_name),
                                                   RJPsi, RJPsi, RJPsi);
        realvar_rjpsi->setConstant(true);
        RooRealVar *normalization_amplitude = new RooRealVar(TString::Format("Normalizationch_normalization_of_%s", varfit_name), TString::Format("Signal normalization of %s", varfit_name),
                                                             hybrid_normalization * histogram_jpsimumu_is_JPsiMu->GetEntries(), 1, 10 * histogram_data->GetEntries(), it_tobefit->vardimension.c_str());
        RooRealVar *signal_amplitude = new RooRealVar(TString::Format("Signal_normalization_of_%s", varfit_name), TString::Format("Signal normalization of %s", varfit_name),
                                                             hybrid_normalization * histogram_jpsimumu_is_JPsiTau->GetEntries(), 1, 10 * histogram_data->GetEntries(), it_tobefit->vardimension.c_str());
        //signal_amplitude->setConstant(true);
        RooRealVar *bkg_amplitude = new RooRealVar(TString::Format("Bkg_normalization_of_%s", varfit_name), TString::Format("Bkg normalization of %s", varfit_name),
                                                   1 * histogram_jpsimu_is_bkg->GetEntries(), 1, histogram_data->GetEntries(), it_tobefit->vardimension.c_str());
        RooRealVar *fake_amplitude = new RooRealVar(TString::Format("Fake_normalization_of_%s", varfit_name), TString::Format("Fake normalization of %s", varfit_name),
                                                    1 * histogram_fakes->GetEntries(), 1, histogram_data->GetEntries(), it_tobefit->vardimension.c_str());
        RooRealVar *total_amplitude = new RooRealVar(TString::Format("Total_normalization_of_%s", varfit_name), TString::Format("Total normalization of %s", varfit_name),
                                                     1, 1, histogram_data->GetEntries(), it_tobefit->vardimension.c_str());

        // Building histograms for roofit
        RooDataHist *binned_sig_mc = new RooDataHist("binned_sig_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_JPsiTau);
        RooDataHist *binned_norm_mc = new RooDataHist("binned_norm_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimumu_is_JPsiMu);
        RooDataHist *binned_bkg_mc = new RooDataHist("binned_bkg_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_jpsimu_is_bkg);
        RooDataHist *binned_fake_mc = new RooDataHist("binned_fake_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_fakes);
        RooDataHist *binned_mc = new RooDataHist("binned_mc", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_mc);
        RooDataHist *binned_mc_asdata = new RooDataHist("binned_mc_asdata", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_asdata_mc);
        RooDataHist *binned_data = new RooDataHist("binned_data", TString::Format("datasetwith(%s)", varfit_name), RooArgList(*varfit), histogram_data);

        // Extracting pdfs from binned samples
        RooHistPdf *binned_pdf_signal = new RooHistPdf(TString::Format("%s_hist_signal_model", varfit_name), TString::Format("%s_hist_signal_model", varfit_name), RooArgList(*varfit), *binned_sig_mc, interpolation_order);
        RooHistPdf *binned_pdf_normalization = new RooHistPdf(TString::Format("%s_hist_normalization_model", varfit_name), TString::Format("%s_hist_signal_model", varfit_name), RooArgList(*varfit), *binned_norm_mc, interpolation_order);
        RooHistPdf *binned_pdf_background = new RooHistPdf(TString::Format("%s_hist_background_model", varfit_name), TString::Format("%s_hist_signal_model", varfit_name), RooArgList(*varfit), *binned_bkg_mc, interpolation_order);
        RooHistPdf *binned_pdf_fake = new RooHistPdf(TString::Format("%s_hist_fake_model", varfit_name), TString::Format("%s_hist_signal_model", varfit_name), RooArgList(*varfit), *binned_fake_mc, interpolation_order);

        // Defining the fit model
        RooExtendPdf fit_binned_function_normalization = RooExtendPdf("fit_binned_function_normalization", "fit_binned_function_normalization",
                                                                      *binned_pdf_normalization, *varfit);
        RooExtendPdf fit_binned_function_signal = RooExtendPdf("fit_binned_function_signal", "fit_binned_function_signal",
                                                               *binned_pdf_signal, *varfit);
        RooExtendPdf fit_binned_function_background = RooExtendPdf("fit_binned_function_background", "fit_binned_function_background",
                                                                   *binned_pdf_background, *varfit);
        RooExtendPdf fit_binned_function_fake = RooExtendPdf("fit_binned_function_fake", "fit_binned_function_fake",
                                                             *binned_pdf_fake, *varfit);

        RooProdPdf fit_binned_function_normalization_fixed = RooProdPdf("fit_binned_function_normalization_fixed", "fit_binned_function_normalization_fixed",
                                                                        RooArgSet(fit_binned_function_normalization, *normalization_amplitude));
        RooProdPdf fit_binned_function_signal_fixed = RooProdPdf("fit_binned_function_signal_fixed", "fit_binned_function_signal_fixed",
                                                                 RooArgSet(fit_binned_function_signal, *realvar_rjpsi, *normalization_amplitude));

        RooAddPdf fit_binned_function_bkg_plus_fakes = RooAddPdf("fit_binned_function_bkg_plus_fakes", "fit_binned_function_bkg_plus_fakes",
                                                                 RooArgList(fit_binned_function_background, fit_binned_function_fake), RooArgList(*bkg_amplitude, *fake_amplitude));
        /*RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model",
                                        RooArgList(fit_binned_function_norm_plus_sig, fit_binned_function_bkg_plus_fakes), RooArgList(*total_amplitude));*/
        RooAddPdf fit_model = RooAddPdf("fit_model", "fit_model",
                                        RooArgList(fit_binned_function_normalization_fixed, fit_binned_function_signal_fixed, fit_binned_function_background), RooArgList(*normalization_amplitude, *signal_amplitude, *bkg_amplitude));

        const char *data_files = "InputFiles/data_ptmax_merged.root";

        TTree *data_tree = (TTree *)(TFile::Open(data_files)->Get("BTo3Mu"));

        //RooDataSet fulldata = RooDataSet("data", "data", data_tree->GetTree(), thevars, it_tobefit->cut_mediumid_true.c_str());

        //RooFitResult *results_data = fit_model.fitTo(*binned_data, RooFit::Save(true), RooFit::Minos(true), Constrain(*realvar_rjpsi));

        RooMinimizer *roofit_minimizer_interface = new RooMinimizer(*(fit_model.createNLL(*binned_mc_asdata)));
        roofit_minimizer_interface->migrad();
        roofit_minimizer_interface->minos();
        roofit_minimizer_interface->hesse();
        RooFitResult *results_data = roofit_minimizer_interface->save(TString::Format("%s_NLL_fit_results", varfit_name), TString::Format("%s_NLL_fit_results", varfit_name));

        Double_t rjpsi = histogram_asdata_jpsimumu_is_JPsiTau->GetEntries()/histogram_asdata_jpsimumu_is_JPsiMu->GetEntries();
        Double_t rjpsi_error = ( histogram_asdata_jpsimumu_is_JPsiTau->GetRMS()/histogram_asdata_jpsimumu_is_JPsiTau->GetEntries() + histogram_asdata_jpsimumu_is_JPsiMu->GetRMS()/histogram_asdata_jpsimumu_is_JPsiMu->GetEntries()) * rjpsi;
        cout << "Signal events: " << histogram_asdata_jpsimumu_is_JPsiTau->GetEntries() << " Normalization events: " << histogram_asdata_jpsimumu_is_JPsiMu->GetEntries() << endl;
        cout << "Rjpsi: " << rjpsi << " +/- " << rjpsi_error << endl;
        RooAbsReal *sigregion_model = fit_binned_function_signal.createIntegral(*varfit, NormSet(*signal_amplitude)); //The "NormSet(x)" normalizes it to the total number of events to give you the fraction n_signal_region_events/n_total_events
        RooAbsReal *normregion_model = fit_binned_function_normalization.createIntegral(*varfit, NormSet(*normalization_amplitude));
        Double_t nsigevents = sigregion_model->getVal() * (signal_amplitude->getVal());
        Double_t nnormevents = normregion_model->getVal() * (normalization_amplitude->getVal());
        Double_t rjpsi_fit = sigregion_model->getVal() / normregion_model->getVal();
        cout << "Sigregion: " << sigregion_model->getVal() << " signal amplitude: " << signal_amplitude->getVal() << endl;
        cout << "Normregion: " << normregion_model->getVal() << " norm amplitude: " << normalization_amplitude->getVal() << endl;
        Double_t sigregion_model_error = signal_amplitude->getPropagatedError(*results_data);
        Double_t normregion_model_error = normalization_amplitude->getPropagatedError(*results_data);
        Double_t rjpsi_fit_error = (sigregion_model_error/sigregion_model->getVal()  + normregion_model_error/normregion_model->getVal()) * rjpsi_fit;
        cout << "SigRegErr: " << sigregion_model_error << " NormRegErr: " << normregion_model_error << endl;
        cout << "TotalErr: " <<  rjpsi_fit_error << endl;
        cout << "Rjpsi_fit: " << rjpsi_fit << " +/- " << rjpsi_fit_error << endl;

        // Plotting
        TCanvas *inclusive_canvas = new TCanvas(TString::Format("%s%s", varfit_name, "inclusivecanvas"), TString::Format("%s%s", varfit_name, "inclusivecanvas"),1366, 768);
        TPad *inclusive_pad_distribution = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad"), TString::Format("%s%s", varfit_name, "inclusivepad"), 0.0, 0.3, 1, 1);
        TPad *inclusive_pad_residuals = new TPad(TString::Format("%s%s", varfit_name, "inclusivepad_res"), TString::Format("%s%s", varfit_name, "inclusivepad_res"), 0.0, 0.0, 1, 0.3);

        TH1F *histogram_fitmodel = new TH1F(TString::Format("%s%s", varfit_name, "fit_model_hist"), TString::Format("%s%s", varfit_name, "fit_model_hist"), bins, minbin, maxbin);
        fit_model.fillHistogram(histogram_fitmodel, *varfit, normalization_amplitude->getVal()+signal_amplitude->getVal()+bkg_amplitude->getVal());
        TH1F *histogram_fitnormalization = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_normalization_fixed"), bins, minbin, maxbin);
        fit_binned_function_normalization_fixed.fillHistogram(histogram_fitnormalization, *varfit, normalization_amplitude->getVal());
        TH1F *histogram_fitsignal = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), TString::Format("%s%s", varfit_name, "fit_binned_function_signal_fixed"), bins, minbin, maxbin);
        fit_binned_function_signal_fixed.fillHistogram(histogram_fitsignal, *varfit, signal_amplitude->getVal());
        TH1F *histogram_fitbackground = new TH1F(TString::Format("%s%s", varfit_name, "fit_binned_function_background"), TString::Format("%s%s", varfit_name, "fit_binned_function_background"), bins, minbin, maxbin);
        fit_binned_function_background.fillHistogram(histogram_fitbackground, *varfit, bkg_amplitude->getVal());

        // Drawing
        inclusive_canvas->cd(0);
        inclusive_canvas->Clear();
        inclusive_canvas->Draw();
        inclusive_pad_distribution->SetTopMargin(0.1);
        inclusive_pad_distribution->SetBottomMargin(0.001);
        inclusive_pad_distribution->SetBorderMode(0);
        inclusive_pad_distribution->SetLeftMargin(0.1);
        inclusive_pad_residuals->SetTopMargin(0.001);
        inclusive_pad_residuals->SetBottomMargin(0.2);
        inclusive_pad_residuals->SetBorderMode(0);
        inclusive_pad_residuals->SetLeftMargin(0.1);
        gStyle->SetOptTitle(kFALSE);
        inclusive_pad_distribution->Draw();
        inclusive_pad_residuals->Draw();

        inclusive_pad_distribution->cd();
        histogram_asdata_mc->SetMarkerStyle(kFullCircle);
        histogram_asdata_mc->SetMarkerSize(1);
        histogram_asdata_mc->SetMarkerColor(kBlack);
        histogram_asdata_mc->SetLineColor(kBlack);
        histogram_asdata_mc->SetFillColor(kBlack);
        histogram_asdata_mc->SetStats(false);
        histogram_asdata_mc->Draw("ME");
        histogram_fitmodel->SetMarkerColor(kBlue);
        histogram_fitmodel->SetLineColor(kBlue);
        histogram_fitmodel->SetLineWidth(3);
        histogram_fitmodel->SetStats(false);
        histogram_fitmodel->Draw("SAME HIST");
        histogram_fitnormalization->SetMarkerColor(kCyan+1);
        histogram_fitnormalization->SetLineColor(kCyan+1);
        histogram_fitnormalization->SetLineStyle(kDashed);
        histogram_fitnormalization->SetLineWidth(3);
        histogram_fitnormalization->SetStats(false);
        histogram_fitnormalization->Draw("SAME HIST");
        histogram_fitsignal->SetMarkerColor(kRed);
        histogram_fitsignal->SetLineColor(kRed);
        histogram_fitsignal->SetLineStyle(kDashed);
        histogram_fitsignal->SetLineWidth(3);
        histogram_fitsignal->SetStats(false);
        histogram_fitsignal->Draw("SAME HIST");
        histogram_fitbackground->SetMarkerColor(kGreen+1);
        histogram_fitbackground->SetLineColor(kGreen+1);
        histogram_fitbackground->SetLineStyle(kDashed);
        histogram_fitbackground->SetLineWidth(3);
        histogram_fitbackground->SetStats(false);
        histogram_fitbackground->Draw("SAME HIST");
        //cout << ((TH1F *)inclusive_histstack_distribution->GetStack()->Last())->GetEntries() << endl;

        TLegend leg_fitwithratio = TLegend();
        leg_fitwithratio.SetX1(it_tobefit->legend_position_x0); leg_fitwithratio.SetX2(it_tobefit->legend_position_x1);
        leg_fitwithratio.SetY1(it_tobefit->legend_position_y0); leg_fitwithratio.SetY2(it_tobefit->legend_position_y1);
        leg_fitwithratio.SetTextSize(0.035);
        leg_fitwithratio.SetBorderSize(0);
        leg_fitwithratio.SetFillColor(0);
        leg_fitwithratio.SetFillStyle(0);

        // RooFit
        leg_fitwithratio.AddEntry("Data", "Data");
        leg_fitwithratio.AddEntry(histogram_fitmodel->GetName(), "Model: Bkg+Sig+Norm");
        leg_fitwithratio.AddEntry(histogram_fitbackground->GetName(), "Bkg: Background", "L");
        //leg_fitwithratio.AddEntry(histogram_fitfakes->GetName(), "Fakes", "L");
        leg_fitwithratio.AddEntry(histogram_fitsignal->GetName(), "Sig: B_{c}#rightarrowJ/#Psi#tau", "L");
        leg_fitwithratio.AddEntry(histogram_fitnormalization->GetName(), "Norm: B_{c}#rightarrowJ/#Psi#mu", "L");
        leg_fitwithratio.AddEntry((TObject *)0, TString::Format("R(J/#Psi) after cut: %.4f #pm %.4f",rjpsi, rjpsi_error), "");
        leg_fitwithratio.AddEntry((TObject *)0, TString::Format("R(J/#Psi) from fit: %.4f #pm %.4f", rjpsi_fit, rjpsi_fit_error), "");
        leg_fitwithratio.Draw("SAME");        
        gPad->SetLogy(0);

        inclusive_pad_residuals->cd();
        TH1F *inclusive_distributions_ratio = new TH1F();
        inclusive_distributions_ratio = (TH1F *)histogram_asdata_mc->Clone();
        inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, histogram_fitmodel);
        inclusive_distributions_ratio->SetStats(false);
        inclusive_distributions_ratio->Draw("M");
        //inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
        inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
        inclusive_distributions_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", it_tobefit->vartobefitted, it_tobefit->vardimension.c_str()));
        inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
        inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.7);
        inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
        inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
        inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
        inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.4);
        inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
        inclusive_pad_residuals->Modified();

        inclusive_canvas->Print(TString::Format("FitRatio_Plot_%s.png", varfit_name).Data());
        inclusive_canvas->Clear();
    }
    return 0;
}