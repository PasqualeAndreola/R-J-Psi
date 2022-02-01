#include "HeaderFiles/NormalizationHybrid.h"

//It can be useful to use these namespaces
using namespace std;
using namespace RooFit;

int NormalizationHybrid(bool debug)
{
  if (debug == false)
  {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //      Variables and PDFs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    RooRealVar mass = RooRealVar("Bmass", "J/#Psi#pi^{#pm} mass", 6.0, 6.6, "GeV");
    RooRealVar mu1pt = RooRealVar("mu1pt", "mu1pt", 0., 1000., "GeV");
    RooRealVar mu2pt = RooRealVar("mu2pt", "mu2pt", 0., 1000., "GeV");
    RooRealVar mu1eta = RooRealVar("mu1eta", "mu1eta", -10., 10.);
    RooRealVar mu2eta = RooRealVar("mu2eta", "mu2eta", -10., 10.);
    RooRealVar bpt = RooRealVar("Bpt", "Bpt", 0., 9000.);
    RooRealVar lxy = RooRealVar("bvtx_lxy", "bvtx_lxy", 0., 9000.);
    RooRealVar lovers = RooRealVar("bvtx_lxy_sig", "bvtx_lxy_sig", 0., 9000.);
    RooRealVar svprob = RooRealVar("bvtx_svprob", "bvtx_svprob", 0., 1.);
    RooRealVar pipt = RooRealVar("kpt", "kpt", 0., 1000., "GeV");
    RooRealVar pieta = RooRealVar("keta", "keta", -10., 10.);
    RooRealVar cos = RooRealVar("bvtx_cos2D", "bvtx_cos2D", 0., 1.);
    RooRealVar mu1id = RooRealVar("mu1_mediumID", "mu1_mediumID", 0., 2.);
    RooRealVar mu2id = RooRealVar("mu2_mediumID", "mu2_mediumID", 0., 2.);
    RooRealVar mu1dxy = RooRealVar("mu1_dxy", "mu1_dxy", -5., 5., "cm");
    RooRealVar mu2dxy = RooRealVar("mu2_dxy", "mu2_dxy", -5., 5., "cm");
    RooRealVar pidxy = RooRealVar("k_dxy", "k_dxy", -5., 5., "cm");
    RooRealVar mu1dz = RooRealVar("mu1_dz", "mu1_dz", -25., 25., "cm");
    RooRealVar mu2dz = RooRealVar("mu2_dz", "mu2_dz", -25., 25., "cm");
    RooRealVar pidz = RooRealVar("k_dz", "k_dz", -25., 25., "cm");

    // only MC
    RooRealVar k_genpdgId = RooRealVar("k_genpdgId", "k_genpdgId", -1e6, 1e6);
    RooRealVar k_mother_pdgId = RooRealVar("k_mother_pdgId", "k_mother_pdgId", -1e6, 1e6);
    RooRealVar mu1_genpdgId = RooRealVar("mu1_genpdgId", "mu1_genpdgId", -1e6, 1e6);
    RooRealVar mu1_mother_pdgId = RooRealVar("mu1_mother_pdgId", "mu1_mother_pdgId", -1e6, 1e6);
    RooRealVar mu1_grandmother_pdgId = RooRealVar("mu1_grandmother_pdgId", "mu1_grandmother_pdgId", -1e6, 1e6);
    RooRealVar mu2_genpdgId = RooRealVar("mu2_genpdgId", "mu2_genpdgId", -1e6, 1e6);
    RooRealVar mu2_mother_pdgId = RooRealVar("mu2_mother_pdgId", "mu2_mother_pdgId", -1e6, 1e6);
    RooRealVar mu2_grandmother_pdgId = RooRealVar("mu2_grandmother_pdgId", "mu2_grandmother_pdgId", -1e6, 1e6);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //      mass ranges
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Double_t fit_range_lo = 5.5;
    Double_t mass_window_lo = 6.275 - 0.15;
    Double_t mass_window_hi = 6.275 + 0.15;
    Double_t fit_range_hi = 7.;

    mass.setRange("left", fit_range_lo, mass_window_lo);
    mass.setRange("right", mass_window_hi, fit_range_hi);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //      PDFs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //    1  argpar      -1.95759e+00   1.03831e+01   2.11846e-03  -1.97032e-01
    //    2  broad_width   5.62194e-02   5.57457e-03   7.32458e-05  -1.09202e+00
    //    3  frac_bkg     4.20044e-01   7.16860e-02   1.56468e-04  -1.60601e-01
    //    4  frac_pi      6.31013e-01   6.77992e-02   2.64847e-04   2.65121e-01
    //    5  frac_sig     2.67041e-01   2.28339e-01   5.99349e-04  -4.84672e-01
    //    6  maxM         6.20639e+00   2.25169e-01   8.23578e-04   7.09100e-01
    //    7  narrow_mean         6.26774e+00   8.02151e-03   7.24866e-05   1.18543e-01
    //    8  narrow_width   2.44845e-02   4.83913e-03   3.78671e-04  -5.35545e-01
    //    9  p1          -5.23507e-02   1.16627e-01   4.07071e-06  -5.23507e-04
    //   10  sg           1.14919e-02   1.00958e-02   1.07686e-03   2.99617e+00

    // combinatorial background poly
    RooRealVar pol_c1 = RooRealVar("pol_c1", "coefficient of x^0 term", -5.23507e-02, -100, 100);
    // pol_c2 = RooRealVar("pol_c2", "coefficient of x^1 term", 0.6, -10, 10)
    // pol_c3 = RooRealVar("pol_c3", "coefficient of x^2 term", 0.5, -10, 10)
    RooChebychev bkg = RooChebychev("bkg_pol", "1st order poly", mass, RooArgList(pol_c1));
    // bkg = RooChebychev("bkg_pol", "2nd order poly", mass, RooArgList(pol_c1, pol_c2))
    // bkg = RooChebychev("bkg_pol", "3rd order poly", mass, RooArgList(pol_c1, pol_c2, pol_c3))

    // expo
    // slope = RooRealVar("slope", "slope", -0.001, -1e6, 1e6)
    // bkg   = RooExponential("bkg_expo", "bkg_expo", mass, slope)

    // argus function, partially reconstructed decays
    RooRealVar argpar = RooRealVar("argpar", "argus shape parameter", -1.95759e+00, -20, 20);
    RooRealVar maxM = RooRealVar("maxM", "argus max m", 6.20639e+00, 6.0, 6.275); //6.2)
    RooArgusBG argus = RooArgusBG("argus", "Argus PDF", mass, maxM, argpar);

    // detector response function
    RooRealVar mg = RooRealVar("mg", "mg", 0);                         //, -0.01, 0.01)
    RooRealVar sg = RooRealVar("sg", "sg", 1.14919e-02, 0.0001, 0.03); //, 0.001,0.2)
    RooGaussian resGauss = RooGaussian("resGauss", "resGauss", mass, mg, sg);
    // construct convolution
    mass.setBins(10000, "fft");
    RooFFTConvPdf lxg = RooFFTConvPdf("lxg", "argus (X) gauss", mass, argus, resGauss);

    // Bc->Jpsi K crystal ball
    RooRealVar jpsik_mean = RooRealVar("jpsik_mean", "narrow_mean", 6.17, 6.10, 6.25);
    RooRealVar jpsik_sigma = RooRealVar("jpsik_sigma", "sigma", 0.03, 0.01, 0.1);
    RooGaussian jpsik_func = RooGaussian("jpsik_func", "jpsik_func", mass, jpsik_mean, jpsik_sigma);
    // jpsik_n     = RooRealVar("jpsik_n"    , "jpsik_n"    , 0.1 , 0.01,   3.  )
    // jpsik_alpha = RooRealVar("jpsik_alpha", "jpsik_alpha", 2   ,  0.1,   4.  )
    // jpsik_func = RooCBShape("jpsik_func", "jpsik_func", mass, jpsik_mean, jpsik_sigma, jpsik_alpha, jpsik_n)

    // signal narrow gaussian
    RooRealVar narrow_mean = RooRealVar("narrow_mean", "narrow_mean", 6.26774e+00, 6.1, 6.4);
    RooRealVar narrow_width = RooRealVar("narrow_width", "narrow_width", 2.44845e-02, 0., 0.1);
    RooGaussian narrow_gaus = RooGaussian("sig_narrow_gaus", "sig_narrow_gaus", mass, narrow_mean, narrow_width);

    // signal broad gaussian
    RooRealVar broad_mean = RooRealVar("broad_mean", "broad_mean", 6.27e+00, 6.1, 6.4);
    RooRealVar broad_width = RooRealVar("broad_width", "broad_width", 5.62194e-02, 0., 1.);
    RooGaussian broad_gaus = RooGaussian("sig_broad_gaus", "sig_broad_gaus", mass, broad_mean, broad_width);

    // absolute yields
    RooRealVar nsig = RooRealVar("signal_yield", "signal_yield", 800, 0., 10000.);
    RooRealVar nsig_narrow = RooRealVar("signal_yield_narrow", "signal_yield_narrow", 700, 0., 10000.);
    RooRealVar nsig_broad = RooRealVar("signal_yield_broad", "signal_yield_broad", 100, 0., 10000.);
    RooRealVar nbkgtot = RooRealVar("nbkgtot", "nbkgtot", 2000, 0., 10000.);
    RooRealVar nbkg = RooRealVar("nbkg", "nbkg", 7000, 0., 10000.);
    RooRealVar nPi = RooRealVar("nPi", "nPi", 1000, 0., 10000.);
    RooRealVar nK = RooRealVar("nK", "nK", 200, 0., 10000.);

    // fractional yields
    // you need these and not absolute yields in combine
    // don"t fit with Extended!
    RooRealVar frac_sig = RooRealVar("frac_sig", "frac_sig", 2.67041e-01, 0., 1.);
    RooRealVar frac_pi = RooRealVar("frac_pi", "frac_pi", 6.31013e-01, 0., 1.);
    RooRealVar frac_bkg = RooRealVar("frac_bkg", "frac_bkg", 4.20044e-01, 0., 1.);
    // fixed to PDG (Jpsi K) / (Jpsi pi) value https://pdglive.lbl.gov/BranchingRatio.action?desig=14&parCode=S091
    Double_t frac_k_value = 0.079 / (1. + 0.079);
    RooRealVar frac_k = RooRealVar("frac_k", "frac_k", frac_k_value);

    // signal function
    RooAddPdf signal_fit_function = RooAddPdf(
        "signal_fit_function",
        "signal_fit_function",
        RooArgList(narrow_gaus, broad_gaus),
        RooArgList(frac_sig));

    // signal Jpsi pi plus Jpsi K
    // RooAddPdf::pi_plus_k_fit_function[ frac_k * jpsik_func + [%] * signal_fit_function ]
    RooAddPdf pi_plus_k_fit_function = RooAddPdf(
        "pi_plus_k_fit_function",
        "pi_plus_k_fit_function",
        RooArgList(jpsik_func, signal_fit_function), // order matters for coefficients in next line https://www.nikhef.nl/~vcroft/SignalAndBackground-CompositeModels.html
        RooArgList(frac_k));

    // background function
    RooAddPdf bkg_fit_function = RooAddPdf(
        "bkg_fit_function",
        "bkg_fit_function",
        //     RooArgList(bkg, lxg, jpsik_func),
        //     RooArgList(frac_pi, frac_k)
        RooArgList(lxg, bkg),
        RooArgList(frac_pi));

    // total function
    RooAddPdf fit_function = RooAddPdf(
        "fit_function",
        "fit_function",
        RooArgList(bkg_fit_function, pi_plus_k_fit_function),
        RooArgList(frac_bkg));

    // MC signal narrow gaussian
    RooRealVar mc_narrow_mean = RooRealVar("mc_narrow_mean", "mc_narrow_mean", 6.275, 5.5, 7.);
    RooRealVar mc_narrow_width = RooRealVar("mc_narrow_width", "mc_narrow_width", 0.038, 0., 1.);
    RooGaussian mc_narrow_gaus = RooGaussian("mc_sig_narrow_gaus", "mc_sig_narrow_gaus", mass, mc_narrow_mean, mc_narrow_width);

    // MC signal broad gaussian
    RooRealVar mc_broad_mean = RooRealVar("mc_broad_mean", "mc_broad_mean", 6.275, 5.5, 7.);
    RooRealVar mc_broad_width = RooRealVar("mc_broad_width", "mc_broad_width", 0.06, 0., 1.);
    RooGaussian mc_broad_gaus = RooGaussian("mc_sig_broad_gaus", "mc_sig_broad_gaus", mass, mc_broad_mean, mc_broad_width);

    RooRealVar mc_nsig = RooRealVar("mc_signal_yield", "mc_signal_yield", 800, 0, 100000);
    RooRealVar mc_nsig_narrow = RooRealVar("mc_signal_yield_narrow", "mc_signal_yield_narrow", 700, 0, 100000);
    RooRealVar mc_nsig_broad = RooRealVar("mc_signal_yield_broad", "mc_signal_yield_broad", 100, 0, 100000);

    // MC signal function
    RooAddPdf mc_signal_fitFunction = RooAddPdf(
        "mc_signal_fit_function",
        "mc_signal_fit_function",
        RooArgList(mc_narrow_gaus, mc_broad_gaus),
        RooArgList(mc_nsig_narrow, mc_nsig_broad));

    RooArgSet thevars = RooArgSet();
    thevars.add(mass);
    thevars.add(mu1pt);
    thevars.add(mu2pt);
    thevars.add(mu1eta);
    thevars.add(mu2eta);
    thevars.add(bpt);
    thevars.add(lovers);
    thevars.add(lxy);
    thevars.add(svprob);
    thevars.add(pipt);
    thevars.add(pieta);
    thevars.add(cos);
    thevars.add(mu1id);
    thevars.add(mu2id);
    thevars.add(mu1dxy);
    thevars.add(mu2dxy);
    thevars.add(pidxy);
    thevars.add(mu1dz);
    thevars.add(mu2dz);
    thevars.add(pidz);

    RooArgSet thevars_mc = thevars;
    thevars_mc.add(k_genpdgId);
    thevars_mc.add(k_mother_pdgId);
    thevars_mc.add(mu1_genpdgId);
    thevars_mc.add(mu1_mother_pdgId);
    thevars_mc.add(mu1_grandmother_pdgId);
    thevars_mc.add(mu2_genpdgId);
    thevars_mc.add(mu2_mother_pdgId);
    thevars_mc.add(mu2_grandmother_pdgId);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // selection on data, plotting, fitting
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // HLT_DoubleMu4_JpsiTrk_Displaced_v14
    const char *selection = "Bmass > 6 & "
                            "Bmass < 6.6 & "
                            // "mu1pt>4.5 & "
                            // "mu2pt>4.5 & "
                            "mu1pt>3 & "
                            "mu2pt>3 & "
                            "abs(mu1eta)<2.4 & "
                            "abs(mu2eta)<2.4 & "
                            //"mu1pt>3.5 & "
                            //"mu2pt>3.5 & "
                            "Bpt>15 & "
                            "bvtx_lxy > 0.01 & " // 100 micron
                            //"bvtx_lxy_sig>3 & "            // da cambiare, significance rotta perché mc sottostima gli errori
                            //"Bsvprob>0.005 & "
                            "bvtx_svprob>0.001 & "
                            "kpt>3 & "
                            "abs(keta)<2.4 & "
                            "bvtx_cos2D>0.999 & "
                            "mu1_mediumID>0.5 & "
                            "mu2_mediumID>0.5 & "
                            "abs(mu1_dz-mu2_dz)<0.4 & "
                            "abs(mu1_dz-k_dz)<0.4 & "
                            "abs(mu2_dz-k_dz)<0.4 & "
                            "abs(k_dxy)<0.2 & "
                            "abs(mu1_dxy)<0.2 & "
                            "abs(mu2_dxy)<0.2  ";

    // add gen matching
    const char *selection_mc = "abs(k_genpdgId)==211 & "
                               "abs(k_mother_pdgId)==541 & "
                               "abs(mu1_genpdgId)==13 & "
                               "abs(mu1_mother_pdgId)==443 & "
                               "abs(mu2_genpdgId)==13 & "
                               "abs(mu2_mother_pdgId)==443 & "
                               "abs(mu1_grandmother_pdgId)==541 & "
                               "abs(mu2_grandmother_pdgId)==541 & "
                               "Bmass > 6 & "
                               "Bmass < 6.6 & "
                               // "mu1pt>4.5 & "
                               // "mu2pt>4.5 & "
                               "mu1pt>3 & "
                               "mu2pt>3 & "
                               "abs(mu1eta)<2.4 & "
                               "abs(mu2eta)<2.4 & "
                               //"mu1pt>3.5 & "
                               //"mu2pt>3.5 & "
                               "Bpt>15 & "
                               "bvtx_lxy > 0.005 & " // 100 micron
                               //"bvtx_lxy_sig>3 & "            // da cambiare, significance rotta perché mc sottostima gli errori
                               //"Bsvprob>0.005 & "
                               "bvtx_svprob>0.001 & "
                               "kpt>3 & "
                               "abs(keta)<2.4 & "
                               "bvtx_cos2D>0.999 & "
                               "mu1_mediumID>0.5 & "
                               "mu2_mediumID>0.5 & "
                               "abs(mu1_dz-mu2_dz)<0.4 & "
                               "abs(mu1_dz-k_dz)<0.4 & "
                               "abs(mu2_dz-k_dz)<0.4 & "
                               "abs(k_dxy)<0.2 & "
                               "abs(mu1_dxy)<0.2 & "
                               "abs(mu2_dxy)<0.2  ";

    const char *data_files = "InputFiles/data_ptmax_merged.root";

    TTree *data_tree = (TTree *)(TFile::Open(data_files)->Get("BTo2MuP"));
    //data_tree.AddFile(data_files);

    const char *signal_files = "InputFiles/BcToJPsiMuMu_is_jpsi_pi_merged.root";

    TTree *signal_tree = (TTree *)(TFile::Open(signal_files)->Get("BTo2MuP"));
    //signal_tree.AddFile(signal_files);

    RooDataSet fulldata = RooDataSet("data", "data", data_tree->GetTree(), thevars, selection);
    RooDataSet fullsignal = RooDataSet("signal", "signal", signal_tree->GetTree(), thevars_mc, selection_mc);

    // plot
    TCanvas c1 = TCanvas("c1", "", 1360, 768);
    c1.Draw();
    RooPlot *frame = mass.frame();
    frame->SetTitle("");
    Int_t nbins = 80;
    fulldata.plotOn(frame, Name("Data"), RooFit::Binning(nbins), RooFit::MarkerSize(1.5));

    // fit
    // results_data = fit_function.fitTo(fulldata, RooFit.Extended(True), RooFit.Save())
    RooFitResult *results_data = fit_function.fitTo(fulldata, RooFit::Save());

    fit_function.plotOn(frame);
    Double_t chi2_datafit = frame->chiSquare();
    fit_function.plotOn(frame, Name("bkg_pol"), RooFit::Components("bkg_pol"), RooFit::LineStyle(kDashed), RooFit::LineColor(kBlue));
    fit_function.plotOn(frame, Name("lxg"), RooFit::Components("lxg"), RooFit::LineStyle(kDashed), RooFit::LineColor(kOrange));
    fit_function.plotOn(frame, Name("signal_fit_function"), RooFit::Components("signal_fit_function"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
    fit_function.plotOn(frame, Name("jpsik_func"), RooFit::Components("jpsik_func"), RooFit::LineStyle(kDashed), RooFit::LineColor(kGreen));

    frame->Draw();
    //CMS_lumi(c1, 4, 0, cmsText = "CMS", extraText = "   Preliminary", lumi_13TeV = "60 fb^{-1}");

    TLegend leg = TLegend(0.58, .65, .90, .90);
    leg.SetBorderSize(0);
    leg.SetFillColor(0);
    leg.SetFillStyle(0);
    leg.SetTextFont(42);
    leg.SetTextSize(0.035);
    // leg.SetNColumns(3);

    // RooFit
    leg.AddEntry("bkg_pol", "Combinatorial bkg");
    leg.AddEntry("lxg", "B_{c}#rightarrowJ/#Psi#pi + X", "L");
    leg.AddEntry("jpsik_func", "B_{c}#rightarrowJ/#PsiK", "L");
    leg.AddEntry("signal_fit_function", "B_{c}#rightarrowJ/#Psi#pi", "L");
    leg.AddEntry("Data", "Observed", "EP");
    leg.Draw("SAME");

    // gPad.SaveAs("sideband_fit.pdf")
    c1.SaveAs("bchybrid_fit.png");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Fit the mc
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    fullsignal.plotOn(frame, RooFit::Binning(nbins), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
    RooFitResult *results_mc = mc_signal_fitFunction.fitTo(fullsignal, RooFit::Extended(kTRUE), RooFit::Save());
    mc_signal_fitFunction.plotOn(frame, Name("mc_signal_fit_function_Norm"), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
    frame->Draw();
    //CMS_lumi(c1, 4, 0, cmsText = "CMS", extraText = "   Preliminary", lumi_13TeV = "60 fb^{-1}");
    leg.AddEntry("mc_signal_fit_function_Norm", "B_{c}#rightarrowJ/#Psi#pi MC", "LP");

    /*TF1 *data_signal_fit_function = new TF1();
  TF1 *mc_signal_fit_function = new TF1();
  data_signal_fit_function = fit_function.asTF(mass, RooArgList(frac_bkg), mass);
  mc_signal_fit_function = mc_signal_fitFunction.asTF(mass, RooArgList(mc_nsig_narrow, mc_nsig_broad), mass);
  Double_t data_signal_integral, mc_signal_integral;
  leg.AddEntry((TObject*)0, TString::Format("Data/MC(NarrowPeak ratio): %.2f", data_signal_fit_function->Eval(narrow_mean.getVal())/mc_signal_fit_function->Eval(mc_narrow_mean.getVal())), "");
  leg.AddEntry((TObject*)0, TString::Format("Data/MC(BroadPeak ratio): %.2f", data_signal_fit_function->Eval(broad_mean.getVal())/mc_signal_fit_function->Eval(mc_broad_mean.getVal())), "");*/
    leg.AddEntry((TObject *)0, TString::Format("Data over MC: %.2f", (fulldata.numEntries() * (1 - frac_bkg.getVal())) / (mc_nsig_narrow.getVal() + mc_nsig_broad.getVal())), "");

    leg.Draw("SAME");
    c1.SaveAs("bchybrid_fit_with_mc.png");

    RooArgSet *params = fit_function.getParameters(mass);
    params->writeToStream(cout, false);
    RooArgSet *mcparams = mc_signal_fitFunction.getParameters(mass);
    mcparams->writeToStream(cout, false);

    // Create a new empty workspace
    RooWorkspace *bchybridworkspace = new RooWorkspace("bchybridworkspace", "bchybridworkspace");

    // Import model and all its components into the workspace
    bchybridworkspace->import(fit_function);

    // Import mc_model and all its components into the workspace
    bchybridworkspace->import(mc_signal_fitFunction);

    // Import data into the workspace
    bchybridworkspace->import(fulldata);

    // Print workspace contents
    bchybridworkspace->Print();

    // S a v e   w o r k s p a c e   i n   f i l e
    // -------------------------------------------

    // Save the workspace into a ROOT file
    bchybridworkspace->writeToFile("bchybridworkspace.root");

    cout << "Fit to data integral " << fulldata.numEntries() * (1 - frac_bkg.getVal()) << endl;
    cout << "Fit to data integral " << frac_sig.getVal() << endl;
    cout << "Fit to mc integral " << (1. + 0.079) * (mc_nsig_narrow.getVal() + mc_nsig_broad.getVal()) << endl;

    c1.cd();
    c1.Clear();
    RooPlot *frame_mc = mass.frame();
    frame_mc->SetTitle("");
    fullsignal.plotOn(frame_mc, RooFit::Binning(nbins), RooFit::LineColor(kBlack), RooFit::MarkerColor(kBlack));
    mc_signal_fitFunction.plotOn(frame_mc, Name("mc_signal_fit_function_Norm"), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
    Double_t chi2_mcfit = frame_mc->chiSquare();
    frame_mc->Draw();
    TLegend leg2 = TLegend(0.58, .65, .90, .90);
    leg2.SetBorderSize(0);
    leg2.SetFillColor(0);
    leg2.SetFillStyle(0);
    leg2.SetTextFont(42);
    leg2.SetTextSize(0.035);
    leg2.AddEntry(fullsignal.GetName(), "MC data", "P");
    leg2.AddEntry("mc_signal_fit_function_Norm", "B_{c}#rightarrowJ/#Psi#pi MC fit", "L");
    leg2.Draw("SAME");
    c1.SaveAs("bchybrid_fit_only_mc.png");
    leg2.Clear();
    
    cout << "Chi2 of the data fit: " <<  chi2_datafit << endl;
    cout << "Chi2 of the MC fit: " <<  chi2_mcfit << endl;
  }

  return 0;
}