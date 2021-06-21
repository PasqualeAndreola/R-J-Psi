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
#include "HeaderFiles/PrintFuncInfo.h"
#include "HeaderFiles/TreeRDFDiscriminants.h"
#include "HeaderFiles/TreeReader.h"
#include "HeaderFiles/VarToBePlotted.h"
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
  bool debug_fakes = true;
  bool debug_inclusive = true;
  bool debug_fit = false;

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

  const char *pre_cuts = "mu1pt>3 && "
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

  /*  TreeRDFDiscriminants(filenames, treenames, branchnames, discriminants, pre_cuts, false);

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

  /*
oooooooooo.                 ooooo   ooooo              .o8                 o8o        .o8  
`888'   `Y8b                `888'   `888'             "888                 `"'       "888  
 888     888  .ooooo.        888     888  oooo    ooo  888oooo.  oooo d8b oooo   .oooo888  
 888oooo888' d88' `"Y8       888ooooo888   `88.  .8'   d88' `88b `888""8P `888  d88' `888  
 888    `88b 888             888     888    `88..8'    888   888  888      888  888   888  
 888    .88P 888   .o8       888     888     `888'     888   888  888      888  888   888  
o888bood8P'  `Y8bod8P'      o888o   o888o     .8'      `Y8bod8P' d888b    o888o `Y8bod88P" 
                                          .o..P'                                           
                                          `Y8P'
*/
  if (debug_bchybridnorm == false)
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

    // RooFit demmerda
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
    leg.Clear();

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
  }
  /*
    oooo      88 ooooooooo.             o8o                       ooooooo  ooooo 
    `888     .8' `888   `Y88.           `"'                        `8888    d8'  
      888    .8'   888   .d88'  .oooo.o oooo           88             Y888..8P    
      888   .8'    888ooo88P'  d88(  "8 `888           88              `8888'     
      888  .8'     888         `"Y88b.   888       8888888888         .8PY888.    
      888 .8'      888         o.  )88b  888           88            d8'  `888b   
  .o. 88P 88      o888o        8""888P' o888o          88          o888o  o88888o 
  `Y888P
  */

  if (debug_jpsixnorm == false)
  {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //      Variables and PDFs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    RooRealVar mass = RooRealVar("Bmass", "J/#PsiK^{+} mass", 5, 5.6, "GeV");
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
    Double_t fit_range_lo = 5;
    Double_t mass_window_lo = 5.275 - 0.15;
    Double_t mass_window_hi = 5.275 + 0.15;
    Double_t fit_range_hi = 6;

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
    RooRealVar maxM = RooRealVar("maxM", "argus max m", 5.20639e+00, 5.0, 5.3); //6.2)
    RooArgusBG argus = RooArgusBG("argus", "Argus PDF", mass, maxM, argpar);

    // detector response function
    RooRealVar mg = RooRealVar("mg", "mg", 0);                         //, -0.01, 0.01)
    RooRealVar sg = RooRealVar("sg", "sg", 1.14919e-02, 0.0001, 0.03); //, 0.001,0.2)
    RooGaussian resGauss = RooGaussian("resGauss", "resGauss", mass, mg, sg);
    // construct convolution
    mass.setBins(10000, "fft");
    RooFFTConvPdf lxg = RooFFTConvPdf("lxg", "argus (X) gauss", mass, argus, resGauss);

    // Bc->Jpsi K crystal ball
    RooRealVar jpsik_mean = RooRealVar("jpsik_mean", "narrow_mean", 5.17, 5.00, 5.30);
    RooRealVar jpsik_sigma = RooRealVar("jpsik_sigma", "sigma", 0.03, 0.01, 0.1);
    RooGaussian jpsik_func = RooGaussian("jpsik_func", "jpsik_func", mass, jpsik_mean, jpsik_sigma);
    // jpsik_n     = RooRealVar("jpsik_n"    , "jpsik_n"    , 0.1 , 0.01,   3.  )
    // jpsik_alpha = RooRealVar("jpsik_alpha", "jpsik_alpha", 2   ,  0.1,   4.  )
    // jpsik_func = RooCBShape("jpsik_func", "jpsik_func", mass, jpsik_mean, jpsik_sigma, jpsik_alpha, jpsik_n)

    // signal narrow gaussian
    RooRealVar narrow_mean = RooRealVar("narrow_mean", "narrow_mean", 5.26774e+00, 5.1, 5.4);
    RooRealVar narrow_width = RooRealVar("narrow_width", "narrow_width", 2.44845e-02, 0., 0.1);
    RooGaussian narrow_gaus = RooGaussian("sig_narrow_gaus", "sig_narrow_gaus", mass, narrow_mean, narrow_width);

    // signal broad gaussian
    RooRealVar broad_mean = RooRealVar("broad_mean", "broad_mean", 5.26774e+00, 5.1, 5.4);
    RooRealVar broad_width = RooRealVar("broad_width", "broad_width", 5.62194e-02, 0., 1.);
    RooGaussian broad_gaus = RooGaussian("sig_broad_gaus", "sig_broad_gaus", mass, broad_mean, broad_width);

    // absolute yields
    RooRealVar nsig = RooRealVar("signal_yield", "signal_yield", 800, 0., 1000000.);
    RooRealVar nsig_narrow = RooRealVar("signal_yield_narrow", "signal_yield_narrow", 700, 0., 1000000.);
    RooRealVar nsig_broad = RooRealVar("signal_yield_broad", "signal_yield_broad", 100, 0., 1000000.);
    RooRealVar nbkgtot = RooRealVar("nbkgtot", "nbkgtot", 2000, 0., 1000000.);
    RooRealVar nbkg = RooRealVar("nbkg", "nbkg", 7000, 0., 1000000.);
    RooRealVar nPi = RooRealVar("nPi", "nPi", 1000, 0., 1000000.);
    RooRealVar nK = RooRealVar("nK", "nK", 200, 0., 1000000.);

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
        RooArgList(bkg_fit_function, signal_fit_function),
        RooArgList(frac_bkg));

    // MC signal narrow gaussian
    RooRealVar mc_narrow_mean = RooRealVar("mc_narrow_mean", "mc_narrow_mean", 5.28, 5., 5.6);
    RooRealVar mc_narrow_width = RooRealVar("mc_narrow_width", "mc_narrow_width", 0.038, 0., 1.);
    RooGaussian mc_narrow_gaus = RooGaussian("mc_sig_narrow_gaus", "mc_sig_narrow_gaus", mass, mc_narrow_mean, mc_narrow_width);

    // MC signal broad gaussian
    RooRealVar mc_broad_mean = RooRealVar("mc_broad_mean", "mc_broad_mean", 5.3, 5, 5.6);
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
    const char *selection = "Bmass > 5 & "
                            "Bmass <5.6 & "
                            // "mu1pt>4.5 & "
                            // "mu2pt>4.5 & "
                            "mu1pt>3 & "
                            "mu2pt>3 & "
                            "abs(mu1eta)<2.4 & "
                            "abs(mu2eta)<2.4 & "
                            //"mu1pt>3.5 & "
                            //"mu2pt>3.5 & "
                            "Bpt>15 & "
                            "bvtx_lxy > 0.015 & " // 150 micron
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
    const char *selection_mc = "abs(mu1_genpdgId)==13 & "      // 13 = muon -> The J/Psi is real
                               "abs(mu1_mother_pdgId)==443 & " // 443 = J/Psi
                               "abs(mu2_genpdgId)==13 & "
                               "abs(mu2_mother_pdgId)==443 & "
                               "abs(k_genpdgId)==321 & " // 321 = K+ -> The unpaired particle is a K+
                               "abs(k_mother_pdgId)== 521 & "
                               "abs(mu1_grandmother_pdgId)==521 & " // 521 = B+
                               "abs(mu2_grandmother_pdgId)==521 & "
                               "Bmass > 5 & "
                               "Bmass <5.6 & "
                               // "mu1pt>4.5 & "
                               // "mu2pt>4.5 & "
                               "mu1pt>3 & "
                               "mu2pt>3 & "
                               "abs(mu1eta)<2.4 & "
                               "abs(mu2eta)<2.4 & "
                               //"mu1pt>3.5 & "
                               //"mu2pt>3.5 & "
                               "Bpt>15 & "
                               "bvtx_lxy > 0.015 & " // 150 micron
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

    TTree *data_tree = (TTree *)(TFile::Open(data_files)->Get("BTo2MuK"));

    const char *signal_files = "InputFiles/HbToJPsiMuMu_ptmax_merged.root";

    TTree *signal_tree = (TTree *)(TFile::Open(signal_files)->Get("BTo2MuK"));

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

    // Importing results from the factory
    TFile jpsix_fitfunctions_file = TFile("jpsix_workspace.root");
    RooWorkspace *jpsix_workspace = (RooWorkspace *)jpsix_fitfunctions_file.Get("w");
    //RooAddPdf *data_fit_function = (RooAddPdf *)jpsix_workspace->function("fit_function");
    //RooAddPdf *mc_signal_fitFunction = (RooAddPdf *)jpsix_workspace->function("mc_signal_fit_function");

    RooFitResult *results_data = fit_function.fitTo(fulldata, RooFit::Save());

    fit_function.plotOn(frame);
    fit_function.plotOn(frame, Name("bkg_pol"), RooFit::Components("bkg_pol"), RooFit::LineStyle(kDashed), RooFit::LineColor(kBlue));
    fit_function.plotOn(frame, Name("lxg"), RooFit::Components("lxg"), RooFit::LineStyle(kDashed), RooFit::LineColor(kOrange));
    fit_function.plotOn(frame, Name("signal_fit_function"), RooFit::Components("signal_fit_function"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));

    frame->Draw();
    //CMS_lumi(c1, 4, 0, cmsText = "CMS", extraText = "   Preliminary", lumi_13TeV = "60 fb^{-1}");

    TLegend leg = TLegend(0.58, .65, .90, .90);
    leg.SetBorderSize(0);
    leg.SetFillColor(0);
    leg.SetFillStyle(0);
    leg.SetTextFont(42);
    leg.SetTextSize(0.035);
    // leg.SetNColumns(3);

    // RooFit demmerda
    leg.AddEntry("bkg_pol", "Combinatorial bkg");
    leg.AddEntry("lxg", "B^{+}#rightarrowJ/#PsiK^{+} + X", "L");
    leg.AddEntry("signal_fit_function", "B^{+}#rightarrowJ/#PsiK^{+}", "L");
    leg.AddEntry("Data", "Observed", "EP");
    leg.Draw("SAME");

    // gPad.SaveAs("sideband_fit.pdf")
    c1.SaveAs("jpsi+x_fit.png");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Fit the mc
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    fullsignal.plotOn(frame, RooFit::Binning(nbins), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
    RooFitResult *results_mc = mc_signal_fitFunction.fitTo(fullsignal, RooFit::Extended(kTRUE), RooFit::Save());
    mc_signal_fitFunction.plotOn(frame, Name("jpsikmc"), RooFit::LineColor(kRed), RooFit::MarkerColor(kRed));
    frame->Draw();
    //CMS_lumi(c1, 4, 0, cmsText = "CMS", extraText = "   Preliminary", lumi_13TeV = "60 fb^{-1}");
    leg.AddEntry("jpsikmc", "B^{+}#rightarrowJ/#PsiK^{+} MC", "LP");

    /*TF1 *data_signal_fit_function_jpsik = new TF1();
        TF1 *mc_signal_fit_function_jpsik = new TF1();
        data_signal_fit_function_jpsik = fit_function.asTF(mass, frac_bkg, mass);
        mc_signal_fit_function_jpsik = mc_signal_fitFunction.asTF(mass, RooArgList(mc_nsig_narrow, mc_nsig_broad), mass);
        data_signal_integral_jpsik = data_signal_fit_function_jpsik->Integral(narrow_mean.getVal()-3*narrow_width.getVal(), narrow_mean.getVal()+3*narrow_width.getVal());
        mc_signal_integral_jpsik = mc_signal_fit_function_jpsik->Integral(mc_narrow_mean.getVal()-3*mc_narrow_width.getVal(), mc_narrow_mean.getVal()+3*mc_narrow_width.getVal());
        leg.AddEntry((TObject*)0, TString::Format("Data/MC (Peak ratio): %.2f", data_signal_fit_function_jpsik->Eval(narrow_mean.getVal())/mc_signal_fit_function_jpsik->Eval(narrow_mean.getVal())), "");
        leg.AddEntry((TObject*)0, TString::Format("Data/MC(BroadPeak ratio): %.2f", data_signal_fit_function_jpsik->Eval(broad_mean.getVal())/mc_signal_fit_function_jpsik->Eval(mc_broad_mean.getVal())), "");*/
    leg.AddEntry((TObject *)0, TString::Format("Data over MC: %.2f", (fulldata.numEntries() * (1 - frac_bkg.getVal())) / (mc_nsig_narrow.getVal() + mc_nsig_broad.getVal())), "");
    leg.Draw("SAME");
    c1.SaveAs("jpsi+x_fit_with_mc.png");

    RooArgSet *params = fit_function.getParameters(mass);
    params->writeToStream(cout, false);
    RooArgSet *mcparams = mc_signal_fitFunction.getParameters(mass);
    mcparams->writeToStream(cout, false);

    // Create a new empty workspace
    RooWorkspace *jpsixworkspace = new RooWorkspace("jpsixworkspace", "jpsixworkspace");

    // Import model and all its components into the workspace
    jpsixworkspace->import(fit_function);

    // Import mc_model and all its components into the workspace
    jpsixworkspace->import(mc_signal_fitFunction);

    // Import data into the workspace
    jpsixworkspace->import(fulldata);

    // Print workspace contents
    jpsixworkspace->Print();

    // S a v e   w o r k s p a c e   i n   f i l e
    // -------------------------------------------

    // Save the workspace into a ROOT file
    jpsixworkspace->writeToFile("jpsix_workspace.root");

    cout << "Fit to data integral " << fulldata.numEntries() * (1 - frac_bkg.getVal()) << endl;
    cout << "Fit to data integral " << frac_sig.getVal() << " " << fulldata.numEntries() * frac_sig.getVal() << endl;
    cout << "Fit to mc integral " << (1. + 0.079) * (mc_nsig_narrow.getVal() + mc_nsig_broad.getVal()) << endl;
  }
  /*
   oooo      88 ooooooooo.             o8o                       ooo        ooooo                                   
   `888     .8' `888   `Y88.           `"'                       `88.       .888'                                   
    888    .8'   888   .d88'  .oooo.o oooo           88           888b     d'888  oooo  oooo   .ooooo.  ooo. .oo.   
    888   .8'    888ooo88P'  d88(  "8 `888           88           8 Y88. .P  888  `888  `888  d88' `88b `888P"Y88b  
    888  .8'     888         `"Y88b.   888       8888888888       8  `888'   888   888   888  888   888  888   888  
    888 .8'      888         o.  )88b  888           88           8    Y     888   888   888  888   888  888   888  
.o. 88P 88      o888o        8""888P' o888o          88          o8o        o888o  `V88V"V8P' `Y8bod8P' o888o o888o 
`Y888P 
*/
  if (debug_jpsimunorm == false)
  {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //      Getting the mass of the 3 muon system in Data and in MC
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ROOT::RDataFrame Data("BTo3Mu", "InputFiles/data_ptmax_merged.root");
    TH1F *histogram_3mumass_data = (TH1F *)Data.Filter(pre_cuts)
                                       .Fill<Float_t>(TH1F("mass3mu_Data", "Data", 80, 0, 10), {"Bmass"})
                                       ->Clone();
    histogram_3mumass_data->Sumw2();
    ROOT::RDataFrame HbToJPsiMuMu3MuFilter("BTo3Mu", "InputFiles/HbToJPsiMuMu3MuFilter_ptmax_merged.root");
    TH1F *histogram_3mumass_mc = (TH1F *)HbToJPsiMuMu3MuFilter.Filter(pre_cuts)
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

    /*
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Fakes
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ROOT::RDataFrame jpsix_mc_dataframe("BTo3Mu", "InputFiles/HbToJPsiMuMu_ptmax_merged.root");
  TH1F *histogram_fakes = (TH1F *)jpsix_mc_dataframe.Filter(pre_cuts)
                                  .Filter("abs(mu1_grandmother_pdgId)!=541 & abs(mu2_grandmother_pdgId)!=541")
                                  .Filter("abs(k_pdgId) == 13")
                                  .Fill<Float_t>(TH1F("mass3mu_HbToJPsiMuMu_ptmax_merged", "jpsix_MC", 80, 0, 10), {"Bmass"})
                                  ->Clone();
  histogram_fakes->Sumw2();
  histogram_fakes->Scale(7);
*/
  }
  /*
oooooooooooo           oooo                  
`888'     `8           `888                  
 888          .oooo.    888  oooo   .ooooo.  
 888oooo8    `P  )88b   888 .8P'   d88' `88b 
 888    "     .oP"888   888888.    888ooo888 
 888         d8(  888   888 `88b.  888    .o 
o888o        `Y888""8o o888o o888o `Y8bod8P'
*/
  if (debug_fakes == false)
  {
    RooWorkspace w("w");

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
    TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_tau", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_JPsiTau->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_mu", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_JPsiMu->Sumw2();
    TH1F *histogram_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic0", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic0->Sumw2();
    TH1F *histogram_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic1", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic1->Sumw2();
    TH1F *histogram_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic2", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic2->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_hc", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_JPsiHC->Sumw2();
    TH1F *histogram_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_hc", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Hc->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_tau", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Psi2stau->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_mu", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Psi2smu->Sumw2();
    TH1F *histogram_hbtojpsix = (TH1F *)HbToJpsiX.Filter(pre_cuts)
                                    .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                    .Filter("abs(k_pdgId) == 13")
                                    .Fill<Float_t>(TH1F("pt_miss_hb_jpsix", "y", 50, 0, 30), {"pt_miss_vec"})
                                    ->Clone();
    histogram_hbtojpsix->Sumw2();
    TH1F *histogram_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(pre_cuts)
                                     .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                     .Filter("abs(k_pdgId) == 13")
                                     .Fill<Float_t>(TH1F("pt_miss_hb_jpsimu", "y", 50, 0, 30), {"pt_miss_vec"})
                                     ->Clone();
    histogram_hbtojpsimu->Sumw2();

    // Data
    TH1F *histogram_data = (TH1F *)BtoMu_data.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_vec_data", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
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
  }
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
    TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_tau", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_JPsiTau->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_mu", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_JPsiMu->Sumw2();
    TH1F *histogram_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic0", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic0->Sumw2();
    TH1F *histogram_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic1", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic1->Sumw2();
    TH1F *histogram_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic2", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic2->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_hc", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_JPsiHC->Sumw2();
    TH1F *histogram_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_hc", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Hc->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_tau", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Psi2stau->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_mu", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Psi2smu->Sumw2();
    TH1F *histogram_hbtojpsix = (TH1F *)HbToJpsiX.Filter(pre_cuts)
                                    .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                    .Filter("abs(k_pdgId) == 13")
                                    .Fill<Float_t>(TH1F("pt_miss_hb_jpsix", "y", 50, 0, 30), {"pt_miss_vec"})
                                    ->Clone();
    histogram_hbtojpsix->Sumw2();
    TH1F *histogram_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(pre_cuts)
                                     .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                     .Filter("abs(k_pdgId) == 13")
                                     .Fill<Float_t>(TH1F("pt_miss_hb_jpsimu", "y", 50, 0, 30), {"pt_miss_vec"})
                                     ->Clone();
    histogram_hbtojpsimu->Sumw2();

    // Data
    TH1F *histogram_data = (TH1F *)BtoMu_data.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_vec_data", "y", 50, 0, 30), {"pt_miss_vec"})->Clone();
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
    // Binning
    Double_t nbins = 90, minbin = 0, maxbin = 30, bin_width = (maxbin - minbin) / nbins;

    // Building variable distribution for various contributions
    // MC Signal and normalizaton
    ROOT::RDataFrame BtoJpsiMuMu_is_JPsiTau("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_tau_merged.root");
    ROOT::RDataFrame BtoJpsiMuMu_is_JPsiMu("BTo3Mu", "InputFiles/BcToJPsiMuMu_is_jpsi_mu_merged.root");
    TH1F *histogram_jpsimumu_is_JPsiTau = (TH1F *)BtoJpsiMuMu_is_JPsiTau.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_tau", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_JPsiTau->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiMu = (TH1F *)BtoJpsiMuMu_is_JPsiMu.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_mu", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
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
    TH1F *histogram_jpsimumu_is_chic0 = (TH1F *)BtoJpsiMuMu_is_Chic0.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic0", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic0->Sumw2();
    TH1F *histogram_jpsimumu_is_chic1 = (TH1F *)BtoJpsiMuMu_is_Chic1.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic1", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic1->Sumw2();
    TH1F *histogram_jpsimumu_is_chic2 = (TH1F *)BtoJpsiMuMu_is_Chic2.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_chic2", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_chic2->Sumw2();
    TH1F *histogram_jpsimumu_is_JPsiHC = (TH1F *)BtoJpsiMuMu_is_JPsiHC.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_jpsi_hc", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_JPsiHC->Sumw2();
    TH1F *histogram_jpsimumu_is_Hc = (TH1F *)BtoJpsiMuMu_is_Hc.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_hc", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Hc->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2stau = (TH1F *)BtoJpsiMuMu_is_Psi2stau.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_tau", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Psi2stau->Sumw2();
    TH1F *histogram_jpsimumu_is_Psi2smu = (TH1F *)BtoJpsiMuMu_is_Psi2smu.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_jpsimu_is_psi2s_mu", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_jpsimumu_is_Psi2smu->Sumw2();
    TH1F *histogram_hbtojpsix = (TH1F *)HbToJpsiX.Filter(pre_cuts)
                                    .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                    .Filter("abs(k_pdgId) != 13")
                                    .Fill<Float_t>(TH1F("pt_miss_hb_jpsix", "y", nbins, minbin, maxbin), {"pt_miss_vec"})
                                    ->Clone();
    histogram_hbtojpsix->Sumw2();
    TH1F *histogram_hbtojpsimu = (TH1F *)HbToJPsiMu.Filter(pre_cuts)
                                    .Filter("abs(mu1_grandmother_pdgId) != 541 && abs(mu2_grandmother_pdgId) != 541")
                                    .Filter("abs(k_pdgId) == 13")
                                    .Fill<Float_t>(TH1F("pt_miss_hb_jpsimu", "y", nbins, minbin, maxbin), {"pt_miss_vec"})
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
    TH1F *histogram_jpsimu_is_bkg = (TH1F*)histstack_bkg_distribution->GetStack()->Last();
    
    // Data
    ROOT::RDataFrame BtoMu_data("BTo3Mu", "InputFiles/data_ptmax_merged.root");
    TH1F *histogram_data = (TH1F *)BtoMu_data.Filter(pre_cuts).Fill<Float_t>(TH1F("pt_miss_vec_data", "y", nbins, minbin, maxbin), {"pt_miss_vec"})->Clone();
    histogram_data->Sumw2();

    // RooFit Initialization
    RooRealVar *pt_miss_vec = new RooRealVar("pt_miss_vec", "pt_miss_vec", minbin, maxbin);
    RooDataHist *binned_sig_mc = new RooDataHist("binned_sig_mc", "datasetwith(pt_miss_vec)", *pt_miss_vec, histogram_jpsimumu_is_JPsiTau);
    RooDataHist *binned_norm_mc = new RooDataHist("binned_norm_mc", "datasetwith(pt_miss_vec)", *pt_miss_vec, histogram_jpsimumu_is_JPsiMu);
    RooDataHist *binned_bkg_mc = new RooDataHist("binned_bkg_mc", "datasetwith(pt_miss_vec)", *pt_miss_vec, histogram_jpsimu_is_bkg);
    RooDataHist *binned_data = new RooDataHist("binned_data", "datasetwith(pt_miss_vec)", *pt_miss_vec, histogram_data);

    RooWorkspace w("w");

    w.import(*binned_sig_mc, RooFit::Rename("binned_sig"));
    w.import(*binned_norm_mc, RooFit::Rename("binned_norm"));
    w.import(*binned_bkg_mc, RooFit::Rename("binned_bkg"));
    w.import(*binned_data, RooFit::Rename("observed_data"));

    w.factory("HistFunc::sig(pt_miss_vec,binned_sig)");
    w.factory("HistFunc::norm(pt_miss_vec,binned_norm)");
    w.factory("HistFunc::bkg(pt_miss_vec,binned_bkg)");

    w.factory(TString::Format("binw[%f]", bin_width));
    w.factory("expr::S('Sscale*binw',Sscale[0.62, 0,1000],binw)");
    w.factory("expr::N('Nscale*binw',Nscale[0.62, 0,1000],binw)");
    w.factory("expr::B('Bscale*binw',Bscale[1, 0,1000],binw)");
    w.factory("ASUM::model(S*sig,N*norm,B*bkg)");

    w.pdf("model")->fitTo(*binned_data);
    
    TCanvas *c1 = new TCanvas("c", "c", 1366, 768);
    RooPlot *frame = w.var("pt_miss_vec")->frame();
    binned_data->plotOn(frame);
    w.pdf("model")->plotOn(frame);
    w.pdf("model")->plotOn(frame, RooFit::Components("bkg"), RooFit::LineStyle(kDashed), RooFit::LineColor(kGreen));
    w.pdf("model")->plotOn(frame, RooFit::Components("sig"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
    w.pdf("model")->plotOn(frame, RooFit::Components("norm"), RooFit::LineStyle(kDashed), RooFit::LineColor(kBlue));
    frame->Draw();
    c1->Draw();

    c1->Print("prova_roofit.png");
  }
  
  return 0;
}