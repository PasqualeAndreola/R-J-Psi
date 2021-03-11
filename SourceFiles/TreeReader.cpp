/*!
 *  \file TreeReader.cpp
 *  \brief Source file for \ref TreeReader function implementation
 */

/*!
 *  \fn int TreeReader(vector<TString> trees2read, vector<TString> var2read); "";
 *  \brief Function used to read selected variables stored in selected trees
 * 
 *  \param file2read name of the file which stores the trees
 *  \param trees2read vector of names of trees that the user wants to explore
 *  \param var2read vector of names of variables that the user wants to read
 */

#include "HeaderFiles/TreeReader.h"

int TreeReader(TString file2read, vector<TString> trees2read, vector<TString> var2read)
{
    Float_t var;
    TFile inputfile(file2read, "read");
    for (vector<TString>::iterator stringit = trees2read.begin(); stringit != trees2read.end(); ++stringit)
    {
        TTree *ReadTree = (TTree *)inputfile.Get(TString::Format("%s", stringit->Data()));
        for (vector<TString>::iterator stringit2 = var2read.begin(); stringit2 != var2read.end(); ++stringit2)
        {
            TBranch *ReadBranch = ReadTree->GetBranch(stringit2->Data());
            ReadBranch->SetAddress(&var);
            unsigned long int nentries = (unsigned long int)ReadTree->GetEntries();
            for (unsigned long int i = 0; i<nentries; i++)
            {
                ReadBranch->GetEntry(i);
                cout << var << endl;
            }
        }
    }
    return 0;
}