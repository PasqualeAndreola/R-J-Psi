#include "HeaderFiles/NormalizationJPsi_Mu.h"

//It can be useful to use these namespaces
using namespace std;
using namespace RooFit;

int NormalizationJPsiMu(const char *cuts,
              bool debug)
{
  if (debug == false)
  {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //      Getting the mass of the 3 muon system in Data and in MC
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ROOT::RDataFrame Data("BTo3Mu", "InputFiles/data_ptmax_merged.root");
    TH1F *histogram_3mumass_data = (TH1F *)Data.Filter(cuts)
                                       .Fill<Float_t>(TH1F("mass3mu_Data", "Data", 80, 0, 10), {"Bmass"})
                                       ->Clone();
    histogram_3mumass_data->Sumw2();
    ROOT::RDataFrame HbToJPsiMuMu3MuFilter("BTo3Mu", "InputFiles/HbToJPsiMuMu3MuFilter_ptmax_merged.root");
    TH1F *histogram_3mumass_mc = (TH1F *)HbToJPsiMuMu3MuFilter.Filter(cuts)
                                     .Filter("abs(mu1_grandmother_pdgId)!=541 & abs(mu2_grandmother_pdgId)!=541")
                                     .Fill<Float_t>(TH1F("mass3mu_HbToJPsiMuMu3MuFilter", "jpsimuon_MC", 80, 0, 10), {"Bmass"})
                                     ->Clone();
    histogram_3mumass_mc->Sumw2();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //      Adding the components to the stack
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    THStack *jpsimu_histstack_distribution = new THStack("jpsimu_histstack_distribution", "jpsimu_histstack_distribution");
    jpsimu_histstack_distribution->Add(histogram_3mumass_mc);
    TH1F *jpsimu_histstack_sum = (TH1F *)jpsimu_histstack_distribution->GetStack()->Last();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //      Defining the sideband for the 3 muon system mass
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Double_t mumass_low = 6.5;
    Double_t mumass_high = 10;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Getting the integral for the sideband and computing the Data/MC ratio
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Double_t mumass_sideband_integral_data = histogram_3mumass_data->Integral(histogram_3mumass_data->FindFixBin(mumass_low), histogram_3mumass_data->FindFixBin(mumass_high));
    Double_t mumass_sideband_integral_mc = jpsimu_histstack_sum->Integral(jpsimu_histstack_sum->FindFixBin(mumass_low), jpsimu_histstack_sum->FindFixBin(mumass_high));
    Double_t jpsimu_normalization = mumass_sideband_integral_data / mumass_sideband_integral_mc;
    cout << jpsimu_normalization << endl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Plotting
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TCanvas *jpsimu_norm_canvas = new TCanvas("jpsimu_norm_canvas", "jpsimu_norm_canvas", 1360, 768);
    jpsimu_norm_canvas->cd(0);

    histogram_3mumass_mc->SetTitle("J/#Psi+#mu sample normalization respect to data");
    histogram_3mumass_mc->GetXaxis()->SetTitle("(3#mu)Mass");
    histogram_3mumass_mc->GetYaxis()->SetTitle("Norm. occur. [a. u.]");
    histogram_3mumass_mc->SetAxisRange(3, 10, "X");
    histogram_3mumass_mc->SetAxisRange(0, histogram_3mumass_mc->GetMaximum(), "Y");
    histogram_3mumass_mc->SetStats(false);
    histogram_3mumass_mc->DrawNormalized("BAR PFC SAME");

    histogram_3mumass_data->SetAxisRange(0, histogram_3mumass_data->GetMaximum(), "Y");
    histogram_3mumass_data->SetStats(false);
    histogram_3mumass_data->SetMarkerStyle(kFullCircle);
    histogram_3mumass_data->SetMarkerSize(1);
    histogram_3mumass_data->SetMarkerColor(kBlack);
    histogram_3mumass_data->SetFillColor(kBlack);
    histogram_3mumass_data->SetLineColor(kBlack);
    histogram_3mumass_data->DrawNormalized("SAME");

    //Adjusting the legend
    TLegend *legend = new TLegend(0.7, 0.81, 0.95, 0.9);
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetFillStyle(0);
    legend->AddEntry(histogram_3mumass_data->GetName(), "Data", "PMC");
    legend->AddEntry(histogram_3mumass_mc->GetName(), "Hb#rightarrowJ/#Psi#mu", "PFC");
    legend->AddEntry((TObject *)0, TString::Format("Data Over MC: %.2f", jpsimu_normalization), "");
    legend->Draw("SAME");
    gPad->SetLogy(0);

    //Zoom on the sideband
    TArrow *arrow_zoom = new TArrow(8, 0.01, 8, 0.02, 0.005, "|>");
    arrow_zoom->Draw();

    TPad *jpsimu_norm_sideband_pad = new TPad("jpsimu_norm_sideband_pad", "jpsimu_norm_sideband_pad", 0.4, 0.31, 0.955, 0.8);
    jpsimu_norm_canvas->cd(1);
    jpsimu_norm_sideband_pad->Draw();
    jpsimu_norm_sideband_pad->Clear();
    jpsimu_norm_sideband_pad->SetTickx(1);
    jpsimu_norm_sideband_pad->SetTicky(1);
    jpsimu_norm_sideband_pad->SetFillStyle(4000);
    jpsimu_norm_sideband_pad->Draw("SAME");

    jpsimu_norm_sideband_pad->cd();
    histogram_3mumass_mc->Scale(1 / mumass_sideband_integral_mc);
    histogram_3mumass_mc->SetAxisRange(mumass_low, mumass_high, "X");
    histogram_3mumass_mc->SetTitle("");
    histogram_3mumass_mc->GetXaxis()->SetTitleSize(0);
    histogram_3mumass_mc->GetYaxis()->SetTitleSize(0);
    histogram_3mumass_mc->Draw("SAME BAR PFC");

    histogram_3mumass_data->Scale(1 / mumass_sideband_integral_data);
    histogram_3mumass_data->SetAxisRange(mumass_low, mumass_high, "X");
    histogram_3mumass_data->GetXaxis()->SetTitleSize(0);
    histogram_3mumass_data->GetYaxis()->SetTitleSize(0);
    histogram_3mumass_data->Draw("SAME");

    jpsimu_norm_canvas->Print("jpsi_mu_normalization.png");
  }
  return 0;
}