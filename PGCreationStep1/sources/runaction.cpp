#include "runaction.h"
#include "../../sharedFiles/shared_params.h"

runaction::runaction(): G4UserRunAction() {
    auto analysisManager = G4AnalysisManager::Instance();
    #ifdef G4MULTITHREADED
        analysisManager->SetNtupleMerging(false);
    #endif
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetVerboseLevel(1);
    
    analysisManager->CreateNtuple("PhaseSpace", "Scored Particles on Outer Cylinder");
    analysisManager->CreateNtupleIColumn("eventid");          // 0
    analysisManager->CreateNtupleIColumn("pdg");              // 1
    analysisManager->CreateNtupleDColumn("energy_MeV");       // 2
    analysisManager->CreateNtupleDColumn("pos_x_cm");         // 3
    analysisManager->CreateNtupleDColumn("pos_y_cm");         // 4
    analysisManager->CreateNtupleDColumn("pos_z_cm");         // 5
    analysisManager->CreateNtupleDColumn("mom_dir_x");        // 6
    analysisManager->CreateNtupleDColumn("mom_dir_y");        // 7
    analysisManager->CreateNtupleDColumn("mom_dir_z");        // 8
    analysisManager->CreateNtupleDColumn("time_ns");          // 9
    
    // New parameters
    analysisManager->CreateNtupleDColumn("creation_energy_MeV"); // 10
    analysisManager->CreateNtupleSColumn("creation_process");    // 11
    analysisManager->CreateNtupleSColumn("nuclear_reaction");    // 12
    analysisManager->CreateNtupleIColumn("interaction_id");      // 13
    
    analysisManager->FinishNtuple(0);
}

runaction::~runaction(){
}

G4Run* runaction::GenerateRun() {
    return new run;
}
void runaction::BeginOfRunAction(const G4Run* urun){   
    auto analysisManager = G4AnalysisManager::Instance();
    G4String fileName = SharedParams::dataPath + "results_run" + std::to_string(urun->GetRunID()) + ".root";
    analysisManager->OpenFile(fileName);
}

void runaction::EndOfRunAction(const G4Run* urun){
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}