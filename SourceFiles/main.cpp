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

#include "HeaderFiles/PrintFuncInfo.h"
#include "HeaderFiles/TreeReader.h"
#include "HeaderFiles/MarkedNames.h"

//It can be useful to use these namespaces
using namespace std;

int main(int argc, char *argv[])
{
  // Input files and relative flags (1 for real data, 0 for MC)
  vector<MarkedNames> filenames;
  filenames.push_back(MarkedNames("BcToJpsiMuNu_UL_2021Jan26.root", 0));
  filenames.push_back(MarkedNames("BcToJpsiTauNu_UL_2021Jan26.root", 0));
  filenames.push_back(MarkedNames("data_UL_2021Jan29.root", 1));

  // Input trees to be processed (1 for real, 0 for MC)
  vector<MarkedNames> treenames;
  treenames.push_back(MarkedNames("BTo3Mu", 1));

  // Input branches to be read (1 for reco, 0 for gene)
  // Please write in a single constructor if you want a direct comparison between reco and gene
  vector<MarkedNames> branchnames;
  branchnames.push_back(MarkedNames("Beta", 1));
  branchnames.push_back(MarkedNames("Bmass", 1));
  branchnames.push_back(MarkedNames("Bphi", 1));
  branchnames.push_back(MarkedNames("Bpt", 0, "Bpt_reco", 1));
  branchnames.push_back(MarkedNames("jpsi_eta", 1));
  branchnames.push_back(MarkedNames("jpsi_mass", 1));
  branchnames.push_back(MarkedNames("jpsi_phi", 1));
  branchnames.push_back(MarkedNames("jpsi_pt", 1));
  branchnames.push_back(MarkedNames("mu1eta", 1));
  branchnames.push_back(MarkedNames("mu1mass", 1));
  branchnames.push_back(MarkedNames("mu1phi", 1));
  branchnames.push_back(MarkedNames("mu1pt", 1));
  branchnames.push_back(MarkedNames("mu2eta", 1));
  branchnames.push_back(MarkedNames("mu2mass", 1));
  branchnames.push_back(MarkedNames("mu2phi", 1));
  branchnames.push_back(MarkedNames("mu2pt", 1));

  // Useful Variables
  vector<TString> question;
  string answer;

  question.push_back("Do you wish to analyze these files? (YES/NO)");
  PrintFuncInfo(question);
  question.clear();
  cin >> answer;
  if (answer.compare("YES") == 0 || answer.compare("yes") == 0 || answer.compare("Yes") == 0 || answer.compare("y") == 0 || answer.compare("Y") == 0)
  {
    for (vector<MarkedNames>::iterator stringit = filenames.begin(); stringit != filenames.end(); ++stringit)
    {
      cout << "\t" << stringit->name.Data() << endl;
    }

    //The user wants to analyze the files
    TreeReader(filenames, treenames, branchnames);
  }
  else
  {
    question.push_back("The user doesn't want to analyze those files");
    PrintFuncInfo(question);
    question.clear();
    return 0;
  }

  return 0;
}