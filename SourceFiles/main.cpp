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
#include "HeaderFiles/MarkedNames.h"
#include "HeaderFiles/PrintFuncInfo.h"
#include "HeaderFiles/TreeRDFDiscriminants.h"
#include "HeaderFiles/TreeReader.h"
#include "HeaderFiles/VarToBePlotted.h"

//It can be useful to use these namespaces
using namespace std;

int main(int argc, char *argv[])
{
  gErrorIgnoreLevel = kWarning;
  gStyle->SetPalette(kRainBow);

  cout << endl
       << "Program compiled correctly." << endl
       << "Starting now..." << endl
       << endl;

  // Input quantities
  // Input files, relative flags (1 for real data, 0 for MC) and a short name (not necessary)
  vector<MarkedNames> filenames;
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_merged.root", 0, "", "JpsiMuNu"));
  filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_tau_merged.root", 0, "", "JpsiTauNu"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint.root", 0, "", "JpsiMuNu_Jpsiconstraint"));*/
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_NONmassConstraint_20bits_precision.root", 0, "", "JpsiMuNu_HighPrecision"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint_12bits_precision.root", 1, "", "JPsiMuNu_12bit_JPsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint_14bits_precision.root", 1, "", "JPsiMuNu_14bit_JPsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint_16bits_precision.root", 1, "", "JPsiMuNu_16bit_JPsiconstraint"));
  //filenames.push_back(MarkedNames("BcToJPsiMuMu_is_jpsi_mu_massConstraint_18bits_precision.root", 1, "", "JPsiMuNu_18bit_JPsiconstraint"));
  //filenames.push_back(MarkedNames("data_UL_2021Jan29.root", 1, "Data"));

  // Input trees to be processed (1 for real, 0 for MC)
  vector<MarkedNames> treenames;
  treenames.push_back(MarkedNames("BTo3Mu", 1));

  // Input branches to be read (1 for reco, 0 for gene)
  // Please write in a single constructor if you want a direct comparison between reco and gene
  vector<MarkedNames> branchnames;
  branchnames.push_back(MarkedNames("Bphi", 0));
  branchnames.push_back(MarkedNames("mu1pt", 1, "mu1_gen_pt", 0, "GeV"));
  branchnames.push_back(MarkedNames("mu1eta", 1, "mu1_gen_eta", 0));
  branchnames.push_back(MarkedNames("mu1phi", 1, "mu1_gen_phi", 0));
  branchnames.push_back(MarkedNames("mu2pt", 1, "mu2_gen_pt", 0, "GeV"));
  branchnames.push_back(MarkedNames("mu2eta", 1, "mu2_gen_eta", 0));
  branchnames.push_back(MarkedNames("mu2phi", 1, "mu2_gen_phi", 0));
  branchnames.push_back(MarkedNames("kpt", 1, "k_gen_pt", 0, "GeV"));
  branchnames.push_back(MarkedNames("keta", 1, "k_gen_eta", 0));
  branchnames.push_back(MarkedNames("kphi", 1, "k_gen_phi", 0));
  branchnames.push_back(MarkedNames("mu1_grandmother_pt", 0));
  branchnames.push_back(MarkedNames("mu1_grandmother_eta", 0));
  branchnames.push_back(MarkedNames("mu1_grandmother_phi", 0));
  branchnames.push_back(MarkedNames("mu1_grandmother_vx", 0));
  branchnames.push_back(MarkedNames("mu1_grandmother_vy", 0));
  branchnames.push_back(MarkedNames("mu1_grandmother_vz", 0));
  branchnames.push_back(MarkedNames("mu1_mother_vx", 0));
  branchnames.push_back(MarkedNames("mu1_mother_vy", 0));
  branchnames.push_back(MarkedNames("mu1_mother_vz", 0));
  branchnames.push_back(MarkedNames("mu1_mother_eta", 0));

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
                         "k_isFromMuT>0.5 "
