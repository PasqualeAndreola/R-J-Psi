/*! 
 * \mainpage R-J-Psi
 *
 * \section intro_sec Introduction
 *
 * This project has been developed as a Master Thesis project used to search Lepton flavour
 * universality violation in CMS.
 */

/*! 
 * \file main.cpp
 * \brief Main file
 */

#include "HeaderFiles/Discriminants.h"
#include "HeaderFiles/DiscriminantsPlotter.h"
#include "HeaderFiles/DiscToBePlotted.h"
#include "HeaderFiles/EfficienciesPlotter.h"
#include "HeaderFiles/EffToBePlotted.h"
#include "HeaderFiles/FakeNNClosureTest.h"
#include "HeaderFiles/FakeNNFactory.h"
#include "HeaderFiles/FakeNNReader.h"
#include "HeaderFiles/JPsiReweighting.h"
#include "HeaderFiles/MarkedNames.h"
#include "HeaderFiles/NormalizationHybrid.h"
#include "HeaderFiles/NormalizationJPsi_X.h"
#include "HeaderFiles/NormalizationJPsi_Mu.h"
#include "HeaderFiles/PrintFuncInfo.h"
#include "HeaderFiles/TMVAMethod.h"
#include "HeaderFiles/TreeRDFDiscriminants.h"
#include "HeaderFiles/TreeReader.h"
#include "HeaderFiles/VarComparer.h"
#include "HeaderFiles/VarFitter.h"
#include "HeaderFiles/VarToBeFitted.h"
#include "HeaderFiles/VarToBePlotted.h"
#include <filesystem>
#include <ostream>
#include <RooFit.h>
#include <RooWorkspace.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooAbsPdf.h>
#include <RooPlot.h>
#include <RooGaussian.h>
#include <RooDataSet.h>
#include <RooChi2Var.h>
#include <RooChebychev.h>
#include <RooArgusBG.h>
#include <RooFFTConvPdf.h>
#include <RooAddPdf.h>
#include <TChain.h>
#include <TArrow.h>
#include <TPad.h>
#include <TF1.h>
#include <TMVA/Config.h>
#include <TMVA/Configurable.h>
#include <TMVA/DataLoader.h>
#include <TMVA/Factory.h>
#include "TMVA/Reader.h"
#include <TMVA/TMVAGui.h>
#include <TMVA/RReader.hxx>
#include "Blue.h"
#include <TMatrixD.h>

//It can be useful to use these namespaces
using namespace std;
using namespace RooFit;

int main(int argc, char *argv[])
{
  gErrorIgnoreLevel = kWarning;
  int colors_palette[3];
  colors_palette[0] = kBlue;
  colors_palette[1] = kGreen + 1;
  colors_palette[2] = kRed;
  gStyle->SetPalette(3, colors_palette);
  bool debug_bchybridnorm = true;
  bool debug_jpsixnorm = true;
  bool debug_jpsimunorm = true;
  bool debug_inclusive = true;
  bool debug_fit = false;
  string debug_fitclosuretest = "DATA & CT";
  bool debug_peak = true;
  bool debug_nnfakes = true;

  cout << endl
       << "Program compiled correctly." << endl
       << "Starting now..." << endl
       << endl; 

/*
// S t a r t p r e p a r i n g t h e i n p u t s f o r l a t e r use i n BLUE
// Numbers o f e s t i m a t e s , u n c e r t a i n t i e s and o b s e r v a b l e 
  static const Int_t NumEst = 2;
  static const Int_t NumUnc = 3;
  static const Int_t NumObs = 1;

// Array of names of estimates (i,j=0,1)
  static const TString NamEst[NumEst] = {"This Work", "LHCb"};

// Array of names of uncertainties
  static const TString NamUnc[NumUnc] = {"Stat", "Syst(Correlated)", "Syst(UnCorrelated)"};

// Array of names of observanles
  static const TString NamObs[NumObs] = {"R(J/#psi)"};

// Array of estimates and uncertainties 
  static const Int_t LenXEst = NumEst * (NumUnc+1);
  static const Double_t XEst[LenXEst] = 
  {0.72, 0.12, 0.125, 0.13, 0.71, 0.17, 0.125, 0.13};

  TMatrixD* InpEst = new TMatrixD(NumEst, NumUnc+1, &XEst[0]);

// Array of statistical precision in systematic uncertainties, copy to matrix
  static const Int_t LenSUnc = NumEst * NumUnc;
  static const Double_t SUnc[LenSUnc]={
  //StatSys1Sys2Sys3Sys4Sys5
  0., 0., 0.,
  0., 0., 0.};
  TMatrixD* InpSta = new TMatrixD(NumEst,NumUnc,&SUnc[0]);

  //Arrayofrho_ijk==constcorrelationsfork<5
  static const Double_t RhoVal[NumUnc]=
  {0.0, 0.0, 0.0};

  //Array for correlation matrix for k == 5 with rho_ij5 != const
  static const Int_t LenInd=NumEst*(NumEst-1)/2;
  static const Double_t RhoInd[LenInd]={1};

  //FormatsandfilenameforfiguresandLatexfile
  static const TString ForVal="%5.2f";
  static const TString ForUnc="%4.2f";
  static const TString ForWei=ForUnc;
  static const TString ForRho="%+4.2f";
  static const TString ForPul=ForUnc;
  static const TString ForChi=ForUnc;
  static const TString ForUni="";
  static const TString FilBas="RJPsi";

  //Axis ranges for Figs.3ab and Fig.6ab
  static const Double_t XvaMax=0.34,YvaMin=-0.05,YvaMax=0.85;
  static const Double_t ValMin=0.7,ValMax=0.73;
  static const Double_t UncMin=0.05,UncMax=0.30;
  //--EndofpreparationofinputsforBLUE

  //--StartusingBLUE---noinputsbelowthisline---
  //--Keywords:relatetotheflowchartofthesoftware
  //Create:callconstructorofBLUEobject
  Blue* myBlue = new Blue(NumEst,NumUnc);

  //Fill:setdisplayformatsanddefinenames
  myBlue->SetFormat(ForVal,ForUnc,ForWei,ForRho,ForPul,ForChi,ForUni);
  myBlue->FillNamEst(&NamEst[0]);
  myBlue->FillNamUnc(&NamUnc[0]);
  myBlue->FillNamObs(&NamObs[0]);

  //Fill:insertestimateswiththeiruncertainties
  myBlue->FillEst(InpEst);

  //Fill:insertstatisticalprecisioninsystematicuncertainties
  myBlue->FillSta(InpSta);

  //Fill:inserttheestimatorcorrelationsforallsourcesofuncertainty
  for(Int_t k=0; k<NumUnc; k++)
  {
    if(k != (NumUnc-1)) {myBlue->FillCor(k,RhoVal[k]);}
    else {myBlue->FillCor(-k,&RhoInd[0]);}
  }
  //myBlue->FillCor({1,0,0, 0,1,0, 0,0,1});

  //Change:
  //...alterationswouldgohere

  //Fix:
  myBlue->FixInp();

  //Combine:
  myBlue->Solve();

  //Digest:showsomeestimatequantities,preparecodeforFigs.3and4
  myBlue->PrintEst();
  myBlue->CorrelPair(0,1,FilBas,-XvaMax,XvaMax,YvaMin,YvaMax);
  myBlue->DisplayPair(0,1,FilBas);

  //Digest:showsomeobservablequantities,preparecodeforFig.5a
  myBlue->PrintChiPro();
  myBlue->DisplayResult(0,FilBas);

  //Digest:writeLatexfile
  myBlue->LatexResult(FilBas);

  //Free:,Fix:,Combine:solveaccordingtoimportance
  myBlue->ReleaseInp();
  myBlue->FixInp();
  myBlue->SolveAccImp(0, 1);

  //Digest:preparecodeforFig.5b
  myBlue->DisplayAccImp(0,FilBas);

  //Free:,Fix:,Combine:solvevaryingthesystematicuncertainties
  myBlue->ReleaseInp();
  myBlue->FixInp();
  myBlue->SolveScaSta(1);

  //Digest:produceFig.6
  myBlue->PrintScaSta(FilBas,ValMin,ValMax,UncMin,UncMax);

  //Delete:deleteobjectaswellaslocalmatricesandreturn
  delete myBlue; myBlue=NULL;
  InpEst->Delete();InpEst=NULL;
  InpSta->Delete();InpSta=NULL;
*/
  // Input quantities
  // Input files, relative flags (1 for real data, 0 for MC) and a short name (not necessary)
  vector<MarkedNames> filenames;
  filenames.push_back(MarkedNames("output_sig_mix.root", 0, "", "JpsiLeptons_regression"));
  filenames.push_back(MarkedNames("output_bkg.root", 0, "", "Background_regression"));
  //filenames.push_back(MarkedNames("data_ptmax_merged.root", 1, "", "Data"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_merged.root", 0, "", "JpsiMuNu"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_tau_merged.root", 0, "", "JpsiTauNu"));
  /*filenames.push_back(MarkedNames("BcToJPsiMuMu_is_chic0_mu_merged.root", 0, "", "Chic0"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_chic1_mu_merged.root", 0, "", "Chic1"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_chic2_mu_merged.root", 0, "", "Chic2"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_hc_merged.root", 0, "", "JPsiHC"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_hc_mu_merged.root", 0, "", "hcMu");
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_psi2s_tau_merged.root", 0, "", "Psi2STau"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_psi2s_mu_merged.root", 0, "" "Psi2SMu"));
  filenames.push_back(MarkedNames("HbToJPsiMuMu_ptmax_merged.root", 0, "", "HbJPsiX"));
  filenames.push_back(MarkedNames("HbToJPsiMuMu3MuFilter_ptmax_merged.root", 0, "", "HbJPsiMu"));*/
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint.root", 0, "", "JpsiMuNu_Jpsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_tau_massConstraint.root", 0, "", "JpsiTauNu_JPsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_NONmassConstraint_20bits_precision.root", 0, "", "JpsiMuNu_HighPrecision"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint_12bits_precision.root", 1, "", "JPsiMuNu_12bit_JPsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint_14bits_precision.root", 1, "", "JPsiMuNu_14bit_JPsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint_16bits_precision.root", 1, "", "JPsiMuNu_16bit_JPsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint_18bits_precision.root", 1, "", "JPsiMuNu_18bit_JPsiconstraint"));
  //filenames.push_back(MarkedNames("data_UL_5021Jan29.root", 1, "Data"));

  // Input trees to be processed (1 for real, 0 for MC)
  vector<MarkedNames> treenames;
  treenames.push_back(MarkedNames("BTo3Mu", 1));

  // Input branches to be read (1 for reco, 0 for gene)
  // Please write in a single constructor if you want a direct comparison between reco and gene
  vector<MarkedNames> *branchnames = new vector<MarkedNames>;
  branchnames->push_back(MarkedNames("Bphi", 0));
  branchnames->push_back(MarkedNames("mu1pt", 1, "mu1_gen_pt", 0, "GeV"));
  branchnames->push_back(MarkedNames("mu1eta", 1, "mu1_gen_eta", 0));
  branchnames->push_back(MarkedNames("mu1phi", 1, "mu1_gen_phi", 0));
  branchnames->push_back(MarkedNames("mu2pt", 1, "mu2_gen_pt", 0, "GeV"));
  branchnames->push_back(MarkedNames("mu2eta", 1, "mu2_gen_eta", 0));
  branchnames->push_back(MarkedNames("mu2phi", 1, "mu2_gen_phi", 0));
  branchnames->push_back(MarkedNames("kpt", 1, "k_gen_pt", 0, "GeV"));
  branchnames->push_back(MarkedNames("keta", 1, "k_gen_eta", 0));
  branchnames->push_back(MarkedNames("kphi", 1, "k_gen_phi", 0));
  branchnames->push_back(MarkedNames("mu1_grandmother_pt", 0));
  branchnames->push_back(MarkedNames("mu1_grandmother_eta", 0));
  branchnames->push_back(MarkedNames("mu1_grandmother_phi", 0));
  branchnames->push_back(MarkedNames("mu1_grandmother_vx", 0));
  branchnames->push_back(MarkedNames("mu1_grandmother_vy", 0));
  branchnames->push_back(MarkedNames("mu1_grandmother_vz", 0));
  branchnames->push_back(MarkedNames("mu1_mother_vx", 0));
  branchnames->push_back(MarkedNames("mu1_mother_vy", 0));
  branchnames->push_back(MarkedNames("mu1_mother_vz", 0));
  branchnames->push_back(MarkedNames("mu1_mother_eta", 0));
  branchnames->push_back(MarkedNames("is_signal_channel", 0));

  /*ooooooooo.   ooooooooo.   oooooooooooo       .oooooo..o oooooooooooo ooooo               .oooooo.   ooooo     ooo ooooooooooooo 
`888   `Y88. `888   `Y88. `888'     `8      d8P'    `Y8 `888'     `8 `888'              d8P'  `Y8b  `888'     `8' 8'   888   `8 
 888   .d88'  888   .d88'  888              Y88bo.       888          888              888           888       8       888      
 888ooo88P'   888ooo88P'   888oooo8          `"Y8888o.   888oooo8     888              888           888       8       888      
 888          888`88b.     888    "              `"Y88b  888    "     888              888           888       8       888      
 888          888  `88b.   888       o      oo     .d8P  888       o  888       o      `88b    ooo   `88.    .8'       888      
o888o        o888o  o888o o888ooooood8      8""88888P'  o888ooooood8 o888ooooood8       `Y8bood8P'     `YbodP'        o888o     */

  TString cuts_preselection = "mu1pt>3 && "
                                                "mu2pt>3 && "
                                                "kpt>2.5 && "
                                                "abs(mu1eta)<2.5 && "
                                                "abs(mu2eta)<2.5 && "
                                                "abs(keta)<2.5 && "
                                                "bvtx_svprob>1e-4 && "
                                                "jpsivtx_svprob>1e-2 && "
                                                "mu1_mediumID>0.5 && "
                                                "mu2_mediumID>0.5 && "
                                                "Bpt_reco>15 && "
                                                "dr12>0.01 && "
                                                "dr13>0.01 && "
                                                "dr23>0.01 && "
                                                "abs(mu1_dz-mu2_dz)<0.2 && "
                                                "abs(mu1_dz-k_dz)<0.2 && "
                                                "abs(mu2_dz-k_dz)<0.2 && "
                                                "abs(k_dxy)<0.05 && "
                                                "abs(mu1_dxy)<0.05 && "
                                                "abs(mu2_dxy)<0.05 && "
                                                "bvtx_cos2D>0.995 && "
                                                "abs(jpsi_mass-3.0969)<0.1 && "
                                                "Bmass<10. && "
                                                "jpsivtx_cos2D>0.99 &&"
                                                "mu1_isFromMuT>0.5 && "
                                                "mu2_isFromMuT>0.5 && "
                                                "mu1_isFromJpsi_MuT>0.5 && "
                                                "mu2_isFromJpsi_MuT>0.5 && "
                                                "k_isFromMuT>0.5 ";

  TString cuts_final = "Bmass  <  6.0 && "
                                          "abs(jpsi_mass - 3.0969) < 0.1 && "
                                          "mu1pt > 3.5 && "
                                          "mu2pt > 3.0 && "
                                          "kpt > 2.5 && "
                                          "jpsi_pt > 6.7 && "
                                          "jpsivtx_svprob > 0.005 && "
                                          "jpsivtx_cos2D > 0.99 && "
                                          "bvtx_svprob > 0.005 && "
                                          "bvtx_cos2D > 0.99 && "
                                          "Bpt_reco > 13.5 && "
                                          "DR_jpsimu < 0.7 && "
                                          "-0.006 < ip3d < 0.02 && "
                                          "mu1_mediumID>0.5 && "
                                          "mu2_mediumID>0.5 && "
                                          "k_mediumID>0.5";

  
  TString pass_region_definition = "k_mediumID>0.5 && k_raw_db_corr_iso03_rel<0.2";
  TString fail_region_definition = "k_mediumID>0.5 && k_raw_db_corr_iso03_rel<0.2";

  string cutmedidtrue = (string)(cuts_final+"&&"+cuts_preselection+"&&"+pass_region_definition);
  string cutmedidfalse = (string)(cuts_final+"&&"+cuts_preselection+"&&"+fail_region_definition);
  string premedidtrue = (string)(cuts_preselection+"&&"+pass_region_definition);
  string premedidfalse = (string)(cuts_preselection+"&&"+fail_region_definition);
