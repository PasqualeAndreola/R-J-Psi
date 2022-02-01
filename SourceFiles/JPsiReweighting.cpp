/*!;;
 *  \file JPsiReweighting.cpp;
 *  \brief Source file for \ref JPsiReweighting function implementation;
 */

/*!;
 *  \fn int TreeReader(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read) "";;
 *  \brief Function used to read selected variables stored in selected trees;
 * ;
 *  \param file2read name of the file which stores the trees;
 *  \param trees2read vector of names of trees that the user wants to explore;
 */
;
#include "HeaderFiles/JPsiReweighting.h"

using namespace std;
using namespace RooFit;

int JPsiReweighting(vector<MarkedNames> datanames, vector<MarkedNames> treenames, vector<VarToBePlotted> *variable2becompared, bool debugfit)
{
    const char *output_splots_name = "jpsi_pi_splots.root";
    if (debugfit == false)
    {
        // ROOT::EnableImplicitMT();
        // ROOT::gStyle.SetOptStat(0);
        gROOT->SetBatch(true);

        gStyle->SetOptStat(true);
        TH1::SetDefaultSumw2();

        ////// important;
        RooAbsData::setDefaultStorageType(RooAbsData::Tree);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        //      Mass type: choose whether bare or post vertex fit;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        // const char* mass_type = "Bmass";
        const char *mass_type = "bvtx_fit_mass";
        ;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        //      Variables and PDFs;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        unordered_map<string, RooRealVar> myvars;
        myvars["mass"] = RooRealVar(mass_type, "J/#Psi#pi^{#pm} mass", 6.0, 6.6, "GeV");
        myvars["jpsi_mass"] = RooRealVar("jpsivtx_fit_mass", "J/#Psimass", 0., 6.6, "GeV");
        myvars["jpsi_eta"] = RooRealVar("jpsivtx_fit_eta", "J/#Psi #eta", -10, 10);
        myvars["mu1pt"] = RooRealVar("mu1pt", "mu1pt", 0, 1000, "GeV");
        myvars["mu2pt"] = RooRealVar("mu2pt", "mu2pt", 0, 1000, "GeV");
        myvars["mu1eta"] = RooRealVar("mu1eta", "mu1eta", -10, 10);
        myvars["mu2eta"] = RooRealVar("mu2eta", "mu2eta", -10, 10);
        myvars["bpt"] = RooRealVar("Bpt", "Bpt", 0., 9000.);
        myvars["blxy"] = RooRealVar("bvtx_lxy", "bvtx_lxy", 0., 9000.);
        myvars["blovers"] = RooRealVar("bvtx_lxy_sig", "bvtx_lxy_sig", 0., 9000.);
        myvars["bsvprob"] = RooRealVar("bvtx_svprob", "bvtx_svprob", 0., 1.);
        myvars["bcos"] = RooRealVar("bvtx_cos2D", "bvtx_cos2D", 0., 1.);
        myvars["bchi2"] = RooRealVar("bvtx_chi2", "bvtx_chi2", 0., 1000.);
        myvars["blxy_unc"] = RooRealVar("bvtx_lxy_unc", "bvtx_lxy_unc", 0., 1000.);
        myvars["jpsilxy"] = RooRealVar("jpsivtx_lxy", "bvtx_lxy", 0., 9000.);
        myvars["jpsilovers"] = RooRealVar("jpsivtx_lxy_sig", "bvtx_lxy_sig", 0., 9000.);
        myvars["jpsisvprob"] = RooRealVar("jpsivtx_svprob", "bvtx_svprob", 0., 1.);
        myvars["jpsicos2D"] = RooRealVar("jpsivtx_cos2D", "jpsivtx_cos2D", 0., 1.);
        myvars["jpsichi2"] = RooRealVar("jpsivtx_chi2", "jpsivtx_chi2", 0., 1000.);
        myvars["jpsilxy_unc"] = RooRealVar("jpsivtx_lxy_unc", "jpsivtx_lxy_unc", 0., 1000.);
        // myvars["jpsicos"    ] = RooRealVar("jpsivtx_log10_svprob" , "jpsivtx_cos2D"       ,   0. ,    1. );
        // myvars["jpsicos"    ] = RooRealVar("jpsivtx_log10_lxy"    , "jpsivtx_cos2D"       ,   0. ,    1. );
        // myvars["jpsicos"    ] = RooRealVar("jpsivtx_log10_lxy_sig", "jpsivtx_cos2D"       ,   0. ,    1. );
        myvars["pipt"] = RooRealVar("kpt", "kpt", 0, 1000, "GeV");
        myvars["pieta"] = RooRealVar("keta", "keta", -10, 10);
        myvars["mu1id"] = RooRealVar("mu1_mediumID", "mu1_mediumID", 0., 2.);
        myvars["mu2id"] = RooRealVar("mu2_mediumID", "mu2_mediumID", 0., 2.);
        myvars["mu1dxy"] = RooRealVar("mu1_dxy", "mu1_dxy", -5., 5., "cm");
        myvars["mu2dxy"] = RooRealVar("mu2_dxy", "mu2_dxy", -5., 5., "cm");
        myvars["pidxy"] = RooRealVar("k_dxy", "k_dxy", -5., 5., "cm");
        myvars["mu1dz"] = RooRealVar("mu1_dz", "mu1_dz", -25., 25., "cm");
        myvars["mu2dz"] = RooRealVar("mu2_dz", "mu2_dz", -25., 25., "cm");
        myvars["mu2_dzErr"] = RooRealVar("mu2_dzErr", "mu2_dzErr", -25., 25., "cm");
        myvars["pidz"] = RooRealVar("k_dz", "k_dz", -25., 25., "cm");
        myvars["Bptreco"] = RooRealVar("Bpt_reco", "Bpt_reco", -1e6, 1e6, "GeV");
        myvars["jpsimass"] = RooRealVar("jpsi_mass", "jpsi_mass", -1e6, 1e6, "GeV");
        myvars["Bmass"] = RooRealVar("Bmass", "Bmass", -1e6, 1e6, "GeV");
        myvars["dr12"] = RooRealVar("dr12", "dr12", -1e6, 1e6, "cm");
        myvars["dr13"] = RooRealVar("dr13", "dr13", -1e6, 1e6, "cm");
        myvars["dr23"] = RooRealVar("dr23", "dr23", -1e6, 1e6, "cm");
        // histos["jpsivtx_chi2"         ] = (RDF.TH1DModel("jpsivtx_chi2"         , "", 50,      0,    50), "vtx(//mu_{1}, //mu_{2}) //chi^{2}"                                , 1);
        // histos["jpsivtx_lxy_unc"      ] = (RDF.TH1DModel("jpsivtx_lxy_unc"      , "",100,      0,  0.02), "//sigma_{L_{xy}} (cm)"                                          , 1);
        // histos["jpsivtx_svprob"       ] = (RDF.TH1DModel("jpsivtx_svprob"       , "", 50,      0,     1), "vtx(//mu_{1}, //mu_{2}) probability"                             , 0);
        // histos["jpsivtx_log10_svprob" ] = (RDF.TH1DModel("jpsivtx_log10_svprob" , "", 51,     -8,     1), "log_{10} vtx(//mu_{1}, //mu_{2}) probability"                    , 1);
        // histos["jpsivtx_log10_lxy"    ] = (RDF.TH1DModel("jpsivtx_log10_lxy"    , "", 51,     -4,     1), "log_{10} vtx(//mu_{1}, //mu_{2}) L_{xy}"                         , 1);
        // histos["jpsivtx_log10_lxy_sig"] = (RDF.TH1DModel("jpsivtx_log10_lxy_sig", "", 51,     -2,     2), "log_{10} vtx(//mu_{1}, //mu_{2}) L_{xy}///sigma_{L_{xy}}"         , 1);
        ;
        // only MC;
        unordered_map<string, RooRealVar> myvarsmc;
        myvarsmc["k_genpdgId"] = RooRealVar("k_genpdgId", "k_genpdgId", -1e6, 1e6);
        myvarsmc["k_mother_pdgId"] = RooRealVar("k_mother_pdgId", "k_mother_pdgId", -1e6, 1e6);
        myvarsmc["mu1_genpdgId"] = RooRealVar("mu1_genpdgId", "mu1_genpdgId", -1e6, 1e6);
        myvarsmc["mu1_mother_pdgId"] = RooRealVar("mu1_mother_pdgId", "mu1_mother_pdgId", -1e6, 1e6);
        myvarsmc["mu1_grandmother_pdgId"] = RooRealVar("mu1_grandmother_pdgId", "mu1_grandmother_pdgId", -1e6, 1e6);
        myvarsmc["mu2_genpdgId"] = RooRealVar("mu2_genpdgId", "mu2_genpdgId", -1e6, 1e6);
        myvarsmc["mu2_mother_pdgId"] = RooRealVar("mu2_mother_pdgId", "mu2_mother_pdgId", -1e6, 1e6);
        myvarsmc["mu2_grandmother_pdgId"] = RooRealVar("mu2_grandmother_pdgId", "mu2_grandmother_pdgId", -1e6, 1e6);
        ;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        //      mass ranges;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        Double_t fit_range_lo = 6;
        Double_t mass_window_lo = 6.275 - 0.15;
        Double_t mass_window_hi = 6.275 + 0.15;
        Double_t fit_range_hi = 6.6;

        RooRealVar mass = myvars["mass"];
        mass.setRange("left", fit_range_lo, mass_window_lo);
        mass.setRange("right", mass_window_hi, fit_range_hi);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        //      PDFs;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;

        // combinatorial background poly;
        RooRealVar pol_c1 = RooRealVar("pol_c1", "coefficient of x^0 term", -5.23507e-02, -100, 100);
        // pol_c2 = RooRealVar("pol_c2", "coefficient of x^1 term", 0.6, -10, 10);
        // pol_c3 = RooRealVar("pol_c3", "coefficient of x^2 term", 0.5, -10, 10);
        RooChebychev bkg = RooChebychev("bkg_pol", "1st order poly", mass, RooArgList(pol_c1));
        // bkg = RooChebychev("bkg_pol", "2nd order poly", mass, RooArgList(pol_c1, pol_c2));
        // bkg = RooChebychev("bkg_pol", "3rd order poly", mass, RooArgList(pol_c1, pol_c2, pol_c3));

        // argus function, partially reconstructed decays;
        RooRealVar argpar = RooRealVar("argpar", "argus shape parameter", -1.95759e+00, -20, 20);
        RooRealVar maxM = RooRealVar("maxM", "argus max m", 6.20639e+00, 6.0, 6.275); // 6.2);
        RooArgusBG argus = RooArgusBG("argus", "Argus PDF", mass, maxM, argpar);

        // detector response function;
        RooRealVar mg = RooRealVar("mg", "mg", 0);                         //, -0.01, 0.01);
        RooRealVar sg = RooRealVar("sg", "sg", 1.14919e-02, 0.0001, 0.03); //, 0.001,0.2);
        RooGaussian resGauss = RooGaussian("resGauss", "resGauss", mass, mg, sg);

        // construct convolution;
        mass.setBins(10000, "fft");
        RooFFTConvPdf lxg = RooFFTConvPdf("lxg", "argus (X) gauss", mass, argus, resGauss);

        // Bc->Jpsi K gaussian;
        RooRealVar jpsik_mean = RooRealVar("jpsik_mean", "mean", 6.17, 6.10, 6.25);
        RooRealVar jpsik_sigma = RooRealVar("jpsik_sigma", "sigma", 0.03, 0.01, 0.1);
        RooGaussian jpsik_func = RooGaussian("jpsik_func", "jpsik_func", mass, jpsik_mean, jpsik_sigma);

        // signal narrow gaussian;
        RooRealVar mean = RooRealVar("mean", "mean", 6.26774e+00, 6.1, 6.4);
        RooRealVar narrow_width = RooRealVar("narrow_width", "narrow_width", 2.44845e-02, 1e-6, 0.1);
        RooGaussian narrow_gaus = RooGaussian("sig_narrow_gaus", "sig_narrow_gaus", mass, mean, narrow_width);

        // signal broad gaussian;
        RooRealVar broad_width = RooRealVar("broad_width", "broad_width", 5.62194e-02, 1e-6, 1.);
        RooGaussian broad_gaus = RooGaussian("sig_broad_gaus", "sig_broad_gaus", mass, mean, broad_width);

        // fractional yields;
        // you need these and not absolute yields in combine;
        // don"t fit with Extended!;
        RooRealVar frac_sig = RooRealVar("frac_sig", "frac_sig", 2.67041e-01, 0., 1.);
        RooRealVar frac_pi = RooRealVar("frac_pi", "frac_pi", 6.31013e-01, 0., 1.);
        RooRealVar frac_bkg = RooRealVar("frac_bkg", "frac_bkg", 4.20044e-01, 0., 1.);
        // fixed to PDG (Jpsi K) / (Jpsi pi) value https://pdglive.lbl.gov/BranchingRatio.action?desig=14&parCode=S091;
        Double_t frac_k_value = 0.079 / (1. + 0.079);
        RooRealVar frac_k = RooRealVar("frac_k", "frac_k", frac_k_value);

        // signal function;
        RooAddPdf signal_fit_function = RooAddPdf(
            "signal_fit_function",
            "signal_fit_function",
            RooArgList(narrow_gaus, broad_gaus),
            RooArgList(frac_sig));

        // signal Jpsi pi plus Jpsi K;
        // RooAddPdf::pi_plus_k_fit_function[ frac_k * jpsik_func + [%] * signal_fit_function ];
        RooAddPdf pi_plus_k_fit_function = RooAddPdf(
            "pi_plus_k_fit_function",
            "pi_plus_k_fit_function",
            RooArgList(jpsik_func, signal_fit_function), // order matters for coefficients in next line https://www.nikhef.nl/~vcroft/SignalAndBackground-CompositeModels.html
            RooArgList(frac_k));

        // background function;
        RooAddPdf bkg_fit_function = RooAddPdf(
            "bkg_fit_function",
            "bkg_fit_function",
            //     RooArgList(bkg, lxg, jpsik_func), ;
            //     RooArgList(frac_pi, frac_k);
            RooArgList(lxg, bkg),
            RooArgList(frac_pi));

        // total function;
        RooAddPdf fit_function = RooAddPdf(
            "fit_function",
            "fit_function",
            RooArgList(bkg_fit_function, pi_plus_k_fit_function),
            RooArgList(frac_bkg));

        // MC signal narrow gaussian;
        RooRealVar mc_mean = RooRealVar("mc_mean", "mc_mean", 6.275, 5.5, 7.);
        RooRealVar mc_narrow_width = RooRealVar("mc_narrow_width", "mc_narrow_width", 0.038, 1e-6, 1.);
        RooGaussian mc_narrow_gaus = RooGaussian("mc_sig_narrow_gaus", "mc_sig_narrow_gaus", mass, mc_mean, mc_narrow_width);

        // MC signal broad gaussian;
        RooRealVar mc_broad_width = RooRealVar("mc_broad_width", "mc_broad_width", 0.06, 1e-6, 1.);
        RooGaussian mc_broad_gaus = RooGaussian("mc_sig_broad_gaus", "mc_sig_broad_gaus", mass, mc_mean, mc_broad_width);

        RooRealVar mc_nsig = RooRealVar("mc_signal_yield", "mc_signal_yield", 800, 0, 10000000);
        RooRealVar mc_nsig_narrow = RooRealVar("mc_signal_yield_narrow", "mc_signal_yield_narrow", 700, 0, 10000000);
        RooRealVar mc_nsig_broad = RooRealVar("mc_signal_yield_broad", "mc_signal_yield_broad", 100, 0, 10000000);

        // MC signal function;
        RooAddPdf mc_signal_fitFunction = RooAddPdf(
            "mc_signal_fit_function",
            "mc_signal_fit_function",
            RooArgList(mc_narrow_gaus, mc_broad_gaus),
            RooArgList(mc_nsig_narrow, mc_nsig_broad));

        RooArgSet thevars = RooArgSet();
        for (unordered_map<string, RooRealVar>::iterator rvarmapit = myvars.begin(); rvarmapit != myvars.end(); rvarmapit++)
        {
            thevars.add(rvarmapit->second);
        }
        RooArgSet thevars_mc = thevars;
        for (unordered_map<string, RooRealVar>::iterator rvarmapit = myvarsmc.begin(); rvarmapit != myvarsmc.end(); rvarmapit++)
        {
            thevars_mc.add(rvarmapit->second);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        // selection on data, plotting, fitting;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        // HLT_DoubleMu4_JpsiTrk_Displaced_v14;
        TString selection = TString::Format("%s>6 &&", mass_type);
        selection += TString::Format("%s<6.6 &&", mass_type);
        selection += "mu1pt>4.5 &&"
                     "mu2pt>4.5 &&"
                     //     "mu1pt>5 &&"
                     //     "mu2pt>5 &&"
                     "abs(mu1eta)<2.4 &&"
                     "abs(mu2eta)<2.4 &&"
                     //     "mu1pt>3.5 &&"
                     //     "mu2pt>3.5 &&"
                     //     "Bpt>15 &&"
                     //     "Blxy>0.01 &&"            , // 100 micron;
                     //     "Blxy_sig>3 &&"
                     //     "Bsvprob>0.005 &&"
                     //     "Bsvprob>0.001 &&"
                     //     "Bsvprob>0.1 &&"
                     "kpt>2.5 &&"
                     //     "kpt>3.5 &&"
                     "abs(keta)<2.4 &&"
                     "bvtx_cos2D>0.999 &&"
                     "mu1_mediumID>0.5 &&"
                     "mu2_mediumID>0.5 &&"
                     "abs(mu1_dz-mu2_dz)<0.2 &&" // *;
                     "abs(mu1_dz-k_dz)<0.2 &&"   // *;
                     "abs(mu2_dz-k_dz)<0.2 &&"   // *;
                     "abs(k_dxy)<0.05 &&"        // *;
                     "abs(mu1_dxy)<0.05 &&"      // *;
                     "abs(mu2_dxy)<0.05 &&"      // *;
                     "(((abs(mu1eta)<1. && abs(mu1eta)<1.) && abs(jpsivtx_fit_mass-3.0969)<0.1) ||"
                     "(!(abs(mu1eta)<1. && abs(mu1eta)<1.) && abs(jpsivtx_fit_mass-3.0969)<0.15))"; // *;

        // add gen matching;
        TString selection_mc = selection;
        selection_mc += "&& abs(k_genpdgId)==211 &&"
                        "abs(k_mother_pdgId)==541 &&"
                        "abs(mu1_genpdgId)==13 &&"
                        "abs(mu1_mother_pdgId)==443 &&"
                        "abs(mu2_genpdgId)==13 &&"
                        "abs(mu2_mother_pdgId)==443 &&"
                        "abs(mu1_grandmother_pdgId)==541 &&"
                        "abs(mu2_grandmother_pdgId)==541";

        TTree *data_tree = new TTree();
        TTree *signal_tree = new TTree();
        for (vector<MarkedNames>::iterator fileiterator = datanames.begin(); fileiterator != datanames.end(); fileiterator++)
        {
            TString filename = fileiterator->name;
            if (fileiterator->datatype.CompareTo("Data") == 0)
                data_tree = (TTree *)(TFile::Open(filename)->Get(fileiterator->treetype.Data()));
            if (fileiterator->datatype.CompareTo("MC") == 0)
                signal_tree = (TTree *)(TFile::Open(filename)->Get(fileiterator->treetype.Data()));
        }
        ROOT::RDataFrame BtoJPsiPi_Data(*data_tree);
        auto b2jpsipinode_data = std::make_unique<ROOT::RDF::RNode>(BtoJPsiPi_Data);
        BtoJPsiPi_Data.Filter(selection.Data()).Snapshot(data_tree->GetName(), data_tree->GetCurrentFile()->GetName());
        // ROOT::RDataFrame DataFrame_Data("BTo2MuP", "InputFiles/data_ptmax_merged.root");
        // ROOT::RDF::RNode data_filtered = DataFrame_Data.Filter(selection.Data());
        Int_t nbins = 80;
        RooDataSet *fulldata = new RooDataSet("data", "data", data_tree, thevars, selection.Data());
        TH1F *histogram_data = (TH1F *)fulldata->createHistogram("fulldatahist", mass, Binning(nbins, mass.getMin(), mass.getMax()))->Clone();
        histogram_data->Sumw2();
        RooDataSet *fullsignal = new RooDataSet("signal", "signal", signal_tree, thevars_mc, selection_mc.Data());
        TH1F *histogram_mc = (TH1F *)fullsignal->createHistogram("fullmchist", mass, Binning(nbins, mass.getMin(), mass.getMax()))->Clone();
        histogram_mc->Sumw2();
        // RooDataHist *fulldata_binned = new RooDataHist("fulldata_binned", "fulldata_binned", mass, histogram_data);
        // if (fulldata_binned==NULL) return 0;
        //  plot;
        TCanvas c1 = TCanvas("c1", "", 1360, 768);
        TPad pad_comparison = TPad("pad_comparison", "pad_comparison", 0.0, 0.3, 1, 1);
        TPad pad_ratio = TPad("pad_ratio", "pad_ratio", 0.0, 0.0, 1, 0.3);
        c1.Draw();
        pad_comparison.SetTopMargin(0.05);
        pad_comparison.SetBottomMargin(0.);
        pad_comparison.SetBorderMode(0);
        pad_comparison.SetLeftMargin(0.1);
        pad_ratio.SetTopMargin(0.);
        pad_ratio.SetBottomMargin(0.2);
        pad_ratio.SetBorderMode(0);
        pad_ratio.SetLeftMargin(0.1);
        gStyle->SetOptTitle(kFALSE);
        pad_comparison.Draw();
        pad_ratio.Draw();
        pad_comparison.cd();
        RooPlot *frame = mass.frame();
        frame->SetTitle("");
        fulldata->plotOn(frame, RooFit::Binning(nbins), RooFit::MarkerSize(1.5));

        // Fit
        RooFitResult *results_data = fit_function.fitTo(*fulldata, RooFit::Save());
        fit_function.plotOn(frame);
        fit_function.plotOn(frame, RooFit::Components("bkg_pol"), Name(bkg.GetName()), RooFit::LineStyle(kDashed), RooFit::LineColor(kBlue));
        fit_function.plotOn(frame, RooFit::Components("lxg"), Name(lxg.GetName()), RooFit::LineStyle(kDashed), RooFit::LineColor(kOrange));
        // fit_function.plotOn(frame, RooFit::Components("argus")              , RooFit::LineStyle(kDashed), RooFit::LineColor(kViolet));
        fit_function.plotOn(frame, RooFit::Components("signal_fit_function"), Name(signal_fit_function.GetName()), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
        fit_function.plotOn(frame, RooFit::Components("jpsik_func"), Name(jpsik_func.GetName()), RooFit::LineStyle(kDashed), RooFit::LineColor(kGreen));
        frame->Draw();

        TLegend leg = VarToBeFitted::SetLegendPosAuto("TR", 5, 0.25);
        // leg.SetNColumns(3);
        RooPlot *frame_data = mass.frame();
        frame_data->SetTitle("");
        fulldata->plotOn(frame_data, RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack));
        fit_function.plotOn(frame_data, Name("data_signal_fit_function_Norm"), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
        Double_t chi2_datafit = frame_data->chiSquare();

        // RooFit
        leg.AddEntry(lxg.GetName(), "B_{c}#rightarrow J/#Psi#pi + X", "L");
        leg.AddEntry(bkg.GetName(), "combinatorial bkg", "L");
        leg.AddEntry(signal_fit_function.GetName(), "B_{c}#rightarrow J/#Psi#pi", "L");
        leg.AddEntry(jpsik_func.GetName(), "B_{c}#rightarrow J/#Psi K", "L");
        leg.AddEntry(fulldata->GetName(), "observed", "EP");
        leg.Draw("SAME");
        TPaveText pavtext_fitwithratio = VarToBePlotted::SetPaveTextPosManual(0, leg.GetY1(), "TR", 4, 0.25);
        pavtext_fitwithratio.AddText(TString::Format("Number of events: %.0d", fulldata->numEntries()));
        pavtext_fitwithratio.AddText(TString::Format("Background events: %.0f#pm%.0f", frac_bkg.getVal() * fulldata->numEntries(), frac_bkg.getError() * fulldata->numEntries()));
        pavtext_fitwithratio.AddText(TString::Format("Signal events: %.0f#pm%.0f", frac_sig.getVal() * fulldata->numEntries(), frac_sig.getError() * fulldata->numEntries()));
        pavtext_fitwithratio.AddText(TString::Format("#chi^{2}: %.2f", chi2_datafit));
        pavtext_fitwithratio.SetTextSize(0.035);
        pavtext_fitwithratio.SetTextFont(42);
        pavtext_fitwithratio.Draw("SAME");
        pad_comparison.Modified();

        pad_ratio.cd();
        TH1F *data_fit_ratio = new TH1F();
        data_fit_ratio = (TH1F *)histogram_data->Clone();
        if (data_fit_ratio == NULL)
            return 0;
        TH1F *histogram_fitmodel = new TH1F("fit_model_hist", "fit_model_hist", nbins, 6, 6.6);
        fit_function.fillHistogram(histogram_fitmodel, mass, fulldata->numEntries() * fit_function.getNorm());
        if (histogram_fitmodel == NULL)
            return 0;
        data_fit_ratio->Divide(data_fit_ratio, histogram_fitmodel);
        data_fit_ratio->Draw("M");
        data_fit_ratio->SetStats(false);
        data_fit_ratio->GetYaxis()->SetNdivisions(5);
        data_fit_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", mass.GetName(), mass.getUnit()));
        data_fit_ratio->GetXaxis()->SetTitleSize(0.1);
        data_fit_ratio->GetXaxis()->SetTitleOffset(0.8);
        data_fit_ratio->GetXaxis()->SetLabelSize(0.09);
        data_fit_ratio->GetYaxis()->SetTitle("Obs/Exp");
        data_fit_ratio->GetYaxis()->SetTitleSize(0.1);
        data_fit_ratio->GetYaxis()->SetTitleOffset(0.45);
        data_fit_ratio->GetYaxis()->SetLabelSize(0.09);
        pad_ratio.Modified();

        c1.SaveAs("sideband_fit.pdf");
        c1.SaveAs("sideband_fit.png");

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        //  Fit the mc;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;
        c1.cd();
        pad_comparison.cd();
        RooFitResult *results_mc = mc_signal_fitFunction.fitTo(*fullsignal, RooFit::Extended(true), RooFit::Save());
        RooPlot *frame_mc = mass.frame();
        fullsignal->plotOn(frame_mc, RooFit::Binning(nbins), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
        mc_signal_fitFunction.plotOn(frame_mc, Name("mc_signal_fit_function_Norm"), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
        frame_mc->Draw();
        frame_mc->SetTitle("");
        fullsignal->plotOn(frame_mc, RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
        mc_signal_fitFunction.plotOn(frame_mc, RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
        Double_t chi2_mcfit = frame_mc->chiSquare();
        TLegend legmc = VarToBeFitted::SetLegendPosAuto("TR", 2, 0.25);
        const char *legcomp_mcsigfit = TString::Format("mc_signal_fit_function_Norm[%s]", mass_type);
        legmc.AddEntry("mc_signal_fit_function_Norm", "B_{c}#rightarrow J/#Psi#pi MC", "LP");
        legmc.AddEntry(fulldata->GetName(), "Observed MC", "EP");
        legmc.Draw("SAME");
        TPaveText pavtext_fitwithratiomc = VarToBePlotted::SetPaveTextPosManual(0, legmc.GetY1(), "TR", 3, 0.25);
        pavtext_fitwithratiomc.AddText(TString::Format("Number of events: %.0d", fullsignal->numEntries()));
        pavtext_fitwithratiomc.AddText(TString::Format("Signal events: %.0f#pm%.0f", mc_nsig_broad.getVal() + mc_nsig_narrow.getVal(), mc_nsig_broad.getError() + mc_nsig_narrow.getError()));
        pavtext_fitwithratiomc.AddText(TString::Format("#chi^{2}: %.2f", chi2_mcfit));
        pavtext_fitwithratiomc.SetTextSize(0.035);
        pavtext_fitwithratiomc.SetTextFont(42);
        pavtext_fitwithratiomc.Draw("SAME");
        pad_comparison.Modified();

        pad_ratio.cd();
        TH1F *mc_fit_ratio = new TH1F();
        mc_fit_ratio = (TH1F *)histogram_mc->Clone();
        if (mc_fit_ratio == NULL)
            return 0;
        TH1F *histogram_fitmodel_mc = new TH1F("fit_model_histmc", "fit_model_histmc", nbins, 6, 6.6);
        mc_signal_fitFunction.fillHistogram(histogram_fitmodel_mc, mass, fullsignal->numEntries() * mc_signal_fitFunction.getNorm());
        if (histogram_fitmodel_mc == NULL)
            return 0;
        mc_fit_ratio->Divide(mc_fit_ratio, histogram_fitmodel_mc);
        mc_fit_ratio->Draw("M");
        mc_fit_ratio->SetStats(false);
        mc_fit_ratio->GetYaxis()->SetNdivisions(5);
        mc_fit_ratio->GetXaxis()->SetTitle(TString::Format("%s[%s]", mass.GetName(), mass.getUnit()));
        mc_fit_ratio->GetXaxis()->SetTitleSize(0.1);
        mc_fit_ratio->GetXaxis()->SetTitleOffset(0.8);
        mc_fit_ratio->GetXaxis()->SetLabelSize(0.09);
        mc_fit_ratio->GetYaxis()->SetTitle("Obs/Exp");
        mc_fit_ratio->GetYaxis()->SetTitleSize(0.1);
        mc_fit_ratio->GetYaxis()->SetTitleOffset(0.45);
        mc_fit_ratio->GetYaxis()->SetLabelSize(0.09);
        pad_ratio.Modified();

        c1.SaveAs("sideband_fit_with_mc.pdf");
        c1.SaveAs("sideband_fit_with_mc.png");

        // create output file;
        TFile *output = TFile::Open("datacard.root", "recreate");

        RooDataSet data = RooDataSet(
            "data_obs",
            "data_obs",
            fulldata,
            RooArgSet(mass));

        // create workspace;
        cout << "creating workspace" << endl;
        RooWorkspace ws = RooWorkspace("w");
        cout << "creating workspace" << endl;

        // mass variable;
        ws.factory(TString::Format("%s[%f, %f]", mass_type, fit_range_lo, fit_range_hi));
        ws.import(data);
        ws.import(bkg_fit_function);
        ws.import(pi_plus_k_fit_function);
        ws.import(signal_fit_function);
        ws.import(fit_function);
        ws.import(frac_bkg);
        // in order to fix the ratio jpsi K / jpsi pi, loop over the variables and fix frac_k ;
        TIterator *varit_workspace = ws.allVars().createIterator();
        RooRealVar *var_in_workspace = new RooRealVar();

        while (var_in_workspace = (RooRealVar *)varit_workspace->Next())
        {
            TString varname = (TString)var_in_workspace->GetName();
            if (varname.CompareTo("mg") == 0) //, "jpsik_mean", "jpsik_sigma"]:;
            {
                var_in_workspace->setConstant();
            }
        }

        ws.writeToFile(output->GetName());
        output->Close();

        // SPlots
        // Now we use the SPlot class to add SWeights to our data set based on our model and our yield variables;
        // for extended pdf fit --> get the yields;
        RooRealVar nsig = RooRealVar("nsig", "nsig", 800, 0., 1000000.);
        RooRealVar nbkg = RooRealVar("nbkg", "nbkg", 7000, 0., 1000000.);

        RooAddPdf *fit_function_extended = (RooAddPdf *)RooAddPdf(
                                               "fit_function_extended",
                                               "fit_function_extended",
                                               RooArgList(bkg_fit_function, pi_plus_k_fit_function),
                                               RooArgList(nbkg, nsig))
                                               .Clone();
        RooFitResult *results_data_extended = fit_function_extended->fitTo(*fulldata, RooFit::Extended(true));
        RooStats::SPlot sData0 = RooStats::SPlot("sData", "An SPlot", *fulldata, fit_function_extended, RooArgList(nsig, nbkg));

        TTree *tree = fulldata->GetClonedTree();
        TTree *treemc = fullsignal->GetClonedTree();

        // Looping on variables that the user wants to plot and to be stored
        vector<string> vartobestored;
        vector<VarToBePlotted>::iterator plotvar = variable2becompared->begin();
        while (plotvar != variable2becompared->end())
        {
            // Defining a column with the required variable
            if (BtoJPsiPi_Data.HasColumn(plotvar->varname) == false)
                b2jpsipinode_data = std::make_unique<ROOT::RDF::RNode>(b2jpsipinode_data->Define(plotvar->varname, plotvar->varexpression).Filter(selection.Data()));
            vartobestored.push_back(plotvar->varname);
            plotvar++;
        }

        // Modifying write options of the root data frame to overwrite other trees
        ROOT::RDF::RSnapshotOptions snapopt;
        snapopt.fMode = "UPDATE";
        snapopt.fOverwriteIfExists = "TRUE";
        b2jpsipinode_data->Snapshot("BTo2MuP_Data", output_splots_name);
        tree->SetNameTitle("BTo2MuP_Data", "BTo2MuP_Data");
        TFile *outputsplotsfile = TFile::Open(output_splots_name, "update");
        tree->Write();
        // b2jpsipinode_data->Snapshot("BTo2MuP_Data", output_splots_name, vartobestored, snapopt);
        outputsplotsfile->Close();

        TFile *filewithws = TFile::Open("datacard.root");
        RooWorkspace *ws_splots = (RooWorkspace *)filewithws->Get("w")->Clone();
        filewithws->Close();
        ws_splots->import(sData0);
        ws_splots->import(nsig);
        ws_splots->import(nbkg);
        TFile *output_splots = TFile::Open("WorkspaceJPsiPi_WithSWeights", "recreate");
        output_splots->cd();
        ws_splots->writeToFile(output_splots->GetName(), false);
        output_splots->Close();

        std::cout << "Calculate sWeights" << std::endl;
        // get what we need out of the workspace to do the fit
        ws_splots->Print();
        RooAbsPdf *model_bkg_fit_function = ws_splots->pdf("bkg_fit_function");
        if (model_bkg_fit_function == NULL)
            return 0;
        RooAbsPdf *model_pi_plus_k_fit_function = ws_splots->pdf("pi_plus_k_fit_function");
        if (model_pi_plus_k_fit_function == NULL)
            return 0;
        RooRealVar *model_normalization = ws_splots->var("frac_bkg");
        if (model_normalization == NULL)
            return 0;
        RooRealVar *bvtx_fit_mass = ws_splots->var("bvtx_fit_mass");
        if (bvtx_fit_mass == NULL)
            return 0;
        bvtx_fit_mass->setMin(6);
        bvtx_fit_mass->setMax(6.5);
        RooAddPdf *model = (RooAddPdf *)RooAddPdf("model", "model", RooArgList(model_bkg_fit_function, model_pi_plus_k_fit_function), RooArgList(*model_normalization)).Clone();
        RooRealVar *nisg_splot = ws_splots->var("nsig");
        if (nisg_splot == NULL)
            return 0;
        RooRealVar *nbkg_splot = ws_splots->var("nbkg");
        if (nbkg_splot == NULL)
            return 0;
        RooDataSet *datasplot = (RooDataSet *)ws_splots->data("data_obs");
        if (datasplot == NULL)
        {
            cout << "miamamma_data" << endl;
            return 0;
        }
        // fit the model to the data.
        ws_splots->Print();
        model->fitTo(*datasplot);
        std::cout << "Calculate sWeights" << std::endl;
        RooMsgService::instance().setSilentMode(true);

        // Now we use the SPlot class to add SWeights to our data set
        // based on our model and our yield variables
        RooStats::SPlot *sData = new RooStats::SPlot("sData", "An SPlot", *datasplot, fit_function_extended, RooArgList(*nisg_splot, *nbkg_splot));
        std::cout << "Calculate sWeights" << std::endl;

        // Check that our weights have the desired properties
        std::cout << "Check SWeights:" << std::endl;
        std::cout << std::endl
                  << "Yield of signal is "
                  << nisg_splot->getVal() << ".  From sWeights it is "
                  << sData->GetYieldFromSWeight("nisg_splot") << std::endl;
        std::cout << "Yield of bkg is "
                  << nbkg_splot->getVal() << ".  From sWeights it is "
                  << sData->GetYieldFromSWeight("nbkg_splot") << std::endl
                  << std::endl;
        for (Int_t i = 0; i < 10; i++)
        {
            std::cout << "Signal Weight   " << sData->GetSWeight(i, "nisg_splot")
                      << "Bkg Weight   " << sData->GetSWeight(i, "nbkg_splot")
                      << "Total Weight   " << sData->GetSumOfEventSWeight(i)
                      << std::endl;
        }
        std::cout << std::endl;
        // import this new dataset with sWeights
        std::cout << "import new dataset with sWeights" << std::endl;
        ws_splots->import(*datasplot, Rename("dataWithSWeights"));
        TFile *filewithsplotweights = TFile::Open("sPlotWeights.root", "recreate");
        ws_splots->writeToFile(filewithsplotweights->GetName());
        filewithsplotweights->Close();

        // Here we make plots of the discriminating variable (invMass) after the fit
        // and of the control variable (b significance in the transverse plane) after unfolding with sPlot.
        std::cout << "make plots" << std::endl;
        // make our canvas
        TCanvas *cdata = new TCanvas("sPlot", "sPlot demo", 1366, 768);
        cdata->Divide(1, 3);

        fit_function_extended->fitTo(*fulldata, Extended());

        // plot invMass for data with full model and individual components overlaid
        //   TCanvas* cdata = new TCanvas();
        cdata->cd(1);
        RooPlot *framesplot = mass.frame();
        fulldata->plotOn(framesplot);
        fit_function_extended->plotOn(framesplot);
        fit_function_extended->plotOn(framesplot, Components(pi_plus_k_fit_function), LineStyle(kDashed), LineColor(kRed));
        fit_function_extended->plotOn(framesplot, Components(bkg_fit_function), LineStyle(kDashed), LineColor(kGreen));
        framesplot->SetTitle("Fit of model to discriminating variable");
        framesplot->Draw();

        // The SPlot class adds a new variable that has the name of the corresponding yield + "_sw".
        cdata->cd(2);
        // create weighted data set
        RooDataSet *dataweighted_sig = new RooDataSet(fulldata->GetName(), fulldata->GetTitle(), fulldata, *fulldata->get(), 0, "nsig_sw");
        RooRealVar *blovers = (RooRealVar *)myvars["blovers"].Clone();
        blovers->setRange(0, 15);
        RooPlot *framesplot2 = blovers->frame();
        dataweighted_sig->plotOn(framesplot2, DataError(RooAbsData::SumW2));
        framesplot2->SetTitle("#frac{L_{xy}}{#sigma_{xy}}(B_{c}^{+}) reweighted for signal weights");
        framesplot2->Draw();

        // The SPlot class adds a new variable that has the name of the corresponding yield + "_sw".
        cdata->cd(3);
        // create weighted data set
        RooDataSet *dataweighted_bkg = new RooDataSet(fulldata->GetName(), fulldata->GetTitle(), fulldata, *fulldata->get(), 0, "nbkg_sw");
        RooPlot *framesplot3 = blovers->frame();
        dataweighted_bkg->plotOn(framesplot3, DataError(RooAbsData::SumW2));
        framesplot3->SetTitle("#frac{L_{xy}}{#sigma_{xy}}(B_{c}^{+}) reweighted for bkg weights");
        framesplot3->Draw();
        cdata->SaveAs("SPlot.png");
    }

    vector<MarkedNames> fnames, tnames;
    {
        MarkedNames outputdata;
        outputdata.name = output_splots_name;
        outputdata.datatype = "Data";
        outputdata.treetype = "tree";
        fnames.push_back(outputdata);
    }
    {
        MarkedNames outputmc;
        outputmc.name = output_splots_name;
        outputmc.datatype = "MC";
        outputmc.treetype = "treemc";
        fnames.push_back(outputmc);
    }
    VarComparer(fnames, tnames, variable2becompared);
    return 0;
}