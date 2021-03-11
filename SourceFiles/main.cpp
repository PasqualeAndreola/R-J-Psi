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

//It can be useful to use these namespaces
using namespace std;

int main(int argc, char *argv[])
{
  // Input files and relative flags (1 for real data, 0 for MC)
  vector<TString> filenames;
  vector<int> sbflags;
  filenames.push_back("InputFiles/BcToJpsiMuNu_UL_2021Jan26");
  sbflags.push_back(0);
  
  // Input trees and input branches to be processed
  vector<TString> treenames;
  treenames.push_back("BTo3Mu");
  vector<TString> branchnames;
  branchnames.push_back("jpsi_pt");

  // Number of events to be processed
  unsigned long eventnumber = 200000;

  // Useful Variables
  int i = 0;
  vector<TString> question;
  string answer;

  question.push_back("Do you wish to analyze these files? (YES/NO)");
  PrintFuncInfo(question);
  question.clear();
  cin >> answer;
  if (answer.compare("YES") == 0 || answer.compare("yes") == 0 || answer.compare("Yes") == 0 || answer.compare("y") == 0 || answer.compare("Y") == 0)
  {
   for (vector<TString>::iterator stringit = filenames.begin(); stringit != filenames.end(); ++stringit)
   {
    cout << "\t" << stringit->Data() << endl;
   }
   cout << endl;
   cin >> answer;
  
   //The user wants to analyze the files
   TreeReader("InputFiles/BcToJpsiMuNu_UL_2021Jan26.root", treenames, branchnames);
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