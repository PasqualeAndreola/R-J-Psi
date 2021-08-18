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
#include "HeaderFiles/MarkedNames.h"
#include "HeaderFiles/NormalizationHybrid.h"
#include "HeaderFiles/NormalizationJPsi_X.h"
#include "HeaderFiles/NormalizationJPsi_Mu.h"
#include "HeaderFiles/PrintFuncInfo.h"
#include "HeaderFiles/TMVAMethod.h"
#include "HeaderFiles/TreeRDFDiscriminants.h"
#include "HeaderFiles/TreeReader.h"
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

//It can be useful to use these namespaces
using namespace std;
using namespace RooFit;

int main(int argc, char *argv[])
{
  gErrorIgnoreLevel = kWarning;
  /*int colors_palette[3];
  colors_palette[0] = kBlue;
  colors_palette[1] = kGreen + 1;
  colors_palette[2] = kRed;
  gStyle->SetPalette(3, colors_palette);*/
  bool debug_bchybridnorm = true;
  bool debug_jpsixnorm = true;
  bool debug_jpsimunorm = true;
  bool debug_inclusive = true;
  bool debug_fit = true;
  bool debug_peak = true;
  bool debug_nnfakes = false;

  cout << endl
       << "Program compiled correctly." << endl
       << "Starting now..." << endl
       << endl;

  // Input quantities
  // Input files, relative flags (1 for real data, 0 for MC) and a short name (not necessary)
  vector<MarkedNames> filenames;
  filenames.push_back(MarkedNames("output_sig_mix.root", 0, "", "JpsiLeptons_regression"));
  filenames.push_back(MarkedNames("output_bkg.root", 0, "", "Background_regression"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_merged.root", 0, "", "JpsiMuNu"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_tau_merged.root", 0, "", "JpsiTauNu"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_chic0_mu_merged.root", 0, "", "Chic0"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_chic1_mu_merged.root", 0, "", "Chic1"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_chic2_mu_merged.root", 0, "", "Chic2"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_hc_merged.root", 0, "", "JPsiHC"));
  //filenames.push_back(MarkedNames("data_ptmax_merged.root", 1, "", "Data"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint.root", 0, "", "JpsiMuNu_Jpsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_tau_massConstraint.root", 0, "", "JpsiTauNu_JPsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_NONmassConstraint_50bits_precision.root", 0, "", "JpsiMuNu_HighPrecision"));
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

  const char *cuts_preselection_mediumid_true = "mu1pt>3 && "
                                                "mu2pt>3 && "
                                                "kpt>2.5 && "
                                                "abs(mu1eta)<2.5 && "
                                                "abs(mu2eta)<2.5 && "
                                                "abs(keta)<2.5 && "
                                                "bvtx_svprob>1e-4 && "
                                                "jpsivtx_svprob>1e-2 && "
                                                "mu1_mediumID>0.5 && "
                                                "mu2_mediumID>0.5 && "
                                                "k_mediumID>0.5 && "
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
                                                "jpsivtx_cos2D>0.99 && "
                                                "mu1_isFromMuT>0.5 && "
                                                "mu2_isFromMuT>0.5 && "
                                                "mu1_isFromJpsi_MuT>0.5 && "
                                                "mu2_isFromJpsi_MuT>0.5 && "
                                                "k_isFromMuT>0.5 ";

  const char *cuts_preselection_mediumid_false = "mu1pt>3 && "
                                                 "mu2pt>3 && "
                                                 "kpt>2.5 && "
                                                 "abs(mu1eta)<2.5 && "
                                                 "abs(mu2eta)<2.5 && "
                                                 "abs(keta)<2.5 && "
                                                 "bvtx_svprob>1e-4 && "
                                                 "jpsivtx_svprob>1e-2 && "
                                                 "mu1_mediumID>0.5 && "
                                                 "mu2_mediumID>0.5 && "
                                                 "k_mediumID<0.5 && "
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
                                                 "jpsivtx_cos2D>0.99 && "
                                                 "mu1_isFromMuT>0.5 && "
                                                 "mu2_isFromMuT>0.5 && "
                                                 "mu1_isFromJpsi_MuT>0.5 && "
                                                 "mu2_isFromJpsi_MuT>0.5 && "
                                                 "k_isFromMuT>0.5 ";

  const char *cuts_final_mediumid_true = "Bmass  <  6.0 && "
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

  const char *cuts_final_mediumid_false = "Bmass  <  6.0 && "
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
                                          "k_mediumID<0.5";

  vector<VarToBeFitted> *vartobefit = new vector<VarToBeFitted>;
  {
    VarToBeFitted fit_missingmasssquared;
    fit_missingmasssquared.vartobefitted = "m_miss_sq";
    fit_missingmasssquared.varname = "m_{miss}^{2}";
    fit_missingmasssquared.vardimension = "GeV^{2}";
    fit_missingmasssquared.bins = 25;
    fit_missingmasssquared.min_bin = 0; fit_missingmasssquared.max_bin = 10;
    fit_missingmasssquared.legpos = "TR";
    vartobefit->push_back(fit_missingmasssquared);
  }
  { 
    VarToBeFitted fit_ptmissvec;
    fit_ptmissvec.vartobefitted = "pt_miss_vec";
    fit_ptmissvec.varname = "p_{T,vec}^{miss}";
    fit_ptmissvec.vardimension = "GeV";
    fit_ptmissvec.bins = 25;
    fit_ptmissvec.min_bin = 0; fit_ptmissvec.max_bin = 30;
    fit_ptmissvec.legpos = "TR";
    vartobefit->push_back(fit_ptmissvec);
  }
  {
    VarToBeFitted fit_qsquared;
    fit_qsquared.vartobefitted = "Q_sq";
    fit_qsquared.varname = "Q^{2}";
    fit_qsquared.vardimension = "GeV^{2}";
    fit_qsquared.bins = 25;
    fit_qsquared.min_bin = 3; fit_qsquared.max_bin = 10.5;
    fit_qsquared.legpos = "TL";
    vartobefit->push_back(fit_qsquared);
  }
  {
    VarToBeFitted fit_emustar;
    fit_emustar.vartobefitted = "E_mu_star";
    fit_emustar.varname = "E_{#mu}^{*}";
    fit_emustar.vardimension = "GeV";
    fit_emustar.bins = 25;
    fit_emustar.min_bin = 0; fit_emustar.max_bin = 2.5;
    fit_emustar.legpos = "TR";
    vartobefit->push_back(fit_emustar);
  }
  for (long unsigned int i = 0; i < vartobefit->size(); i++)
  {
    vartobefit->at(i).cut_mediumid_true = (string)cuts_final_mediumid_true;
    vartobefit->at(i).cut_mediumid_false = (string)cuts_final_mediumid_false;
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
  Q2_Gene_disc.discriminantlabel = "Q^{2}_{Gene}";
  Q2_Gene_disc.discriminantdimension = "GeV^{2}";
  Q2_Gene_disc.discriminantbins = 100;
  Q2_Gene_disc.discriminantmin = -10;
  Q2_Gene_disc.discriminantmax = 11;
  Q2_Gene_disc.flag_rocplot = false;
  Q2_Gene_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  discriminantsefficienciestobeplotted->push_back(Q2_Gene_disc);

  DiscToBePlotted Q2_Pisa_disc;
  Q2_Pisa_disc.discriminantvariable = "Q2";
  Q2_Pisa_disc.discriminantname = "Q2_Pisa";
  Q2_Pisa_disc.discriminantlabel = "Q^{2}_{Pisa}";
  Q2_Pisa_disc.discriminantdimension = "GeV^{2}";
  Q2_Pisa_disc.discriminantbins = 100;
  Q2_Pisa_disc.discriminantmin = -10;
  Q2_Pisa_disc.discriminantmax = 11;
  Q2_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  discriminantsefficienciestobeplotted->push_back(Q2_Pisa_disc);

  DiscToBePlotted Q2_Jona_disc;
  Q2_Jona_disc.discriminantvariable = "Q2";
  Q2_Jona_disc.discriminantname = "Q2_Jona";
  Q2_Jona_disc.discriminantlabel = "Q^{2}_{Jona}";
  Q2_Jona_disc.discriminantdimension = "GeV^{2}";
  Q2_Jona_disc.discriminantbins = 100;
  Q2_Jona_disc.discriminantmin = -10;
  Q2_Jona_disc.discriminantmax = 11;
  Q2_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  discriminantsefficienciestobeplotted->push_back(Q2_Jona_disc);

  DiscToBePlotted Q2_regression_Pisa_disc;
  Q2_regression_Pisa_disc.discriminantvariable = "Q2_regression";
  Q2_regression_Pisa_disc.discriminantname = "Q2_Pisa_regression";
  Q2_regression_Pisa_disc.discriminantlabel = "Q^{2}_{Regression-Pisa}";
  Q2_regression_Pisa_disc.discriminantdimension = "GeV^{2}";
  Q2_regression_Pisa_disc.discriminantbins = 100;
  Q2_regression_Pisa_disc.discriminantmin = -10;
  Q2_regression_Pisa_disc.discriminantmax = 11;
  Q2_regression_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  discriminantsefficienciestobeplotted->push_back(Q2_regression_Pisa_disc);

  DiscToBePlotted Q2_regression_Jona_disc;
  Q2_regression_Jona_disc.discriminantvariable = "Q2_regression";
  Q2_regression_Jona_disc.discriminantname = "Q2_Jona_regression";
  Q2_regression_Jona_disc.discriminantlabel = "Q^{2}_{Regression-Jona}";
  Q2_regression_Jona_disc.discriminantdimension = "GeV^{2}";
  Q2_regression_Jona_disc.discriminantbins = 100;
  Q2_regression_Jona_disc.discriminantmin = -10;
  Q2_regression_Jona_disc.discriminantmax = 11;
  Q2_regression_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Q_squared";
  discriminantsefficienciestobeplotted->push_back(Q2_regression_Jona_disc);

  DiscToBePlotted MissingM2_Gene_disc;
  MissingM2_Gene_disc.discriminantvariable = "MissingM2";
  MissingM2_Gene_disc.discriminantname = "MissingM2_Gene";
  MissingM2_Gene_disc.discriminantlabel = "m^{2}_{Miss,Gene}";
  MissingM2_Gene_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Gene_disc.discriminantbins = 100;
  MissingM2_Gene_disc.discriminantmin = -10;
  MissingM2_Gene_disc.discriminantmax = 11;
  MissingM2_Gene_disc.flag_rocplot = false;
  MissingM2_Gene_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  discriminantsefficienciestobeplotted->push_back(MissingM2_Gene_disc);

  DiscToBePlotted MissingM2_Pisa_disc;
  MissingM2_Pisa_disc.discriminantvariable = "MissingM2";
  MissingM2_Pisa_disc.discriminantname = "MissingM2_Pisa";
  MissingM2_Pisa_disc.discriminantlabel = "m^{2}_{Miss,Pisa}";
  MissingM2_Pisa_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Pisa_disc.discriminantbins = 100;
  MissingM2_Pisa_disc.discriminantmin = -10;
  MissingM2_Pisa_disc.discriminantmax = 11;
  MissingM2_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  discriminantsefficienciestobeplotted->push_back(MissingM2_Pisa_disc);

  DiscToBePlotted MissingM2_Jona_disc;
  MissingM2_Jona_disc.discriminantvariable = "MissingM2";
  MissingM2_Jona_disc.discriminantname = "MissingM2_Jona";
  MissingM2_Jona_disc.discriminantlabel = "m^{2}_{Miss,Jona}";
  MissingM2_Jona_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Jona_disc.discriminantbins = 100;
  MissingM2_Jona_disc.discriminantmin = -10;
  MissingM2_Jona_disc.discriminantmax = 11;
  MissingM2_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  discriminantsefficienciestobeplotted->push_back(MissingM2_Jona_disc);

  DiscToBePlotted MissingM2_Pisa_regression_disc;
  MissingM2_Pisa_regression_disc.discriminantvariable = "MissingM2_regression";
  MissingM2_Pisa_regression_disc.discriminantname = "MissingM2_Pisa_regression";
  MissingM2_Pisa_regression_disc.discriminantlabel = "m^{2}_{Miss,Pisa}";
  MissingM2_Pisa_regression_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Pisa_regression_disc.discriminantbins = 100;
  MissingM2_Pisa_regression_disc.discriminantmin = -10;
  MissingM2_Pisa_regression_disc.discriminantmax = 11;
  MissingM2_Pisa_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  discriminantsefficienciestobeplotted->push_back(MissingM2_Pisa_regression_disc);

  DiscToBePlotted MissingM2_Jona_regression_disc;
  MissingM2_Jona_regression_disc.discriminantvariable = "MissingM2_regression";
  MissingM2_Jona_regression_disc.discriminantname = "MissingM2_Jona_regression";
  MissingM2_Jona_regression_disc.discriminantlabel = "m^{2}_{Miss,Jona}";
  MissingM2_Jona_regression_disc.discriminantdimension = "GeV^{2}";
  MissingM2_Jona_regression_disc.discriminantbins = 100;
  MissingM2_Jona_regression_disc.discriminantmin = -10;
  MissingM2_Jona_regression_disc.discriminantmax = 11;
  MissingM2_Jona_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_mass_squared";
  discriminantsefficienciestobeplotted->push_back(MissingM2_Jona_regression_disc);

  DiscToBePlotted MissingPt_Gene_disc;
  MissingPt_Gene_disc.discriminantvariable = "MissingPt";
  MissingPt_Gene_disc.discriminantname = "MissingPt_Gene";
  MissingPt_Gene_disc.discriminantlabel = "p^{miss}_{T,Gene}";
  MissingPt_Gene_disc.discriminantdimension = "GeV";
  MissingPt_Gene_disc.discriminantbins = 100;
  MissingPt_Gene_disc.discriminantmin = -10;
  MissingPt_Gene_disc.discriminantmax = 70;
  MissingPt_Gene_disc.flag_rocplot = false;
  MissingPt_Gene_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  discriminantsefficienciestobeplotted->push_back(MissingPt_Gene_disc);

  DiscToBePlotted MissingPt_Pisa_disc;
  MissingPt_Pisa_disc.discriminantvariable = "MissingPt";
  MissingPt_Pisa_disc.discriminantname = "MissingPt_Pisa";
  MissingPt_Pisa_disc.discriminantlabel = "p^{miss}_{T,Pisa}";
  MissingPt_Pisa_disc.discriminantdimension = "GeV";
  MissingPt_Pisa_disc.discriminantbins = 100;
  MissingPt_Pisa_disc.discriminantmin = -10;
  MissingPt_Pisa_disc.discriminantmax = 70;
  MissingPt_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  discriminantsefficienciestobeplotted->push_back(MissingPt_Pisa_disc);

  DiscToBePlotted MissingPt_Jona_disc;
  MissingPt_Jona_disc.discriminantvariable = "MissingPt";
  MissingPt_Jona_disc.discriminantname = "MissingPt_Jona";
  MissingPt_Jona_disc.discriminantlabel = "p^{miss}_{T,Jona}";
  MissingPt_Jona_disc.discriminantdimension = "GeV";
  MissingPt_Jona_disc.discriminantbins = 100;
  MissingPt_Jona_disc.discriminantmin = -10;
  MissingPt_Jona_disc.discriminantmax = 70;
  MissingPt_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  discriminantsefficienciestobeplotted->push_back(MissingPt_Jona_disc);

  DiscToBePlotted MissingPt_Pisa_regression_disc;
  MissingPt_Pisa_regression_disc.discriminantvariable = "MissingPt_regression";
  MissingPt_Pisa_regression_disc.discriminantname = "MissingPt_Pisa_regression";
  MissingPt_Pisa_regression_disc.discriminantlabel = "p^{miss}_{T,Pisa}";
  MissingPt_Pisa_regression_disc.discriminantdimension = "GeV";
  MissingPt_Pisa_regression_disc.discriminantbins = 100;
  MissingPt_Pisa_regression_disc.discriminantmin = -10;
  MissingPt_Pisa_regression_disc.discriminantmax = 50;
  MissingPt_Pisa_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  discriminantsefficienciestobeplotted->push_back(MissingPt_Pisa_regression_disc);

  DiscToBePlotted MissingPt_Jona_regression_disc;
  MissingPt_Jona_regression_disc.discriminantvariable = "MissingPt_regression";
  MissingPt_Jona_regression_disc.discriminantname = "MissingPt_Jona_regression";
  MissingPt_Jona_regression_disc.discriminantlabel = "p^{miss}_{T,Jona}";
  MissingPt_Jona_regression_disc.discriminantdimension = "GeV";
  MissingPt_Jona_regression_disc.discriminantbins = 100;
  MissingPt_Jona_regression_disc.discriminantmin = -10;
  MissingPt_Jona_regression_disc.discriminantmax = 50;
  MissingPt_Jona_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/Missing_pt";
  discriminantsefficienciestobeplotted->push_back(MissingPt_Jona_regression_disc);

  DiscToBePlotted ctau_Gene_disc;
  ctau_Gene_disc.discriminantvariable = "ctau";
  ctau_Gene_disc.discriminantname = "ctau_Gene";
  ctau_Gene_disc.discriminantlabel = "c#tau_{Gene}";
  ctau_Gene_disc.discriminantdimension = "mm";
  ctau_Gene_disc.discriminantbins = 100;
  ctau_Gene_disc.discriminantmin = 0;
  ctau_Gene_disc.discriminantmax = 0.16;
  ctau_Gene_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  ctau_Gene_disc.flag_rocplot = false;
  ctau_Gene_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  discriminantsefficienciestobeplotted->push_back(ctau_Gene_disc);

  DiscToBePlotted ctau_Pisa_disc;
  ctau_Pisa_disc.discriminantvariable = "ctau";
  ctau_Pisa_disc.discriminantname = "ctau_Pisa";
  ctau_Pisa_disc.discriminantlabel = "c#tau_{Pisa}";
  ctau_Pisa_disc.discriminantdimension = "mm";
  ctau_Pisa_disc.discriminantbins = 100;
  ctau_Pisa_disc.discriminantmin = 0;
  ctau_Pisa_disc.discriminantmax = 0.16;
  ctau_Pisa_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  ctau_Pisa_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  discriminantsefficienciestobeplotted->push_back(ctau_Pisa_disc);

  DiscToBePlotted ctau_Jona_disc;
  ctau_Jona_disc.discriminantvariable = "ctau";
  ctau_Jona_disc.discriminantname = "ctau_Jona";
  ctau_Jona_disc.discriminantlabel = "c#tau_{Jona}";
  ctau_Jona_disc.discriminantdimension = "mm";
  ctau_Jona_disc.discriminantbins = 100;
  ctau_Jona_disc.discriminantmin = 0;
  ctau_Jona_disc.discriminantmax = 0.1;
  ctau_Jona_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  ctau_Jona_disc.distributionlogscale = 1;
  ctau_Jona_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  discriminantsefficienciestobeplotted->push_back(ctau_Jona_disc);

  DiscToBePlotted ctau_Pisa_regression_disc;
  ctau_Pisa_regression_disc.discriminantvariable = "ctau_regression";
  ctau_Pisa_regression_disc.discriminantname = "ctau_Pisa_regression";
  ctau_Pisa_regression_disc.discriminantlabel = "c#tau_{Pisa}";
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
  ctau_Jona_regression_disc.discriminantlabel = "c#tau_{Jona}";
  ctau_Jona_regression_disc.discriminantdimension = "mm";
  ctau_Jona_regression_disc.discriminantbins = 100;
  ctau_Jona_regression_disc.discriminantmin = 0;
  ctau_Jona_regression_disc.discriminantmax = 0.16;
  ctau_Jona_regression_disc.distributionplotlegend = new TLegend(0.72, 0.81, 0.9, 0.9);
  ctau_Jona_regression_disc.output_directory_png = "OutputFiles/PNGPlots/Efficiencies/ctau";
  discriminantsefficienciestobeplotted->push_back(ctau_Jona_regression_disc);

  /*  TreeRDFDiscriminants(filenames, treenames, branchnames, discriminants, cuts_preselection_mediumid_true, false);

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

  DiscriminantsPlotter(filenames, treenames, var2bestored, vartobeplotted, false);

  unordered_map<string, pair<string, string>> data_holder;

  data_holder["Signal"] = pair("ManipulatedVariables.root", "JpsiLeptons_regression_BTo3Mu_signal");
  data_holder["Normalization"] = pair("ManipulatedVariables.root", "JpsiLeptons_regression_BTo3Mu_normal");
  data_holder["Signorm"] = pair("ManipulatedVariables.root", "JpsiLeptons_regression_BTo3Mu");
  data_holder["Background"] = pair("ManipulatedVariables.root", "Background_regression_BTo3Mu");
  EfficienciesPlotter(data_holder, discriminantsefficienciestobeplotted);*/

  /*
ooooo      ooo   .oooooo.   ooooooooo.   ooo        ooooo       .o.       ooooo        ooooo  oooooooooooo       .o.       ooooooooooooo ooooo   .oooooo.   ooooo      ooo 
`888b.     `8'  d8P'  `Y8b  `888   `Y88. `88.       .888'      .888.      `888'        `888' d'""""""d888'      .888.      8'   888   `8 `888'  d8P'  `Y8b  `888b.     `8' 
 8 `88b.    8  888      888  888   .d88'  888b     d'888      .8"888.      888          888        .888P       .8"888.          888       888  888      888  8 `88b.    8  
 8   `88b.  8  888      888  888ooo88P'   8 Y88. .P  888     .8' `888.     888          888       d888'       .8' `888.         888       888  888      888  8   `88b.  8  
 8     `88b.8  888      888  888`88b.     8  `888'   888    .88ooo8888.    888          888     .888P        .88ooo8888.        888       888  888      888  8     `88b.8  
 8       `888  `88b    d88'  888  `88b.   8    Y     888   .8'     `888.   888       o  888    d888'    .P  .8'     `888.       888       888  `88b    d88'  8       `888  
o8o        `8   `Y8bood8P'  o888o  o888o o8o        o888o o88o     o8888o o888ooooood8 o888o .8888888888P  o88o     o8888o     o888o     o888o  `Y8bood8P'  o8o        `8
*/

  NormalizationHybrid(debug_bchybridnorm);
  NormalizationJPsiX(debug_jpsixnorm);
  NormalizationJPsiMu(cuts_preselection_mediumid_true, debug_jpsimunorm);

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
    TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_tau", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiTau->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_mu", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiMu->Sumw2();
    TH1F *histogram_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic0", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_chic0->Sumw2();
    TH1F *histogram_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic1", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_chic1->Sumw2();
    TH1F *histogram_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic2", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_chic2->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_hc", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiHC->Sumw2();
    TH1F *histogram_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_hc", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_Hc->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_tau", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_Psi2stau->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_mu", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_Psi2smu->Sumw2();
    TH1F *histogram_hbtojpsix = (TH1F *)HbToJpsiX.Filter(cuts_preselection_mediumid_true)
                                    .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                    .Filter("abs(k_pdgId) == 13")
                                    .Fill<Float_t>(TH1F("pt_miss_hb_jpsix", "y", 50, 0, 30), {"m_miss_sq"})
                                    ->Clone();
    histogram_hbtojpsix->Sumw2();
    TH1F *histogram_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(cuts_preselection_mediumid_true)
                                     .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                     .Filter("abs(k_pdgId) == 13")
                                     .Fill<Float_t>(TH1F("pt_miss_hb_jpsimu", "y", 50, 0, 30), {"m_miss_sq"})
                                     ->Clone();
    histogram_hbtojpsimu->Sumw2();

    // Data
    TH1F *histogram_data = (TH1F *)BtoMu_data.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("m_miss_sq_data", "y", 50, 0, 30), {"m_miss_sq"})->Clone();
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
ooooooooo.                       oooooooooooo  o8o      .   
`888   `Y88.                     `888'     `8  `"'    .o8   
 888   .d88'  .ooooo.   .ooooo.   888         oooo  .o888oo 
 888ooo88P'  d88' `88b d88' `88b  888oooo8    `888    888   
 888`88b.    888   888 888   888  888    "     888    888   
 888  `88b.  888   888 888   888  888          888    888 . 
o888o  o888o `Y8bod8P' `Y8bod8P' o888o        o888o   "888"
*/
  if (debug_fit == false)
  {
    VarFitter(vartobefit);
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
    strcpy(zero_peak_filter, cuts_preselection_mediumid_true);
    strcat(zero_peak_filter, "& m_miss_sq < 0.02");

    // Building variable distribution for various contributions
    // MC Signal and normalizaton
    ROOT::RDataFrame BtoJpsiMuMu_is_JPsiTau("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_tau_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_JPsiMu("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_mu_merged.root");
    TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_tau", "y", nbins, minbin, maxbin), {"m_miss_sq"})->Clone();
    histogram_jpsimumu_is_JPsiTau->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_mu", "y", nbins, minbin, maxbin), {"m_miss_sq"})->Clone();
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
    TH1F *histogram_data = (TH1F *)BtoMu_data.Filter(cuts_preselection_mediumid_true).Fill<Float_t>(TH1F("m_miss_sq_data", "y", nbins, minbin, maxbin), {"m_miss_sq"})->Clone();
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

  if (debug_nnfakes == false)
  {
    const char *preselection = "mu1pt>4 &&"
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
    //pass_id = "k_mediumID>0.5";
    //pass_id = "k_tightID>0.5";
    const char *fail_id = "!(k_mediumID>0.5 && k_raw_db_corr_iso03_rel<0.2)";
    //fail_id = "!(k_mediumID>0.5) & !(k_raw_db_corr_iso03_rel<0.2)";

    TString preselection_mc = TString::Format("%s && !(abs(k_genpdgId)==13)", preselection);

    //Hb to jpsi X MC sample
    const char *mc_path = "InputFiles/HbToJPsiMuMu_ptmax_merged.root";

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

    //preselection and not-true muon request
    ROOT::RDataFrame mc("BTo3Mu", mc_path);
    auto mc_filtered = ROOT::RDF::RNode(mc.Filter(preselection_mc.Data()));
    auto passing_mc_tmp = mc_filtered.Filter(pass_id);
    auto failing_mc_tmp = mc_filtered.Filter(fail_id);

    Float_t closure_test_threshold = 0.7;
    auto passing_mc = passing_mc_tmp.Range(0, int(passing_mc_tmp.Count().GetValue() * closure_test_threshold));
    auto failing_mc = failing_mc_tmp.Range(0, int(failing_mc_tmp.Count().GetValue() * closure_test_threshold));

    //samples for the closure test
    auto passing_mc_ct = passing_mc_tmp.Range(int(passing_mc_tmp.Count().GetValue() * closure_test_threshold), 0).Define("Target", "return 1");
    auto failing_mc_ct = failing_mc_tmp.Range(int(failing_mc_tmp.Count().GetValue() * closure_test_threshold), 0).Define("Target", "return 0");

    cout << "" << endl;
    cout << "Model" << endl;
    cout << "" << endl;

    string MVA_PassFakes_Input_Tree = "Pass_events", MVA_FailFakes_Input_Tree = "Fail_events";
    string MVA_ClosurePassFakes_Input_Tree = "ClosurePass_events", MVA_ClosureFailFakes_Input_Tree = "ClosureFail_events";
    string MVA_Fakes_Input_File = "MVA_Fakes_Input", MVA_Fakes_Output_File = "MVA_Fakes_Output";
    TString infilename(TString::Format("%s.root", MVA_Fakes_Input_File.data()));
    TString outfilename(TString::Format("%s.root", MVA_Fakes_Output_File.data()));
    ROOT::RDF::RSnapshotOptions tmva_mc_snapop;
    tmva_mc_snapop.fMode = "RECREATE";
    passing_mc.Snapshot(MVA_PassFakes_Input_Tree, infilename, features, tmva_mc_snapop);
    tmva_mc_snapop.fMode = "UPDATE";
    failing_mc.Snapshot(MVA_FailFakes_Input_Tree, infilename, features, tmva_mc_snapop);
    vector<string> features_ct = features;
    features_ct.push_back("Target");
    for (vector<VarToBeFitted>::iterator varit = vartobefit->begin(); varit < vartobefit->end(); varit++)
      if (passing_mc_ct.HasColumn(varit->vartobefitted) == true)
        features_ct.push_back(varit->vartobefitted);
    passing_mc_ct.Snapshot(MVA_ClosurePassFakes_Input_Tree, infilename, features_ct, tmva_mc_snapop);
    failing_mc_ct.Snapshot(MVA_ClosureFailFakes_Input_Tree, infilename, features_ct, tmva_mc_snapop);
    TFile *inputfile = TFile::Open(infilename, "read");
    TFile *outputfile = TFile::Open(outfilename, "recreate");
    TTree *signal = (TTree *)inputfile->Get(MVA_PassFakes_Input_Tree.data());
    TTree *background = (TTree *)inputfile->Get(MVA_FailFakes_Input_Tree.data());
    Double_t signalweight = 1.0, backgroundweight = 1.0;
    TMVA::gConfig().GetIONames().fWeightFileDirPrefix = "TMVAResults";
    TMVA::gConfig().GetIONames().fWeightFileDir = "Weights";
    TMVA::DataLoader *loader = new TMVA::DataLoader("");
    TMVA::Factory *factory = new TMVA::Factory("TMVAClassification", outputfile, "AnalysisType=Classification");
    for (vector<string>::iterator mvavar = features.begin(); mvavar < features.end(); mvavar++)
    {
      loader->AddVariable(mvavar->data(), mvavar->data(), "", 'signal->GetBranch(mvavar->data())->GetClassName()');
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

    vector<TMVAMethod> tmvamethods;
    {
      TMVAMethod kmlp1;
      kmlp1.tmvamethodname = "kmlp1";
      TString config  = "CreateMVAPdfs:VarTransform=N:NCycles=1000:HiddenLayers=N+2,N+1,N:NeuronType=sigmoid:EstimatorType=CE:";
              config += "TrainingMethod=BP:LearningRate=0.2:DecayRate=0.1:ConvergenceTests=10";
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
    for (vector<TMVAMethod>::iterator tmvamethit = tmvamethods.begin(); tmvamethit < tmvamethods.end(); tmvamethit++)
    {
      factory->BookMethod(loader_with_variance_threshold, tmvamethit->tmvamethodtype, tmvamethit->tmvamethodname, tmvamethit->tmvamethodconfiguration);
    }
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();
    inputfile->Close();
    outputfile->Close();
    if(!gROOT->IsBatch()) TMVA::TMVAGui(outfilename);

    // Initializing variables to set up the reader
    vector<Float_t> Input_feature_var(features.size());
    unordered_map<string, Float_t> MVA_feature_var(features.size());
    if (1 > 0)
    {
      TFile *eventinputfile = TFile::Open(infilename.Data(), "update");
      TFile *eventoutputfile = TFile::Open(TString::Format("risultatidechenonsisa.root"), "recreate");
      TList *list = new TList();
      TTree *PassFakesTree = (TTree *)eventinputfile->Get(MVA_ClosurePassFakes_Input_Tree.data());
      TTree *FailFakesTree = (TTree *)eventinputfile->Get(MVA_ClosureFailFakes_Input_Tree.data());
      Double_t passentries = PassFakesTree->GetEntries(), failentries = FailFakesTree->GetEntries();
      cout <<  passentries << endl;
      cout <<  failentries << endl;
      Double_t fractionsignal = passentries/(passentries+failentries); 
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
      TString loader_with_variance_threshold_filename = TString::Format("%s.root", MVA_Fakes_Output_File.data());
      TString loader_with_variance_threshold_treename = TString::Format("%s/TrainTree", loader_with_variance_threshold->GetName());
      ROOT::RDataFrame loader_with_variance_threshold_dataframe(loader_with_variance_threshold_treename, loader_with_variance_threshold_filename);

      for (vector<TMVAMethod>::iterator tmvamethit = tmvamethods.begin(); tmvamethit < tmvamethods.end(); tmvamethit++)
      {
        cout << endl << endl << endl << endl;
        Event->SetName(TString::Format("FakeClosureTest_%s", tmvamethit->tmvamethodname));

        //Looking for any weight of the specified method in the folder containing the training results
        TString weightfilename = TString::Format("TMVAResults/%s/Weights/TMVAClassification_%s.weights.xml", loader_with_variance_threshold->GetName(), tmvamethit->tmvamethodname);

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
    }
    if (1 > 0)
    {
      TFile *eventinputfile = TFile::Open(TString::Format("InputFiles/BcToJPsiMuMu_is_jpsi_mu_merged.root"), "read");
      TTree *Event_old = (TTree *)eventinputfile->Get("BTo3Mu");
      TFile *eventoutputfile = TFile::Open(TString::Format("OutputFiles/BcToJPsiMuMu_is_jpsi_mu_merged.root"), "recreate");
      TTree *Event = Event_old->CloneTree();
      Double_t FakeWeight = 0, FakeProba = 0, FakeRarity = 0;
      TBranch *Branch_FakeWeight = Event->Branch("FakeWeight", &FakeWeight, "FakeWeight/D");
      TBranch *Branch_FakeProba = Event->Branch("FakeProba", &FakeProba, "FakeProba/D");
      TBranch *Branch_FakeRarity = Event->Branch("FakeRarity", &FakeRarity, "FakeRarity/D");
      int event_entries = Event->GetEntries();
      for (vector<string>::iterator mvavar = features.begin(); mvavar < features.end(); mvavar++)
        Event->SetBranchAddress(mvavar->data(), &(Input_feature_var[distance(features.begin(), mvavar)]));

      //Creating a dataframe which holds only names of variables with a variance greater than the threshold set up before
      TString loader_with_variance_threshold_filename = TString::Format("%s.root", MVA_Fakes_Output_File.data());
      TString loader_with_variance_threshold_treename = TString::Format("%s/TrainTree", loader_with_variance_threshold->GetName());
      ROOT::RDataFrame loader_with_variance_threshold_dataframe(loader_with_variance_threshold_treename, loader_with_variance_threshold_filename);

      for (vector<TMVAMethod>::iterator tmvamethit = tmvamethods.begin(); tmvamethit < tmvamethods.end(); tmvamethit++)
      {
        cout << event_entries << endl << endl << endl << endl;
        //Event->SetName(TString::Format("", tmvamethit->tmvamethodname));

        //Looking for any weight of the specified method in the folder containing the training results
        TString weightfilename = TString::Format("TMVAResults/%s/Weights/TMVAClassification_%s.weights.xml", loader_with_variance_threshold->GetName(), tmvamethit->tmvamethodname);

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
        eventinputfile->Close();
        eventoutputfile->Close();
      }
    }

    vector<string> nn_weighted_variables;
    for (vector<TMVAMethod>::iterator tmvamethitct = tmvamethods.begin(); tmvamethitct < tmvamethods.end(); tmvamethitct++)
    {
      TString weightfilename = TString::Format("TMVAResults/%s/Weights/TMVAClassification_%s.weights.xml", loader_with_variance_threshold->GetName(), tmvamethitct->tmvamethodname);
      TMVA::Experimental::RReader model(weightfilename.Data());
      auto variables = model.GetVariableNames();
      //auto prediction = model.Compute();

      ROOT::RDataFrame NN_Closure_original(TString::Format("FakeClosureTest_%s", tmvamethitct->tmvamethodname), "risultatidechenonsisa.root");
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
  }

  return 0;
}