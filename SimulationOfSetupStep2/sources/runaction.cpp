#include "runaction.h"

runaction::runaction(G4String material, G4String step) : G4UserRunAction(), uMaterial(material), uStep(step) {
    auto analysisManager = G4AnalysisManager::Instance();
    #ifdef G4MULTITHREADED
        analysisManager->SetNtupleMerging(false);
    #endif
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetVerboseLevel(1);
    
    // Define the NTuple structure once at initialization
    analysisManager->CreateNtuple("incident_analysis", "Scored Particles and AC logic");
    analysisManager->CreateNtupleIColumn("eventid");
    analysisManager->CreateNtupleIColumn("pdg");
    analysisManager->CreateNtupleSColumn("process");
    analysisManager->CreateNtupleDColumn("creation_energy");
    analysisManager->CreateNtupleDColumn("global_time");
    analysisManager->CreateNtupleDColumn("edep_cebr3");
    analysisManager->CreateNtupleDColumn("edep_bgo1");
    analysisManager->CreateNtupleDColumn("edep_bgo2");
    analysisManager->CreateNtupleDColumn("edep_bgo3");
    analysisManager->CreateNtupleDColumn("edep_bgo4");
    analysisManager->CreateNtupleDColumn("edep_bgo5");
    analysisManager->CreateNtupleDColumn("edep_bgo6");
    analysisManager->CreateNtupleDColumn("edep_bgo7");
    analysisManager->CreateNtupleDColumn("edep_bgo8");
    analysisManager->CreateNtupleDColumn("first_bgo_time");
    analysisManager->FinishNtuple(0);
}

runaction::~runaction(){
}

G4Run* runaction::GenerateRun() {
    return new run;
}
void runaction::BeginOfRunAction(const G4Run* urun){   
    auto analysisManager = G4AnalysisManager::Instance();
    G4String fileName = SharedParams::dataPath + uStep + "_" + uMaterial + "_run" + std::to_string(urun->GetRunID()) + ".root";
    analysisManager->OpenFile(fileName);
}
void runaction::EndOfRunAction(const G4Run* urun){
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}