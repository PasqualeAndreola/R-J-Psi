/*!
 *  \file FakeNNReader.cpp
 *  \brief Source file for \ref FakeNNReader function implementation
 */

/*!
 *  \fn int TreeReader(vector<MarkedNames> file2read, vector<MarkedNames> trees2read, vector<MarkedNames> var2read) "";
 *  \brief Function used to read selected variables stored in selected trees
 * 
 *  \param file2read name of the file which stores the trees
 *  \param trees2read vector of names of trees that the user wants to explore
 */

#include "HeaderFiles/FakeNNReader.h"

int FakeNNReader(vector<MarkedNames> filenames,
                 vector<string> *fake_factory_names,
                 TString preselection,
                 vector<string> features,
                 vector<Float_t> Input_feature_var,
                 unordered_map<string, Float_t> MVA_feature_var,
                 vector<TMVAMethod> tmvamethods)
{
    for (vector<MarkedNames>::iterator fileit = filenames.begin(); fileit < filenames.end(); fileit++)
    {
        TFile *eventinputfile = TFile::Open(TString::Format("InputFiles/%s", fileit->name.Data()), "read");
        TTree *Event_old = (TTree *)eventinputfile->Get("BTo3Mu");

        ROOT::RDataFrame rdataframeold = ROOT::RDataFrame(*Event_old);
        auto rdataframe = rdataframeold.Filter(preselection.Data());
        rdataframe.Snapshot("BTo3Mu", TString::Format("OutputFiles/TMVAResults/%s", fileit->name.Data()));
        TFile *eventoutputfile = TFile::Open(TString::Format("OutputFiles/TMVAResults/%s", fileit->name.Data()), "update");
        TTree *Event = (TTree *)eventoutputfile->Get("BTo3Mu");

        Double_t FakeWeight = 0, FakeProba = 0, FakeRarity = 0;
        TBranch *Branch_FakeWeight = Event->Branch("FakeWeight", &FakeWeight, "FakeWeight/D");
        TBranch *Branch_FakeProba = Event->Branch("FakeProba", &FakeProba, "FakeProba/D");
        TBranch *Branch_FakeRarity = Event->Branch("FakeRarity", &FakeRarity, "FakeRarity/D");
        int event_entries = Event->GetEntries();
        for (vector<string>::iterator mvavar = features.begin(); mvavar < features.end(); mvavar++)
        {
            Event->SetBranchAddress(mvavar->data(), &(Input_feature_var[distance(features.begin(), mvavar)]));
        }
        //Creating a dataframe which holds only names of variables with a variance greater than the threshold set up before
        TString loader_with_variance_threshold_filename = TString::Format("%s", fake_factory_names->at(2).data());
        TString loader_with_variance_threshold_treename = TString::Format("%s/TrainTree", "vt_transformed_dataset");
        ROOT::RDataFrame loader_with_variance_threshold_dataframe(loader_with_variance_threshold_treename, loader_with_variance_threshold_filename);

        for (vector<TMVAMethod>::iterator tmvamethit = tmvamethods.begin(); tmvamethit < tmvamethods.end(); tmvamethit++)
        {
            cout << event_entries << endl
                 << endl
                 << endl
                 << endl;
            //Event->SetName(TString::Format("", tmvamethit->tmvamethodname));

            //Looking for any weight of the specified method in the folder containing the training results
            TString weightfilename = TString::Format("TMVAResults/%s/Weights/TMVAClassification_%s.weights.xml", "vt_transformed_dataset", tmvamethit->tmvamethodname);

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
        }
        eventoutputfile->cd();
        Event->Write("", TObject::kOverwrite);
        eventinputfile->Close();
        eventoutputfile->Close();
    }
    return 0;
}