;

  /*
oooooooooo.   ooooo  .oooooo..o   .oooooo.   ooooooooo.   ooooo ooo        ooooo ooooo ooooo      ooo       .o.       ooooo      ooo ooooooooooooo  .oooooo..o 
`888'   `Y8b  `888' d8P'    `Y8  d8P'  `Y8b  `888   `Y88. `888' `88.       .888' `888' `888b.     `8'      .888.      `888b.     `8' 8'   888   `8 d8P'    `Y8 
 888      888  888  Y88bo.      888           888   .d88'  888   888b     d'888   888   8 `88b.    8      .8"888.      8 `88b.    8       888      Y88bo.      
 888      888  888   `"Y8888o.  888           888ooo88P'   888   8 Y88. .P  888   888   8   `88b.  8     .8' `888.     8   `88b.  8       888       `"Y8888o.  
 888      888  888       `"Y88b 888           888`88b.     888   8  `888'   888   888   8     `88b.8    .88ooo8888.    8     `88b.8       888           `"Y88b 
 888     d88'  888  oo     .d8P `88b    ooo   888  `88b.   888   8    Y     888   888   8       `888   .8'     `888.   8       `888       888      oo     .d8P 
o888bood8P'   o888o 8""88888P'   `Y8bood8P'  o888o  o888o o888o o8o        o888o o888o o8o        `8  o88o     o8888o o8o        `8      o888o     8""88888P'  */

  // Definition of variables needed for each discriminant and variables plotter options
  unordered_map<string, vector<string>> discriminants;
  vector<VarToBePlotted> vartobeplotted;

  // Discriminants that will be used in the fit
  branchnames.push_back(MarkedNames("b_c_pisa_threemomentum", 1, "", "#vec{B_{c}^{+}}"));
  discriminants["b_c_pisa_threemomentum"] = {"mu1pt", "mu1eta", "mu1phi",
                                             "mu2pt", "mu2eta", "mu2phi",
                                             "kpt", "keta", "kphi",
                                             "beamspot_x", "beamspot_y", "pv_z",
                                             "bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z"};

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
  VarToBePlotted Q2_gene;
  branchnames.push_back(MarkedNames("Q2_gene", 1, "GeV^{2}", "Q^{2}_{gen}"));
  discriminants["Q2_gene"] = {"mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi",
                              "mu1_gen_pt", "mu1_gen_eta", "mu1_gen_phi",
                              "mu2_gen_pt", "mu2_gen_eta", "mu2_gen_phi"};
  Q2_gene.varname = "Q2_gene";
  Q2_gene.varexpression = "Q2_gene";
  Q2_gene.tobeprinted = false;
  Q2_gene.comparisonflag = true;
  Q2_gene.comparisonname = "q2_comparison";
  Q2_gene.comparisonlabel = "Q^{2}[GeV^{2}]";
  Q2_gene.comparisontitle = "Comparison of various Q^{2}";
  Q2_gene.xminlim = -8;
  Q2_gene.xmaxlim = 12;
  Q2_gene.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(Q2_gene);

  // Squared transferred momentum: B_c_pisa
  VarToBePlotted Q2_Pisa;
  branchnames.push_back(MarkedNames("Q2_Pisa", 1, "GeV^{2}", "Q^{2}_{Pisa}"));
  discriminants["Q2_Pisa"] = {"b_c_pisa_threemomentum", "Bmass",
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
  Q2_Pisa.comparisonratioexpr = "Q2_Pisa / Q2_gene";
  Q2_Pisa.comparisonratiotitle = "Q^{2}_{Pisa} over Q^{2}_{gene}";
  Q2_Pisa.comparisonratioxlabel = "#frac{Q^{2}_{Pisa}}{Q^{2}_{gene}}";
  Q2_Pisa.comparisonscattername = "scatter_q2pisa_vs_q2gene";
  Q2_Pisa.comparisonscattervars = {"Q2_gene", "Q2_Pisa"};
  Q2_Pisa.comparisonscattertitle = "Q^{2}_{Pisa} versus Q^{2}_{gene}";
  Q2_Pisa.comparisonscatterxlabel = "Q^{2}_{gene}[GeV^{2}]";
  Q2_Pisa.comparisonscatterylabel = "Q^{2}_{Pisa}[GeV^{2}]";
  Q2_Pisa.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(Q2_Pisa);

  // Squared transferred momentum: B_c_pisa
  VarToBePlotted Q2_Jona;
  branchnames.push_back(MarkedNames("Q2_Jona", 1, "GeV^{2}", "Q^{2}_{Jona}"));
  discriminants["Q2_Jona"] = {"Bpt", "Beta", "Bphi", "Bmass",
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
  Q2_Jona.comparisonratioexpr = "Q2_Jona / Q2_gene";
  Q2_Jona.comparisonratiotitle = "Q^{2}_{Jona} over Q^{2}_{gene}";
  Q2_Jona.comparisonratioxlabel = "#frac{Q^{2}_{Jona}}{Q^{2}_{gene}}";
  Q2_Jona.comparisonscattername = "scatter_q2jona_vs_q2gene";
  Q2_Jona.comparisonscattervars = {"Q2_gene", "Q2_Jona"};
  Q2_Jona.comparisonscattertitle = "Q^{2}_{Jona} versus Q^{2}_{gene}";
  Q2_Jona.comparisonscatterxlabel = "Q^{2}_{gene}[GeV^{2}]";
  Q2_Jona.comparisonscatterylabel = "Q^{2}_{Jona}[GeV^{2}]";
  Q2_Jona.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(Q2_Jona);

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
  VarToBePlotted missingm2_gene;
  branchnames.push_back(MarkedNames("missingm2_gene", 1, "GeV^{2}", "m_{miss,gene}^{2}"));
  discriminants["missingm2_gene"] = {"mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi",
                                    "mu1_gen_pt", "mu1_gen_eta", "mu1_gen_phi",
                                    "mu2_gen_pt", "mu2_gen_eta", "mu2_gen_phi",
                                    "k_gen_pt", "k_gen_eta", "k_gen_phi"};
  missingm2_gene.varname = "missingm2_gene";
  missingm2_gene.varexpression = "missingm2_gene";
  missingm2_gene.tobeprinted = false;
  missingm2_gene.comparisonflag = true;
  missingm2_gene.comparisonname = "missingm2_comparison";
  missingm2_gene.comparisonlabel = "m_{miss}^{2}[GeV^{2}]";
  missingm2_gene.comparisontitle = "Comparison of various m_{miss}^{2}";
  missingm2_gene.xminlim = -8;
  missingm2_gene.xmaxlim = 12;
  missingm2_gene.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(missingm2_gene);

  VarToBePlotted missingm2_Pisa;
  branchnames.push_back(MarkedNames("missingm2_Pisa", 1, "GeV^{2}", "m_{miss,Pisa}^{2}"));
  discriminants["missingm2_Pisa"] = {"b_c_pisa_threemomentum", "Bmass",
                                     "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                     "mu2pt", "mu2eta", "mu2phi", "mu2mass",
                                     "kpt", "keta", "kphi", "kmass"};
  missingm2_Pisa.varname = "missingm2_Pisa";
  missingm2_Pisa.varexpression = "missingm2_Pisa";
  missingm2_Pisa.tobeprinted = false;
  missingm2_Pisa.comparisonflag = true;
  missingm2_Pisa.comparisonname = "missingm2_comparison";
  missingm2_Pisa.comparisonlabel = "m_{miss}^{2}[GeV^{2}]";
  missingm2_Pisa.comparisontitle = "Comparison of various m_{miss}^{2}";
  missingm2_Pisa.xminlim = -8;
  missingm2_Pisa.xmaxlim = 12;
  missingm2_Pisa.comparisonrationame = "ratio_missingm2pisa_over_missingm2gene";
  missingm2_Pisa.comparisonratioexpr = "missingm2_Pisa / missingm2_gene";
  missingm2_Pisa.comparisonratiotitle = "m^{2}_{miss,Pisa} over m^{2}_{miss,gene}";
  missingm2_Pisa.comparisonratioxlabel = "#frac{m^{2}_{miss,Pisa}}{m^{2}_{miss,gene}}";
  missingm2_Pisa.comparisonscattername = "scatter_missingm2pisa_vs_missingm2gene";
  missingm2_Pisa.comparisonscattervars = {"missingm2_gene", "missingm2_Pisa"};
  missingm2_Pisa.comparisonscattertitle = "m^{2}_{miss,Pisa} versus m^{2}_{miss,gene}";
  missingm2_Pisa.comparisonscatterxlabel = "m^{2}_{miss,gene}[GeV^{2}]";
  missingm2_Pisa.comparisonscatterylabel = "m^{2}_{miss,Pisa}[GeV^{2}]";
  missingm2_Pisa.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(missingm2_Pisa);

  VarToBePlotted missingm2_Jona;
  branchnames.push_back(MarkedNames("missingm2_Jona", 1, "GeV^{2}", "m_{miss,Jona}^{2}"));
  discriminants["missingm2_Jona"] = {"Bpt", "Beta", "Bphi", "Bmass",
                                     "mu1pt", "mu1eta", "mu1phi", "mu1mass",
                                     "mu2pt", "mu2eta", "mu2phi", "mu2mass",
                                     "kpt", "keta", "kphi", "kmass"};
  missingm2_Jona.varname = "missingm2_Jona";
  missingm2_Jona.varexpression = "missingm2_Jona";
  missingm2_Jona.tobeprinted = false;
  missingm2_Jona.comparisonflag = true;
  missingm2_Jona.comparisonname = "missingm2_comparison";
  missingm2_Jona.comparisonlabel = "m_{miss}^{2}[GeV^{2}]";
  missingm2_Jona.comparisontitle = "Comparison of various m_{miss}^{2}";
  missingm2_Jona.xminlim = -8;
  missingm2_Jona.xmaxlim = 12;
  missingm2_Jona.comparisonrationame = "ratio_missingm2jona_over_missingm2gene";
  missingm2_Jona.comparisonratioexpr = "missingm2_Jona / missingm2_gene";
  missingm2_Jona.comparisonratiotitle = "m^{2}_{miss,Jona} over m^{2}_{miss,gene}";
  missingm2_Jona.comparisonratioxlabel = "#frac{m^{2}_{miss,Jona}}{m^{2}_{miss,gene}}";
  missingm2_Jona.comparisonscattername = "scatter_missingm2jona_vs_missingm2gene";
  missingm2_Jona.comparisonscattervars = {"missingm2_gene", "missingm2_Jona"};
  missingm2_Jona.comparisonscattertitle = "m^{2}_{miss,Jona} versus m^{2}_{miss,gene}";
  missingm2_Jona.comparisonscatterxlabel = "m^{2}_{miss,Jona}[GeV^{2}]";
  missingm2_Jona.comparisonscatterylabel = "m^{2}_{miss,Jona}[GeV^{2}]";
  missingm2_Jona.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(missingm2_Jona);

/*
ooo        ooooo ooooo  .oooooo..o  .oooooo..o ooooo ooooo      ooo   .oooooo.    ooooooooo.   ooooooooooooo 
`88.       .888' `888' d8P'    `Y8 d8P'    `Y8 `888' `888b.     `8'  d8P'  `Y8b   `888   `Y88. 8'   888   `8 
 888b     d'888   888  Y88bo.      Y88bo.       888   8 `88b.    8  888            888   .d88'      888      
 8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.   888   8   `88b.  8  888            888ooo88P'       888      
 8  `888'   888   888       `"Y88b      `"Y88b  888   8     `88b.8  888     ooooo  888              888      
 8    Y     888   888  oo     .d8P oo     .d8P  888   8       `888  `88.    .88'   888              888      
o8o        o888o o888o 8""88888P'  8""88888P'  o888o o8o        `8   `Y8bood8P'   o888o            o888o    
*/

  // Missing mass squared needs four momentum of B, mu1, mu2, k
  VarToBePlotted missingpt_gene;
  branchnames.push_back(MarkedNames("missingpt_gene", 1, "GeV^{2}", "m_{miss,gene}^{2}"));
  discriminants["missingpt_gene"] = {"mu1_grandmother_pt", "mu1_gen_pt", "mu2_gen_pt", "k_gen_pt"};
  missingpt_gene.varname = "missingpt_gene";
  missingpt_gene.varexpression = "missingpt_gene";
  missingpt_gene.tobeprinted = false;
  missingpt_gene.comparisonflag = true;
  missingpt_gene.comparisonname = "missingpt_comparison";
  missingpt_gene.comparisonlabel = "p_{T}^{miss}[GeV]";
  missingpt_gene.comparisontitle = "Comparison of various p_{T}^{miss}";
  missingpt_gene.xminlim = -8;
  missingpt_gene.xmaxlim = 12;
  missingpt_gene.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(missingpt_gene);

  VarToBePlotted missingpt_Pisa;
  branchnames.push_back(MarkedNames("missingpt_Pisa", 1, "GeV^{2}", "m_{miss,Pisa}^{2}"));
  discriminants["missingpt_Pisa"] = {"b_c_pisa_threemomentum", "Bmass", "mu1pt", "mu2pt", "kpt"};
  missingpt_Pisa.varname = "missingpt_Pisa";
  missingpt_Pisa.varexpression = "missingpt_Pisa";
  missingpt_Pisa.tobeprinted = false;
  missingpt_Pisa.comparisonflag = true;
  missingpt_Pisa.comparisonname = "missingpt_comparison";
  missingpt_Pisa.comparisonlabel = "p_{T}^{miss}[GeV]";
  missingpt_Pisa.comparisontitle = "Comparison of various p_{T}^{miss}";
  missingpt_Pisa.xminlim = -8;
  missingpt_Pisa.xmaxlim = 12;
  missingpt_Pisa.comparisonrationame = "ratio_missingptpisa_over_missingptgene";
  missingpt_Pisa.comparisonratioexpr = "missingpt_Pisa / missingpt_gene";
  missingpt_Pisa.comparisonratiotitle = "p^{miss,Pisa}_{T} over p^{miss,gene}_{T}";
  missingpt_Pisa.comparisonratioxlabel = "#frac{p^{miss}_{T,Pisa}}{p^{miss}_{T,gene}}";
  missingpt_Pisa.comparisonscattername = "scatter_missingptpisa_vs_missingptgene";
  missingpt_Pisa.comparisonscattervars = {"missingpt_gene", "missingpt_Pisa"};
  missingpt_Pisa.comparisonscattertitle = "p_{T}^{miss,Pisa} versus p_{T}^{miss,gene}";
  missingpt_Pisa.comparisonscatterxlabel = "p_{T}^{miss,gene}[GeV]";
  missingpt_Pisa.comparisonscatterylabel = "p_{T}^{miss,Pisa}[GeV]";
  missingpt_Pisa.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(missingpt_Pisa);

  VarToBePlotted missingpt_Jona;
  branchnames.push_back(MarkedNames("missingpt_Jona", 1, "GeV^{2}", "m_{miss,Jona}^{2}"));
  discriminants["missingpt_Jona"] = {"Bpt", "Bmass", "mu1pt", "mu2pt", "kpt"};
  missingpt_Jona.varname = "missingpt_Jona";
  missingpt_Jona.varexpression = "missingpt_Jona";
  missingpt_Jona.tobeprinted = false;
  missingpt_Jona.comparisonflag = true;
  missingpt_Jona.comparisonname = "missingpt_comparison";
  missingpt_Jona.comparisonlabel = "p_{T}^{miss}[GeV]";
  missingpt_Jona.comparisontitle = "Comparison of various p_{T}^{miss}";
  missingpt_Jona.xminlim = -8;
  missingpt_Jona.xmaxlim = 12;
  missingpt_Jona.comparisonrationame = "ratio_missingptjona_over_missingptgene";
  missingpt_Jona.comparisonratioexpr = "missingpt_Jona / missingpt_gene";
  missingpt_Jona.comparisonratiotitle = "p^{miss,Jona}_{T} over p^{miss,gene}_{T}";
  missingpt_Jona.comparisonratioxlabel = "#frac{p^{miss,Jona}_{T}}{p^{miss,gene}_{T}}";
  missingpt_Jona.comparisonscattername = "scatter_missingptjona_vs_missingptgene";
  missingpt_Jona.comparisonscattervars = {"missingpt_gene", "missingpt_Jona"};
  missingpt_Jona.comparisonscattertitle = "p_{T}^{miss,Jona} versus p_{T}^{miss,gene}";
  missingpt_Jona.comparisonscatterxlabel = "p_{T}^{miss,gene}[GeV]";
  missingpt_Jona.comparisonscatterylabel = "p_{T}^{miss,Jona}[GeV]";
  missingpt_Jona.wheretoplot = "OutputFiles/PNGPlots/Discriminants/";
  vartobeplotted.push_back(missingpt_Jona);

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
  branchnames.push_back(MarkedNames("eta_gene", 1, "", "#eta_{PVSVgen}"));
  discriminants["eta_gene"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                               "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle of vector from PV gene to SV reco respect to original axis
  branchnames.push_back(MarkedNames("eta_PVgeneSVreco", 1, "", "#eta_{PVgenSVrec}"));
  discriminants["eta_PVgeneSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                                       "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle of vector from PV reco to SV gene respect to original axis
  branchnames.push_back(MarkedNames("eta_PVrecoSVgene", 1, "", "#eta_{PVrecSVgen}"));
  discriminants["eta_PVrecoSVgene"] = {"beamspot_x", "beamspot_y", "pv_z",
                                       "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle of vector from PV reco to SV reco respect to original axis
  branchnames.push_back(MarkedNames("eta_reco", 1, "", "#eta_{PVSVrec}"));
  discriminants["eta_reco"] = {"pv_x", "pv_y", "pv_z",
                               "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle of vector from PV reco-beam to B reco respect to original axis
  branchnames.push_back(MarkedNames("eta_breco_beam", 1, "", "#eta_{PV_{beamspot}}"));
  discriminants["eta_breco_beam"] = {
      "beamspot_x",
      "beamspot_y",
      "pv_z",
      "bvtx_vtx_x",
      "bvtx_vtx_y",
      "bvtx_vtx_z",
  };

  // eta Angle of vector from PV reco-beam to B reco respect to original axis
  branchnames.push_back(MarkedNames("eta_breco_pv", 1, "", "#eta_{PV_{reco}}"));
  discriminants["eta_breco_pv"] = {
      "pv_x",
      "pv_y",
      "pv_z",
      "bvtx_vtx_x",
      "bvtx_vtx_y",
      "bvtx_vtx_z",
  };

  // eta Angle of vector from beamspot to SV reco respect to original axis
  branchnames.push_back(MarkedNames("eta_beamsvreco", 1, "", "#eta_{BrecSVrec}"));
  discriminants["eta_beamsvreco"] = {"beamspot_x", "beamspot_y", "pv_z",
                                     "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle between the vector from PV gene to SV gene and the vector from PV reco to SV reco
  branchnames.push_back(MarkedNames("eta_genePVSVreco", 1, "", "#eta_{B_{c}^{+}gene-PVSVrec}"));
  discriminants["eta_genePVSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                       "beamspot_x", "beamspot_y", "pv_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle between the vector from PV gene to SV gene and the vector from PV reco to SV gene
  branchnames.push_back(MarkedNames("eta_genePVrecoSVgene", 1, "", "#eta_{B_{c}^{+}gene-PVrecSVgen}"));
  discriminants["eta_genePVrecoSVgene"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                           "beamspot_x", "beamspot_y", "pv_z", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle between the vector from PV gene to SV gene and the vector from PV gene to SV reco
  branchnames.push_back(MarkedNames("eta_genePVgeneSVreco", 1, "", "#eta_{B_{c}^{+}gene-PVgenSVrec}"));
  discriminants["eta_genePVgeneSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                           "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle difference between eta(3muons) and the one of the vector from PV gene to SV gene
  branchnames.push_back(MarkedNames("eta_BcPVgeneSVgene", 1, "", "#eta_{3mu-PVgenSVgen}"));
  discriminants["eta_BcPVgeneSVgene"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                         "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle difference between eta(3muons) and the one of the vector from PV gene to SV reco
  branchnames.push_back(MarkedNames("eta_BcPVgeneSVreco", 1, "", "#eta_{3mu-PVgenSVrec}"));
  discriminants["eta_BcPVgeneSVreco"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                         "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle difference between eta(3muons) and the one of the vector from PV reco to SV gene
  branchnames.push_back(MarkedNames("eta_BcPVrecoSVgene", 1, "", "#eta_{3mu-PVrecSVgen}"));
  discriminants["eta_BcPVrecoSVgene"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                         "beamspot_x", "beamspot_y", "pv_z", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // eta Angle difference between eta(3muons) and the one of the vector from PV reco to SV reco
  branchnames.push_back(MarkedNames("eta_BcPVrecoSVreco", 1, "", "#eta_{B_{c}^{+}gene-PVgenSVrec}"));
  discriminants["eta_BcPVrecoSVreco"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                         "beamspot_x", "beamspot_y", "pv_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // eta Angle of 3 muons from true momenta
  branchnames.push_back(MarkedNames("eta_3mumomenta_gene", 1, "", "#eta_{3#mu,gene}"));
  discriminants["eta_3mumomenta_gene"] = {"mu1_gen_pt", "mu1_gen_eta", "mu1_gen_phi",
                                          "mu2_gen_pt", "mu2_gen_eta", "mu2_gen_phi",
                                          "k_gen_pt", "k_gen_eta", "k_gen_phi"};

  // eta Angle of 3 muons from reco momenta
  branchnames.push_back(MarkedNames("eta_3mumomenta_reco", 1, "", "#eta_{3#mu,reco}"));
  discriminants["eta_3mumomenta_reco"] = {"mu1pt", "mu1eta", "mu1phi",
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
  vartobeplotted.push_back(eta_resolution_grandmother_pvgensvgen);

  VarToBePlotted eta_resolution_grandmother_pvrecojpsireco;
  eta_resolution_grandmother_pvrecojpsireco.varname = "eta_resolution_grandmother_pvrecojpsireco";
  eta_resolution_grandmother_pvrecojpsireco.varexpression = "mu1_grandmother_eta - eta_reco";
  eta_resolution_grandmother_pvrecojpsireco.tobeprinted = true;
  eta_resolution_grandmother_pvrecojpsireco.plottitle = "#eta_{Grandmother}-#eta_{PVrecoJ/#psireco}";
  eta_resolution_grandmother_pvrecojpsireco.plotxlabel = "#eta_{Grandmother}-#eta_{PVrecoJ/#psireco}[]";
  eta_resolution_grandmother_pvrecojpsireco.xminlim = -1;
  eta_resolution_grandmother_pvrecojpsireco.xmaxlim = +1;
  eta_resolution_grandmother_pvrecojpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(eta_resolution_grandmother_pvrecojpsireco);

  VarToBePlotted eta_resolution_grandmother_pvrecoBreco;
  eta_resolution_grandmother_pvrecoBreco.varname = "eta_resolution_grandmother_pvrecoBreco";
  eta_resolution_grandmother_pvrecoBreco.varexpression = "mu1_grandmother_eta - eta_breco_pv";
  eta_resolution_grandmother_pvrecoBreco.tobeprinted = true;
  eta_resolution_grandmother_pvrecoBreco.plottitle = "#eta_{Grandmother}-#eta_{PVrecoBreco}";
  eta_resolution_grandmother_pvrecoBreco.plotxlabel = "#eta_{Grandmother}-#eta_{PVrecoBreco}[]";
  eta_resolution_grandmother_pvrecoBreco.xminlim = -1;
  eta_resolution_grandmother_pvrecoBreco.xmaxlim = +1;
  eta_resolution_grandmother_pvrecoBreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(eta_resolution_grandmother_pvrecoBreco);

  VarToBePlotted eta_resolution_grandmother_beamjpsireco;
  eta_resolution_grandmother_beamjpsireco.varname = "eta_resolution_grandmother_beamjpsireco";
  eta_resolution_grandmother_beamjpsireco.varexpression = "mu1_grandmother_eta - eta_beamsvreco";
  eta_resolution_grandmother_beamjpsireco.tobeprinted = true;
  eta_resolution_grandmother_beamjpsireco.plottitle = "#eta_{Grandmother}-#eta_{BeamspotJ/#psireco}";
  eta_resolution_grandmother_beamjpsireco.plotxlabel = "#eta_{Grandmother}-#eta_{BeamspotJ/#psireco}[]";
  eta_resolution_grandmother_beamjpsireco.xminlim = -1;
  eta_resolution_grandmother_beamjpsireco.xmaxlim = +1;
  eta_resolution_grandmother_beamjpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(eta_resolution_grandmother_beamjpsireco);

  VarToBePlotted eta_resolution_grandmother_beambreco;
  eta_resolution_grandmother_beambreco.varname = "eta_resolution_grandmother_beambreco";
  eta_resolution_grandmother_beambreco.varexpression = "mu1_grandmother_eta - eta_breco_beam";
  eta_resolution_grandmother_beambreco.tobeprinted = true;
  eta_resolution_grandmother_beambreco.plottitle = "#eta_{Grandmother}-#eta_{BeamspotBreco}";
  eta_resolution_grandmother_beambreco.plotxlabel = "#eta_{Grandmother}-#eta_{BeamspotBreco}[]";
  eta_resolution_grandmother_beambreco.xminlim = -1;
  eta_resolution_grandmother_beambreco.xmaxlim = +1;
  eta_resolution_grandmother_beambreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(eta_resolution_grandmother_beambreco);

  VarToBePlotted eta_resolution_grandmother_3mumomentareco;
  eta_resolution_grandmother_3mumomentareco.varname = "eta_resolution_grandmother_3mumomentareco";
  eta_resolution_grandmother_3mumomentareco.varexpression = "mu1_grandmother_eta - eta_3mumomenta_reco";
  eta_resolution_grandmother_3mumomentareco.tobeprinted = true;
  eta_resolution_grandmother_3mumomentareco.plottitle = "#eta_{Grandmother}-#eta_{3#mureco}";
  eta_resolution_grandmother_3mumomentareco.plotxlabel = "#eta_{Grandmother}-#eta_{3#mureco}[]";
  eta_resolution_grandmother_3mumomentareco.xminlim = -1;
  eta_resolution_grandmother_3mumomentareco.xmaxlim = +1;
  eta_resolution_grandmother_3mumomentareco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(eta_resolution_grandmother_3mumomentareco);

  /*ooooooooo.   ooooo   ooooo ooooo 
 `888   `Y88. `888'   `888' `888' 
  888   .d88'  888     888   888  
  888ooo88P'   888ooooo888   888  
  888          888     888   888  
  888          888     888   888  
 o888o        o888o   o888o o888o*/

  // phi Angle of vector from PV gene to SV gene respect to original axis
  branchnames.push_back(MarkedNames("phi_gene", 1, "", "#phi_{PVSVgen}"));
  discriminants["phi_gene"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                               "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle of vector from PV gene to SV reco respect to original axis
  branchnames.push_back(MarkedNames("phi_PVgeneSVreco", 1, "", "#phi_{PVgenSVrec}"));
  discriminants["phi_PVgeneSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
                                       "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle of vector from PV reco to SV gene respect to original axis
  branchnames.push_back(MarkedNames("phi_PVrecoSVgene", 1, "", "#phi_{PVrecSVgen}"));
  discriminants["phi_PVrecoSVgene"] = {"beamspot_x", "beamspot_y", "pv_z",
                                       "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle of vector from PV reco to SV reco respect to original axis
  branchnames.push_back(MarkedNames("phi_reco", 1, "", "#phi_{PVSVrec}"));
  discriminants["phi_reco"] = {"pv_x", "pv_y", "pv_z",
                               "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle of vector from PV reco-beam to B reco respect to original axis
  branchnames.push_back(MarkedNames("phi_breco_beam", 1, "", "#phi_{PV_{beamspot}}"));
  discriminants["phi_breco_beam"] = {
      "beamspot_x",
      "beamspot_y",
      "pv_z",
      "bvtx_vtx_x",
      "bvtx_vtx_y",
      "bvtx_vtx_z",
  };

  // phi Angle of vector from PV reco-beam to B reco respect to original axis
  branchnames.push_back(MarkedNames("phi_breco_pv", 1, "", "#phi_{PV_{reco}}"));
  discriminants["phi_breco_pv"] = {
      "pv_x",
      "pv_y",
      "pv_z",
      "bvtx_vtx_x",
      "bvtx_vtx_y",
      "bvtx_vtx_z",
  };

  // phi Angle of vector from beamspot to SV reco respect to original axis
  branchnames.push_back(MarkedNames("phi_beamsvreco", 1, "", "#phi_{BrecSVrec}"));
  discriminants["phi_beamsvreco"] = {"beamspot_x", "beamspot_y", "pv_z",
                                     "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle between the vector from PV gene to SV gene and the vector from PV reco to SV reco
  branchnames.push_back(MarkedNames("phi_genePVSVreco", 1, "", "#phi_{B_{c}^{+}gene-PVSVrec}"));
  discriminants["phi_genePVSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                       "beamspot_x", "beamspot_y", "pv_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle between the vector from PV gene to SV gene and the vector from PV reco to SV gene
  branchnames.push_back(MarkedNames("phi_genePVrecoSVgene", 1, "", "#phi_{B_{c}^{+}gene-PVrecSVgen}"));
  discriminants["phi_genePVrecoSVgene"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                           "beamspot_x", "beamspot_y", "pv_z", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle between the vector from PV gene to SV gene and the vector from PV gene to SV reco
  branchnames.push_back(MarkedNames("phi_genePVgeneSVreco", 1, "", "#phi_{B_{c}^{+}gene-PVgenSVrec}"));
  discriminants["phi_genePVgeneSVreco"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz",
                                           "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle difference between phi(3muons) and the one of the vector from PV gene to SV gene
  branchnames.push_back(MarkedNames("phi_BcPVgeneSVgene", 1, "", "#phi_{3mu-PVgenSVgen}"));
  discriminants["phi_BcPVgeneSVgene"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                         "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle difference between phi(3muons) and the one of the vector from PV gene to SV reco
  branchnames.push_back(MarkedNames("phi_BcPVgeneSVreco", 1, "", "#phi_{3mu-PVgenSVrec}"));
  discriminants["phi_BcPVgeneSVreco"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                         "mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle difference between phi(3muons) and the one of the vector from PV reco to SV gene
  branchnames.push_back(MarkedNames("phi_BcPVrecoSVgene", 1, "", "#phi_{3mu-PVrecSVgen}"));
  discriminants["phi_BcPVrecoSVgene"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                         "beamspot_x", "beamspot_y", "pv_z", "mu1_mother_vx", "mu1_mother_vy", "mu1_mother_vz"};

  // phi Angle difference between phi(3muons) and the one of the vector from PV reco to SV reco
  branchnames.push_back(MarkedNames("phi_BcPVrecoSVreco", 1, "", "#phi_{B_{c}^{+}gene-PVgenSVrec}"));
  discriminants["phi_BcPVrecoSVreco"] = {"bvtx_vtx_x", "bvtx_vtx_y", "bvtx_vtx_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z",
                                         "beamspot_x", "beamspot_y", "pv_z", "jpsivtx_vtx_x", "jpsivtx_vtx_y", "jpsivtx_vtx_z"};

  // phi Angle of 3 muons from true momenta
  branchnames.push_back(MarkedNames("phi_3mumomenta_gene", 1, "", "#phi_{3#mu,gene}"));
  discriminants["phi_3mumomenta_gene"] = {"mu1_gen_pt", "mu1_gen_phi", "mu1_gen_phi",
                                          "mu2_gen_pt", "mu2_gen_phi", "mu2_gen_phi",
                                          "k_gen_pt", "k_gen_phi", "k_gen_phi"};

  // phi Angle of 3 muons from reco momenta
  branchnames.push_back(MarkedNames("phi_3mumomenta_reco", 1, "", "#phi_{3#mu,reco}"));
  discriminants["phi_3mumomenta_reco"] = {"mu1pt", "mu1phi", "mu1phi",
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
  vartobeplotted.push_back(phi_resolution_grandmother_pvgensvgen);

  VarToBePlotted phi_resolution_grandmother_pvrecojpsireco;
  phi_resolution_grandmother_pvrecojpsireco.varname = "phi_resolution_grandmother_pvrecojpsireco";
  phi_resolution_grandmother_pvrecojpsireco.varexpression = "mu1_grandmother_phi - phi_reco";
  phi_resolution_grandmother_pvrecojpsireco.tobeprinted = true;
  phi_resolution_grandmother_pvrecojpsireco.plottitle = "#phi_{Grandmother}-#phi_{PVrecoJ/#psireco}";
  phi_resolution_grandmother_pvrecojpsireco.plotxlabel = "#phi_{Grandmother}-#phi_{PVrecoJ/#psireco}[]";
  phi_resolution_grandmother_pvrecojpsireco.xminlim = -1;
  phi_resolution_grandmother_pvrecojpsireco.xmaxlim = +1;
  phi_resolution_grandmother_pvrecojpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(phi_resolution_grandmother_pvrecojpsireco);

  VarToBePlotted phi_resolution_grandmother_pvrecoBreco;
  phi_resolution_grandmother_pvrecoBreco.varname = "phi_resolution_grandmother_pvrecoBreco";
  phi_resolution_grandmother_pvrecoBreco.varexpression = "mu1_grandmother_phi - phi_breco_pv";
  phi_resolution_grandmother_pvrecoBreco.tobeprinted = true;
  phi_resolution_grandmother_pvrecoBreco.plottitle = "#phi_{Grandmother}-#phi_{PVrecoBreco}";
  phi_resolution_grandmother_pvrecoBreco.plotxlabel = "#phi_{Grandmother}-#phi_{PVrecoBreco}[]";
  phi_resolution_grandmother_pvrecoBreco.xminlim = -1;
  phi_resolution_grandmother_pvrecoBreco.xmaxlim = +1;
  phi_resolution_grandmother_pvrecoBreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(phi_resolution_grandmother_pvrecoBreco);

  VarToBePlotted phi_resolution_grandmother_beamjpsireco;
  phi_resolution_grandmother_beamjpsireco.varname = "phi_resolution_grandmother_beamjpsireco";
  phi_resolution_grandmother_beamjpsireco.varexpression = "mu1_grandmother_phi - phi_beamsvreco";
  phi_resolution_grandmother_beamjpsireco.tobeprinted = true;
  phi_resolution_grandmother_beamjpsireco.plottitle = "#phi_{Grandmother}-#phi_{BeamspotJ/#psireco}";
  phi_resolution_grandmother_beamjpsireco.plotxlabel = "#phi_{Grandmother}-#phi_{BeamspotJ/#psireco}[]";
  phi_resolution_grandmother_beamjpsireco.xminlim = -1;
  phi_resolution_grandmother_beamjpsireco.xmaxlim = +1;
  phi_resolution_grandmother_beamjpsireco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(phi_resolution_grandmother_beamjpsireco);

  VarToBePlotted phi_resolution_grandmother_beambreco;
  phi_resolution_grandmother_beambreco.varname = "phi_resolution_grandmother_beambreco";
  phi_resolution_grandmother_beambreco.varexpression = "mu1_grandmother_phi - phi_breco_beam";
  phi_resolution_grandmother_beambreco.tobeprinted = true;
  phi_resolution_grandmother_beambreco.plottitle = "#phi_{Grandmother}-#phi_{BeamspotBreco}";
  phi_resolution_grandmother_beambreco.plotxlabel = "#phi_{Grandmother}-#phi_{BeamspotBreco}[]";
  phi_resolution_grandmother_beambreco.xminlim = -1;
  phi_resolution_grandmother_beambreco.xmaxlim = +1;
  phi_resolution_grandmother_beambreco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(phi_resolution_grandmother_beambreco);

  VarToBePlotted phi_resolution_grandmother_3mumomentareco;
  phi_resolution_grandmother_3mumomentareco.varname = "phi_resolution_grandmother_3mumomentareco";
  phi_resolution_grandmother_3mumomentareco.varexpression = "mu1_grandmother_phi - phi_3mumomenta_reco";
  phi_resolution_grandmother_3mumomentareco.tobeprinted = true;
  phi_resolution_grandmother_3mumomentareco.plottitle = "#phi_{Grandmother}-#phi_{3#mureco}";
  phi_resolution_grandmother_3mumomentareco.plotxlabel = "#phi_{Grandmother}-#phi_{3#mureco}[]";
  phi_resolution_grandmother_3mumomentareco.xminlim = -1;
  phi_resolution_grandmother_3mumomentareco.xmaxlim = +1;
  phi_resolution_grandmother_3mumomentareco.wheretoplot = "OutputFiles/PNGPlots/Angles/";
  vartobeplotted.push_back(phi_resolution_grandmother_3mumomentareco);

  /*
      .o.       ooooo      ooo   .oooooo.    ooooo        oooooooooooo 
     .888.      `888b.     `8'  d8P'  `Y8b   `888'        `888'     `8 
    .8"888.      8 `88b.    8  888            888          888         
   .8' `888.     8   `88b.  8  888            888          888oooo8    
  .88ooo8888.    8     `88b.8  888     ooooo  888          888    "    
 .8'     `888.   8       `888  `88.    .88'   888       o  888       o 
o88o     o8888o o8o        `8   `Y8bood8P'   o888ooooood8 o888ooooood8 */

  VarToBePlotted angle_grandmother_pvsvgen;
  branchnames.push_back(MarkedNames("angle_grandmother_pvsvgen", 1, "", "#vec{Grandmother,Mother}#angle#vec{PVgen,SVgen}"));
  discriminants["angle_grandmother_pvsvgen"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
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
  vartobeplotted.push_back(angle_grandmother_pvsvgen);

  VarToBePlotted angle_grandmother_PVrecoBreco;
  branchnames.push_back(MarkedNames("angle_grandmother_PVrecoBreco", 1, "", "#vec{Grandmother,Mother}#angle#vec{PVrec,Brec}"));
  discriminants["angle_grandmother_PVrecoBreco"] = {"pv_x", "pv_y", "pv_z",
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
  vartobeplotted.push_back(angle_grandmother_PVrecoBreco);

  VarToBePlotted angle_grandmother_BeamBreco;
  branchnames.push_back(MarkedNames("angle_grandmother_BeamBreco", 1, "", "#vec{(Grandmother,Mother)}#angle#vec{(Beam,Brec)}"));
  discriminants["angle_grandmother_BeamBreco"] = {"beamspot_x", "beamspot_y", "pv_z",
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
  vartobeplotted.push_back(angle_grandmother_BeamBreco);

  VarToBePlotted angle_grandmother_PVrecoJpsireco;
  branchnames.push_back(MarkedNames("angle_grandmother_PVrecoJpsireco", 1, "", "#vec{(Grandmother,Mother)}#angle#vec{(PVrec,J/#psirec)}"));
  discriminants["angle_grandmother_PVrecoJpsireco"] = {"pv_x", "pv_y", "pv_z",
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
  vartobeplotted.push_back(angle_grandmother_PVrecoJpsireco);

  VarToBePlotted angle_grandmother_BeamJpsireco;
  branchnames.push_back(MarkedNames("angle_grandmother_BeamJpsireco", 1, "", "#vec{(Grandmother,Mother)}#angle#vec{(Beam,J/#psirec)}"));
  discriminants["angle_grandmother_BeamJpsireco"] = {"beamspot_x", "beamspot_y", "pv_z",
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
  vartobeplotted.push_back(angle_grandmother_BeamJpsireco);

  VarToBePlotted angle_grandmother_3mureco;
  branchnames.push_back(MarkedNames("angle_grandmother_3mureco", 1, "", "#vec{(Grandmother,Mother)}#angle#vec{(3#mureco)}"));
  discriminants["angle_grandmother_3mureco"] = {"mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi",
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
  vartobeplotted.push_back(angle_grandmother_3mureco);

  VarToBePlotted cos_angle_grandmother_pvsvgen;
  branchnames.push_back(MarkedNames("cos_angle_grandmother_pvsvgen", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVgen,SVgen)})"));
  discriminants["cos_angle_grandmother_pvsvgen"] = {"mu1_grandmother_vx", "mu1_grandmother_vy", "mu1_grandmother_vz",
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
  vartobeplotted.push_back(cos_angle_grandmother_pvsvgen);

  VarToBePlotted cos_angle_grandmother_PVrecoBreco;
  branchnames.push_back(MarkedNames("cos_angle_grandmother_PVrecoBreco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVrec,Brec)})"));
  discriminants["cos_angle_grandmother_PVrecoBreco"] = {"pv_x", "pv_y", "pv_z",
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
  vartobeplotted.push_back(cos_angle_grandmother_PVrecoBreco);

  VarToBePlotted cos_angle_grandmother_BeamBreco;
  branchnames.push_back(MarkedNames("cos_angle_grandmother_BeamBreco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(Beam,Brec)})"));
  discriminants["cos_angle_grandmother_BeamBreco"] = {"beamspot_x", "beamspot_y", "pv_z",
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
  vartobeplotted.push_back(cos_angle_grandmother_BeamBreco);

  VarToBePlotted cos_angle_grandmother_PVrecoJpsireco;
  branchnames.push_back(MarkedNames("cos_angle_grandmother_PVrecoJpsireco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(PVrec,J/#psirec)})"));
  discriminants["cos_angle_grandmother_PVrecoJpsireco"] = {"pv_x", "pv_y", "pv_z",
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
  vartobeplotted.push_back(cos_angle_grandmother_PVrecoJpsireco);

  VarToBePlotted cos_angle_grandmother_BeamJpsireco;
  branchnames.push_back(MarkedNames("cos_angle_grandmother_BeamJpsireco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(Beam,J/#psirec)})"));
  discriminants["cos_angle_grandmother_BeamJpsireco"] = {"beamspot_x", "beamspot_y", "pv_z",
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
  vartobeplotted.push_back(cos_angle_grandmother_BeamJpsireco);

  VarToBePlotted cos_angle_grandmother_3mureco;
  branchnames.push_back(MarkedNames("cos_angle_grandmother_3mureco", 1, "", "cos(#vec{(Grandmother,Mother)}#angle#vec{(3#mureco)})"));
  discriminants["cos_angle_grandmother_3mureco"] = {"mu1_grandmother_pt", "mu1_grandmother_eta", "mu1_grandmother_phi",
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
  vartobeplotted.push_back(cos_angle_grandmother_3mureco);

  // Useful Variables
  /*  vector<TString> question;
  string answer;

  question.push_back("Do you wish to analyze these files? (YES/NO)");
  PrintFuncInfo(question);
  question.clear();
  for (vector<MarkedNames>::iterator stringit = filenames.begin(); stringit != filenames.end(); ++stringit)
  {
    cout << "\t" << stringit->name.Data() << endl;
  }
  cin >> answer;
  if (answer.compare("YES") == 0 || answer.compare("yes") == 0 || answer.compare("Yes") == 0 || answer.compare("y") == 0 || answer.compare("Y") == 0)
  {*/
  //The user wants to analyze the files
  //TreeRDFDiscriminants(filenames, treenames, branchnames, discriminants, pre_cuts, true);
  //DiscriminantsPlotter(filenames, treenames, vartobeplotted);

  // Plotting
  ROOT::RDataFrame *BtoMu = new ROOT::RDataFrame("JpsiMuNu_BTo3Mu", "ManipulatedVariables.root");
  ROOT::RDataFrame *BtoTau = new ROOT::RDataFrame("JpsiTauNu_BTo3Mu", "ManipulatedVariables.root");
  ROOT::RDF::RResultPtr<TH1F> histcompare_norm, histcompare_sig;
  TH1F histcompare_ratio, histcompare_scatter;
  TCanvas canvas_comparison = TCanvas("canvas_comparison", "canvas_comparison", 1360, 768);
  TCanvas canvas_comparison_ratio = TCanvas("canvas_comparison_ratio", "canvas_comparison_ratio", 1360, 768);

/*
  .oooooo.        .oooo.          .oooooo.    oooooooooooo ooooo      ooo oooooooooooo 
 d8P'  `Y8b     .dP""Y88b        d8P'  `Y8b   `888'     `8 `888b.     `8' `888'     `8 
888      888          ]8P'      888            888          8 `88b.    8   888         
888      888        .d8P'       888            888oooo8     8   `88b.  8   888oooo8    
888      888      .dP'          888     ooooo  888    "     8     `88b.8   888    "    
`88b    d88b    .oP     .o      `88.    .88'   888       o  8       `888   888       o 
 `Y8bood8P'Ybd' 8888888888       `Y8bood8P'   o888ooooood8 o8o        `8  o888ooooood8
*/

  Float_t q2_bins = 100, q2_min_cut = -10, q2_max_cut = +11;
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("q2_gene_norm", "q2_gene_norm", 100, 0, 11), {"Q2_gene"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("GEN: Q^{2}_{sig} and Q^{2}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("Q^{2}[GeV^{2}]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("q2_gene_sig", "q2_gene_sig", 100, 0, 11), {"Q2_gene"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  auto *legend = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend->AddEntry("q2_gene_norm", "q2_gene_norm", "PLC PMC");
  legend->AddEntry("q2_gene_sig", "q2_gene_sig", "PLC PMC");
  legend->SetTextSize(0.025);
  legend->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/q2_gene_sig_norm_comparison.png");
  canvas_comparison.Clear();

/*
  .oooooo.        .oooo.        ooooooooo.   ooooo  .oooooo..o       .o.       
 d8P'  `Y8b     .dP""Y88b       `888   `Y88. `888' d8P'    `Y8      .888.      
888      888          ]8P'       888   .d88'  888  Y88bo.          .8"888.     
888      888        .d8P'        888ooo88P'   888   `"Y8888o.     .8' `888.    
888      888      .dP'           888          888       `"Y88b   .88ooo8888.   
`88b    d88b    .oP     .o       888          888  oo     .d8P  .8'     `888.  
 `Y8bood8P'Ybd' 8888888888      o888o        o888o 8""88888P'  o88o     o8888o
*/

  // Plotting
  canvas_comparison.Clear();
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("q2_pisa_norm", "q2_pisa_norm", q2_bins, q2_min_cut, q2_max_cut), {"Q2_Pisa"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("PISA: Q^{2}_{sig} and Q^{2}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("Q^{2}[GeV^{2}]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("q2_pisa_sig", "q2_pisa_sig", q2_bins, q2_min_cut, q2_max_cut), {"Q2_Pisa"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  auto *legend2 = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend2->AddEntry("q2_pisa_norm", "q2_pisa_norm", "PLC PMC");
  legend2->AddEntry("q2_pisa_sig", "q2_pisa_sig", "PLC PMC");
  legend2->SetTextSize(0.025);
  legend2->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/q2_pisa_sig_norm_comparison.png");
  canvas_comparison.Clear();

  // Ratio
  histcompare_norm->Sumw2();
  histcompare_sig->Sumw2();
  histcompare_norm->Scale(1/histcompare_norm->Integral());
  histcompare_sig->Scale(1/histcompare_sig->Integral());
  histcompare_ratio = *(TH1F*)histcompare_sig->Clone();
  TH1F *histsum = (TH1F*)(*histcompare_sig+*histcompare_norm).Clone();
  histcompare_ratio.Divide(histcompare_sig.GetPtr(), histsum);
  canvas_comparison_ratio.cd();
  histcompare_ratio.SetName("q2_pisa_ratio_sig_norm");
  histcompare_ratio.SetTitle("PISA: Ratio between Q^{2}_{pisa,sig} and Q^{2}_{pisa,norm}+Q^{2}_{pisa,sig}");
  histcompare_ratio.SetStats(kTRUE);
  histcompare_ratio.SetFillStyle(1001);
  histcompare_ratio.SetFillColor(kBlue);
  histcompare_ratio.SetFillColorAlpha(kBlue, 0.8);
  histcompare_ratio.SetMarkerStyle(kFullSquare);
  histcompare_ratio.SetMarkerColor(kBlue);
  histcompare_ratio.SetMarkerSize(1);
  histcompare_ratio.GetXaxis()->SetTitle("#frac{Q^{2}_{pisa,sig}}{Q^{2}_{pisa,norm}+Q^{2}_{pisa,sig}}");
  histcompare_ratio.GetXaxis()->SetTitleOffset(1.4);
  histcompare_ratio.GetXaxis()->SetTitleSize(0.025);
  histcompare_ratio.GetYaxis()->SetTitle("Occurences");
  histcompare_ratio.Draw("HF PMC");
  canvas_comparison_ratio.Update();

  //Adjusting the stats
  TPaveStats *ratiostats2 = (TPaveStats *)histcompare_ratio.GetListOfFunctions()->FindObject("stats");
  ratiostats2->SetOptStat(112211);
  ratiostats2->SetX1NDC(0.1);
  ratiostats2->SetX2NDC(0.28);
  ratiostats2->SetY1NDC(0.66);
  ratiostats2->SetY2NDC(0.78);
  ratiostats2->Draw("same");

  //Adjusting the legend
  auto *legend_comparison_ratio = new TLegend;
  legend_comparison_ratio->SetHeader("Pisa", "C");
  legend_comparison_ratio->AddEntry("q2_pisa_ratio_sig_norm", "#frac{Q^{2}_{Pisa,sig}}{Q^{2}_{Pisa,norm}+Q^{2}_{pisa,sig}}", "F PMC");
  legend_comparison_ratio->Draw("same");
  gPad->Update();
  legend_comparison_ratio->SetTextSize(0.019);
  legend_comparison_ratio->SetX1NDC(0.1);
  legend_comparison_ratio->SetX2NDC(0.28);
  legend_comparison_ratio->SetY1NDC(0.78);
  legend_comparison_ratio->SetY2NDC(0.9);
  auto header = (TLegendEntry *)legend_comparison_ratio->GetListOfPrimitives()->First();
  header->SetTextSize(0.019);
  header->Draw("same");
  gPad->Modified();
  canvas_comparison_ratio.Print("OutputFiles/PNGPlots/q2_pisa_ratio_sig_norm.png");
  histcompare_ratio.Reset("ICESM");
  legend_comparison_ratio->Clear();
  
  vector<Double_t> q2_pisa_cuts, q2_pisa_sig_eff, q2_pisa_norm_eff, purity_pisa; 
  for (int i=0; i<q2_bins; i++)
  {
    q2_pisa_cuts.push_back(q2_min_cut+i*(q2_max_cut-q2_min_cut)/q2_bins);
    Double_t q2_pisa_sig_integral = histcompare_sig->Integral(histcompare_sig->FindFixBin(q2_pisa_cuts[i]), histcompare_sig->FindFixBin(q2_max_cut));
    Double_t q2_pisa_norm_integral = histcompare_norm->Integral(histcompare_norm->FindFixBin(q2_pisa_cuts[i]),histcompare_norm->FindFixBin(q2_max_cut));
    q2_pisa_sig_eff.push_back(q2_pisa_sig_integral);
    q2_pisa_norm_eff.push_back(q2_pisa_norm_integral);
    purity_pisa.push_back( 1/ (1 + ( ( q2_pisa_norm_integral*(1- q2_pisa_norm_integral) ) / (0.71 * q2_pisa_sig_integral * q2_pisa_sig_integral) ) ) );
  }

  TCanvas *canvas_eff_curve = new TCanvas("Signal-Background efficiencies", "Signal-Background efficiencies", 1366, 768);
  TGraph *q2_pisa_eff_curve_sig = new TGraph(q2_pisa_sig_eff.size(), q2_pisa_cuts.data(), q2_pisa_sig_eff.data());
  canvas_eff_curve->cd();
  q2_pisa_eff_curve_sig->SetTitle("Signal-Background efficiencies");
  q2_pisa_eff_curve_sig->GetXaxis()->SetTitle("Cut applied on Q^{2}[GeV^{2}]");
  q2_pisa_eff_curve_sig->GetYaxis()->SetTitle("Efficiencies");
  q2_pisa_eff_curve_sig->SetMarkerStyle(21);
  q2_pisa_eff_curve_sig->SetMarkerColor(kBlue);
  q2_pisa_eff_curve_sig->Draw("apl PLC");
  TGraph *q2_pisa_eff_curve_norm = new TGraph(q2_pisa_sig_eff.size(), q2_pisa_cuts.data(), q2_pisa_norm_eff.data());
  q2_pisa_eff_curve_norm->SetTitle("Normalization Efficiency");
  q2_pisa_eff_curve_norm->SetMarkerStyle(21);
  q2_pisa_eff_curve_norm->SetMarkerColor(kRed);
  q2_pisa_eff_curve_norm->Draw("pl PLC SAME");

  //Adjusting the legend
  auto *legend_roc = new TLegend;
  legend_roc->SetHeader("Pisa", "C");
  legend_roc->AddEntry(q2_pisa_eff_curve_sig, "Signal", "PLC");
  legend_roc->AddEntry(q2_pisa_eff_curve_norm, "Normalization", "pl PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.75);
  legend_roc->SetX2NDC(0.9);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_eff_curve->Print("q2_pisa_eff_curve.png");

/*
  .oooooo.        .oooo.           oooo   .oooooo.   ooooo      ooo       .o.       
 d8P'  `Y8b     .dP""Y88b          `888  d8P'  `Y8b  `888b.     `8'      .888.      
888      888          ]8P'          888 888      888  8 `88b.    8      .8"888.     
888      888        .d8P'           888 888      888  8   `88b.  8     .8' `888.    
888      888      .dP'              888 888      888  8     `88b.8    .88ooo8888.   
`88b    d88b    .oP     .o          888 `88b    d88'  8       `888   .8'     `888.  
 `Y8bood8P'Ybd' 8888888888      .o. 88P  `Y8bood8P'  o8o        `8  o88o     o8888o 
                                `Y888P
*/

  // Plotting
  canvas_comparison.Clear();
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("q2_jona_norm", "q2_jona_norm", q2_bins, q2_min_cut, q2_max_cut), {"Q2_Jona"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("JONA: Q^{2}_{sig} and Q^{2}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("Q^{2}[GeV^{2}]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("q2_jona_sig", "q2_jona_sig", q2_bins, q2_min_cut, q2_max_cut), {"Q2_Jona"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  auto *legend3 = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend3->AddEntry("q2_jona_norm", "q2_jona_norm", "PLC PMC");
  legend3->AddEntry("q2_jona_sig", "q2_jona_sig", "PLC PMC");
  legend3->SetTextSize(0.025);
  legend3->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/q2_jona_sig_norm_comparison.png");
  canvas_comparison.Clear();

  // Ratio
  histcompare_norm->Sumw2();
  histcompare_sig->Sumw2();
  histcompare_norm->Scale(1/histcompare_norm->Integral());
  histcompare_sig->Scale(1/histcompare_sig->Integral());
  histcompare_ratio = *(TH1F*)histcompare_sig->Clone();
  histsum = (TH1F*)(*histcompare_sig+*histcompare_norm).Clone();
  histcompare_ratio.Divide(histcompare_sig.GetPtr(), histsum);
  canvas_comparison_ratio.cd();
  histcompare_ratio.SetName("q2_jona_ratio_sig_norm");
  histcompare_ratio.SetTitle("Jona: Ratio between Q^{2}_{Jona,sig} and Q^{2}_{Jona,norm}+Q^{2}_{Jona,sig}");
  histcompare_ratio.SetStats(kTRUE);
  histcompare_ratio.SetFillStyle(1001);
  histcompare_ratio.SetFillColor(kBlue);
  histcompare_ratio.SetFillColorAlpha(kBlue, 0.8);
  histcompare_ratio.SetMarkerStyle(kFullSquare);
  histcompare_ratio.SetMarkerColor(kBlue);
  histcompare_ratio.SetMarkerSize(1);
  histcompare_ratio.GetXaxis()->SetTitle("#frac{Q^{2}_{Jona,sig}}{Q^{2}_{Jona,norm}+Q^{2}_{Jona,sig}}");
  histcompare_ratio.GetXaxis()->SetTitleOffset(1.4);
  histcompare_ratio.GetXaxis()->SetTitleSize(0.025);
  histcompare_ratio.GetYaxis()->SetTitle("Occurences");
  histcompare_ratio.Draw("HF PMC");
  canvas_comparison_ratio.Update();

  //Adjusting the stats
  TPaveStats *ratiostats3 = (TPaveStats *)histcompare_ratio.GetListOfFunctions()->FindObject("stats");
  ratiostats3->SetOptStat(112211);
  ratiostats3->SetX1NDC(0.1);
  ratiostats3->SetX2NDC(0.28);
  ratiostats3->SetY1NDC(0.66);
  ratiostats3->SetY2NDC(0.78);
  ratiostats3->Draw("same");

  //Adjusting the legend
  legend_comparison_ratio->Clear();
  legend_comparison_ratio->SetX1NDC(0.1);
  legend_comparison_ratio->SetX2NDC(0.28);
  legend_comparison_ratio->SetY1NDC(0.78);
  legend_comparison_ratio->SetY2NDC(0.9);
  legend_comparison_ratio->SetHeader("Jona", "C");
  legend_comparison_ratio->AddEntry("q2_jona_ratio_sig_norm", "#frac{Q^{2}_{Jona,sig}}{Q^{2}_{Jona,norm}}", "PMC");
  legend_comparison_ratio->SetTextSize(0.02);
  legend_comparison_ratio->Draw("same");
  header = (TLegendEntry *)legend_comparison_ratio->GetListOfPrimitives()->First();
  header->SetTextSize(0.02);
  header->Draw("same");
  canvas_comparison_ratio.Update();
  canvas_comparison_ratio.Print("OutputFiles/PNGPlots/q2_jona_ratio_sig_norm.png");
  histcompare_ratio.Reset("ICESM");  

  vector<Double_t> q2_jona_cuts, q2_jona_sig_eff, q2_jona_norm_eff, purity_jona; 
  for (int i=0; i<q2_bins; i++)
  {
    q2_jona_cuts.push_back(q2_min_cut+i*(q2_max_cut-q2_min_cut)/q2_bins);
    Double_t q2_jona_sig_integral = histcompare_sig->Integral(histcompare_sig->FindFixBin(q2_jona_cuts[i]), histcompare_sig->FindFixBin(q2_max_cut));
    Double_t q2_jona_norm_integral = histcompare_norm->Integral(histcompare_norm->FindFixBin(q2_jona_cuts[i]),histcompare_norm->FindFixBin(q2_max_cut));
    q2_jona_sig_eff.push_back(q2_jona_sig_integral);
    q2_jona_norm_eff.push_back(q2_jona_norm_integral);
    purity_jona.push_back( 1/ (1 + ( ( q2_jona_norm_integral*(1- q2_jona_norm_integral) ) / (0.71 * q2_jona_sig_integral * q2_jona_sig_integral) ) ) );
  }

  TGraph *q2_jona_eff_curve_sig = new TGraph(q2_jona_sig_eff.size(), q2_jona_cuts.data(), q2_jona_sig_eff.data());
  canvas_eff_curve->Clear();
  canvas_eff_curve->cd();
  q2_jona_eff_curve_sig->SetTitle("Signal-Background efficiencies");
  q2_jona_eff_curve_sig->GetXaxis()->SetTitle("Cut applied on Q^{2}[GeV^{2}]");
  q2_jona_eff_curve_sig->GetYaxis()->SetTitle("Efficiencies");
  q2_jona_eff_curve_sig->SetMarkerStyle(21);
  q2_jona_eff_curve_sig->SetMarkerColor(kBlue);
  q2_jona_eff_curve_sig->Draw("apl PLC");
  TGraph *q2_jona_eff_curve_norm = new TGraph(q2_jona_sig_eff.size(), q2_jona_cuts.data(), q2_jona_norm_eff.data());
  q2_jona_eff_curve_norm->SetTitle("Normalization Efficiency");
  q2_jona_eff_curve_norm->SetMarkerStyle(21);
  q2_jona_eff_curve_norm->SetMarkerColor(kRed);
  q2_jona_eff_curve_norm->Draw("pl PLC SAME");

  //Adjusting the legend
  legend_roc->Clear();
  legend_roc->SetHeader("Jona", "C");
  legend_roc->AddEntry(q2_jona_eff_curve_sig, "Signal", "PLC");
  legend_roc->AddEntry(q2_jona_eff_curve_norm, "Normalization", "pl PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.75);
  legend_roc->SetX2NDC(0.9);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_eff_curve->Print("q2_jona_eff_curve.png");

/*
  .oooooo.        .oooo.        ooooooooo.     .oooooo.     .oooooo.   
 d8P'  `Y8b     .dP""Y88b       `888   `Y88.  d8P'  `Y8b   d8P'  `Y8b  
888      888          ]8P'       888   .d88' 888      888 888          
888      888        .d8P'        888ooo88P'  888      888 888          
888      888      .dP'           888`88b.    888      888 888          
`88b    d88b    .oP     .o       888  `88b.  `88b    d88' `88b    ooo  
 `Y8bood8P'Ybd' 8888888888      o888o  o888o  `Y8bood8P'   `Y8bood8P'
*/
  // ROC comparison
  TCanvas *canvas_roc_curve = new TCanvas("ROC curve comparison", "ROC curve comparison", 1366, 768);
  TGraph *q2_pisa_roc_curve = new TGraph(purity_pisa.size(), purity_pisa.data(), q2_pisa_sig_eff.data());
  TGraph *q2_jona_roc_curve = new TGraph(purity_jona.size(), purity_jona.data(), q2_jona_sig_eff.data());
  canvas_roc_curve->Clear();
  canvas_roc_curve->cd();
  q2_pisa_roc_curve->SetTitle("ROC curve: Q^{2}_{Pisa} and Q^{2}_{Jona} comparison");
  q2_pisa_roc_curve->GetXaxis()->SetTitle("Purity");
  q2_pisa_roc_curve->GetYaxis()->SetTitle("Signal efficiency");
  q2_pisa_roc_curve->SetMarkerStyle(21);
  q2_pisa_roc_curve->SetMarkerColor(kBlue);
  q2_pisa_roc_curve->Draw("apl PLC");
  q2_jona_roc_curve->SetMarkerStyle(21);
  q2_jona_roc_curve->SetMarkerColor(kRed);
  q2_jona_roc_curve->Draw("pl PLC SAME");

  //Adjusting the legend
  legend_roc->Clear();
  legend_roc->SetHeader("ROC curve for Q^{2}", "C");
  legend_roc->AddEntry(q2_pisa_roc_curve, "Pisa Q^{2}", "PLC");
  legend_roc->AddEntry(q2_jona_roc_curve, "Jona Q^{2}", "PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.1);
  legend_roc->SetX2NDC(0.25);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_roc_curve->Print("q2_roc_curve.png");

/*
ooo        ooooo   .oooo.   ooo        ooooo ooooo  .oooooo..o  .oooooo..o        .oooooo.    oooooooooooo ooooo      ooo oooooooooooo 
`88.       .888' .dP""Y88b  `88.       .888' `888' d8P'    `Y8 d8P'    `Y8       d8P'  `Y8b   `888'     `8 `888b.     `8' `888'     `8 
 888b     d'888        ]8P'  888b     d'888   888  Y88bo.      Y88bo.           888            888          8 `88b.    8   888         
 8 Y88. .P  888      .d8P'   8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.       888            888oooo8     8   `88b.  8   888oooo8    
 8  `888'   888    .dP'      8  `888'   888   888       `"Y88b      `"Y88b      888     ooooo  888    "     8     `88b.8   888    "    
 8    Y     888  .oP     .o  8    Y     888   888  oo     .d8P oo     .d8P      `88.    .88'   888       o  8       `888   888       o 
o8o        o888o 8888888888 o8o        o888o o888o 8""88888P'  8""88888P'        `Y8bood8P'   o888ooooood8 o8o        `8  o888ooooood8
*/

  Double_t missingm2_bins = 100, missingm2_min_cut = -10, missingm2_max_cut = +11;
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("missingm2_gene_norm", "missingm2_gene_norm", 100, 0, 11), {"missingm2_gene"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("GEN: m_{miss}^{2}_{sig} and m_{miss}^{2}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("m_{miss}^{2}[GeV^{2}]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("missingm2_gene_sig", "missingm2_gene_sig", 100, 0, 11), {"missingm2_gene"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  legend = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend->AddEntry("missingm2_gene_norm", "missingm2_gene_norm", "PLC PMC");
  legend->AddEntry("missingm2_gene_sig", "missingm2_gene_sig", "PLC PMC");
  legend->SetTextSize(0.025);
  legend->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/missingm2_gene_sig_norm_comparison.png");
  canvas_comparison.Clear();

/*
ooo        ooooo   .oooo.   ooo        ooooo ooooo  .oooooo..o  .oooooo..o      ooooooooo.   ooooo  .oooooo..o       .o.       
`88.       .888' .dP""Y88b  `88.       .888' `888' d8P'    `Y8 d8P'    `Y8      `888   `Y88. `888' d8P'    `Y8      .888.      
 888b     d'888        ]8P'  888b     d'888   888  Y88bo.      Y88bo.            888   .d88'  888  Y88bo.          .8"888.     
 8 Y88. .P  888      .d8P'   8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.        888ooo88P'   888   `"Y8888o.     .8' `888.    
 8  `888'   888    .dP'      8  `888'   888   888       `"Y88b      `"Y88b       888          888       `"Y88b   .88ooo8888.   
 8    Y     888  .oP     .o  8    Y     888   888  oo     .d8P oo     .d8P       888          888  oo     .d8P  .8'     `888.  
o8o        o888o 8888888888 o8o        o888o o888o 8""88888P'  8""88888P'       o888o        o888o 8""88888P'  o88o     o8888o
*/

  // Plotting
  canvas_comparison.Clear();
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("missingm2_pisa_norm", "missingm2_pisa_norm", missingm2_bins, missingm2_min_cut, missingm2_max_cut), {"missingm2_Pisa"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("PISA: m_{miss}^{2}_{sig} and m_{miss}^{2}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("m_{miss}^{2}[GeV^{2}]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("missingm2_pisa_sig", "missingm2_pisa_sig", missingm2_bins, missingm2_min_cut, missingm2_max_cut), {"missingm2_Pisa"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  legend2 = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend2->AddEntry("missingm2_pisa_norm", "missingm2_pisa_norm", "PLC PMC");
  legend2->AddEntry("missingm2_pisa_sig", "missingm2_pisa_sig", "PLC PMC");
  legend2->SetTextSize(0.025);
  legend2->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/missingm2_pisa_sig_norm_comparison.png");
  canvas_comparison.Clear();

  // Ratio
  histcompare_norm->Sumw2();
  histcompare_sig->Sumw2();
  histcompare_norm->Scale(1/histcompare_norm->Integral());
  histcompare_sig->Scale(1/histcompare_sig->Integral());
  histcompare_ratio = *(TH1F*)histcompare_sig->Clone();
  histsum = (TH1F*)(*histcompare_sig+*histcompare_norm).Clone();
  histcompare_ratio.Divide(histcompare_sig.GetPtr(), histsum);
  canvas_comparison_ratio.cd();
  histcompare_ratio.SetName("missingm2_pisa_ratio_sig_norm");
  histcompare_ratio.SetTitle("PISA: Ratio between m_{miss}^{2}_{pisa,sig} and m_{miss}^{2}_{pisa,norm}+m_{miss}^{2}_{pisa,sig}");
  histcompare_ratio.SetStats(kTRUE);
  histcompare_ratio.SetFillStyle(1001);
  histcompare_ratio.SetFillColor(kBlue);
  histcompare_ratio.SetFillColorAlpha(kBlue, 0.8);
  histcompare_ratio.SetMarkerStyle(kFullSquare);
  histcompare_ratio.SetMarkerColor(kBlue);
  histcompare_ratio.SetMarkerSize(1);
  histcompare_ratio.GetXaxis()->SetTitle("#frac{m_{miss}^{2}_{pisa,sig}}{m_{miss}^{2}_{pisa,norm}+m_{miss}^{2}_{pisa,sig}}");
  histcompare_ratio.GetXaxis()->SetTitleOffset(1.4);
  histcompare_ratio.GetXaxis()->SetTitleSize(0.025);
  histcompare_ratio.GetYaxis()->SetTitle("Occurences");
  histcompare_ratio.Draw("HF PMC");
  canvas_comparison_ratio.Update();

  //Adjusting the stats
  ratiostats2 = (TPaveStats *)histcompare_ratio.GetListOfFunctions()->FindObject("stats");
  ratiostats2->SetOptStat(112211);
  ratiostats2->SetX1NDC(0.1);
  ratiostats2->SetX2NDC(0.28);
  ratiostats2->SetY1NDC(0.66);
  ratiostats2->SetY2NDC(0.78);
  ratiostats2->Draw("same");

  //Adjusting the legend
  legend_comparison_ratio = new TLegend;
  legend_comparison_ratio->SetHeader("Pisa", "C");
  legend_comparison_ratio->AddEntry("missingm2_pisa_ratio_sig_norm", "#frac{m_{miss}^{2}_{Pisa,sig}}{m_{miss}^{2}_{Pisa,norm}+m_{miss}^{2}_{pisa,sig}}", "F PMC");
  legend_comparison_ratio->Draw("same");
  gPad->Update();
  legend_comparison_ratio->SetTextSize(0.019);
  legend_comparison_ratio->SetX1NDC(0.1);
  legend_comparison_ratio->SetX2NDC(0.28);
  legend_comparison_ratio->SetY1NDC(0.78);
  legend_comparison_ratio->SetY2NDC(0.9);
  header = (TLegendEntry *)legend_comparison_ratio->GetListOfPrimitives()->First();
  header->SetTextSize(0.019);
  header->Draw("same");
  gPad->Modified();
  canvas_comparison_ratio.Print("OutputFiles/PNGPlots/missingm2_pisa_ratio_sig_norm.png");
  histcompare_ratio.Reset("ICESM");
  legend_comparison_ratio->Clear();
  
  vector<Double_t> missingm2_pisa_cuts, missingm2_pisa_sig_eff, missingm2_pisa_norm_eff; 
  purity_pisa.clear();
  for (int i=0; i<missingm2_bins; i++)
  {
    missingm2_pisa_cuts.push_back(missingm2_min_cut+i*(missingm2_max_cut-missingm2_min_cut)/missingm2_bins);
    Double_t missingm2_pisa_sig_integral = histcompare_sig->Integral(histcompare_sig->FindFixBin(missingm2_pisa_cuts[i]), histcompare_sig->FindFixBin(missingm2_max_cut));
    Double_t missingm2_pisa_norm_integral = histcompare_norm->Integral(histcompare_norm->FindFixBin(missingm2_pisa_cuts[i]),histcompare_norm->FindFixBin(missingm2_max_cut));
    missingm2_pisa_sig_eff.push_back(missingm2_pisa_sig_integral);
    missingm2_pisa_norm_eff.push_back(missingm2_pisa_norm_integral);
    purity_pisa.push_back( 1/ (1 + ( ( missingm2_pisa_norm_integral*(1- missingm2_pisa_norm_integral) ) / (0.71 * missingm2_pisa_sig_integral * missingm2_pisa_sig_integral) ) ) );
  }

  TGraph *missingm2_pisa_eff_curve_sig = new TGraph(missingm2_pisa_sig_eff.size(), missingm2_pisa_cuts.data(), missingm2_pisa_sig_eff.data());
  canvas_eff_curve->Clear();
  canvas_eff_curve->cd();
  missingm2_pisa_eff_curve_sig->SetTitle("Signal-Background efficiencies");
  missingm2_pisa_eff_curve_sig->GetXaxis()->SetTitle("Cut applied on m_{miss}^{2}[GeV^{2}]");
  missingm2_pisa_eff_curve_sig->GetYaxis()->SetTitle("Efficiencies");
  missingm2_pisa_eff_curve_sig->SetMarkerStyle(21);
  missingm2_pisa_eff_curve_sig->SetMarkerColor(kBlue);
  missingm2_pisa_eff_curve_sig->Draw("apl PLC");
  TGraph *missingm2_pisa_eff_curve_norm = new TGraph(missingm2_pisa_sig_eff.size(), missingm2_pisa_cuts.data(), missingm2_pisa_norm_eff.data());
  missingm2_pisa_eff_curve_norm->SetTitle("Normalization Efficiency");
  missingm2_pisa_eff_curve_norm->SetMarkerStyle(21);
  missingm2_pisa_eff_curve_norm->SetMarkerColor(kRed);
  missingm2_pisa_eff_curve_norm->Draw("pl PLC SAME");

  //Adjusting the legend
  legend_roc = new TLegend;
  legend_roc->SetHeader("Pisa", "C");
  legend_roc->AddEntry(missingm2_pisa_eff_curve_sig, "Signal", "PLC");
  legend_roc->AddEntry(missingm2_pisa_eff_curve_norm, "Normalization", "pl PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.75);
  legend_roc->SetX2NDC(0.9);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_eff_curve->Print("missingm2_pisa_eff_curve.png");

/*
ooo        ooooo   .oooo.   ooo        ooooo ooooo  .oooooo..o  .oooooo..o         oooo   .oooooo.   ooooo      ooo       .o.       
`88.       .888' .dP""Y88b  `88.       .888' `888' d8P'    `Y8 d8P'    `Y8         `888  d8P'  `Y8b  `888b.     `8'      .888.      
 888b     d'888        ]8P'  888b     d'888   888  Y88bo.      Y88bo.               888 888      888  8 `88b.    8      .8"888.     
 8 Y88. .P  888      .d8P'   8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.           888 888      888  8   `88b.  8     .8' `888.    
 8  `888'   888    .dP'      8  `888'   888   888       `"Y88b      `"Y88b          888 888      888  8     `88b.8    .88ooo8888.   
 8    Y     888  .oP     .o  8    Y     888   888  oo     .d8P oo     .d8P          888 `88b    d88'  8       `888   .8'     `888.  
o8o        o888o 8888888888 o8o        o888o o888o 8""88888P'  8""88888P'       .o. 88P  `Y8bood8P'  o8o        `8  o88o     o8888o 
                                                                                `Y888P                                              
*/

  // Plotting
  canvas_comparison.Clear();
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("missingm2_jona_norm", "missingm2_jona_norm", missingm2_bins, missingm2_min_cut, missingm2_max_cut), {"missingm2_Jona"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("JONA: m_{miss}^{2}_{sig} and m_{miss}^{2}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("m_{miss}^{2}[GeV^{2}]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("missingm2_jona_sig", "missingm2_jona_sig", missingm2_bins, missingm2_min_cut, missingm2_max_cut), {"missingm2_Jona"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  legend3 = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend3->AddEntry("missingm2_jona_norm", "missingm2_jona_norm", "PLC PMC");
  legend3->AddEntry("missingm2_jona_sig", "missingm2_jona_sig", "PLC PMC");
  legend3->SetTextSize(0.025);
  legend3->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/missingm2_jona_sig_norm_comparison.png");
  canvas_comparison.Clear();

  // Ratio
  histcompare_norm->Sumw2();
  histcompare_sig->Sumw2();
  histcompare_norm->Scale(1/histcompare_norm->Integral());
  histcompare_sig->Scale(1/histcompare_sig->Integral());
  histcompare_ratio = *(TH1F*)histcompare_sig->Clone();
  histsum = (TH1F*)(*histcompare_sig+*histcompare_norm).Clone();
  histcompare_ratio.Divide(histcompare_sig.GetPtr(), histsum);
  canvas_comparison_ratio.cd();
  histcompare_ratio.SetName("missingm2_jona_ratio_sig_norm");
  histcompare_ratio.SetTitle("Jona: Ratio between m_{miss}^{2}_{Jona,sig} and m_{miss}^{2}_{Jona,norm}+m_{miss}^{2}_{Jona,sig}");
  histcompare_ratio.SetStats(kTRUE);
  histcompare_ratio.SetFillStyle(1001);
  histcompare_ratio.SetFillColor(kBlue);
  histcompare_ratio.SetFillColorAlpha(kBlue, 0.8);
  histcompare_ratio.SetMarkerStyle(kFullSquare);
  histcompare_ratio.SetMarkerColor(kBlue);
  histcompare_ratio.SetMarkerSize(1);
  histcompare_ratio.GetXaxis()->SetTitle("#frac{m_{miss}^{2}_{Jona,sig}}{m_{miss}^{2}_{Jona,norm}+m_{miss}^{2}_{Jona,sig}}");
  histcompare_ratio.GetXaxis()->SetTitleOffset(1.4);
  histcompare_ratio.GetXaxis()->SetTitleSize(0.025);
  histcompare_ratio.GetYaxis()->SetTitle("Occurences");
  histcompare_ratio.Draw("HF PMC");
  canvas_comparison_ratio.Update();

  //Adjusting the stats
  ratiostats3 = (TPaveStats *)histcompare_ratio.GetListOfFunctions()->FindObject("stats");
  ratiostats3->SetOptStat(112211);
  ratiostats3->SetX1NDC(0.1);
  ratiostats3->SetX2NDC(0.28);
  ratiostats3->SetY1NDC(0.66);
  ratiostats3->SetY2NDC(0.78);
  ratiostats3->Draw("same");

  //Adjusting the legend
  legend_comparison_ratio->Clear();
  legend_comparison_ratio->SetX1NDC(0.1);
  legend_comparison_ratio->SetX2NDC(0.28);
  legend_comparison_ratio->SetY1NDC(0.78);
  legend_comparison_ratio->SetY2NDC(0.9);
  legend_comparison_ratio->SetHeader("Jona", "C");
  legend_comparison_ratio->AddEntry("missingm2_jona_ratio_sig_norm", "#frac{m_{miss}^{2}_{Jona,sig}}{m_{miss}^{2}_{Jona,norm}}", "PMC");
  legend_comparison_ratio->SetTextSize(0.02);
  legend_comparison_ratio->Draw("same");
  header = (TLegendEntry *)legend_comparison_ratio->GetListOfPrimitives()->First();
  header->SetTextSize(0.02);
  header->Draw("same");
  canvas_comparison_ratio.Update();
  canvas_comparison_ratio.Print("OutputFiles/PNGPlots/missingm2_jona_ratio_sig_norm.png");
  histcompare_ratio.Reset("ICESM");  

  vector<Double_t> missingm2_jona_cuts, missingm2_jona_sig_eff, missingm2_jona_norm_eff;
  purity_jona.clear();
  for (int i=0; i<missingm2_bins; i++)
  {
    missingm2_jona_cuts.push_back(missingm2_min_cut+i*(missingm2_max_cut-missingm2_min_cut)/missingm2_bins);
    Double_t missingm2_jona_sig_integral = histcompare_sig->Integral(histcompare_sig->FindFixBin(missingm2_jona_cuts[i]), histcompare_sig->FindFixBin(missingm2_max_cut));
    Double_t missingm2_jona_norm_integral = histcompare_norm->Integral(histcompare_norm->FindFixBin(missingm2_jona_cuts[i]),histcompare_norm->FindFixBin(missingm2_max_cut));
    missingm2_jona_sig_eff.push_back(missingm2_jona_sig_integral);
    missingm2_jona_norm_eff.push_back(missingm2_jona_norm_integral);
    purity_jona.push_back( 1/ (1 + ( ( missingm2_jona_norm_integral*(1- missingm2_jona_norm_integral) ) / (0.71 * missingm2_jona_sig_integral * missingm2_jona_sig_integral) ) ) );
  }

  TGraph *missingm2_jona_eff_curve_sig = new TGraph(missingm2_jona_sig_eff.size(), missingm2_jona_cuts.data(), missingm2_jona_sig_eff.data());
  canvas_eff_curve->Clear();
  canvas_eff_curve->cd();
  missingm2_jona_eff_curve_sig->SetTitle("Signal-Background efficiencies");
  missingm2_jona_eff_curve_sig->GetXaxis()->SetTitle("Cut applied on m_{miss}^{2}[GeV^{2}]");
  missingm2_jona_eff_curve_sig->GetYaxis()->SetTitle("Efficiencies");
  missingm2_jona_eff_curve_sig->SetMarkerStyle(21);
  missingm2_jona_eff_curve_sig->SetMarkerColor(kBlue);
  missingm2_jona_eff_curve_sig->Draw("apl PLC");
  TGraph *missingm2_jona_eff_curve_norm = new TGraph(missingm2_jona_sig_eff.size(), missingm2_jona_cuts.data(), missingm2_jona_norm_eff.data());
  missingm2_jona_eff_curve_norm->SetTitle("Normalization Efficiency");
  missingm2_jona_eff_curve_norm->SetMarkerStyle(21);
  missingm2_jona_eff_curve_norm->SetMarkerColor(kRed);
  missingm2_jona_eff_curve_norm->Draw("pl PLC SAME");

  //Adjusting the legend
  legend_roc->Clear();
  legend_roc->SetHeader("Jona", "C");
  legend_roc->AddEntry(missingm2_jona_eff_curve_sig, "Signal", "PLC");
  legend_roc->AddEntry(missingm2_jona_eff_curve_norm, "Normalization", "pl PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.75);
  legend_roc->SetX2NDC(0.9);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_eff_curve->Print("missingm2_jona_eff_curve.png");

/*
ooo        ooooo   .oooo.   ooo        ooooo ooooo  .oooooo..o  .oooooo..o      ooooooooo.     .oooooo.     .oooooo.   
`88.       .888' .dP""Y88b  `88.       .888' `888' d8P'    `Y8 d8P'    `Y8      `888   `Y88.  d8P'  `Y8b   d8P'  `Y8b  
 888b     d'888        ]8P'  888b     d'888   888  Y88bo.      Y88bo.            888   .d88' 888      888 888          
 8 Y88. .P  888      .d8P'   8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.        888ooo88P'  888      888 888          
 8  `888'   888    .dP'      8  `888'   888   888       `"Y88b      `"Y88b       888`88b.    888      888 888          
 8    Y     888  .oP     .o  8    Y     888   888  oo     .d8P oo     .d8P       888  `88b.  `88b    d88' `88b    ooo  
o8o        o888o 8888888888 o8o        o888o o888o 8""88888P'  8""88888P'       o888o  o888o  `Y8bood8P'   `Y8bood8P'  
*/

  // ROC comparison
  TGraph *missingm2_pisa_roc_curve = new TGraph(purity_pisa.size(), purity_pisa.data(), missingm2_pisa_sig_eff.data());
  TGraph *missingm2_jona_roc_curve = new TGraph(purity_jona.size(), purity_jona.data(), missingm2_jona_sig_eff.data());
  canvas_roc_curve->Clear();
  canvas_roc_curve->cd();
  missingm2_pisa_roc_curve->SetTitle("ROC curve: m_{miss}^{2}_{Pisa} and m_{miss}^{2}_{Jona} comparison");
  missingm2_pisa_roc_curve->GetXaxis()->SetTitle("Purity");
  missingm2_pisa_roc_curve->GetYaxis()->SetTitle("Signal efficiency");
  missingm2_pisa_roc_curve->SetMarkerStyle(21);
  missingm2_pisa_roc_curve->SetMarkerColor(kBlue);
  missingm2_pisa_roc_curve->Draw("apl PLC");
  missingm2_jona_roc_curve->SetMarkerStyle(21);
  missingm2_jona_roc_curve->SetMarkerColor(kRed);
  missingm2_jona_roc_curve->Draw("pl PLC SAME");

  //Adjusting the legend
  legend_roc->Clear();
  legend_roc->SetHeader("ROC curve for m_{miss}^{2}", "C");
  legend_roc->AddEntry(missingm2_pisa_roc_curve, "Pisa m_{miss}^{2}", "PLC");
  legend_roc->AddEntry(missingm2_jona_roc_curve, "Jona m_{miss}^{2}", "PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.1);
  legend_roc->SetX2NDC(0.25);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_roc_curve->Print("missingm2_roc_curve.png");
  
  /*
ooooooooo.   ooooooooooooo ooo        ooooo ooooo  .oooooo..o  .oooooo..o        .oooooo.    oooooooooooo ooooo      ooo oooooooooooo 
`888   `Y88. 8'   888   `8 `88.       .888' `888' d8P'    `Y8 d8P'    `Y8       d8P'  `Y8b   `888'     `8 `888b.     `8' `888'     `8 
 888   .d88'      888       888b     d'888   888  Y88bo.      Y88bo.           888            888          8 `88b.    8   888         
 888ooo88P'       888       8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.       888            888oooo8     8   `88b.  8   888oooo8    
 888              888       8  `888'   888   888       `"Y88b      `"Y88b      888     ooooo  888    "     8     `88b.8   888    "    
 888              888       8    Y     888   888  oo     .d8P oo     .d8P      `88.    .88'   888       o  8       `888   888       o 
o888o            o888o     o8o        o888o o888o 8""88888P'  8""88888P'        `Y8bood8P'   o888ooooood8 o8o        `8  o888ooooood8
*/

  Double_t missingpt_bins = 100, missingpt_min_cut = -10, missingpt_max_cut = 70;
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("missingpt_gene_norm", "missingpt_gene_norm", 100, 0, 11), {"missingpt_gene"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("GEN: p_{T}^{miss}_{sig} and p_{T}^{miss}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("p_{T}^{miss}[GeV]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("missingpt_gene_sig", "missingpt_gene_sig", 100, 0, 11), {"missingpt_gene"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  legend = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend->AddEntry("missingpt_gene_norm", "missingpt_gene_norm", "PLC PMC");
  legend->AddEntry("missingpt_gene_sig", "missingpt_gene_sig", "PLC PMC");
  legend->SetTextSize(0.025);
  legend->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/missingpt_gene_sig_norm_comparison.png");
  canvas_comparison.Clear();

/*
ooooooooo.   ooooooooooooo ooo        ooooo ooooo  .oooooo..o  .oooooo..o      ooooooooo.   ooooo  .oooooo..o       .o.       
`888   `Y88. 8'   888   `8 `88.       .888' `888' d8P'    `Y8 d8P'    `Y8      `888   `Y88. `888' d8P'    `Y8      .888.      
 888   .d88'      888       888b     d'888   888  Y88bo.      Y88bo.            888   .d88'  888  Y88bo.          .8"888.     
 888ooo88P'       888       8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.        888ooo88P'   888   `"Y8888o.     .8' `888.    
 888              888       8  `888'   888   888       `"Y88b      `"Y88b       888          888       `"Y88b   .88ooo8888.   
 888              888       8    Y     888   888  oo     .d8P oo     .d8P       888          888  oo     .d8P  .8'     `888.  
o888o            o888o     o8o        o888o o888o 8""88888P'  8""88888P'       o888o        o888o 8""88888P'  o88o     o8888o
*/

  // Plotting
  canvas_comparison.Clear();
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("missingpt_pisa_norm", "missingpt_pisa_norm", missingpt_bins, missingpt_min_cut, missingpt_max_cut), {"missingpt_Pisa"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("PISA: p_{T}^{miss}_{sig} and p_{T}^{miss}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("p_{T}^{miss}[GeV]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("missingpt_pisa_sig", "missingpt_pisa_sig", missingpt_bins, missingpt_min_cut, missingpt_max_cut), {"missingpt_Pisa"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  legend2 = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend2->AddEntry("missingpt_pisa_norm", "missingpt_pisa_norm", "PLC PMC");
  legend2->AddEntry("missingpt_pisa_sig", "missingpt_pisa_sig", "PLC PMC");
  legend2->SetTextSize(0.025);
  legend2->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/missingpt_pisa_sig_norm_comparison.png");
  canvas_comparison.Clear();

  // Ratio
  histcompare_norm->Sumw2();
  histcompare_sig->Sumw2();
  histcompare_norm->Scale(1/histcompare_norm->Integral());
  histcompare_sig->Scale(1/histcompare_sig->Integral());
  histcompare_ratio = *(TH1F*)histcompare_sig->Clone();
  histsum = (TH1F*)(*histcompare_sig+*histcompare_norm).Clone();
  histcompare_ratio.Divide(histcompare_sig.GetPtr(), histsum);
  canvas_comparison_ratio.cd();
  histcompare_ratio.SetName("missingpt_pisa_ratio_sig_norm");
  histcompare_ratio.SetTitle("PISA: Ratio between p_{T}^{miss}_{pisa,sig} and p_{T}^{miss}_{pisa,norm}+p_{T}^{miss}_{pisa,sig}");
  histcompare_ratio.SetStats(kTRUE);
  histcompare_ratio.SetFillStyle(1001);
  histcompare_ratio.SetFillColor(kBlue);
  histcompare_ratio.SetFillColorAlpha(kBlue, 0.8);
  histcompare_ratio.SetMarkerStyle(kFullSquare);
  histcompare_ratio.SetMarkerColor(kBlue);
  histcompare_ratio.SetMarkerSize(1);
  histcompare_ratio.GetXaxis()->SetTitle("#frac{p_{T}^{miss}_{pisa,sig}}{p_{T}^{miss}_{pisa,norm}+p_{T}^{miss}_{pisa,sig}}");
  histcompare_ratio.GetXaxis()->SetTitleOffset(1.4);
  histcompare_ratio.GetXaxis()->SetTitleSize(0.025);
  histcompare_ratio.GetYaxis()->SetTitle("Occurences");
  histcompare_ratio.Draw("HF PMC");
  canvas_comparison_ratio.Update();

  //Adjusting the stats
  ratiostats2 = (TPaveStats *)histcompare_ratio.GetListOfFunctions()->FindObject("stats");
  ratiostats2->SetOptStat(112211);
  ratiostats2->SetX1NDC(0.1);
  ratiostats2->SetX2NDC(0.28);
  ratiostats2->SetY1NDC(0.66);
  ratiostats2->SetY2NDC(0.78);
  ratiostats2->Draw("same");

  //Adjusting the legend
  legend_comparison_ratio = new TLegend;
  legend_comparison_ratio->SetHeader("Pisa", "C");
  legend_comparison_ratio->AddEntry("missingpt_pisa_ratio_sig_norm", "#frac{p_{T}^{miss}_{Pisa,sig}}{p_{T}^{miss}_{Pisa,norm}+p_{T}^{miss}_{pisa,sig}}", "F PMC");
  legend_comparison_ratio->Draw("same");
  gPad->Update();
  legend_comparison_ratio->SetTextSize(0.019);
  legend_comparison_ratio->SetX1NDC(0.1);
  legend_comparison_ratio->SetX2NDC(0.28);
  legend_comparison_ratio->SetY1NDC(0.78);
  legend_comparison_ratio->SetY2NDC(0.9);
  header = (TLegendEntry *)legend_comparison_ratio->GetListOfPrimitives()->First();
  header->SetTextSize(0.019);
  header->Draw("same");
  gPad->Modified();
  canvas_comparison_ratio.Print("OutputFiles/PNGPlots/missingpt_pisa_ratio_sig_norm.png");
  histcompare_ratio.Reset("ICESM");
  legend_comparison_ratio->Clear();
  
  vector<Double_t> missingpt_pisa_cuts, missingpt_pisa_sig_eff, missingpt_pisa_norm_eff; 
  purity_pisa.clear();
  for (int i=0; i<missingpt_bins; i++)
  {
    missingpt_pisa_cuts.push_back(missingpt_min_cut+i*(missingpt_max_cut-missingpt_min_cut)/missingpt_bins);
    Double_t missingpt_pisa_sig_integral = histcompare_sig->Integral(histcompare_sig->FindFixBin(missingpt_pisa_cuts[i]), histcompare_sig->FindFixBin(missingpt_max_cut));
    Double_t missingpt_pisa_norm_integral = histcompare_norm->Integral(histcompare_norm->FindFixBin(missingpt_pisa_cuts[i]),histcompare_norm->FindFixBin(missingpt_max_cut));
    missingpt_pisa_sig_eff.push_back(missingpt_pisa_sig_integral);
    missingpt_pisa_norm_eff.push_back(missingpt_pisa_norm_integral);
    purity_pisa.push_back( 1/ (1 + ( ( missingpt_pisa_norm_integral*(1- missingpt_pisa_norm_integral) ) / (0.71 * missingpt_pisa_sig_integral * missingpt_pisa_sig_integral) ) ) );
  }

  TGraph *missingpt_pisa_eff_curve_sig = new TGraph(missingpt_pisa_sig_eff.size(), missingpt_pisa_cuts.data(), missingpt_pisa_sig_eff.data());
  canvas_eff_curve->Clear();
  canvas_eff_curve->cd();
  missingpt_pisa_eff_curve_sig->SetTitle("Signal-Background efficiencies");
  missingpt_pisa_eff_curve_sig->GetXaxis()->SetTitle("Cut applied on p_{T}^{miss}[GeV]");
  missingpt_pisa_eff_curve_sig->GetYaxis()->SetTitle("Efficiencies");
  missingpt_pisa_eff_curve_sig->SetMarkerStyle(21);
  missingpt_pisa_eff_curve_sig->SetMarkerColor(kBlue);
  missingpt_pisa_eff_curve_sig->Draw("apl PLC");
  TGraph *missingpt_pisa_eff_curve_norm = new TGraph(missingpt_pisa_sig_eff.size(), missingpt_pisa_cuts.data(), missingpt_pisa_norm_eff.data());
  missingpt_pisa_eff_curve_norm->SetTitle("Normalization Efficiency");
  missingpt_pisa_eff_curve_norm->SetMarkerStyle(21);
  missingpt_pisa_eff_curve_norm->SetMarkerColor(kRed);
  missingpt_pisa_eff_curve_norm->Draw("pl PLC SAME");

  //Adjusting the legend
  legend_roc = new TLegend;
  legend_roc->SetHeader("Pisa", "C");
  legend_roc->AddEntry(missingpt_pisa_eff_curve_sig, "Signal", "PLC");
  legend_roc->AddEntry(missingpt_pisa_eff_curve_norm, "Normalization", "pl PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.75);
  legend_roc->SetX2NDC(0.9);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_eff_curve->Print("missingpt_pisa_eff_curve.png");

/*
ooooooooo.   ooooooooooooo ooo        ooooo ooooo  .oooooo..o  .oooooo..o         oooo   .oooooo.   ooooo      ooo       .o.       
`888   `Y88. 8'   888   `8 `88.       .888' `888' d8P'    `Y8 d8P'    `Y8         `888  d8P'  `Y8b  `888b.     `8'      .888.      
 888   .d88'      888       888b     d'888   888  Y88bo.      Y88bo.               888 888      888  8 `88b.    8      .8"888.     
 888ooo88P'       888       8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.           888 888      888  8   `88b.  8     .8' `888.    
 888              888       8  `888'   888   888       `"Y88b      `"Y88b          888 888      888  8     `88b.8    .88ooo8888.   
 888              888       8    Y     888   888  oo     .d8P oo     .d8P          888 `88b    d88'  8       `888   .8'     `888.  
o888o            o888o     o8o        o888o o888o 8""88888P'  8""88888P'       .o. 88P  `Y8bood8P'  o8o        `8  o88o     o8888o 
                                                                               `Y888P                                              
*/

  // Plotting
  canvas_comparison.Clear();
  histcompare_norm = BtoMu->Fill<Float_t>(TH1F("missingpt_jona_norm", "missingpt_jona_norm", missingpt_bins, missingpt_min_cut, missingpt_max_cut), {"missingpt_Jona"});
  canvas_comparison.cd();
  histcompare_norm->SetTitle("JONA: p_{T}^{miss}_{sig} and p_{T}^{miss}_{norm} comparison");
  histcompare_norm->GetXaxis()->SetTitle("p_{T}^{miss}[GeV]");
  histcompare_norm->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_norm->SetStats(false);
  histcompare_norm->SetFillStyle(1001);
  histcompare_norm->SetMarkerStyle(kFullSquare);
  histcompare_norm->SetMarkerSize(1.5);
  histcompare_norm->DrawNormalized("PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  histcompare_sig = BtoTau->Fill<Float_t>(TH1F("missingpt_jona_sig", "missingpt_jona_sig", missingpt_bins, missingpt_min_cut, missingpt_max_cut), {"missingpt_Jona"});
  canvas_comparison.cd();
  histcompare_sig->GetYaxis()->SetTitle("Normalized Occurences [a.u.]");
  histcompare_sig->SetStats(false);
  histcompare_sig->SetFillStyle(1001);
  histcompare_sig->SetMarkerStyle(kFullSquare);
  histcompare_sig->SetMarkerSize(1.5);
  histcompare_sig->DrawNormalized("SAME PLC PMC");
  canvas_comparison.SetLogy();
  canvas_comparison.Update();

  //Adjusting the legend
  legend3 = new TLegend(0.1, 0.84, 0.28, 0.9);
  legend3->AddEntry("missingpt_jona_norm", "missingpt_jona_norm", "PLC PMC");
  legend3->AddEntry("missingpt_jona_sig", "missingpt_jona_sig", "PLC PMC");
  legend3->SetTextSize(0.025);
  legend3->Draw("SAME");
  canvas_comparison.Update();
  canvas_comparison.Print("OutputFiles/PNGPlots/missingpt_jona_sig_norm_comparison.png");
  canvas_comparison.Clear();

  // Ratio
  histcompare_norm->Sumw2();
  histcompare_sig->Sumw2();
  histcompare_norm->Scale(1/histcompare_norm->Integral());
  histcompare_sig->Scale(1/histcompare_sig->Integral());
  histcompare_ratio = *(TH1F*)histcompare_sig->Clone();
  histsum = (TH1F*)(*histcompare_sig+*histcompare_norm).Clone();
  histcompare_ratio.Divide(histcompare_sig.GetPtr(), histsum);
  canvas_comparison_ratio.cd();
  histcompare_ratio.SetName("missingpt_jona_ratio_sig_norm");
  histcompare_ratio.SetTitle("Jona: Ratio between p_{T}^{miss}_{Jona,sig} and p_{T}^{miss}_{Jona,norm}+p_{T}^{miss}_{Jona,sig}");
  histcompare_ratio.SetStats(kTRUE);
  histcompare_ratio.SetFillStyle(1001);
  histcompare_ratio.SetFillColor(kBlue);
  histcompare_ratio.SetFillColorAlpha(kBlue, 0.8);
  histcompare_ratio.SetMarkerStyle(kFullSquare);
  histcompare_ratio.SetMarkerColor(kBlue);
  histcompare_ratio.SetMarkerSize(1);
  histcompare_ratio.GetXaxis()->SetTitle("#frac{p_{T}^{miss}_{Jona,sig}}{p_{T}^{miss}_{Jona,norm}+p_{T}^{miss}_{Jona,sig}}");
  histcompare_ratio.GetXaxis()->SetTitleOffset(1.4);
  histcompare_ratio.GetXaxis()->SetTitleSize(0.025);
  histcompare_ratio.GetYaxis()->SetTitle("Occurences");
  histcompare_ratio.Draw("HF PMC");
  canvas_comparison_ratio.Update();

  //Adjusting the stats
  ratiostats3 = (TPaveStats *)histcompare_ratio.GetListOfFunctions()->FindObject("stats");
  ratiostats3->SetOptStat(112211);
  ratiostats3->SetX1NDC(0.1);
  ratiostats3->SetX2NDC(0.28);
  ratiostats3->SetY1NDC(0.66);
  ratiostats3->SetY2NDC(0.78);
  ratiostats3->Draw("same");

  //Adjusting the legend
  legend_comparison_ratio->Clear();
  legend_comparison_ratio->SetX1NDC(0.1);
  legend_comparison_ratio->SetX2NDC(0.28);
  legend_comparison_ratio->SetY1NDC(0.78);
  legend_comparison_ratio->SetY2NDC(0.9);
  legend_comparison_ratio->SetHeader("Jona", "C");
  legend_comparison_ratio->AddEntry("missingpt_jona_ratio_sig_norm", "#frac{p_{T}^{miss}_{Jona,sig}}{p_{T}^{miss}_{Jona,norm}}", "PMC");
  legend_comparison_ratio->SetTextSize(0.02);
  legend_comparison_ratio->Draw("same");
  header = (TLegendEntry *)legend_comparison_ratio->GetListOfPrimitives()->First();
  header->SetTextSize(0.02);
  header->Draw("same");
  canvas_comparison_ratio.Update();
  canvas_comparison_ratio.Print("OutputFiles/PNGPlots/missingpt_jona_ratio_sig_norm.png");
  histcompare_ratio.Reset("ICESM");  

  vector<Double_t> missingpt_jona_cuts, missingpt_jona_sig_eff, missingpt_jona_norm_eff;
  purity_jona.clear();
  for (int i=0; i<missingpt_bins; i++)
  {
    missingpt_jona_cuts.push_back(missingpt_min_cut+i*(missingpt_max_cut-missingpt_min_cut)/missingpt_bins);
    Double_t missingpt_jona_sig_integral = histcompare_sig->Integral(histcompare_sig->FindFixBin(missingpt_jona_cuts[i]), histcompare_sig->FindFixBin(missingpt_max_cut));
    Double_t missingpt_jona_norm_integral = histcompare_norm->Integral(histcompare_norm->FindFixBin(missingpt_jona_cuts[i]),histcompare_norm->FindFixBin(missingpt_max_cut));
    missingpt_jona_sig_eff.push_back(missingpt_jona_sig_integral);
    missingpt_jona_norm_eff.push_back(missingpt_jona_norm_integral);
    purity_jona.push_back( 1/ (1 + ( ( missingpt_jona_norm_integral*(1- missingpt_jona_norm_integral) ) / (0.71 * missingpt_jona_sig_integral * missingpt_jona_sig_integral) ) ) );
  }

  TGraph *missingpt_jona_eff_curve_sig = new TGraph(missingpt_jona_sig_eff.size(), missingpt_jona_cuts.data(), missingpt_jona_sig_eff.data());
  canvas_eff_curve->Clear();
  canvas_eff_curve->cd();
  missingpt_jona_eff_curve_sig->SetTitle("Signal-Background efficiencies");
  missingpt_jona_eff_curve_sig->GetXaxis()->SetTitle("Cut applied on p_{T}^{miss}[GeV]");
  missingpt_jona_eff_curve_sig->GetYaxis()->SetTitle("Efficiencies");
  missingpt_jona_eff_curve_sig->SetMarkerStyle(21);
  missingpt_jona_eff_curve_sig->SetMarkerColor(kBlue);
  missingpt_jona_eff_curve_sig->Draw("apl PLC");
  TGraph *missingpt_jona_eff_curve_norm = new TGraph(missingpt_jona_sig_eff.size(), missingpt_jona_cuts.data(), missingpt_jona_norm_eff.data());
  missingpt_jona_eff_curve_norm->SetTitle("Normalization Efficiency");
  missingpt_jona_eff_curve_norm->SetMarkerStyle(21);
  missingpt_jona_eff_curve_norm->SetMarkerColor(kRed);
  missingpt_jona_eff_curve_norm->Draw("pl PLC SAME");

  //Adjusting the legend
  legend_roc->Clear();
  legend_roc->SetHeader("Jona", "C");
  legend_roc->AddEntry(missingpt_jona_eff_curve_sig, "Signal", "PLC");
  legend_roc->AddEntry(missingpt_jona_eff_curve_norm, "Normalization", "pl PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.75);
  legend_roc->SetX2NDC(0.9);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_eff_curve->Print("missingpt_jona_eff_curve.png");

/*
ooooooooo.   ooooooooooooo ooo        ooooo ooooo  .oooooo..o  .oooooo..o      ooooooooo.     .oooooo.     .oooooo.   
`888   `Y88. 8'   888   `8 `88.       .888' `888' d8P'    `Y8 d8P'    `Y8      `888   `Y88.  d8P'  `Y8b   d8P'  `Y8b  
 888   .d88'      888       888b     d'888   888  Y88bo.      Y88bo.            888   .d88' 888      888 888          
 888ooo88P'       888       8 Y88. .P  888   888   `"Y8888o.   `"Y8888o.        888ooo88P'  888      888 888          
 888              888       8  `888'   888   888       `"Y88b      `"Y88b       888`88b.    888      888 888          
 888              888       8    Y     888   888  oo     .d8P oo     .d8P       888  `88b.  `88b    d88' `88b    ooo  
o888o            o888o     o8o        o888o o888o 8""88888P'  8""88888P'       o888o  o888o  `Y8bood8P'   `Y8bood8P'  
*/

  // ROC comparison
  TGraph *missingpt_pisa_roc_curve = new TGraph(purity_pisa.size(), purity_pisa.data(), missingpt_pisa_sig_eff.data());
  TGraph *missingpt_jona_roc_curve = new TGraph(purity_jona.size(), purity_jona.data(), missingpt_jona_sig_eff.data());
  canvas_roc_curve->Clear();
  canvas_roc_curve->cd();
  missingpt_pisa_roc_curve->SetTitle("ROC curve: p_{T}^{miss}_{Pisa} and p_{T}^{miss}_{Jona} comparison");
  missingpt_pisa_roc_curve->GetXaxis()->SetTitle("Purity");
  missingpt_pisa_roc_curve->GetYaxis()->SetTitle("Signal efficiency");
  missingpt_pisa_roc_curve->SetMarkerStyle(21);
  missingpt_pisa_roc_curve->SetMarkerColor(kBlue);
  missingpt_pisa_roc_curve->Draw("apl PLC");
  missingpt_jona_roc_curve->SetMarkerStyle(21);
  missingpt_jona_roc_curve->SetMarkerColor(kRed);
  missingpt_jona_roc_curve->Draw("pl PLC SAME");

  //Adjusting the legend
  legend_roc->Clear();
  legend_roc->SetHeader("ROC curve for p_{T}^{miss}", "C");
  legend_roc->AddEntry(missingpt_pisa_roc_curve, "Pisa p_{T}^{miss}", "PLC");
  legend_roc->AddEntry(missingpt_jona_roc_curve, "Jona p_{T}^{miss}", "PLC");
  legend_roc->Draw("same");
  gPad->Update();
  legend_roc->SetTextSize(0.019);
  legend_roc->SetX1NDC(0.1);
  legend_roc->SetX2NDC(0.25);
  legend_roc->SetY1NDC(0.78);
  legend_roc->SetY2NDC(0.9);
  gPad->Modified();
  canvas_roc_curve->Print("missingpt_roc_curve.png");
  /*
  else
  {
    question.push_back("The user doesn't want to analyze those files");
    PrintFuncInfo(question);
    question.clear();
    return 0;
  }*/

  return 0;
}