/*
  ooooooooo.                       oooooooooooo  o8o      .   
  `888   `Y88.                     `888'     `8  `"'    .o8   
  888   .d88'  .ooooo.   .ooooo.   888         oooo  .o888oo 
  888ooo88P'  d88' `88b d88' `88b  888oooo8    `888    888   
  888`88b.    888   888 888   888  888    "     888    888   
  888  `88b.  888   888 888   888  888          888    888 . 
  o888o  o888o `Y8bod8P' `Y8bod8P' o888o        o888o   "888"
*/

  int numberbin_min = 36, numberbin_max = 37;
  const char *cuts_preselection_pass = (cuts_preselection+"&&"+pass_region_definition).Data();
  TString cuts_preselection_pass_3mu = cuts_preselection+"&&"+pass_region_definition;
  vector<VarToBeFitted> *vartobefit = new vector<VarToBeFitted>;
  {
    VarToBeFitted *fit_missingmasssquared;
    fit_missingmasssquared = new VarToBeFitted;
    fit_missingmasssquared->vartobefitted = "m_miss_sq";
    fit_missingmasssquared->varname = "m_{miss}^{2}";
    fit_missingmasssquared->vardimension = "GeV^{2}";
    fit_missingmasssquared->min_bin = 0; fit_missingmasssquared->max_bin = 10;
    fit_missingmasssquared->legpos = "TR";
    fit_missingmasssquared->legxlength = 0.28;
    fit_missingmasssquared->cut_mediumid_true = cutmedidtrue;
    fit_missingmasssquared->cut_mediumid_false = cutmedidfalse;
    fit_missingmasssquared->preselectioncut_mediumid_true = premedidtrue;
    fit_missingmasssquared->preselectioncut_mediumid_false = premedidfalse;
    for (int i = numberbin_min; i<numberbin_max; i++)
    {
      fit_missingmasssquared->bins = i;
      //if (debug_fit == false) VarFitter(fit_missingmasssquared, true, debug_fitclosuretest);
    }  
  }
  { 
    VarToBeFitted fit_ptmissvec;
    fit_ptmissvec.vartobefitted = "pt_miss_vec";
    fit_ptmissvec.varname = "p_{T,vec}^{miss}";
    fit_ptmissvec.vardimension = "GeV";
    fit_ptmissvec.min_bin = 0; fit_ptmissvec.max_bin = 30;
    fit_ptmissvec.legpos = "TR";
    fit_ptmissvec.legxlength = 0.28;
    for (int i = numberbin_min; i<numberbin_max; i++)
    {
      fit_ptmissvec.bins = i;
      //vartobefit->push_back(fit_ptmissvec);
    }
  }
  {
    VarToBeFitted *fit_qsquared;
    fit_qsquared = new VarToBeFitted;
    fit_qsquared->vartobefitted = "Q_sq";
    fit_qsquared->varname = "Q^{2}";
    fit_qsquared->vardimension = "GeV^{2}";
    fit_qsquared->min_bin = 1; fit_qsquared->max_bin = 10.5;
    fit_qsquared->legpos = "TL";
    fit_qsquared->legxlength = 0.28;
    fit_qsquared->cut_mediumid_true = cutmedidtrue;
    fit_qsquared->cut_mediumid_false = cutmedidfalse;
    fit_qsquared->preselectioncut_mediumid_true = premedidtrue;
    fit_qsquared->preselectioncut_mediumid_false = premedidfalse;
    for (int i = numberbin_min; i<numberbin_max; i++)
    {
      fit_qsquared->bins = i;
      if (debug_fit == false) VarFitter(fit_qsquared, true, debug_fitclosuretest);
    }
  }

  {
    VarToBeFitted fit_emustar;
    fit_emustar.vartobefitted = "E_mu_star";
    fit_emustar.varname = "E_{#mu}^{*}";
    fit_emustar.vardimension = "GeV";
    fit_emustar.min_bin = 0; fit_emustar.max_bin = 2.5;
    fit_emustar.legpos = "TR";
    fit_emustar.legxlength = 0.28;
    for (int i = numberbin_min; i<numberbin_max; i++)
    {
      fit_emustar.bins = i;
      //vartobefit->push_back(fit_emustar);
    }  
  }

  /*
oooooooooo.   ooooo  .oooooo..o   .oooooo.   ooooooooo.   ooooo ooo        ooooo ooooo ooooo      ooo       .o.       ooooo      ooo ooooooooooooo  .oooooo..o 
`888'   `Y8b  `888' d8P'    `Y8  d8P'  `Y8b  `888   `Y88. `888' `88.       .888' `888' `888b.     `8'      .888.      `888b.     `8' 8'   888   `8 d8P'    `Y8 
 888      888  888  Y88bo.      888           888   .d88'  888   888b     d'888   888   8 `88b.    8      .8"888.      8 `88b.    8       888      Y88bo.      
 888      888  888   `"Y8888o.  888           888ooo88P'   888   8 Y88. .P  888   888   8   `88b.  8     .8' `888.     8   `88b.  8       888       `"Y8888o.  
 888      888  888       `"Y88b 888           888`88b.     888   8  `888'   888   888   8     `88b.8    .88ooo8888.    8     `88b.8       888           `"Y88b 
 888     d88'  888  oo     .d8P `88b    ooo   888  `88b.   888   8    Y     888   888   8       `888   .8'     `888.   8       `888       888      oo     .d8P 
o888bood8P'   o888o 8""88888P'   `Y8bood8P'  o888o  o888o o888o o8o        o888o o888o o8o        `8  o88o     o8888o o8o        `8      o888o     8""88888P'  */

  // Definition of variables needed for each discriminant and variables plotter options
  unordered_map<string, vector<string>> *discriminants = new unordered_map<string, vector<string>>;
  vector<VarToBePlotted> *vartobeplotted = new vector<VarToBePlotted>;

  // Discriminants that will be used in the fit
  branchnames->push_back(MarkedNames("b_c_pisa_threemomentum", 1, "", "#vec{B_{c}^{+}}"));
  (*discriminants)["b_c_pisa_threemomentum"] = {"mu1pt", "mu1eta", "mu1phi",
                                                "mu2pt", "mu2eta", "mu2phi",
                                                "kpt", "keta", "kphi",
                                                "beamspot_x", "beamspot_y", "pv_z",
                                                "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z"};

  branchnames->push_back(MarkedNames("b_c_pisa_threemomentum_regression", 1, "", "#vec{B_{c}^{+}}"));
  (*discriminants)["b_c_pisa_threemomentum_regression"] = {"mu1pt", "mu1eta", "mu1phi",
                                                           "mu2pt", "mu2eta", "mu2phi",
                                                           "kpt", "keta", "kphi",
                                                           "beamspot_x", "beamspot_y", "pv_z",
                                                           "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                                           "mu1_grandmother_pt_regression"};
  /*
  .oooooo.        .oooo.   
 d8P'  `Y8b     .dP""Y88b  
888      888          ]8P' 
888      888        .d8P'  
888      888      .dP'     
`88b    d88b    .oP     .o 
 `Y8bood8P'Ybd' 8888888888
*/

  // Squared transferred momentum needs four momentum of B, mu1, mu2
  VarToBePlotted Q2_Gene;
  branchnames->push_back(MarkedNames("Q2_Gene", 1, "GeV^{2}", "Q^{2}_{gen}"));
  (*discriminants)["Q2_Gene"] = {"mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi",
                                 "mu1_gen_pt", "mu1_gen_eta", "mu1_gen_phi",
                                 "mu2_gen_pt", "mu2_gen_eta", "mu2_gen_phi"};
  Q2_Gene.varname = "Q2_Gene";
  Q2_Gene.varexpression = "Q2_Gene";
  Q2_Gene.tobeprinted = false;
  Q2_Gene.comparisonflag = true;
  Q2_Gene.comparisonname = "q2_comparison";
  Q2_Gene.comparisonlabel = "Q^{2}[GeV^{2}]";
  Q2_Gene.comparisontitle = "Comparison of various Q^{2}";
  Q2_Gene.xminlim = -8;
  Q2_Gene.xmaxlim = 12;
  Q2_Gene.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(Q2_Gene);

  VarToBePlotted Q2_Gene_regression;
  branchnames->push_back(MarkedNames("Q2_Gene_regression", 1, "GeV^{2}", "Q^{2}_{gen}"));
  (*discriminants)["Q2_Gene_regression"] = {"mu1_grandmother_pt_regression", "mu1_grandmother_eta", "mu1_grandmother_phi",
                                            "mu1_gen_pt", "mu1_gen_eta", "mu1_gen_phi",
                                            "mu2_gen_pt", "mu2_gen_eta", "mu2_gen_phi"};

  // Squared transferred momentum: B_c_pisa
  VarToBePlotted Q2_Pisa;
  branchnames->push_back(MarkedNames("Q2_Pisa", 1, "GeV^{2}", "Q^{2}_{Pisa}"));
  (*discriminants)["Q2_Pisa"] = {"b_c_pisa_threemomentum", "Bmass",
                                 "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                 "mu2pt", "mu2eta", "mu2phi", "mu2mass"};
  Q2_Pisa.varname = "Q2_Pisa";
  Q2_Pisa.varexpression = "Q2_Pisa";
  Q2_Pisa.tobeprinted = false;
  Q2_Pisa.comparisonflag = true;
  Q2_Pisa.comparisonname = "q2_comparison";
  Q2_Pisa.comparisonlabel = "Q^{2}[GeV^{2}]";
  Q2_Pisa.xminlim = -8;
  Q2_Pisa.xmaxlim = 12;
  Q2_Pisa.comparisonrationame = "ratio_q2pisa_over_q2gene";
  Q2_Pisa.comparisonratioexpr = "Q2_Pisa / Q2_Gene";
  Q2_Pisa.comparisonratiotitle = "Q^{2}_{Pisa} over Q^{2}_{gene}";
  Q2_Pisa.comparisonratioxlabel = "#frac{Q^{2}_{Pisa}}{Q^{2}_{gene}}";
  Q2_Pisa.comparisonscattername = "scatter_q2pisa_vs_q2gene";
  Q2_Pisa.comparisonscattervars = {"Q2_Gene", "Q2_Pisa"};
  Q2_Pisa.comparisonscattertitle = "Q^{2}_{Pisa} versus Q^{2}_{gene}";
  Q2_Pisa.comparisonscatterxlabel = "Q^{2}_{gene}[GeV^{2}]";
  Q2_Pisa.comparisonscatterylabel = "Q^{2}_{Pisa}[GeV^{2}]";
  Q2_Pisa.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(Q2_Pisa);

  branchnames->push_back(MarkedNames("Q2_Pisa_regression", 1, "GeV^{2}", "Q^{2}_{Pisa}"));
  (*discriminants)["Q2_Pisa_regression"] = {"b_c_pisa_threemomentum_regression", "Bmass",
                                            "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                            "mu2pt", "mu2eta", "mu2phi", "mu2mass"};

  // Squared transferred momentum: B_c_pisa
  VarToBePlotted Q2_Jona;
  branchnames->push_back(MarkedNames("Q2_Jona", 1, "GeV^{2}", "Q^{2}_{Jona}"));
  (*discriminants)["Q2_Jona"] = {"Bpt", "Beta", "Bphi", "Bmass",
                                 "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                 "mu2pt", "mu2eta", "mu2phi", "mu2mass"};
  Q2_Jona.varname = "Q2_Jona";
  Q2_Jona.varexpression = "Q2_Jona";
  Q2_Jona.tobeprinted = false;
  Q2_Jona.comparisonflag = true;
  Q2_Jona.comparisonname = "q2_comparison";
  Q2_Jona.comparisonlabel = "Q^{2}[GeV^{2}]";
  Q2_Jona.xminlim = -8;
  Q2_Jona.xmaxlim = 12;
  Q2_Jona.comparisonrationame = "ratio_q2jona_over_q2gene";
  Q2_Jona.comparisonratioexpr = "Q2_Jona / Q2_Gene";
  Q2_Jona.comparisonratiotitle = "Q^{2}_{Jona} over Q^{2}_{gene}";
  Q2_Jona.comparisonratioxlabel = "#frac{Q^{2}_{Jona}}{Q^{2}_{gene}}";
  Q2_Jona.comparisonscattername = "scatter_q2jona_vs_q2gene";
  Q2_Jona.comparisonscattervars = {"Q2_Gene", "Q2_Jona"};
  Q2_Jona.comparisonscattertitle = "Q^{2}_{Jona} versus Q^{2}_{gene}";
  Q2_Jona.comparisonscatterxlabel = "Q^{2}_{gene}[GeV^{2}]";
  Q2_Jona.comparisonscatterylabel = "Q^{2}_{Jona}[GeV^{2}]";
  Q2_Jona.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(Q2_Jona);

  branchnames->push_back(MarkedNames("Q2_Jona_regression", 1, "GeV^{2}", "Q^{2}_{Jona}"));
  (*discriminants)["Q2_Jona_regression"] = {"mu1_grandmother_pt_regression", "Beta", "Bphi", "Bmass",
                                            "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                            "mu2pt", "mu2eta", "mu2phi", "mu2mass"};

  /*
ooo        ooooo ooooo  .oooooo..o  .oooooo..o ooooo ooooo      ooo   .oooooo.    ooo        ooooo   .oooo.   
`88.       .888' `888' d8P'    `Y8 d8P'    `Y8 `888' `888b.     `8'  d8P'  `Y8b   `88.       .888' .dP""Y88b  
 888b     d'888   888  Y88bo.      Y88bo.       888   8 `88b.    8  888            888b     d'888        ]8P' 
 8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.   888   8   `88b.  8  888            8 Y88. .P  888      .d8P'  
 8  `888'   888   888       `"Y88b      `"Y88b  888   8     `88b.8  888     ooooo  8  `888'   888    .dP'     
 8    Y     888   888  oo     .d8P oo     .d8P  888   8       `888  `88.    .88'   8    Y     888  .oP     .o 
o8o        o888o o888o 8""88888P'  8""88888P'  o888o o8o        `8   `Y8bood8P'   o8o        o888o 8888888888
*/

  // Missing mass squared needs four momentum of B, mu1, mu2, k
  VarToBePlotted MissingM2_Gene;
  branchnames->push_back(MarkedNames("MissingM2_Gene", 1, "GeV^{2}", "m_{miss,gene}^{2}"));
  (*discriminants)["MissingM2_Gene"] = {"mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi",
                                        "mu1_gen_pt", "mu1_gen_eta", "mu1_gen_phi",
                                        "mu2_gen_pt", "mu2_gen_eta", "mu2_gen_phi",
                                        "k_gen_pt", "k_gen_eta", "k_gen_phi"};
  MissingM2_Gene.varname = "MissingM2_Gene";
  MissingM2_Gene.varexpression = "MissingM2_Gene";
  MissingM2_Gene.tobeprinted = false;
  MissingM2_Gene.comparisonflag = true;
  MissingM2_Gene.comparisonname = "missingm2_comparison";
  MissingM2_Gene.comparisonlabel = "m_{miss}^{2}[GeV^{2}]";
  MissingM2_Gene.comparisontitle = "Comparison of various m_{miss}^{2}";
  MissingM2_Gene.xminlim = -8;
  MissingM2_Gene.xmaxlim = 12;
  MissingM2_Gene.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(MissingM2_Gene);

  branchnames->push_back(MarkedNames("MissingM2_Gene_regression", 1, "GeV^{2}", "m_{miss,gene}^{2}"));
  (*discriminants)["MissingM2_Gene_regression"] = {"mu1_grandmother_pt_regression", "mu1_grandmother_eta", "mu1_grandmother_phi",
                                                   "mu1_gen_pt", "mu1_gen_eta", "mu1_gen_phi",
                                                   "mu2_gen_pt", "mu2_gen_eta", "mu2_gen_phi",
                                                   "k_gen_pt", "k_gen_eta", "k_gen_phi"};

  // Missing mass squared Pisa
  VarToBePlotted MissingM2_Pisa;
  branchnames->push_back(MarkedNames("MissingM2_Pisa", 1, "GeV^{2}", "m_{miss,Pisa}^{2}"));
  (*discriminants)["MissingM2_Pisa"] = {"b_c_pisa_threemomentum", "Bmass",
                                        "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                        "mu2pt", "mu2eta", "mu2phi", "mu2mass",
                                        "kpt", "keta", "kphi", "kmass"};
  MissingM2_Pisa.varname = "MissingM2_Pisa";
  MissingM2_Pisa.varexpression = "MissingM2_Pisa";
  MissingM2_Pisa.tobeprinted = false;
  MissingM2_Pisa.comparisonflag = true;
  MissingM2_Pisa.comparisonname = "missingm2_comparison";
  MissingM2_Pisa.comparisonlabel = "m_{miss}^{2}[GeV^{2}]";
  MissingM2_Pisa.comparisontitle = "Comparison of various m_{miss}^{2}";
  MissingM2_Pisa.xminlim = -8;
  MissingM2_Pisa.xmaxlim = 12;
  MissingM2_Pisa.comparisonrationame = "ratio_missingm2pisa_over_missingm2gene";
  MissingM2_Pisa.comparisonratioexpr = "MissingM2_Pisa / MissingM2_Gene";
  MissingM2_Pisa.comparisonratiotitle = "m^{2}_{miss,Pisa} over m^{2}_{miss,gene}";
  MissingM2_Pisa.comparisonratioxlabel = "#frac{m^{2}_{miss,Pisa}}{m^{2}_{miss,gene}}";
  MissingM2_Pisa.comparisonscattername = "scatter_missingm2pisa_vs_missingm2gene";
  MissingM2_Pisa.comparisonscattervars = {"MissingM2_Gene", "MissingM2_Pisa"};
  MissingM2_Pisa.comparisonscattertitle = "m^{2}_{miss,Pisa} versus m^{2}_{miss,gene}";
  MissingM2_Pisa.comparisonscatterxlabel = "m^{2}_{miss,gene}[GeV^{2}]";
  MissingM2_Pisa.comparisonscatterylabel = "m^{2}_{miss,Pisa}[GeV^{2}]";
  MissingM2_Pisa.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(MissingM2_Pisa);

  branchnames->push_back(MarkedNames("MissingM2_Pisa_regression", 1, "GeV^{2}", "m_{miss,Pisa}^{2}"));
  (*discriminants)["MissingM2_Pisa_regression"] = {"b_c_pisa_threemomentum_regression", "Bmass",
                                                   "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                                   "mu2pt", "mu2eta", "mu2phi", "mu2mass",
                                                   "kpt", "keta", "kphi", "kmass"};

  // Missing mass squared Jona
  VarToBePlotted MissingM2_Jona;
  branchnames->push_back(MarkedNames("MissingM2_Jona", 1, "GeV^{2}", "m_{miss,Jona}^{2}"));
  (*discriminants)["MissingM2_Jona"] = {"Bpt", "Beta", "Bphi", "Bmass",
                                        "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                        "mu2pt", "mu2eta", "mu2phi", "mu2mass",
                                        "kpt", "keta", "kphi", "kmass"};
  MissingM2_Jona.varname = "MissingM2_Jona";
  MissingM2_Jona.varexpression = "MissingM2_Jona";
  MissingM2_Jona.tobeprinted = false;
  MissingM2_Jona.comparisonflag = true;
  MissingM2_Jona.comparisonname = "missingm2_comparison";
  MissingM2_Jona.comparisonlabel = "m_{miss}^{2}[GeV^{2}]";
  MissingM2_Jona.comparisontitle = "Comparison of various m_{miss}^{2}";
  MissingM2_Jona.xminlim = -8;
  MissingM2_Jona.xmaxlim = 12;
  MissingM2_Jona.comparisonrationame = "ratio_missingm2jona_over_missingm2gene";
  MissingM2_Jona.comparisonratioexpr = "MissingM2_Jona / MissingM2_Gene";
  MissingM2_Jona.comparisonratiotitle = "m^{2}_{miss,Jona} over m^{2}_{miss,gene}";
  MissingM2_Jona.comparisonratioxlabel = "#frac{m^{2}_{miss,Jona}}{m^{2}_{miss,gene}}";
  MissingM2_Jona.comparisonscattername = "scatter_missingm2jona_vs_missingm2gene";
  MissingM2_Jona.comparisonscattervars = {"MissingM2_Gene", "MissingM2_Jona"};
  MissingM2_Jona.comparisonscattertitle = "m^{2}_{miss,Jona} versus m^{2}_{miss,gene}";
  MissingM2_Jona.comparisonscatterxlabel = "m^{2}_{miss,Jona}[GeV^{2}]";
  MissingM2_Jona.comparisonscatterylabel = "m^{2}_{miss,Jona}[GeV^{2}]";
  MissingM2_Jona.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(MissingM2_Jona);

  branchnames->push_back(MarkedNames("MissingM2_Jona_regression", 1, "GeV^{2}", "m_{miss,Jona}^{2}"));
  (*discriminants)["MissingM2_Jona_regression"] = {"mu1_grandmother_pt_regression", "Beta", "Bphi", "Bmass",
                                                   "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                                   "mu2pt", "mu2eta", "mu2phi", "mu2mass",
                                                   "kpt", "keta", "kphi", "kmass"};

  /*
ooo        ooooo ooooo  .oooooo..o  .oooooo..o ooooo ooooo      ooo   .oooooo.    ooooooooo.   ooooooooooooo 
`88.       .888' `888' d8P'    `Y8 d8P'    `Y8 `888' `888b.     `8'  d8P'  `Y8b   `888   `Y88. 8'   888   `8 
 888b     d'888   888  Y88bo.      Y88bo.       888   8 `88b.    8  888            888   .d88'      888      
 8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.   888   8   `88b.  8  888            888ooo88P'       888      
 8  `888'   888   888       `"Y88b      `"Y88b  888   8     `88b.8  888     ooooo  888              888      
 8    Y     888   888  oo     .d8P oo     .d8P  888   8       `888  `88.    .88'   888              888      
o8o        o888o o888o 8""88888P'  8""88888P'  o888o o8o        `8   `Y8bood8P'   o888o            o888o    
*/

  // Missing pt squared needs four momentum of B, mu1, mu2, k
  VarToBePlotted MissingPt_Gene;
  branchnames->push_back(MarkedNames("MissingPt_Gene", 1, "GeV^{2}", "m_{miss,gene}^{2}"));
  (*discriminants)["MissingPt_Gene"] = {"mu1_grandmother_pt", "mu1_gen_pt", "mu2_gen_pt", "k_gen_pt"};
  MissingPt_Gene.varname = "MissingPt_Gene";
  MissingPt_Gene.varexpression = "MissingPt_Gene";
  MissingPt_Gene.tobeprinted = false;
  MissingPt_Gene.comparisonflag = true;
  MissingPt_Gene.comparisonname = "missingpt_comparison";
  MissingPt_Gene.comparisonlabel = "p_{T}^{miss}[GeV]";
  MissingPt_Gene.comparisontitle = "Comparison of various p_{T}^{miss}";
  MissingPt_Gene.xminlim = -8;
  MissingPt_Gene.xmaxlim = 12;
  MissingPt_Gene.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(MissingPt_Gene);

  branchnames->push_back(MarkedNames("MissingPt_Gene_regression", 1, "GeV^{2}", "m_{miss,gene}^{2}"));
  (*discriminants)["MissingPt_Gene_regression"] = {"mu1_grandmother_pt_regression", "mu1_gen_pt", "mu2_gen_pt", "k_gen_pt"};

  // Missing transverse momentum Pisa
  VarToBePlotted MissingPt_Pisa;
  branchnames->push_back(MarkedNames("MissingPt_Pisa", 1, "GeV^{2}", "m_{miss,Pisa}^{2}"));
  (*discriminants)["MissingPt_Pisa"] = {"b_c_pisa_threemomentum", "Bmass", "mu1pt", "mu2pt", "kpt"};
  MissingPt_Pisa.varname = "MissingPt_Pisa";
  MissingPt_Pisa.varexpression = "MissingPt_Pisa";
  MissingPt_Pisa.tobeprinted = false;
  MissingPt_Pisa.comparisonflag = true;
  MissingPt_Pisa.comparisonname = "missingpt_comparison";
  MissingPt_Pisa.comparisonlabel = "p_{T}^{miss}[GeV]";
  MissingPt_Pisa.comparisontitle = "Comparison of various p_{T}^{miss}";
  MissingPt_Pisa.xminlim = -8;
  MissingPt_Pisa.xmaxlim = 12;
  MissingPt_Pisa.comparisonrationame = "ratio_missingptpisa_over_missingptgene";
  MissingPt_Pisa.comparisonratioexpr = "MissingPt_Pisa / MissingPt_Gene";
  MissingPt_Pisa.comparisonratiotitle = "p^{miss,Pisa}_{T} over p^{miss,gene}_{T}";
  MissingPt_Pisa.comparisonratioxlabel = "#frac{p^{miss}_{T,Pisa}}{p^{miss}_{T,gene}}";
  MissingPt_Pisa.comparisonscattername = "scatter_missingptpisa_vs_missingptgene";
  MissingPt_Pisa.comparisonscattervars = {"MissingPt_Gene", "MissingPt_Pisa"};
  MissingPt_Pisa.comparisonscattertitle = "p_{T}^{miss,Pisa} versus p_{T}^{miss,gene}";
  MissingPt_Pisa.comparisonscatterxlabel = "p_{T}^{miss,gene}[GeV]";
  MissingPt_Pisa.comparisonscatterylabel = "p_{T}^{miss,Pisa}[GeV]";
  MissingPt_Pisa.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(MissingPt_Pisa);

  branchnames->push_back(MarkedNames("MissingPt_Pisa_regression", 1, "GeV^{2}", "m_{miss,Pisa}^{2}"));
  (*discriminants)["MissingPt_Pisa_regression"] = {"b_c_pisa_threemomentum_regression", "mu1pt", "mu2pt", "kpt"};

  // Missing transverse momentum Jona
  VarToBePlotted MissingPt_Jona;
  branchnames->push_back(MarkedNames("MissingPt_Jona", 1, "GeV^{2}", "m_{miss,Jona}^{2}"));
  (*discriminants)["MissingPt_Jona"] = {"Bpt", "Bmass", "mu1pt", "mu2pt", "kpt"};
  MissingPt_Jona.varname = "MissingPt_Jona";
  MissingPt_Jona.varexpression = "MissingPt_Jona";
  MissingPt_Jona.tobeprinted = false;
  MissingPt_Jona.comparisonflag = true;
  MissingPt_Jona.comparisonname = "missingpt_comparison";
  MissingPt_Jona.comparisonlabel = "p_{T}^{miss}[GeV]";
  MissingPt_Jona.comparisontitle = "Comparison of various p_{T}^{miss}";
  MissingPt_Jona.xminlim = -8;
  MissingPt_Jona.xmaxlim = 12;
  MissingPt_Jona.comparisonrationame = "ratio_missingptjona_over_missingptgene";
  MissingPt_Jona.comparisonratioexpr = "MissingPt_Jona / MissingPt_Gene";
  MissingPt_Jona.comparisonratiotitle = "p^{miss,Jona}_{T} over p^{miss,gene}_{T}";
  MissingPt_Jona.comparisonratioxlabel = "#frac{p^{miss,Jona}_{T}}{p^{miss,gene}_{T}}";
  MissingPt_Jona.comparisonscattername = "scatter_missingptjona_vs_missingptgene";
  MissingPt_Jona.comparisonscattervars = {"MissingPt_Gene", "MissingPt_Jona"};
  MissingPt_Jona.comparisonscattertitle = "p_{T}^{miss,Jona} versus p_{T}^{miss,gene}";
  MissingPt_Jona.comparisonscatterxlabel = "p_{T}^{miss,gene}[GeV]";
  MissingPt_Jona.comparisonscatterylabel = "p_{T}^{miss,Jona}[GeV]";
  MissingPt_Jona.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(MissingPt_Jona);

  branchnames->push_back(MarkedNames("MissingPt_Jona_regression", 1, "GeV^{2}", "m_{miss,Jona}^{2}"));
  (*discriminants)["MissingPt_Jona_regression"] = {"mu1_grandmother_pt_regression", "mu1pt", "mu2pt", "kpt"};

  /*
ooooooooo.   ooooooooo.     .oooooo.   ooooooooo.   oooooooooooo ooooooooo.        ooooooooooooo ooooo ooo        ooooo oooooooooooo 
`888   `Y88. `888   `Y88.  d8P'  `Y8b  `888   `Y88. `888'     `8 `888   `Y88.      8'   888   `8 `888' `88.       .888' `888'     `8 
 888   .d88'  888   .d88' 888      888  888   .d88'  888          888   .d88'           888       888   888b     d'888   888         
 888ooo88P'   888ooo88P'  888      888  888ooo88P'   888oooo8     888ooo88P'            888       888   8 Y88. .P  888   888oooo8    
 888          888`88b.    888      888  888          888    "     888`88b.              888       888   8  `888'   888   888    "    
 888          888  `88b.  `88b    d88'  888          888       o  888  `88b.            888       888   8    Y     888   888       o 
o888o        o888o  o888o  `Y8bood8P'  o888o        o888ooooood8 o888o  o888o          o888o     o888o o8o        o888o o888ooooood8   
*/

  // Missing pt squared needs four momentum of B, mu1, mu2, k
  VarToBePlotted ctau_Gene;
  branchnames->push_back(MarkedNames("ctau_Gene", 1, "mm", "c#tau_{Gene}"));
  (*discriminants)["ctau_Gene"] = {"pv_x", "pv_y", "pv_z", "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                   "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  ctau_Gene.varname = "ctau_Gene";
  ctau_Gene.varexpression = "ctau_Gene";
  ctau_Gene.tobeprinted = false;
  ctau_Gene.comparisonflag = true;
  ctau_Gene.comparisonname = "ctau_comparison";
  ctau_Gene.comparisonlabel = "c#tau_{Gene}[m]";
  ctau_Gene.comparisontitle = "Comparison of various c#tau";
  ctau_Gene.xminlim = 0;
  ctau_Gene.xmaxlim = 0.16;
  ctau_Gene.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(ctau_Gene);

  branchnames->push_back(MarkedNames("ctau_Gene_regression", 1, "mm", "c#tau[m]"));
  (*discriminants)["ctau_Gene_regression"] = {"pv_x", "pv_y", "pv_z", "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                              "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};

  // Proper time Pisa
  VarToBePlotted ctau_Pisa;
  branchnames->push_back(MarkedNames("ctau_Pisa", 1, "mm", "c#tau_{Pisa}"));
  (*discriminants)["ctau_Pisa"] = {"pv_x", "pv_y", "pv_z", "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                   "b_c_pisa_threemomentum", "Bmass"};
  ctau_Pisa.varname = "ctau_Pisa";
  ctau_Pisa.varexpression = "ctau_Pisa";
  ctau_Pisa.tobeprinted = false;
  ctau_Pisa.comparisonflag = true;
  ctau_Pisa.comparisonname = "ctau_comparison";
  ctau_Pisa.comparisonlabel = "c#tau_{Pisa}[m]";
  ctau_Pisa.comparisontitle = "Comparison of various c#tau";
  ctau_Pisa.xminlim = 0;
  ctau_Pisa.xmaxlim = 0.16;
  ctau_Pisa.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(ctau_Pisa);

  branchnames->push_back(MarkedNames("ctau_Pisa_regression", 1, "mm", "c#tau[m]"));
  (*discriminants)["ctau_Pisa_regression"] = {"pv_x", "pv_y", "pv_z", "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                              "b_c_pisa_threemomentum_regression", "Bmass"};

  // Proper time Jona
  VarToBePlotted ctau_Jona;
  branchnames->push_back(MarkedNames("ctau_Jona", 1, "mm", "c#tau_{Jona}"));
  (*discriminants)["ctau_Jona"] = {"pv_x", "pv_y", "pv_z", "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                   "Bpt", "Beta", "Bphi", "Bmass"};
  ctau_Jona.varname = "ctau_Jona";
  ctau_Jona.varexpression = "ctau_Jona";
  ctau_Jona.tobeprinted = false;
  ctau_Jona.comparisonflag = true;
  ctau_Jona.comparisonname = "ctau_comparison";
  ctau_Jona.comparisonlabel = "c#tau_{Jona}[m]";
  ctau_Jona.comparisontitle = "Comparison of various c#tau";
  ctau_Jona.xminlim = 0;
  ctau_Jona.xmaxlim = 0.16;
  ctau_Jona.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted->push_back(ctau_Jona);

  branchnames->push_back(MarkedNames("ctau_Jona_regression", 1, "mm", "c#tau[m]"));
  (*discriminants)["ctau_Jona_regression"] = {"pv_x", "pv_y", "pv_z", "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                              "mu1_grandmother_pt_regression", "Beta", "Bphi", "Bmass"};

  /*      .o.       ooooo      ooo   .oooooo.    ooooo        oooooooooooo      ooooooooo.   oooooooooooo  .oooooo..o 
     .888.      `888b.     `8'  d8P'  `Y8b   `888'        `888'     `8      `888   `Y88. `888'     `8 d8P'    `Y8 
    .8"888.      8 `88b.    8  888            888          888               888   .d88'  888         Y88bo.      
   .8' `888.     8   `88b.  8  888            888          888oooo8          888ooo88P'   888oooo8     `"Y8888o.  
  .88ooo8888.    8     `88b.8  888     ooooo  888          888    "          888`88b.     888    "         `"Y88b 
 .8'     `888.   8       `888  `88.    .88'   888       o  888       o       888  `88b.   888       o oo     .d8P 
o88o     o8888o o8o        `8   `Y8bood8P'   o888ooooood8 o888ooooood8      o888o  o888o o888ooooood8 8""88888P'  */

  /*oooooooooooo ooooooooooooo       .o.       
`888'     `8 8'   888   `8      .888.      
 888              888          .8"888.     
 888oooo8         888         .8' `888.    
 888    "         888        .88ooo8888.   
 888       o      888       .8'     `888.  
o888ooooood8     o888o     o88o     o8888o*/

  // Angular resolution. Remember that Eta and phi require two vertices positions, instead Deltaeta and Deltaphi require four vertices
  // eta Angle of vector from PV gene to SV gene respect to original axis
  branchnames->push_back(MarkedNames("eta_gene", 1, "", "#eta_{PVSVgen}"));
  (*discriminants)["eta_gene"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                                  "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle of vector from PV gene to SV reco respect to original axis
  branchnames->push_back(MarkedNames("eta_PVgeneSVreco", 1, "", "#eta_{PVgenSVrec}"));
  (*discriminants)["eta_PVgeneSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                                          "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle of vector from PV reco to SV gene respect to original axis
  branchnames->push_back(MarkedNames("eta_PVrecoSVgene", 1, "", "#eta_{PVrecSVgen}"));
  (*discriminants)["eta_PVrecoSVgene"] = {"beamspot_x", "beamspot_y", "pv_z",
                                          "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle of vector from PV reco to SV reco respect to original axis
  branchnames->push_back(MarkedNames("eta_reco", 1, "", "#eta_{PVSVrec}"));
  (*discriminants)["eta_reco"] = {"pv_x", "pv_y", "pv_z",
                                  "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle of vector from PV reco-beam to B reco respect to original axis
  branchnames->push_back(MarkedNames("eta_breco_beam", 1, "", "#eta_{PV_{beamspot}}"));
  (*discriminants)["eta_breco_beam"] = {
      "beamspot_x",
      "beamspot_y",
      "pv_z",
      "bvtx_vtx_x",
      "bvtx_vtx_y",
      "bvtx_vtx_z",
  };

  // eta Angle of vector from PV reco-beam to B reco respect to original axis
  branchnames->push_back(MarkedNames("eta_breco_pv", 1, "", "#eta_{PV_{reco}}"));
  (*discriminants)["eta_breco_pv"] = {
      "pv_x",
      "pv_y",
      "pv_z",
      "bvtx_vtx_x",
      "bvtx_vtx_y",
      "bvtx_vtx_z",
  };

  // eta Angle of vector from beamspot to SV reco respect to original axis
  branchnames->push_back(MarkedNames("eta_beamsvreco", 1, "", "#eta_{BrecSVrec}"));
  (*discriminants)["eta_beamsvreco"] = {"beamspot_x", "beamspot_y", "pv_z",
                                        "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle between the vector from PV gene to SV gene and the vector from PV reco to SV reco
  branchnames->push_back(MarkedNames("eta_genePVSVreco", 1, "", "#eta_{B_{c}^{+}gene-PVSVrec}"));
  (*discriminants)["eta_genePVSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                          "beamspot_x", "beamspot_y", "pv_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle between the vector from PV gene to SV gene and the vector from PV reco to SV gene
  branchnames->push_back(MarkedNames("eta_genePVrecoSVgene", 1, "", "#eta_{B_{c}^{+}gene-PVrecSVgen}"));
  (*discriminants)["eta_genePVrecoSVgene"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                              "beamspot_x", "beamspot_y", "pv_z", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle between the vector from PV gene to SV gene and the vector from PV gene to SV reco
  branchnames->push_back(MarkedNames("eta_genePVgeneSVreco", 1, "", "#eta_{B_{c}^{+}gene-PVgenSVrec}"));
  (*discriminants)["eta_genePVgeneSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                              "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle difference between eta(3muons) and the one of the vector from PV gene to SV gene
  branchnames->push_back(MarkedNames("eta_BcPVgeneSVgene", 1, "", "#eta_{3mu-PVgenSVgen}"));
  (*discriminants)["eta_BcPVgeneSVgene"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                            "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle difference between eta(3muons) and the one of the vector from PV gene to SV reco
  branchnames->push_back(MarkedNames("eta_BcPVgeneSVreco", 1, "", "#eta_{3mu-PVgenSVrec}"));
  (*discriminants)["eta_BcPVgeneSVreco"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                            "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle difference between eta(3muons) and the one of the vector from PV reco to SV gene
  branchnames->push_back(MarkedNames("eta_BcPVrecoSVgene", 1, "", "#eta_{3mu-PVrecSVgen}"));
  (*discriminants)["eta_BcPVrecoSVgene"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                            "beamspot_x", "beamspot_y", "pv_z", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle difference between eta(3muons) and the one of the vector from PV reco to SV reco
  branchnames->push_back(MarkedNames("eta_BcPVrecoSVreco", 1, "", "#eta_{B_{c}^{+}gene-PVgenSVrec}"));
  (*discriminants)["eta_BcPVrecoSVreco"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                            "beamspot_x", "beamspot_y", "pv_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle of 3 muons from true momenta
  branchnames->push_back(MarkedNames("eta_3mumomenta_gene", 1, "", "#eta_{3#mu,gene}"));
  (*discriminants)["eta_3mumomenta_gene"] = {"mu1_gen_pt", "mu1_gen_eta", "mu1_gen_phi",
                                             "mu2_gen_pt", "mu2_gen_eta", "mu2_gen_phi",
                                             "k_gen_pt", "k_gen_eta", "k_gen_phi"};

  // eta Angle of 3 muons from reco momenta
  branchnames->push_back(MarkedNames("eta_3mumomenta_reco", 1, "", "#eta_{3#mu,reco}"));
  (*discriminants)["eta_3mumomenta_reco"] = {"mu1pt", "mu1eta", "mu1phi",
                                             "mu2pt", "mu2eta", "mu2phi",
                                             "kpt", "keta", "kphi"};

  VarToBePlotted eta_resolution_grandmother_pvgensvgen;
  eta_resolution_grandmother_pvgensvgen.varname = "eta_resolution_grandmother_pvgensvgen";
  eta_resolution_grandmother_pvgensvgen.varexpression = "mu1_grandmother_eta - eta_gene";
  eta_resolution_grandmother_pvgensvgen.tobeprinted = true;
  eta_resolution_grandmother_pvgensvgen.plottitle = "#eta_{Grandmother}-#eta_{PVgeneSVgene}";
  eta_resolution_grandmother_pvgensvgen.plotxlabel = "#eta_{Grandmother}-#eta_{PVgeneSVgene}[]";
  eta_resolution_grandmother_pvgensvgen.xminlim = -1;
  eta_resolution_grandmother_pvgensvgen.xmaxlim = +1;
  eta_resolution_grandmother_pvgensvgen.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(eta_resolution_grandmother_pvgensvgen);

  VarToBePlotted eta_resolution_grandmother_pvrecojpsireco;
  eta_resolution_grandmother_pvrecojpsireco.varname = "eta_resolution_grandmother_pvrecojpsireco";
  eta_resolution_grandmother_pvrecojpsireco.varexpression = "mu1_grandmother_eta - eta_reco";
  eta_resolution_grandmother_pvrecojpsireco.tobeprinted = true;
  eta_resolution_grandmother_pvrecojpsireco.plottitle = "#eta_{Grandmother}-#eta_{PVrecoJ/#psireco}";
  eta_resolution_grandmother_pvrecojpsireco.plotxlabel = "#eta_{Grandmother}-#eta_{PVrecoJ/#psireco}[]";
  eta_resolution_grandmother_pvrecojpsireco.xminlim = -1;
  eta_resolution_grandmother_pvrecojpsireco.xmaxlim = +1;
  eta_resolution_grandmother_pvrecojpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(eta_resolution_grandmother_pvrecojpsireco);

  VarToBePlotted eta_resolution_grandmother_pvrecoBreco;
  eta_resolution_grandmother_pvrecoBreco.varname = "eta_resolution_grandmother_pvrecoBreco";
  eta_resolution_grandmother_pvrecoBreco.varexpression = "mu1_grandmother_eta - eta_breco_pv";
  eta_resolution_grandmother_pvrecoBreco.tobeprinted = true;
  eta_resolution_grandmother_pvrecoBreco.plottitle = "#eta_{Grandmother}-#eta_{PVrecoBreco}";
  eta_resolution_grandmother_pvrecoBreco.plotxlabel = "#eta_{Grandmother}-#eta_{PVrecoBreco}[]";
  eta_resolution_grandmother_pvrecoBreco.xminlim = -1;
  eta_resolution_grandmother_pvrecoBreco.xmaxlim = +1;
  eta_resolution_grandmother_pvrecoBreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(eta_resolution_grandmother_pvrecoBreco);

  VarToBePlotted eta_resolution_grandmother_beamjpsireco;
  eta_resolution_grandmother_beamjpsireco.varname = "eta_resolution_grandmother_beamjpsireco";
  eta_resolution_grandmother_beamjpsireco.varexpression = "mu1_grandmother_eta - eta_beamsvreco";
  eta_resolution_grandmother_beamjpsireco.tobeprinted = true;
  eta_resolution_grandmother_beamjpsireco.plottitle = "#eta_{Grandmother}-#eta_{BeamspotJ/#psireco}";
  eta_resolution_grandmother_beamjpsireco.plotxlabel = "#eta_{Grandmother}-#eta_{BeamspotJ/#psireco}[]";
  eta_resolution_grandmother_beamjpsireco.xminlim = -1;
  eta_resolution_grandmother_beamjpsireco.xmaxlim = +1;
  eta_resolution_grandmother_beamjpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(eta_resolution_grandmother_beamjpsireco);

  VarToBePlotted eta_resolution_grandmother_beambreco;
  eta_resolution_grandmother_beambreco.varname = "eta_resolution_grandmother_beambreco";
  eta_resolution_grandmother_beambreco.varexpression = "mu1_grandmother_eta - eta_breco_beam";
  eta_resolution_grandmother_beambreco.tobeprinted = true;
  eta_resolution_grandmother_beambreco.tobesquareprinted = true;
  eta_resolution_grandmother_beambreco.plottitle = "#eta_{Grandmother}-#eta_{BeamspotBreco}";
  eta_resolution_grandmother_beambreco.plotxlabel = "#eta_{Grandmother}-#eta_{BeamspotBreco}[]";
  eta_resolution_grandmother_beambreco.xminlim = -1;
  eta_resolution_grandmother_beambreco.xmaxlim = +1;
  eta_resolution_grandmother_beambreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(eta_resolution_grandmother_beambreco);

  VarToBePlotted eta_resolution_grandmother_3mumomentareco;
  eta_resolution_grandmother_3mumomentareco.varname = "eta_resolution_grandmother_3mumomentareco";
  eta_resolution_grandmother_3mumomentareco.varexpression = "mu1_grandmother_eta - eta_3mumomenta_reco";
  eta_resolution_grandmother_3mumomentareco.tobeprinted = true;
  eta_resolution_grandmother_3mumomentareco.tobesquareprinted = true;
  eta_resolution_grandmother_3mumomentareco.plottitle = "#eta_{Grandmother}-#eta_{3#mureco}";
  eta_resolution_grandmother_3mumomentareco.plotxlabel = "#eta_{Grandmother}-#eta_{3#mureco}[]";
  eta_resolution_grandmother_3mumomentareco.xminlim = -1;
  eta_resolution_grandmother_3mumomentareco.xmaxlim = +1;
  eta_resolution_grandmother_3mumomentareco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(eta_resolution_grandmother_3mumomentareco);

  /*ooooooooo.   ooooo   ooooo ooooo 
 `888   `Y88. `888'   `888' `888' 
  888   .d88'  888     888   888  
  888ooo88P'   888ooooo888   888  
  888          888     888   888  
  888          888     888   888  
 o888o        o888o   o888o o888o*/

  // phi Angle of vector from PV gene to SV gene respect to original axis
  branchnames->push_back(MarkedNames("phi_gene", 1, "", "#phi_{PVSVgen}"));
  (*discriminants)["phi_gene"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                                  "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle of vector from PV gene to SV reco respect to original axis
  branchnames->push_back(MarkedNames("phi_PVgeneSVreco", 1, "", "#phi_{PVgenSVrec}"));
  (*discriminants)["phi_PVgeneSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                                          "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle of vector from PV reco to SV gene respect to original axis
  branchnames->push_back(MarkedNames("phi_PVrecoSVgene", 1, "", "#phi_{PVrecSVgen}"));
  (*discriminants)["phi_PVrecoSVgene"] = {"beamspot_x", "beamspot_y", "pv_z",
                                          "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle of vector from PV reco to SV reco respect to original axis
  branchnames->push_back(MarkedNames("phi_reco", 1, "", "#phi_{PVSVrec}"));
  (*discriminants)["phi_reco"] = {"pv_x", "pv_y", "pv_z",
                                  "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle of vector from PV reco-beam to B reco respect to original axis
  branchnames->push_back(MarkedNames("phi_breco_beam", 1, "", "#phi_{PV_{beamspot}}"));
  (*discriminants)["phi_breco_beam"] = {"beamspot_x", "beamspot_y", "pv_z",
                                        "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z"};

  // phi Angle of vector from PV reco-beam to B reco respect to original axis
  branchnames->push_back(MarkedNames("phi_breco_pv", 1, "", "#phi_{PV_{reco}}"));
  (*discriminants)["phi_breco_pv"] = {
      "pv_x",
      "pv_y",
      "pv_z",
      "bvtx_vtx_x",
      "bvtx_vtx_y",
      "bvtx_vtx_z",
  };

  // phi Angle of vector from beamspot to SV reco respect to original axis
  branchnames->push_back(MarkedNames("phi_beamsvreco", 1, "", "#phi_{BrecSVrec}"));
  (*discriminants)["phi_beamsvreco"] = {"beamspot_x", "beamspot_y", "pv_z",
                                        "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle between the vector from PV gene to SV gene and the vector from PV reco to SV reco
  branchnames->push_back(MarkedNames("phi_genePVSVreco", 1, "", "#phi_{B_{c}^{+}gene-PVSVrec}"));
  (*discriminants)["phi_genePVSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                          "beamspot_x", "beamspot_y", "pv_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle between the vector from PV gene to SV gene and the vector from PV reco to SV gene
  branchnames->push_back(MarkedNames("phi_genePVrecoSVgene", 1, "", "#phi_{B_{c}^{+}gene-PVrecSVgen}"));
  (*discriminants)["phi_genePVrecoSVgene"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                              "beamspot_x", "beamspot_y", "pv_z", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle between the vector from PV gene to SV gene and the vector from PV gene to SV reco
  branchnames->push_back(MarkedNames("phi_genePVgeneSVreco", 1, "", "#phi_{B_{c}^{+}gene-PVgenSVrec}"));
  (*discriminants)["phi_genePVgeneSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                              "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle difference between phi(3muons) and the one of the vector from PV gene to SV gene
  branchnames->push_back(MarkedNames("phi_BcPVgeneSVgene", 1, "", "#phi_{3mu-PVgenSVgen}"));
  (*discriminants)["phi_BcPVgeneSVgene"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                            "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle difference between phi(3muons) and the one of the vector from PV gene to SV reco
  branchnames->push_back(MarkedNames("phi_BcPVgeneSVreco", 1, "", "#phi_{3mu-PVgenSVrec}"));
  (*discriminants)["phi_BcPVgeneSVreco"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                            "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle difference between phi(3muons) and the one of the vector from PV reco to SV gene
  branchnames->push_back(MarkedNames("phi_BcPVrecoSVgene", 1, "", "#phi_{3mu-PVrecSVgen}"));
  (*discriminants)["phi_BcPVrecoSVgene"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                            "beamspot_x", "beamspot_y", "pv_z", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle difference between phi(3muons) and the one of the vector from PV reco to SV reco
  branchnames->push_back(MarkedNames("phi_BcPVrecoSVreco", 1, "", "#phi_{B_{c}^{+}gene-PVgenSVrec}"));
  (*discriminants)["phi_BcPVrecoSVreco"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                            "beamspot_x", "beamspot_y", "pv_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle of 3 muons from true momenta
  branchnames->push_back(MarkedNames("phi_3mumomenta_gene", 1, "", "#phi_{3#mu,gene}"));
  (*discriminants)["phi_3mumomenta_gene"] = {"mu1_gen_pt", "mu1_gen_phi", "mu1_gen_phi",
                                             "mu2_gen_pt", "mu2_gen_phi", "mu2_gen_phi",
                                             "k_gen_pt", "k_gen_phi", "k_gen_phi"};

  // phi Angle of 3 muons from reco momenta
  branchnames->push_back(MarkedNames("phi_3mumomenta_reco", 1, "", "#phi_{3#mu,reco}"));
  (*discriminants)["phi_3mumomenta_reco"] = {"mu1pt", "mu1phi", "mu1phi",
                                             "mu2pt", "mu2phi", "mu2phi",
                                             "kpt", "kphi", "kphi"};

  VarToBePlotted phi_resolution_grandmother_pvgensvgen;
  phi_resolution_grandmother_pvgensvgen.varname = "phi_resolution_grandmother_pvgensvgen";
  phi_resolution_grandmother_pvgensvgen.varexpression = "mu1_grandmother_phi - phi_gene";
  phi_resolution_grandmother_pvgensvgen.tobeprinted = true;
  phi_resolution_grandmother_pvgensvgen.plottitle = "#phi_{Grandmother}-#phi_{PVgeneSVgene}";
  phi_resolution_grandmother_pvgensvgen.plotxlabel = "#phi_{Grandmother}-#phi_{PVgeneSVgene}[]";
  phi_resolution_grandmother_pvgensvgen.xminlim = -1;
  phi_resolution_grandmother_pvgensvgen.xmaxlim = +1;
  phi_resolution_grandmother_pvgensvgen.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(phi_resolution_grandmother_pvgensvgen);

  VarToBePlotted phi_resolution_grandmother_pvrecojpsireco;
  phi_resolution_grandmother_pvrecojpsireco.varname = "phi_resolution_grandmother_pvrecojpsireco";
  phi_resolution_grandmother_pvrecojpsireco.varexpression = "mu1_grandmother_phi - phi_reco";
  phi_resolution_grandmother_pvrecojpsireco.tobeprinted = true;
  phi_resolution_grandmother_pvrecojpsireco.plottitle = "#phi_{Grandmother}-#phi_{PVrecoJ/#psireco}";
  phi_resolution_grandmother_pvrecojpsireco.plotxlabel = "#phi_{Grandmother}-#phi_{PVrecoJ/#psireco}[]";
  phi_resolution_grandmother_pvrecojpsireco.xminlim = -1;
  phi_resolution_grandmother_pvrecojpsireco.xmaxlim = +1;
  phi_resolution_grandmother_pvrecojpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(phi_resolution_grandmother_pvrecojpsireco);

  VarToBePlotted phi_resolution_grandmother_pvrecoBreco;
  phi_resolution_grandmother_pvrecoBreco.varname = "phi_resolution_grandmother_pvrecoBreco";
  phi_resolution_grandmother_pvrecoBreco.varexpression = "mu1_grandmother_phi - phi_breco_pv";
  phi_resolution_grandmother_pvrecoBreco.tobeprinted = true;
  phi_resolution_grandmother_pvrecoBreco.plottitle = "#phi_{Grandmother}-#phi_{PVrecoBreco}";
  phi_resolution_grandmother_pvrecoBreco.plotxlabel = "#phi_{Grandmother}-#phi_{PVrecoBreco}[]";
  phi_resolution_grandmother_pvrecoBreco.xminlim = -1;
  phi_resolution_grandmother_pvrecoBreco.xmaxlim = +1;
  phi_resolution_grandmother_pvrecoBreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(phi_resolution_grandmother_pvrecoBreco);

  VarToBePlotted phi_resolution_grandmother_beamjpsireco;
  phi_resolution_grandmother_beamjpsireco.varname = "phi_resolution_grandmother_beamjpsireco";
  phi_resolution_grandmother_beamjpsireco.varexpression = "mu1_grandmother_phi - phi_beamsvreco";
  phi_resolution_grandmother_beamjpsireco.tobeprinted = true;
  phi_resolution_grandmother_beamjpsireco.plottitle = "#phi_{Grandmother}-#phi_{BeamspotJ/#psireco}";
  phi_resolution_grandmother_beamjpsireco.plotxlabel = "#phi_{Grandmother}-#phi_{BeamspotJ/#psireco}[]";
  phi_resolution_grandmother_beamjpsireco.xminlim = -1;
  phi_resolution_grandmother_beamjpsireco.xmaxlim = +1;
  phi_resolution_grandmother_beamjpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(phi_resolution_grandmother_beamjpsireco);

  VarToBePlotted phi_resolution_grandmother_beambreco;
  phi_resolution_grandmother_beambreco.varname = "phi_resolution_grandmother_beambreco";
  phi_resolution_grandmother_beambreco.varexpression = "mu1_grandmother_phi - phi_breco_beam";
  phi_resolution_grandmother_beambreco.tobeprinted = true;
  phi_resolution_grandmother_beambreco.tobesquareprinted = true;
  phi_resolution_grandmother_beambreco.plottitle = "#phi_{Grandmother}-#phi_{BeamspotBreco}";
  phi_resolution_grandmother_beambreco.plotxlabel = "#phi_{Grandmother}-#phi_{BeamspotBreco}[]";
  phi_resolution_grandmother_beambreco.xminlim = -1;
  phi_resolution_grandmother_beambreco.xmaxlim = +1;
  phi_resolution_grandmother_beambreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(phi_resolution_grandmother_beambreco);

  VarToBePlotted phi_resolution_grandmother_3mumomentareco;
  phi_resolution_grandmother_3mumomentareco.varname = "phi_resolution_grandmother_3mumomentareco";
  phi_resolution_grandmother_3mumomentareco.varexpression = "mu1_grandmother_phi - phi_3mumomenta_reco";
  phi_resolution_grandmother_3mumomentareco.tobeprinted = true;
  phi_resolution_grandmother_3mumomentareco.tobesquareprinted = true;
  phi_resolution_grandmother_3mumomentareco.plottitle = "#phi_{Grandmother}-#phi_{3#mureco}";
  phi_resolution_grandmother_3mumomentareco.plotxlabel = "#phi_{Grandmother}-#phi_{3#mureco}[]";
  phi_resolution_grandmother_3mumomentareco.xminlim = -1;
  phi_resolution_grandmother_3mumomentareco.xmaxlim = +1;
  phi_resolution_grandmother_3mumomentareco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(phi_resolution_grandmother_3mumomentareco);

  /*
      .o.       ooooo      ooo   .oooooo.    ooooo        oooooooooooo 
     .888.      `888b.     `8'  d8P'  `Y8b   `888'        `888'     `8 
    .8"888.      8 `88b.    8  888            888          888         
   .8' `888.     8   `88b.  8  888            888          888oooo8    
  .88ooo8888.    8     `88b.8  888     ooooo  888          888    "    
 .8'     `888.   8       `888  `88.    .88'   888       o  888       o 
o88o     o8888o o8o        `8   `Y8bood8P'   o888ooooood8 o888ooooood8 */

  VarToBePlotted angle_grandmother_pvsvgen;
  branchnames->push_back(MarkedNames("angle_grandmother_pvsvgen", 1, "", "#vec{Grandmother,Mother}#angle#vec{PVgen,SVgen}"));
  (*discriminants)["angle_grandmother_pvsvgen"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                                                   "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                                   "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  angle_grandmother_pvsvgen.varname = "angle_grandmother_pvsvgen";
  angle_grandmother_pvsvgen.varexpression = "angle_grandmother_pvsvgen";
  angle_grandmother_pvsvgen.tobeprinted = true;
  angle_grandmother_pvsvgen.plottitle = "Angle between #vec{B_{c}^{+}} from kinematics and #vec{(PVgen,SVgen)}";
  angle_grandmother_pvsvgen.plotxlabel = "#vec{(Grandmother,Mother)}#angle#vec{(PVgen,SVgen)}[]";
  //angle_grandmother_pvsvgen.xminlim = -5;
  //angle_grandmother_pvsvgen.xmaxlim = +100;
  angle_grandmother_pvsvgen.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(angle_grandmother_pvsvgen);

  VarToBePlotted angle_grandmother_PVrecoBreco;
  branchnames->push_back(MarkedNames("angle_grandmother_PVrecoBreco", 1, "", "#vec{Grandmother,Mother}#angle#vec{PVrec,Brec}"));
  (*discriminants)["angle_grandmother_PVrecoBreco"] = {"pv_x", "pv_y", "pv_z",
                                                       "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                                       "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  angle_grandmother_PVrecoBreco.varname = "angle_grandmother_PVrecoBreco";
  angle_grandmother_PVrecoBreco.varexpression = "angle_grandmother_PVrecoBreco";
  angle_grandmother_PVrecoBreco.tobeprinted = true;
  angle_grandmother_PVrecoBreco.plottitle = "Angle between #vec{B_{c}^{+}} from kinematics and #vec{(PVrec,Brec)}";
  angle_grandmother_PVrecoBreco.plotxlabel = "#vec{(Grandmother,Mother)}#angle#vec{(PVrec,Brec)}[]";
  //angle_grandmother_pvsvgen.xminlim = -5;
  //angle_grandmother_pvsvgen.xmaxlim = +100;
  angle_grandmother_PVrecoBreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(angle_grandmother_PVrecoBreco);

  VarToBePlotted angle_grandmother_BeamBreco;
  branchnames->push_back(MarkedNames("angle_grandmother_BeamBreco", 1, "", "#vec{(Grandmother,Mother)}#angle#vec{(Beam,Brec)}"));
  (*discriminants)["angle_grandmother_BeamBreco"] = {"beamspot_x", "beamspot_y", "pv_z",
                                                     "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                                     "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  angle_grandmother_BeamBreco.varname = "angle_grandmother_BeamBreco";
  angle_grandmother_BeamBreco.varexpression = "angle_grandmother_BeamBreco";
  angle_grandmother_BeamBreco.tobeprinted = true;
  angle_grandmother_BeamBreco.plottitle = "Angle between #vec{B_{c}^{+}} from kinematics and #vec{(Beam,Brec)}";
  angle_grandmother_BeamBreco.plotxlabel = "#vec{(Grandmother,Mother)}#angle#vec{(Beam,Brec)}[]";
  //angle_grandmother_pvsvgen.xminlim = -5;
  //angle_grandmother_pvsvgen.xmaxlim = +100;
  angle_grandmother_BeamBreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(angle_grandmother_BeamBreco);

  VarToBePlotted angle_grandmother_PVrecoJpsireco;
  branchnames->push_back(MarkedNames("angle_grandmother_PVrecoJpsireco", 1, "", "#vec{(Grandmother,Mother)}#angle#vec{(PVrec,J/#psirec)}"));
  (*discriminants)["angle_grandmother_PVrecoJpsireco"] = {"pv_x", "pv_y", "pv_z",
                                                          "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                                          "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  angle_grandmother_PVrecoJpsireco.varname = "angle_grandmother_PVrecoJpsireco";
  angle_grandmother_PVrecoJpsireco.varexpression = "angle_grandmother_PVrecoJpsireco";
  angle_grandmother_PVrecoJpsireco.tobeprinted = true;
  angle_grandmother_PVrecoJpsireco.plottitle = "Angle between #vec{B_{c}^{+}} from kinematics and #vec{(PVrec,J/#Psirec)}";
  angle_grandmother_PVrecoJpsireco.plotxlabel = "#vec{(Grandmother,Mother)}#angle#vec{(PVrec,J/#psirec)}[]";
  //angle_grandmother_pvsvgen.xminlim = -5;
  //angle_grandmother_pvsvgen.xmaxlim = +100;
  angle_grandmother_PVrecoJpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(angle_grandmother_PVrecoJpsireco);

  VarToBePlotted angle_grandmother_BeamJpsireco;
  branchnames->push_back(MarkedNames("angle_grandmother_BeamJpsireco", 1, "", "#vec{(Grandmother,Mother)}#angle#vec{(Beam,J/#psirec)}"));
  (*discriminants)["angle_grandmother_BeamJpsireco"] = {"beamspot_x", "beamspot_y", "pv_z",
                                                        "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                                        "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  angle_grandmother_BeamJpsireco.varname = "angle_grandmother_BeamJpsireco";
  angle_grandmother_BeamJpsireco.varexpression = "angle_grandmother_BeamJpsireco";
  angle_grandmother_BeamJpsireco.tobeprinted = true;
  angle_grandmother_BeamJpsireco.plottitle = "Angle between #vec{B_{c}^{+}} from kinematics and #vec{(Beam,J/#psirec)}";
  angle_grandmother_BeamJpsireco.plotxlabel = "#vec{(Grandmother,Mother)}#angle#vec{(Beam,J/#psirec)}[]";
  //angle_grandmother_pvsvgen.xminlim = -5;
  //angle_grandmother_pvsvgen.xmaxlim = +100;
  angle_grandmother_BeamJpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(angle_grandmother_BeamJpsireco);

  VarToBePlotted angle_grandmother_3mureco;
  branchnames->push_back(MarkedNames("angle_grandmother_3mureco", 1, "", "#vec{(Grandmother,Mother)}#angle#vec{(3#mureco)}"));
  (*discriminants)["angle_grandmother_3mureco"] = {"mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi",
                                                   "mu1pt", "mu1eta", "mu1phi",
                                                   "mu2pt", "mu2eta", "mu2phi",
                                                   "kpt", "keta", "kphi"};
  angle_grandmother_3mureco.varname = "angle_grandmother_3mureco";
  angle_grandmother_3mureco.varexpression = "angle_grandmother_3mureco";
  angle_grandmother_3mureco.tobeprinted = true;
  angle_grandmother_3mureco.plottitle = "Angle between #vec{B_{c}^{+}} from kinematics and #vec{(3#mureco)}";
  angle_grandmother_3mureco.plotxlabel = "#vec{(Grandmother,Mother)}#angle#vec{(3#mureco)}[]";
  //angle_grandmother_pvsvgen.xminlim = -5;
  //angle_grandmother_pvsvgen.xmaxlim = +100;
  angle_grandmother_3mureco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(angle_grandmother_3mureco);

  VarToBePlotted cos_angle_grandmother_pvsvgen;
  branchnames->push_back(MarkedNames("cos_angle_grandmother_pvsvgen", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVgen,SVgen)})"));
  (*discriminants)["cos_angle_grandmother_pvsvgen"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                                                       "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                                       "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  cos_angle_grandmother_pvsvgen.varname = "cos_angle_grandmother_pvsvgen";
  cos_angle_grandmother_pvsvgen.varexpression = "cos_angle_grandmother_pvsvgen";
  cos_angle_grandmother_pvsvgen.tobeprinted = true;
  cos_angle_grandmother_pvsvgen.plottitle = "Cosine of the angle between #vec{B_{c}^{+}} from kinematics and #vec{(PVgen,SVgen)}";
  cos_angle_grandmother_pvsvgen.plotxlabel = "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVgen,SVgen)})[]";
  //cos_angle_grandmother_pvsvgen.xminlim = -5;
  //cos_angle_grandmother_pvsvgen.xmaxlim = +100;
  cos_angle_grandmother_pvsvgen.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(cos_angle_grandmother_pvsvgen);

  VarToBePlotted cos_angle_grandmother_PVrecoBreco;
  branchnames->push_back(MarkedNames("cos_angle_grandmother_PVrecoBreco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVrec,Brec)})"));
  (*discriminants)["cos_angle_grandmother_PVrecoBreco"] = {"pv_x", "pv_y", "pv_z",
                                                           "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                                           "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  cos_angle_grandmother_PVrecoBreco.varname = "cos_angle_grandmother_PVrecoBreco";
  cos_angle_grandmother_PVrecoBreco.varexpression = "cos_angle_grandmother_PVrecoBreco";
  cos_angle_grandmother_PVrecoBreco.tobeprinted = true;
  cos_angle_grandmother_PVrecoBreco.plottitle = "Cosine of the angle between #vec{B_{c}^{+}} from kinematics and #vec{(PVrec,Brec)}";
  cos_angle_grandmother_PVrecoBreco.plotxlabel = "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVrec,Brec)})[]";
  //cos_angle_grandmother_pvsvgen.xminlim = -5;
  //cos_angle_grandmother_pvsvgen.xmaxlim = +100;
  cos_angle_grandmother_PVrecoBreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(cos_angle_grandmother_PVrecoBreco);

  VarToBePlotted cos_angle_grandmother_BeamBreco;
  branchnames->push_back(MarkedNames("cos_angle_grandmother_BeamBreco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(Beam,Brec)})"));
  (*discriminants)["cos_angle_grandmother_BeamBreco"] = {"beamspot_x", "beamspot_y", "pv_z",
                                                         "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z",
                                                         "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  cos_angle_grandmother_BeamBreco.varname = "cos_angle_grandmother_BeamBreco";
  cos_angle_grandmother_BeamBreco.varexpression = "cos_angle_grandmother_BeamBreco";
  cos_angle_grandmother_BeamBreco.tobeprinted = true;
  cos_angle_grandmother_BeamBreco.plottitle = "Cosine of the angle between #vec{B_{c}^{+}} from kinematics and #vec{(Beam,Brec)}";
  cos_angle_grandmother_BeamBreco.plotxlabel = "cos(#vec{(Grandmother,Mother)}#angle#vec{(Beam,Brec)})[]";
  //cos_angle_grandmother_pvsvgen.xminlim = -5;
  //cos_angle_grandmother_pvsvgen.xmaxlim = +100;
  cos_angle_grandmother_BeamBreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(cos_angle_grandmother_BeamBreco);

  VarToBePlotted cos_angle_grandmother_PVrecoJpsireco;
  branchnames->push_back(MarkedNames("cos_angle_grandmother_PVrecoJpsireco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVrec,J/#psirec)})"));
  (*discriminants)["cos_angle_grandmother_PVrecoJpsireco"] = {"pv_x", "pv_y", "pv_z",
                                                              "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                                              "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  cos_angle_grandmother_PVrecoJpsireco.varname = "cos_angle_grandmother_PVrecoJpsireco";
  cos_angle_grandmother_PVrecoJpsireco.varexpression = "cos_angle_grandmother_PVrecoJpsireco";
  cos_angle_grandmother_PVrecoJpsireco.tobeprinted = true;
  cos_angle_grandmother_PVrecoJpsireco.plottitle = "Cosine of the angle between #vec{B_{c}^{+}} from kinematics and #vec{(PVrec,J/#Psirec)}";
  cos_angle_grandmother_PVrecoJpsireco.plotxlabel = "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVrec,J/#psirec)})[]";
  //cos_angle_grandmother_pvsvgen.xminlim = -5;
  //cos_angle_grandmother_pvsvgen.xmaxlim = +100;
  cos_angle_grandmother_PVrecoJpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(cos_angle_grandmother_PVrecoJpsireco);

  VarToBePlotted cos_angle_grandmother_BeamJpsireco;
  branchnames->push_back(MarkedNames("cos_angle_grandmother_BeamJpsireco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(Beam,J/#psirec)})"));
  (*discriminants)["cos_angle_grandmother_BeamJpsireco"] = {"beamspot_x", "beamspot_y", "pv_z",
                                                            "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                                            "mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi"};
  cos_angle_grandmother_BeamJpsireco.varname = "cos_angle_grandmother_BeamJpsireco";
  cos_angle_grandmother_BeamJpsireco.varexpression = "cos_angle_grandmother_BeamJpsireco";
  cos_angle_grandmother_BeamJpsireco.tobeprinted = true;
  cos_angle_grandmother_BeamJpsireco.plottitle = "Cosine of the angle between #vec{B_{c}^{+}} from kinematics and #vec{(Beam,J/#psirec)}";
  cos_angle_grandmother_BeamJpsireco.plotxlabel = "cos(#vec{(Grandmother,Mother)}#angle#vec{(Beam,J/#psirec)})[]";
  //cos_angle_grandmother_pvsvgen.xminlim = -5;
  //cos_angle_grandmother_pvsvgen.xmaxlim = +100;
  cos_angle_grandmother_BeamJpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(cos_angle_grandmother_BeamJpsireco);

  VarToBePlotted cos_angle_grandmother_3mureco;
  branchnames->push_back(MarkedNames("cos_angle_grandmother_3mureco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(3#mureco)})"));
  (*discriminants)["cos_angle_grandmother_3mureco"] = {"mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi",
                                                       "mu1pt", "mu1eta", "mu1phi",
                                                       "mu2pt", "mu2eta", "mu2phi",
                                                       "kpt", "keta", "kphi"};
  cos_angle_grandmother_3mureco.varname = "cos_angle_grandmother_3mureco";
  cos_angle_grandmother_3mureco.varexpression = "cos_angle_grandmother_3mureco";
  cos_angle_grandmother_3mureco.tobeprinted = true;
  cos_angle_grandmother_3mureco.plottitle = "Cosine of the angle between #vec{B_{c}^{+}} from kinematics and #vec{(3#mureco)}";
  cos_angle_grandmother_3mureco.plotxlabel = "cos(#vec{(Grandmother,Mother)}#angle#vec{(3#mureco)})[]";
  //cos_angle_grandmother_pvsvgen.xminlim = -5;
  //cos_angle_grandmother_pvsvgen.xmaxlim = +100;
  cos_angle_grandmother_3mureco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted->push_back(cos_angle_grandmother_3mureco);

  /*
oooooooooooo oooooooooooo oooooooooooo ooooo   .oooooo.   ooooo oooooooooooo ooooo      ooo   .oooooo.   ooooo oooooooooooo  .oooooo..o 
`888'     `8 `888'     `8 `888'     `8 `888'  d8P'  `Y8b  `888' `888'     `8 `888b.     `8'  d8P'  `Y8b  `888' `888'     `8 d8P'    `Y8 
 888          888          888          888  888           888   888          8 `88b.    8  888           888   888         Y88bo.      
 888oooo8     888oooo8     888oooo8     888  888           888   888oooo8     8   `88b.  8  888           888   888oooo8     `"Y8888o.  
 888    "     888    "     888    "     888  888           888   888    "     8     `88b.8  888           888   888    "         `"Y88b 
 888       o  888          888          888  `88b    ooo   888   888       o  8       `888  `88b    ooo   888   888       o oo     .d8P 
o888ooooood8 o888o        o888o        o888o  `Y8bood8P'  o888o o888ooooood8 o8o        `8   `Y8bood8P'  o888o o888ooooood8 8""88888P'
*/

  // Please put discriminants that have to be compared one next to the other
  vector<DiscToBePlotted> *discriminantsefficienciestobeplotted = new vector<DiscToBePlotted>;

  DiscToBePlotted Q2_Gene_disc;
  Q2_Gene_disc.discriminantvariable = "Q2";
  Q2_Gene_disc.discriminantname = "Q2_Gene";
  Q2_Gene_disc.discriminantprettyname = "Q2_Gen";
  Q2_Gene_disc.discriminantlabel = "Q^{2}_{Gen}";
  Q2_Gene_disc.discriminantdimension = "GeV^{2}";
  Q2_Gene_disc.discriminantbins = 100;
  Q2_Gene_disc.discriminantmin = -5;
  Q2_Gene_disc.discriminantmax = 11;
  Q2_Gene_disc.flag_rocplot = false;
  Q2_Gene_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  Q2_Gene_disc.legxlength = 0.28;
  discriminantsefficienciestobeplotted->push_back(Q2_Gene_disc);

  DiscToBePlotted Q2_Pisa_disc;
  Q2_Pisa_disc.discriminantvariable = "Q2";
  Q2_Pisa_disc.discriminantname = "Q2_Pisa";
  Q2_Pisa_disc.discriminantprettyname = "Q2_Vertices";
  Q2_Pisa_disc.discriminantlabel = "Q^{2}_{Vertices}";
  Q2_Pisa_disc.discriminantdimension = "GeV^{2}";
  Q2_Pisa_disc.discriminantbins = 100;
  Q2_Pisa_disc.discriminantmin = -5;
  Q2_Pisa_disc.discriminantmax = 11;
  Q2_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  Q2_Pisa_disc.legxlength = 0.28;
  discriminantsefficienciestobeplotted->push_back(Q2_Pisa_disc);

  DiscToBePlotted Q2_Jona_disc;
  Q2_Jona_disc.discriminantvariable = "Q2";
  Q2_Jona_disc.discriminantname = "Q2_Jona";
  Q2_Jona_disc.discriminantprettyname = "Q2_Collinear";
  Q2_Jona_disc.discriminantlabel = "Q^{2}_{Collinear}";
  Q2_Jona_disc.discriminantdimension = "GeV^{2}";
  Q2_Jona_disc.discriminantbins = 100;
  Q2_Jona_disc.discriminantmin = -5;
  Q2_Jona_disc.discriminantmax = 11;
  Q2_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  Q2_Jona_disc.legxlength = 0.28;
  discriminantsefficienciestobeplotted->push_back(Q2_Jona_disc);
/*
  DiscToBePlotted Q2_regression_Pisa_disc;
  Q2_regression_Pisa_disc.discriminantvariable = "Q2_regression";
  Q2_regression_Pisa_disc.discriminantname = "Q2_Pisa_regression";
  Q2_regression_Pisa_disc.discriminantprettyname = "Q2_Pisa_regression";
  Q2_regression_Pisa_disc.discriminantlabel = "Q^{2}_{Regression-Vertices}";
  Q2_regression_Pisa_disc.discriminantdimension = "GeV^{2}";
  Q2_regression_Pisa_disc.discriminantbins = 100;
  Q2_regression_Pisa_disc.discriminantmin = -10;
  Q2_regression_Pisa_disc.discriminantmax = 11;
  Q2_regression_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  discriminantsefficienciestobeplotted->push_back(Q2_regression_Pisa_disc);

  DiscToBePlotted Q2_regression_Jona_disc;
  Q2_regression_Jona_disc.discriminantvariable = "Q2_regression";
  Q2_regression_Jona_disc.discriminantname = "Q2_Jona_regression";
  Q2_regression_Jona_disc.discriminantprettyname = "Q2_Jona_regression";
  Q2_regression_Jona_disc.discriminantlabel = "Q^{2}_{Regression-Collinear}";
  Q2_regression_Jona_disc.discriminantdimension = "GeV^{2}";
  Q2_regression_Jona_disc.discriminantbins = 100;
  Q2_regression_Jona_disc.discriminantmin = -10;
  Q2_regression_Jona_disc.discriminantmax = 11;
  Q2_regression_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  discriminantsefficienciestobeplotted->push_back(Q2_regression_Jona_disc);
*/
  DiscToBePlotted MissingM2_Gene_disc;
  MissingM2_Gene_disc.discriminantvariable = "MissingM2";
  MissingM2_Gene_disc.discriminantname = "MissingM2_Gene";
  MissingM2_Gene_disc.discriminantprettyname = "MissingM2_Gen";
  MissingM2_Gene_disc.discriminantlabel = "m^{2}_{Miss,Gen}";
  MissingM2_Gene_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Gene_disc.discriminantbins = 100;
  MissingM2_Gene_disc.discriminantmin = -10;
  MissingM2_Gene_disc.discriminantmax = 11;
  MissingM2_Gene_disc.flag_rocplot = false;
  MissingM2_Gene_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  MissingM2_Gene_disc.legxlength = 0.37;
  discriminantsefficienciestobeplotted->push_back(MissingM2_Gene_disc);

  DiscToBePlotted MissingM2_Pisa_disc;
  MissingM2_Pisa_disc.discriminantvariable = "MissingM2";
  MissingM2_Pisa_disc.discriminantname = "MissingM2_Pisa";
  MissingM2_Pisa_disc.discriminantprettyname = "MissingM2_Vertices";
  MissingM2_Pisa_disc.discriminantlabel = "m^{2}_{Miss,Vertices}";
  MissingM2_Pisa_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Pisa_disc.discriminantbins = 100;
  MissingM2_Pisa_disc.discriminantmin = -10;
  MissingM2_Pisa_disc.discriminantmax = 11;
  MissingM2_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  MissingM2_Pisa_disc.legxlength = 0.37;
  discriminantsefficienciestobeplotted->push_back(MissingM2_Pisa_disc);

  DiscToBePlotted MissingM2_Jona_disc;
  MissingM2_Jona_disc.discriminantvariable = "MissingM2";
  MissingM2_Jona_disc.discriminantname = "MissingM2_Jona";
  MissingM2_Jona_disc.discriminantprettyname = "MissingM2_Collinear";
  MissingM2_Jona_disc.discriminantlabel = "m^{2}_{Miss,Collinear}";
  MissingM2_Jona_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Jona_disc.discriminantbins = 100;
  MissingM2_Jona_disc.discriminantmin = -10;
  MissingM2_Jona_disc.discriminantmax = 11;
  MissingM2_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  MissingM2_Jona_disc.legxlength = 0.37;
  discriminantsefficienciestobeplotted->push_back(MissingM2_Jona_disc);
/*
  DiscToBePlotted MissingM2_Pisa_regression_disc;
  MissingM2_Pisa_regression_disc.discriminantvariable = "MissingM2_regression";
  MissingM2_Pisa_regression_disc.discriminantname = "MissingM2_Pisa_regression";
  MissingM2_Pisa_regression_disc.discriminantprettyname = "MissingM2_Pisa_regression";
  MissingM2_Pisa_regression_disc.discriminantlabel = "m^{2}_{Miss,Vertices}";
  MissingM2_Pisa_regression_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Pisa_regression_disc.discriminantbins = 100;
  MissingM2_Pisa_regression_disc.discriminantmin = -10;
  MissingM2_Pisa_regression_disc.discriminantmax = 11;
  MissingM2_Pisa_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  discriminantsefficienciestobeplotted->push_back(MissingM2_Pisa_regression_disc);

  DiscToBePlotted MissingM2_Jona_regression_disc;
  MissingM2_Jona_regression_disc.discriminantvariable = "MissingM2_regression";
  MissingM2_Jona_regression_disc.discriminantname = "MissingM2_Jona_regression";
  MissingM2_Jona_regression_disc.discriminantprettyname = "MissingM2_Jona_regression";
  MissingM2_Jona_regression_disc.discriminantlabel = "m^{2}_{Miss,Collinear}";
  MissingM2_Jona_regression_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Jona_regression_disc.discriminantbins = 100;
  MissingM2_Jona_regression_disc.discriminantmin = -10;
  MissingM2_Jona_regression_disc.discriminantmax = 11;
  MissingM2_Jona_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  discriminantsefficienciestobeplotted->push_back(MissingM2_Jona_regression_disc);
*/
  DiscToBePlotted MissingPt_Gene_disc;
  MissingPt_Gene_disc.discriminantvariable = "MissingPt";
  MissingPt_Gene_disc.discriminantname = "MissingPt_Gene";
  MissingPt_Gene_disc.discriminantprettyname = "MissingPt_Gen";
  MissingPt_Gene_disc.discriminantlabel = "p^{miss}_{T,Gen}";
  MissingPt_Gene_disc.discriminantdimension = "GeV";
  MissingPt_Gene_disc.discriminantbins = 100;
  MissingPt_Gene_disc.discriminantmin = -10;
  MissingPt_Gene_disc.discriminantmax = 30;
  MissingPt_Gene_disc.flag_rocplot = false;
  MissingPt_Gene_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  MissingPt_Gene_disc.legpos = "TR";
  MissingPt_Gene_disc.legxlength = 0.37;
  discriminantsefficienciestobeplotted->push_back(MissingPt_Gene_disc);

  DiscToBePlotted MissingPt_Pisa_disc;
  MissingPt_Pisa_disc.discriminantvariable = "MissingPt";
  MissingPt_Pisa_disc.discriminantname = "MissingPt_Pisa";
  MissingPt_Pisa_disc.discriminantprettyname = "MissingPt_Vertices";
  MissingPt_Pisa_disc.discriminantlabel = "p^{miss}_{T,Vertices}";
  MissingPt_Pisa_disc.discriminantdimension = "GeV";
  MissingPt_Pisa_disc.discriminantbins = 100;
  MissingPt_Pisa_disc.discriminantmin = -10;
  MissingPt_Pisa_disc.discriminantmax = 30;
  MissingPt_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  MissingPt_Pisa_disc.legpos = "TR";
  MissingPt_Pisa_disc.legxlength = 0.37;
  discriminantsefficienciestobeplotted->push_back(MissingPt_Pisa_disc);

  DiscToBePlotted MissingPt_Jona_disc;
  MissingPt_Jona_disc.discriminantvariable = "MissingPt";
  MissingPt_Jona_disc.discriminantname = "MissingPt_Jona";
  MissingPt_Jona_disc.discriminantprettyname = "MissingPt_Collinear";
  MissingPt_Jona_disc.discriminantlabel = "p^{miss}_{T,Collinear}";
  MissingPt_Jona_disc.discriminantdimension = "GeV";
  MissingPt_Jona_disc.discriminantbins = 100;
  MissingPt_Jona_disc.discriminantmin = -10;
  MissingPt_Jona_disc.discriminantmax = 30;
  MissingPt_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  MissingPt_Jona_disc.legpos = "TR";
  MissingPt_Jona_disc.legxlength = 0.37;
  discriminantsefficienciestobeplotted->push_back(MissingPt_Jona_disc);
/*
  DiscToBePlotted MissingPt_Pisa_regression_disc;
  MissingPt_Pisa_regression_disc.discriminantvariable = "MissingPt_regression";
  MissingPt_Pisa_regression_disc.discriminantname = "MissingPt_Pisa_regression";
  MissingPt_Pisa_regression_disc.discriminantprettyname = "MissingPt_Pisa_regression";
  MissingPt_Pisa_regression_disc.discriminantlabel = "p^{miss}_{T,Vertices}";
  MissingPt_Pisa_regression_disc.discriminantdimension = "GeV";
  MissingPt_Pisa_regression_disc.discriminantbins = 100;
  MissingPt_Pisa_regression_disc.discriminantmin = -10;
  MissingPt_Pisa_regression_disc.discriminantmax = 50;
  MissingPt_Pisa_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  discriminantsefficienciestobeplotted->push_back(MissingPt_Pisa_regression_disc);

  DiscToBePlotted MissingPt_Jona_regression_disc;
  MissingPt_Jona_regression_disc.discriminantvariable = "MissingPt_regression";
  MissingPt_Jona_regression_disc.discriminantname = "MissingPt_Jona_regression";
  MissingPt_Jona_regression_disc.discriminantprettyname = "MissingPt_Jona_regression";
  MissingPt_Jona_regression_disc.discriminantlabel = "p^{miss}_{T,Collinear}";
  MissingPt_Jona_regression_disc.discriminantdimension = "GeV";
  MissingPt_Jona_regression_disc.discriminantbins = 100;
  MissingPt_Jona_regression_disc.discriminantmin = -10;
  MissingPt_Jona_regression_disc.discriminantmax = 50;
  MissingPt_Jona_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  discriminantsefficienciestobeplotted->push_back(MissingPt_Jona_regression_disc);
*/
  DiscToBePlotted ctau_Gene_disc;
  ctau_Gene_disc.discriminantvariable = "ctau";
  ctau_Gene_disc.discriminantname = "ctau_Gene";
  ctau_Gene_disc.discriminantprettyname = "ctau_Gen";
  ctau_Gene_disc.discriminantlabel = "c#tau_{Gen}";
  ctau_Gene_disc.discriminantdimension = "mm";
  ctau_Gene_disc.discriminantbins = 100;
  ctau_Gene_disc.discriminantmin = 0;
  ctau_Gene_disc.discriminantmax = 0.1;
  ctau_Gene_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  ctau_Gene_disc.flag_rocplot = false;
  ctau_Gene_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  ctau_Gene_disc.legpos = "TR";
  ctau_Gene_disc.legxlength = 0.28;
  discriminantsefficienciestobeplotted->push_back(ctau_Gene_disc);

  DiscToBePlotted ctau_Pisa_disc;
  ctau_Pisa_disc.discriminantvariable = "ctau";
  ctau_Pisa_disc.discriminantname = "ctau_Pisa";
  ctau_Pisa_disc.discriminantprettyname = "ctau_Vertices";
  ctau_Pisa_disc.discriminantlabel = "c#tau_{Vertices}";
  ctau_Pisa_disc.discriminantdimension = "mm";
  ctau_Pisa_disc.discriminantbins = 100;
  ctau_Pisa_disc.discriminantmin = 0;
  ctau_Pisa_disc.discriminantmax = 0.1;
  ctau_Pisa_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  ctau_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  ctau_Pisa_disc.legpos = "TR";
  ctau_Pisa_disc.legxlength = 0.28;
  discriminantsefficienciestobeplotted->push_back(ctau_Pisa_disc);

  DiscToBePlotted ctau_Jona_disc;
  ctau_Jona_disc.discriminantvariable = "ctau";
  ctau_Jona_disc.discriminantname = "ctau_Jona";
  ctau_Jona_disc.discriminantprettyname = "ctau_Collinear";
  ctau_Jona_disc.discriminantlabel = "c#tau_{Collinear}";
  ctau_Jona_disc.discriminantdimension = "mm";
  ctau_Jona_disc.discriminantbins = 100;
  ctau_Jona_disc.discriminantmin = 0;
  ctau_Jona_disc.discriminantmax = 0.1;
  ctau_Jona_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  //ctau_Jona_disc.distributionlogscale = 1;
  ctau_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  ctau_Jona_disc.legpos = "TR";
  ctau_Jona_disc.legxlength = 0.28;
  discriminantsefficienciestobeplotted->push_back(ctau_Jona_disc);

  DiscToBePlotted ctau_Pisa_regression_disc;
  ctau_Pisa_regression_disc.discriminantvariable = "ctau_regression";
  ctau_Pisa_regression_disc.discriminantname = "ctau_Pisa_regression";
  ctau_Pisa_regression_disc.discriminantprettyname = "ctau_Vertices_regression";
  ctau_Pisa_regression_disc.discriminantlabel = "c#tau_{Vertices}";
  ctau_Pisa_regression_disc.discriminantdimension = "mm";
  ctau_Pisa_regression_disc.discriminantbins = 100;
  ctau_Pisa_regression_disc.discriminantmin = 0;
  ctau_Pisa_regression_disc.discriminantmax = 0.16;
  ctau_Pisa_regression_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  ctau_Pisa_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  discriminantsefficienciestobeplotted->push_back(ctau_Pisa_regression_disc);

  DiscToBePlotted ctau_Jona_regression_disc;
  ctau_Jona_regression_disc.discriminantvariable = "ctau_regression";
  ctau_Jona_regression_disc.discriminantname = "ctau_Jona_regression";
  ctau_Jona_regression_disc.discriminantprettyname = "ctau_Collinear_regression";
  ctau_Jona_regression_disc.discriminantlabel = "c#tau_{Collinear}";
  ctau_Jona_regression_disc.discriminantdimension = "mm";
  ctau_Jona_regression_disc.discriminantbins = 100;
  ctau_Jona_regression_disc.discriminantmin = 0;
  ctau_Jona_regression_disc.discriminantmax = 0.16;
  ctau_Jona_regression_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  ctau_Jona_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  discriminantsefficienciestobeplotted->push_back(ctau_Jona_regression_disc);
/*
  TreeRDFDiscriminants(filenames, treenames, branchnames, discriminants, cuts_preselection.Data(), false);

  vector<const char *> *var2bestored = new vector<const char *>;
  var2bestored->push_back("Q2_Gene_regression");
  var2bestored->push_back("Q2_Pisa_regression");
  var2bestored->push_back("Q2_Jona_regression");
  var2bestored->push_back("MissingM2_Gene_regression");
  var2bestored->push_back("MissingM2_Pisa_regression");
  var2bestored->push_back("MissingM2_Jona_regression");
  var2bestored->push_back("MissingPt_Gene_regression");
  var2bestored->push_back("MissingPt_Pisa_regression");
  var2bestored->push_back("MissingPt_Jona_regression");  
  var2bestored->push_back("ctau_Gene_regression");
  var2bestored->push_back("ctau_Pisa_regression");
  var2bestored->push_back("ctau_Jona_regression");

  //DiscriminantsPlotter(filenames, treenames, var2bestored, vartobeplotted, false);

  unordered_map<string, pair<string, string>> data_holder;

  data_holder["Signal"] = pair("ManipulatedVariables.root", "JpsiLeptons_regression_BTo3Mu_signal");
  data_holder["Normalization"] = pair("ManipulatedVariables.root", "JpsiLeptons_regression_BTo3Mu_normal");
  data_holder["Signorm"] = pair("ManipulatedVariables.root", "JpsiLeptons_regression_BTo3Mu");
  data_holder["Background"] = pair("ManipulatedVariables.root", "Background_regression_BTo3Mu");
  EfficienciesPlotter(data_holder, discriminantsefficienciestobeplotted);
*/
/*
ooooo      ooo   .oooooo.   ooooooooo.   ooo        ooooo       .o.       ooooo        ooooo  oooooooooooo       .o.       ooooooooooooo ooooo   .oooooo.   ooooo      ooo 
`888b.     `8'  d8P'  `Y8b  `888   `Y88. `88.       .888'      .888.      `888'        `888' d'""""""d888'      .888.      8'   888   `8 `888'  d8P'  `Y8b  `888b.     `8' 
 8 `88b.    8  888      888  888   .d88'  888b     d'888      .8"888.      888          888        .888P       .8"888.          888       888  888      888  8 `88b.    8  
 8   `88b.  8  888      888  888ooo88P'   8 Y88. .P  888     .8' `888.     888          888       d888'       .8' `888.         888       888  888      888  8   `88b.  8  
 8     `88b.8  888      888  888`88b.     8  `888'   888    .88ooo8888.    888          888     .888P        .88ooo8888.        888       888  888      888  8     `88b.8  
 8       `888  `88b    d88'  888  `88b.   8    Y     888   .8'     `888.   888       o  888    d888'    .P  .8'     `888.       888       888  `88b    d88'  8       `888  
o8o        `8   `Y8bood8P'  o888o  o888o o8o        o888o o88o     o8888o o888ooooood8 o888o .8888888888P  o88o     o8888o     o888o     o888o  `Y8bood8P'  o8o        `8
*/

  if (debug_bchybridnorm == false) NormalizationHybrid(debug_bchybridnorm);
  if (debug_jpsixnorm == false) NormalizationJPsiX(debug_jpsixnorm);
  if (debug_jpsimunorm == false) NormalizationJPsiMu(cuts_preselection_pass_3mu, debug_jpsimunorm);

  /*
ooooo                       oooo                        o8o                        
`888'                       `888                        `"'                        
 888  ooo. .oo.    .ooooo.   888  oooo  oooo   .oooo.o oooo  oooo    ooo  .ooooo.  
 888  `888P"Y88b  d88' `"Y8  888  `888  `888  d88(  "8 `888   `88.  .8'  d88' `88b 
 888   888   888  888        888   888   888  `"Y88b.   888    `88..8'   888ooo888 
 888   888   888  888   .o8  888   888   888  o.  )88b  888     `888'    888    .o 
o888o o888o o888o `Y8bod8P' o888o  `V88V"V8P' 8""888P' o888o     `8'     `Y8bod8P'
*/
  if (debug_inclusive == false)
  {

    // Initializing a RootDataFrame from the input tree
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

    // Building variable distribution for various contributions
    // MC
    TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_tau", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiTau->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_mu", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiMu->Sumw2();
    TH1F *histogram_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic0", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_chic0->Sumw2();
    TH1F *histogram_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic1", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_chic1->Sumw2();
    TH1F *histogram_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic2", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_chic2->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_hc", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiHC->Sumw2();
    TH1F *histogram_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_hc", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_Hc->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_tau", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_Psi2stau->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_mu", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_Psi2smu->Sumw2();
    TH1F *histogram_hbtojpsix = (TH1F *)HbToJpsiX.Filter(cuts_preselection_pass)
                                    .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                    .Filter("abs(k_pdgId) == 13")
                                    .Fill<Float_t>(TH1F("pt_miss_hb_jpsix", "y", 50, 0, 30), {"m_miss_sq"})
                                    ->Clone();
    histogram_hbtojpsix->Sumw2();
    TH1F *histogram_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(cuts_preselection_pass)
                                     .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                     .Filter("abs(k_pdgId) == 13")
                                     .Fill<Float_t>(TH1F("pt_miss_hb_jpsimu", "y", 50, 0, 30), {"m_miss_sq"})
                                     ->Clone();
    histogram_hbtojpsimu->Sumw2();

    // Data
    TH1F *histogram_data = (TH1F *)BtoMu_data.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("m_miss_sq_data", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_data->Sumw2();

    // Fakes
    TH1F *histogram_fakes = new TH1F("histogram_fakes", "histogram_fakes", 50, 0, 30);
    *histogram_fakes = (*histogram_data - (*histogram_hbtojpsix * 11.8));

    // Scaling
    Double_t hybrid_normalization = 0.62;     // Normalization of the hybrid sample
    Double_t hbtojpsix_normalization = 11.8;  // Normalization of the hb to jpsix sample
    Double_t hbtojpsimu_normalization = 9.39; // Normalization of the hb to jpsimu sample

    histogram_jpsimumu_is_JPsiTau->Scale(hybrid_normalization);
    histogram_jpsimumu_is_JPsiMu->Scale(hybrid_normalization);
    histogram_jpsimumu_is_chic0->Scale(hybrid_normalization);
    histogram_jpsimumu_is_chic1->Scale(hybrid_normalization);
    histogram_jpsimumu_is_chic2->Scale(hybrid_normalization);
    histogram_jpsimumu_is_JPsiHC->Scale(hybrid_normalization);
    histogram_jpsimumu_is_Hc->Scale(hybrid_normalization);
    histogram_jpsimumu_is_Psi2stau->Scale(hybrid_normalization);
    histogram_jpsimumu_is_Psi2smu->Scale(hybrid_normalization);
    histogram_hbtojpsix->Scale(hbtojpsix_normalization);
    histogram_hbtojpsimu->Scale(hbtojpsimu_normalization);
    histogram_data->Scale(2);

    // Plotting
    TCanvas *inclusive_canvas = new TCanvas("inclusive_canvas", "inclusive_canvas", 1366, 768);
    TPad *inclusive_pad_distribution = new TPad("inclusive_pad", "inclusive_pad", 0.0, 0.3, 1, 1);
    TPad *inclusive_pad_residuals = new TPad("inclusive_pad_residuals", "inclusive_pad_residuals", 0.0, 0.0, 1, 0.3);
    THStack *inclusive_histstack_distribution = new THStack("inclusive_histstack_distribution", "inclusive_histstack_distribution");

    histogram_jpsimumu_is_JPsiTau->SetAxisRange(0, histogram_jpsimumu_is_JPsiTau->GetMaximum(), "Y");
    histogram_jpsimumu_is_JPsiTau->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_JPsiTau);

    histogram_jpsimumu_is_JPsiMu->SetAxisRange(0, histogram_jpsimumu_is_JPsiMu->GetMaximum(), "Y");
    histogram_jpsimumu_is_JPsiMu->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_JPsiMu);

    histogram_jpsimumu_is_chic0->SetAxisRange(0, histogram_jpsimumu_is_chic0->GetMaximum(), "Y");
    histogram_jpsimumu_is_chic0->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_chic0);

    histogram_jpsimumu_is_chic1->SetAxisRange(0, histogram_jpsimumu_is_chic1->GetMaximum(), "Y");
    histogram_jpsimumu_is_chic1->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_chic1);

    histogram_jpsimumu_is_chic2->SetAxisRange(0, histogram_jpsimumu_is_chic2->GetMaximum(), "Y");
    histogram_jpsimumu_is_chic2->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_chic2);

    histogram_jpsimumu_is_JPsiHC->SetAxisRange(0, histogram_jpsimumu_is_JPsiHC->GetMaximum(), "Y");
    histogram_jpsimumu_is_JPsiHC->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_JPsiHC);

    histogram_jpsimumu_is_Hc->SetAxisRange(0, histogram_jpsimumu_is_Hc->GetMaximum(), "Y");
    histogram_jpsimumu_is_Hc->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_Hc);

    histogram_jpsimumu_is_Psi2stau->SetAxisRange(0, histogram_jpsimumu_is_Psi2stau->GetMaximum(), "Y");
    histogram_jpsimumu_is_Psi2stau->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_Psi2stau);

    histogram_jpsimumu_is_Psi2smu->SetAxisRange(0, histogram_jpsimumu_is_Psi2smu->GetMaximum(), "Y");
    histogram_jpsimumu_is_Psi2smu->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_jpsimumu_is_Psi2smu);

    /*histogram_hbtojpsix->SetAxisRange(0, histogram_hbtojpsix->GetMaximum(), "Y");
  histogram_hbtojpsix->SetStats(false);
  inclusive_histstack_distribution->Add(histogram_hbtojpsix);*/

    histogram_hbtojpsimu->SetAxisRange(0, histogram_hbtojpsimu->GetMaximum(), "Y");
    histogram_hbtojpsimu->SetStats(false);
    inclusive_histstack_distribution->Add(histogram_hbtojpsimu);

    histogram_data->SetAxisRange(0, histogram_data->GetMaximum(), "Y");
    histogram_data->SetStats(false);
    histogram_data->SetMarkerStyle(kFullCircle);
    histogram_data->SetMarkerSize(1);
    histogram_data->SetMarkerColor(kBlack);
    histogram_data->SetFillColor(kBlack);
    histogram_data->SetLineColor(kBlack);

    // Drawing
    inclusive_canvas->cd(0);
    inclusive_canvas->Clear();
    inclusive_canvas->Draw();
    inclusive_pad_distribution->SetTopMargin(0.1);
    inclusive_pad_distribution->SetBottomMargin(0.001);
    inclusive_pad_distribution->SetBorderMode(0);
    inclusive_pad_distribution->SetLeftMargin(0.1);
    inclusive_pad_distribution->SetTickx(1);
    inclusive_pad_distribution->SetTicky(1);
    inclusive_pad_residuals->SetTopMargin(0.001);
    inclusive_pad_residuals->SetBottomMargin(0.2);
    inclusive_pad_residuals->SetBorderMode(0);
    inclusive_pad_residuals->SetLeftMargin(0.1);
    inclusive_pad_residuals->SetTickx(1);
    inclusive_pad_residuals->SetTicky(1);
    gStyle->SetOptTitle(kFALSE);
    inclusive_pad_distribution->Draw();
    inclusive_pad_residuals->Draw();

    inclusive_pad_distribution->cd();
    histogram_data->Draw();
    cout << ((TH1F *)inclusive_histstack_distribution->GetStack()->Last())->GetEntries() << endl;
    inclusive_histstack_distribution->Draw("BAR PFC SAME");

    //Adjusting the legend
    TLegend *legend = new TLegend(0.5, 0.81, 0.9, 0.9);
    legend->SetNColumns(4);
    legend->AddEntry(histogram_jpsimumu_is_JPsiTau->GetName(), "B_{c}->J/#Psi#tau", "PFC");
    legend->AddEntry(histogram_jpsimumu_is_JPsiMu->GetName(), "B_{c}->J/#Psi#mu", "PFC");
    legend->AddEntry(histogram_jpsimumu_is_chic0->GetName(), "B_{c}->#Chi_{c0}", "PFC");
    legend->AddEntry(histogram_jpsimumu_is_chic1->GetName(), "B_{c}->#Chi_{c1}", "PFC");
    legend->AddEntry(histogram_jpsimumu_is_chic2->GetName(), "B_{c}->#Chi_{c2}", "PFC");
    legend->AddEntry(histogram_jpsimumu_is_JPsiHC->GetName(), "B_{c}->J/#PsiHc", "PFC");
    legend->AddEntry(histogram_jpsimumu_is_Hc->GetName(), "B_{c}->h_c", "PFC");
    legend->AddEntry(histogram_jpsimumu_is_Psi2stau->GetName(), "B_{c}->#Psi(2s)#tau", "PFC");
    legend->AddEntry(histogram_jpsimumu_is_Psi2smu->GetName(), "B_{c}->#Psi(2s)#mu", "PFC");
    legend->AddEntry(histogram_hbtojpsix->GetName(), "Fakes", "PFC");
    legend->AddEntry(histogram_hbtojpsimu->GetName(), "H_{b}->J#Psi#mu", "PFC");
    legend->AddEntry(histogram_data->GetName(), "Data", "PMC");
    legend->SetTextSize(0.025);
    legend->Draw("SAME");
    gPad->SetLogy(0);

    inclusive_pad_residuals->cd();
    TH1F *inclusive_distributions_ratio = new TH1F();
    inclusive_distributions_ratio = (TH1F *)histogram_data->Clone();
    inclusive_distributions_ratio->Divide(inclusive_distributions_ratio, (TH1F *)inclusive_histstack_distribution->GetStack()->Last());
    inclusive_distributions_ratio->SetStats(false);
    inclusive_distributions_ratio->Draw("M");
    //inclusive_distributions_ratio->SetAxisRange(0, 2, "Y");
    inclusive_distributions_ratio->GetYaxis()->SetNdivisions(5);
    inclusive_distributions_ratio->GetXaxis()->SetTitle("p_{T, Vec}^{miss}");
    inclusive_distributions_ratio->GetXaxis()->SetTitleSize(0.1);
    inclusive_distributions_ratio->GetXaxis()->SetTitleOffset(0.7);
    inclusive_distributions_ratio->GetXaxis()->SetLabelSize(0.09);
    inclusive_distributions_ratio->GetYaxis()->SetTitle("Obs/Exp");
    inclusive_distributions_ratio->GetYaxis()->SetTitleSize(0.1);
    inclusive_distributions_ratio->GetYaxis()->SetTitleOffset(0.4);
    inclusive_distributions_ratio->GetYaxis()->SetLabelSize(0.09);
    inclusive_pad_residuals->Modified();

    inclusive_canvas->Print("prova_ratioplot.png");
    inclusive_canvas->Clear();
    legend->Clear();
  }

  /*
 oooooooooooo oooooooooooo ooooooooo.     .oooooo.   ooooooooo.   oooooooooooo       .o.       oooo    oooo 
d'""""""d888' `888'     `8 `888   `Y88.  d8P'  `Y8b  `888   `Y88. `888'     `8      .888.      `888   .8P'  
      .888P    888          888   .d88' 888      888  888   .d88'  888             .8"888.      888  d8'    
     d888'     888oooo8     888ooo88P'  888      888  888ooo88P'   888oooo8       .8' `888.     88888[      
   .888P       888    "     888`88b.    888      888  888          888    "      .88ooo8888.    888`88b.    
  d888'    .P  888       o  888  `88b.  `88b    d88'  888          888       o  .8'     `888.   888  `88b.  
.8888888888P  o888ooooood8 o888o  o888o  `Y8bood8P'  o888o        o888ooooood8 o88o     o8888o o888o  o888o
*/
  if (debug_peak == false)
  {
    // Binning
    Double_t nbins = 100, minbin = -0.01, maxbin = 0.02;
    Int_t n_events_tobeshown = 1000;
    char zero_peak_filter[1500];
    strcpy(zero_peak_filter, cuts_preselection+"&&"+pass_region_definition);
    strcat(zero_peak_filter, "& m_miss_sq < 0.02");

    // Building variable distribution for various contributions
    // MC Signal and normalizaton
    ROOT::RDataFrame BtoJpsiMuMu_is_JPsiTau("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_tau_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_JPsiMu("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_mu_merged.root");
    TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_tau", "y", nbins, minbin, maxbin), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiTau->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_mu", "y", nbins, minbin, maxbin), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiMu->Sumw2();

    // MC Background: Build-up and sum of the components
    ROOT::RDataFrame BtoJpsiMuMu_is_Chic0("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_chic0_mu_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_Chic1("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_chic1_mu_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_Chic2("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_chic2_mu_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_JPsiHC("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_hc_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_Hc("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_hc_mu_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_Psi2stau("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_psi2s_tau_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_Psi2smu("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_psi2s_mu_merged.root");
    ROOT::RDataFrame HbToJpsiX("BTo3Mu", "InputFiles/HbToJPsiMuMu_ptmax_merged.root");
    ROOT::RDataFrame HbToJPsiMu("BTo3Mu", "InputFiles/HbToJPsiMuMu3MuFilter_ptmax_merged.root");
    TH1F *histogram_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(zero_peak_filter)
                                            .Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic0", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                            ->Clone();
    histogram_jpsimumu_is_chic0->Sumw2();
    TH1F *histogram_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(zero_peak_filter)
                                            .Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic1", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                            ->Clone();
    histogram_jpsimumu_is_chic1->Sumw2();
    TH1F *histogram_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(zero_peak_filter)
                                            .Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic2", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                            ->Clone();
    histogram_jpsimumu_is_chic2->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(zero_peak_filter)
                                             .Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_hc", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                             ->Clone();
    histogram_jpsimumu_is_JPsiHC->Sumw2();
    TH1F *histogram_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(zero_peak_filter)
                                         .Fill<Float_t>(TH1F("pt_miss_jpsimu_is_hc", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                         ->Clone();
    histogram_jpsimumu_is_Hc->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(zero_peak_filter)
                                               .Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_tau", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                               ->Clone();
    histogram_jpsimumu_is_Psi2stau->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(zero_peak_filter)
                                              .Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_mu", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                              ->Clone();
    histogram_jpsimumu_is_Psi2smu->Sumw2();
    TH1F *histogram_hbtojpsix = (TH1F *)HbToJpsiX.Filter(zero_peak_filter)
                                    .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                    .Filter("abs(k_pdgId) != 13")
                                    .Fill<Float_t>(TH1F("pt_miss_hb_jpsix", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                    ->Clone();
    histogram_hbtojpsix->Sumw2();
    TH1F *histogram_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(zero_peak_filter)
                                     .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                     .Filter("abs(k_pdgId) == 13")
                                     .Fill<Float_t>(TH1F("pt_miss_hb_jpsimu", "y", nbins, minbin, maxbin), {"m_miss_sq"})
                                     ->Clone();
    Double_t hybrid_normalization = 0.62;     // Normalization of the hybrid sample
    Double_t hbtojpsix_normalization = 11.8;  // Normalization of the hb to jpsix sample
    Double_t hbtojpsimu_normalization = 9.05; // Normalization of the hb to jpsimu sample
    histogram_jpsimumu_is_chic0->Scale(hybrid_normalization);
    histogram_jpsimumu_is_chic1->Scale(hybrid_normalization);
    histogram_jpsimumu_is_chic2->Scale(hybrid_normalization);
    histogram_jpsimumu_is_JPsiHC->Scale(hybrid_normalization);
    histogram_jpsimumu_is_Hc->Scale(hybrid_normalization);
    histogram_jpsimumu_is_Psi2stau->Scale(hybrid_normalization);
    histogram_jpsimumu_is_Psi2smu->Scale(hybrid_normalization);
    histogram_hbtojpsix->Scale(hbtojpsix_normalization);
    histogram_hbtojpsimu->Scale(hbtojpsimu_normalization);

    THStack *histstack_bkg_distribution = new THStack("histstack_bkg_distribution", "histstack_bkg_distribution");
    histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic0);
    histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic1);
    histstack_bkg_distribution->Add(histogram_jpsimumu_is_chic2);
    histstack_bkg_distribution->Add(histogram_jpsimumu_is_JPsiHC);
    histstack_bkg_distribution->Add(histogram_jpsimumu_is_Hc);
    histstack_bkg_distribution->Add(histogram_jpsimumu_is_Psi2stau);
    histstack_bkg_distribution->Add(histogram_jpsimumu_is_Psi2smu);
    histstack_bkg_distribution->Add(histogram_hbtojpsix);
    histstack_bkg_distribution->Add(histogram_hbtojpsimu);
    //histstack_bkg_distribution->Add(histogram_fakes)
    TH1F *histogram_jpsimu_is_bkg = (TH1F *)histstack_bkg_distribution->GetStack()->Last();

    // Data
    ROOT::RDataFrame BtoMu_data("BTo3Mu", "InputFiles/data_ptmax_merged.root");
    TH1F *histogram_data = (TH1F *)BtoMu_data.Filter(cuts_preselection_pass).Fill<Float_t>(TH1F("m_miss_sq_data", "y", nbins, minbin, maxbin), {"m_miss_sq"})->Clone();
    histogram_data->Sumw2();

    TCanvas peakzero_canvas("peakzero_canvas", "peakzero_canvas", 1360, 768);
    histogram_jpsimu_is_bkg->Draw();
    peakzero_canvas.Print("peakzero.png");

    cout << endl
         << "Bc to chic0 and muon" << endl;
    cout << BtoJpsiMuMu_is_Chic0.Filter(zero_peak_filter)
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;

    cout << endl
         << "Bc to chic1 andmuon" << endl;
    cout << BtoJpsiMuMu_is_Chic1.Filter(zero_peak_filter)
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;
    cout << endl
         << "Bc to chic2 and muon" << endl;
    cout << BtoJpsiMuMu_is_Chic2.Filter(zero_peak_filter)
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;
    cout << endl
         << "Bc to JPsi plus Hc" << endl;
    cout << BtoJpsiMuMu_is_JPsiHC.Filter(zero_peak_filter)
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;
    cout << endl
         << "Bc to Hc and muon" << endl;
    cout << BtoJpsiMuMu_is_Hc.Filter(zero_peak_filter)
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;
    cout << endl
         << "Bc to Psi2s and tau" << endl;
    cout << BtoJpsiMuMu_is_Psi2stau.Filter(zero_peak_filter)
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;
    cout << endl
         << "Bc to Psi2s and mu" << endl;
    cout << BtoJpsiMuMu_is_Psi2smu.Filter(zero_peak_filter)
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;
    cout << endl
         << "Hb to jpsi plus x" << endl;
    cout << HbToJpsiX.Filter(zero_peak_filter)
                .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                .Filter("abs(k_pdgId) != 13")
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;
    cout << endl
         << "Hb to jpsi plus mu" << endl;
    cout << HbToJPsiMu.Filter(zero_peak_filter)
                .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                .Filter("abs(k_pdgId) == 13")
                .Display({"mu1_pdgId", "mu1_mother_pdgId", "mu1_grandmother_pdgId", "mu1_grandgrandmother_pdgId", "k_genpdgId", "k_mother_pdgId", "k_grandmother_pdgId", "m_miss_sq"}, n_events_tobeshown)
                ->AsString()
         << endl;
  }
  /*
    oooooooooooo           oooo                  
    888'     `8           `888                  
    888          .oooo.    888  oooo   .ooooo.  
    888oooo8    `P  )88b   888 .8P'   d88' `88b 
    888    "     .oP"888   888888.    888ooo888 
    888         d8(  888   888 `88b.  888    .o 
    o888o        Y888""8o o888o o888o `Y8bod8P'
  */
  if (debug_nnfakes == false)
  {
    TString     preselection = "mu1pt>4 &&"
                               "mu2pt>4 &&"
                               "kpt>2.5 &&"
                               "abs(mu1eta)<2.5 &&"
                               "abs(mu2eta)<2.5 &&"
                               "abs(keta)<2.5 &&"
                               "bvtx_svprob>1e-4 &&"
                               "jpsivtx_svprob>1e-2 &&"
                               "Bmass<6.3 &&"
                               "mu1_mediumID>0.5 &&"
                               "mu2_mediumID>0.5 &&"
                               "dr12>0.01 &&"
                               "dr13>0.01 &&"
                               "dr23>0.01 &&"
                               "abs(mu1_dz-mu2_dz)<0.2 &&"
                               "abs(mu1_dz-k_dz)<0.2 &&"
                               "abs(mu2_dz-k_dz)<0.2 &&"
                               "abs(k_dxy)<0.05 &&"
                               "abs(mu1_dxy)<0.05 &&"
                               "abs(mu2_dxy)<0.05 &&"
                               "abs(jpsi_mass-3.0969)<0.1 &&"
                               "mu1_isFromMuT>0.5 &&"
                               "mu2_isFromMuT>0.5 &&"
                               "mu1_isFromJpsi_MuT>0.5 &&"
                               "mu2_isFromJpsi_MuT>0.5 &&"
                               "k_isFromMuT>0.5";

    const char *pass_id = "k_mediumID>0.5 && k_raw_db_corr_iso03_rel<0.2";
    const char *fail_id = "!(k_mediumID>0.5 && k_raw_db_corr_iso03_rel<0.2)";

    TString preselection_mc = TString::Format("%s && !(abs(k_genpdgId)==13)", preselection.Data());
    TString finalselection_mc = (string)(cuts_final+"&& !(abs(k_genpdgId)==13)");
    
    vector<string> features = {
        "kpt",
        "keta",
        "kphi",
        "kmass",
        "mu1pt",
        "mu1eta",
        "mu1phi",
        "mu1mass",
        "mu2pt",
        "mu2eta",
        "mu2mass",
        "mu2phi",
        //"dr12",
        //"dr13",
        //"dr23",
        "Bmass"
        //'Bpt_reco'
    };

    vector<TMVAMethod> tmvamethods;
    {
      TMVAMethod kmlp1;
      kmlp1.tmvamethodname = "kmlp1";
      TString config  = "CreateMVAPdfs:VarTransform=N:NCycles=10000:HiddenLayers=N+2,N+1,N:NeuronType=sigmoid:EstimatorType=CE:";
              config += "TrainingMethod=BP:LearningRate=0.1:DecayRate=0.05:ConvergenceTests=50";
      kmlp1.tmvamethodconfiguration = config;
      kmlp1.tmvamethodtype = TMVA::Types::EMVA::kMLP;
      tmvamethods.push_back(kmlp1);
    }
    {
      TMVAMethod kmlp2;
      kmlp2.tmvamethodname = "kmlp2";
      TString config  = "CreateMVAPdfs:VarTransform=N:NCycles=5000:HiddenLayers=N+5:NeuronType=sigmoid:EstimatorType=CE:";
              config += "TrainingMethod=BP:LearningRate=0.05:DecayRate=0.05:CalculateErrors=True:ConvergenceTests=10";
      kmlp2.tmvamethodconfiguration = config;
      kmlp2.tmvamethodtype = TMVA::Types::EMVA::kMLP;
      //tmvamethods.push_back(kmlp2);
    }
    {
      TMVAMethod kdl1;
      kdl1.tmvamethodname = "kdl1";
      TString config  = "CreateMVAPdfs:!H:V";
              config += ":VarTransform=N";
              config += ":ErrorStrategy=CROSSENTROPY";
              config += ":WeightInitialization=XAVIERUNIFORM";
              config += ":Layout=TANH|100, TANH|50, TANH|10, LINEAR";
              config += ":TrainingStrategy=LearningRate=1e-2,Momentum=0.5, Repetitions=1,ConvergenceSteps=100,BatchSize=100,DropConfig=0.0+0.5+0.5+0.0";
              config += ",WeightDecay=0.001,Regularization=L2,TestRepetitions=15,Multithreading=True";
      kdl1.tmvamethodconfiguration = config;
      kdl1.tmvamethodtype = TMVA::Types::EMVA::kDL;
      //tmvamethods.push_back(kdl1);
    }
    {
      TMVAMethod kbdt1;
      kbdt1.tmvamethodname = "kbdt1";
      TString config  = "CreateMVAPdfs:Ntrees=100:MaxDepth=5:MinNodeSize=5%:nCuts=-1:BoostType=RealAdaBoost:UseRandomisedTrees=True";
              config += ":UseNvars=3:UsePoissonNvars=True";
      kbdt1.tmvamethodconfiguration = config;
      kbdt1.tmvamethodtype = TMVA::Types::EMVA::kBDT;
      //tmvamethods.push_back(kbdt1);
    }

    vector<string> *fake_factory_names = new vector<string>; /*!< 0 = fake mc path, 1-2= in/out file, 3-4= pass/fail trees, 5-6 pass/fail closure test trees*/
    const char *mc_path = "InputFiles/HbToJPsiMuMu_ptmax_merged.root"; 
    fake_factory_names->push_back(mc_path);
    string MVA_Fakes_Input_File = "MVA_Fakes_Input.root";
    fake_factory_names->push_back(MVA_Fakes_Input_File);
    string MVA_Fakes_Output_File = "MVA_Fakes_Output.root";
    fake_factory_names->push_back(MVA_Fakes_Output_File);
    string MVA_PassFakes_Input_Tree = "Pass_events";
    fake_factory_names->push_back(MVA_PassFakes_Input_Tree);
    string MVA_FailFakes_Input_Tree = "Fail_events";
    fake_factory_names->push_back(MVA_FailFakes_Input_Tree);
    string MVA_ClosurePassFakes_Input_Tree = "ClosurePass_events";
    fake_factory_names->push_back(MVA_ClosurePassFakes_Input_Tree);
    string MVA_ClosureFailFakes_Input_Tree = "ClosureFail_events";
    fake_factory_names->push_back(MVA_ClosureFailFakes_Input_Tree);
    FakeNNFactory(finalselection_mc, fake_factory_names, pass_id, features, vartobefit, tmvamethods);

    // Initializing variables to set up the reader
    vector<Float_t> Input_feature_var(features.size());
    unordered_map<string, Float_t> MVA_feature_var(features.size());
    //FakeNNClosureTest(fake_factory_names, features, Input_feature_var, MVA_feature_var, vartobefit, tmvamethods);
    FakeNNReader(filenames, fake_factory_names, cuts_final, features, Input_feature_var, MVA_feature_var, tmvamethods);
  }
  
  vector<MarkedNames> fnames;
  {
    MarkedNames filedati;
    filedati.name = "InputFiles/data_ptmax_merged.root";
    filedati.datatype = "Data";
    filedati.treetype = "BTo2MuP";
    fnames.push_back(filedati);
  }
  {
    MarkedNames filebcjpsipimc;
    filebcjpsipimc.name = "InputFiles/BcToJPsiMuMu_is_jpsi_pi_merged_v5.root";
    filebcjpsipimc.datatype = "MC";
    filebcjpsipimc.treetype = "BTo2MuP";
    fnames.push_back(filebcjpsipimc);
  }
  // Input trees to be processed (1 for real, 0 for MC)
  vector<MarkedNames> tnames;
  tnames.push_back(MarkedNames("BTo2MuP", 0));

        TString mass_type = "bvtx_fit_mass";
        TString jpsipi_cut = "bvtx_fit_mass>6 &&";
        jpsipi_cut += "bvtx_fit_mass<6.6 &&";
        jpsipi_cut += "mu1pt>4.5 &&"
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
        TString jpsipi_cut_mc = jpsipi_cut;
        jpsipi_cut_mc += "&& abs(k_genpdgId)==211 &&"
                        "abs(k_mother_pdgId)==541 &&"
                        "abs(mu1_genpdgId)==13 &&"
                        "abs(mu1_mother_pdgId)==443 &&"
                        "abs(mu2_genpdgId)==13 &&"
                        "abs(mu2_mother_pdgId)==443 &&"
                        "abs(mu1_grandmother_pdgId)==541 &&"
                        "abs(mu2_grandmother_pdgId)==541";

  TString cuts_varcomparer = "mu1pt>3 && "
                                                "mu2pt>3 && "
                                                "kpt>2.5 && "
                                                "abs(mu1eta)<2.5 && "
                                                "abs(mu2eta)<2.5 && "
                                                "abs(keta)<2.5 && "
                                                "bvtx_svprob>1e-4 && "
                                                "jpsivtx_svprob>1e-2 && "
                                                "mu1_mediumID>0.5 && "
                                                "mu2_mediumID>0.5 && "
                                                "Bpt_reco>15 && "
                                                "dr12>0.01 && "
                                                "dr13>0.01 && "
                                                "dr23>0.01 && "
                                                "abs(mu1_dz-mu2_dz)<0.2 && "
                                                "abs(mu1_dz-k_dz)<0.2 && "
                                                "abs(mu2_dz-k_dz)<0.2 && "
                                                "abs(k_dxy)<0.05 && "
                                                "abs(mu1_dxy)<0.05 && "
                                                "abs(mu2_dxy)<0.05 && "
                                                "bvtx_cos2D>0.995 && "
                                                "abs(jpsi_mass-3.0969)<0.1 && "
                                                "Bmass<10. && "
                                                "jpsivtx_cos2D>0.99 ";

  // Vars to be compared
  vector<VarToBePlotted>* vnames = new vector<VarToBePlotted> ;
  // Squared transferred momentum: B_c_pisa
  VarToBePlotted bvtx_lxy_sig_compared;
  bvtx_lxy_sig_compared.varname = "bvtx_lxy_sig";
  bvtx_lxy_sig_compared.varexpression = "bvtx_lxy_sig";
  bvtx_lxy_sig_compared.tobeprinted = false;
  bvtx_lxy_sig_compared.comparisonflag = true;
  bvtx_lxy_sig_compared.comparisonname = "q2_comparison";
  bvtx_lxy_sig_compared.comparisonlabel = "Q^{2}[GeV^{2}]";
  bvtx_lxy_sig_compared.xminlim = 0;
  bvtx_lxy_sig_compared.xmaxlim = 15;
  bvtx_lxy_sig_compared.vardimension = "";
  bvtx_lxy_sig_compared.comparisonrationame = "ratio_q2pisa_over_q2gene";
  bvtx_lxy_sig_compared.comparisonratioexpr = "bvtx_lxy_sig_compared / Q2_Gene";
  bvtx_lxy_sig_compared.comparisonratiotitle = "Q^{2}_{Pisa} over Q^{2}_{gene}";
  bvtx_lxy_sig_compared.comparisonratioxlabel = "#frac{Q^{2}_{Pisa}}{Q^{2}_{gene}}";
  bvtx_lxy_sig_compared.comparisonscattername = "scatter_q2pisa_vs_q2gene";
  bvtx_lxy_sig_compared.comparisonscattervars = {"Q2_Gene", "bvtx_lxy_sig_compared"};
  bvtx_lxy_sig_compared.comparisonscattertitle = "Q^{2}_{Pisa} versus Q^{2}_{gene}";
  bvtx_lxy_sig_compared.comparisonscatterxlabel = "Q^{2}_{gene}[GeV^{2}]";
  bvtx_lxy_sig_compared.comparisonscatterylabel = "Q^{2}_{Pisa}[GeV^{2}]";
  bvtx_lxy_sig_compared.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  bvtx_lxy_sig_compared.cut = jpsipi_cut;
  bvtx_lxy_sig_compared.cut = jpsipi_cut_mc;
  vnames->push_back(bvtx_lxy_sig_compared);

  VarToBePlotted bvtx_cos2D_compared;
  bvtx_cos2D_compared.varname = "bvtx_cos2D";
  bvtx_cos2D_compared.varexpression = "bvtx_cos2D";
  bvtx_cos2D_compared.tobeprinted = false;
  bvtx_cos2D_compared.comparisonflag = true;
  bvtx_cos2D_compared.comparisonname = "q2_comparison";
  bvtx_cos2D_compared.comparisonlabel = "Q^{2}[GeV^{2}]";
  bvtx_cos2D_compared.xminlim = 0.9988;
  bvtx_cos2D_compared.xmaxlim = 1;
  bvtx_cos2D_compared.vardimension = "";
  bvtx_cos2D_compared.comparisonrationame = "ratio_q2pisa_over_q2gene";
  bvtx_cos2D_compared.comparisonratioexpr = "bvtx_cos2D_compared / Q2_Gene";
  bvtx_cos2D_compared.comparisonratiotitle = "Q^{2}_{Pisa} over Q^{2}_{gene}";
  bvtx_cos2D_compared.comparisonratioxlabel = "#frac{Q^{2}_{Pisa}}{Q^{2}_{gene}}";
  bvtx_cos2D_compared.comparisonscattername = "scatter_q2pisa_vs_q2gene";
  bvtx_cos2D_compared.comparisonscattervars = {"Q2_Gene", "bvtx_cos2D_compared"};
  bvtx_cos2D_compared.comparisonscattertitle = "Q^{2}_{Pisa} versus Q^{2}_{gene}";
  bvtx_cos2D_compared.comparisonscatterxlabel = "Q^{2}_{gene}[GeV^{2}]";
  bvtx_cos2D_compared.comparisonscatterylabel = "Q^{2}_{Pisa}[GeV^{2}]";
  bvtx_cos2D_compared.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  bvtx_cos2D_compared.cut = jpsipi_cut.Data();
  bvtx_cos2D_compared.cut = jpsipi_cut_mc.Data();
  vnames->push_back(bvtx_cos2D_compared);

  VarToBePlotted mu2_zsig_compared;
  mu2_zsig_compared.varname = "mu2_zsig";
  mu2_zsig_compared.varexpression = "mu2_dz/mu2_dzErr";
  mu2_zsig_compared.tobeprinted = false;
  mu2_zsig_compared.comparisonflag = true;
  mu2_zsig_compared.comparisonname = "q2_comparison";
  mu2_zsig_compared.comparisonlabel = "Q^{2}[GeV^{2}]";
  mu2_zsig_compared.xminlim = 0;
  mu2_zsig_compared.xmaxlim = 15;
  mu2_zsig_compared.vardimension = "";
  mu2_zsig_compared.comparisonrationame = "ratio_q2pisa_over_q2gene";
  mu2_zsig_compared.comparisonratioexpr = "mu2_zsig_compared / Q2_Gene";
  mu2_zsig_compared.comparisonratiotitle = "Q^{2}_{Pisa} over Q^{2}_{gene}";
  mu2_zsig_compared.comparisonratioxlabel = "#frac{Q^{2}_{Pisa}}{Q^{2}_{gene}}";
  mu2_zsig_compared.comparisonscattername = "scatter_q2pisa_vs_q2gene";
  mu2_zsig_compared.comparisonscattervars = {"Q2_Gene", "mu2_zsig_compared"};
  mu2_zsig_compared.comparisonscattertitle = "Q^{2}_{Pisa} versus Q^{2}_{gene}";
  mu2_zsig_compared.comparisonscatterxlabel = "Q^{2}_{gene}[GeV^{2}]";
  mu2_zsig_compared.comparisonscatterylabel = "Q^{2}_{Pisa}[GeV^{2}]";
  mu2_zsig_compared.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  mu2_zsig_compared.cut = jpsipi_cut.Data();
  mu2_zsig_compared.cut = jpsipi_cut_mc.Data();
  vnames->push_back(mu2_zsig_compared);

  JPsiReweighting(fnames, treenames, vnames);
  return 0;
}