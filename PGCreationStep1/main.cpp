//===============================================================================================
// Std libraries
//===============================================================================================
#include <iostream>
#include <set>
#include <chrono>
#include <fstream>
#include <functional>
#include <string>

//===============================================================================================
// Geant libraries
//===============================================================================================
#include "G4MTRunManager.hh"
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"
#include "G4Threading.hh"
#include "G4ParallelWorldPhysics.hh"

//===============================================================================================
// User libraries
//===============================================================================================
#include "geometry.h"
#include "actions.h"
#include "physics.h"
#include "../../sharedFiles/shared_materials.h" // Adjust path for Step 2: "../sharedFiles..."
#include "../../sharedFiles/shared_params.h"

int main(int argc, char** argv) {
    uint16_t sessionType = 0; 
    uint16_t nThreads = 1; 
    uint16_t runNum = 0;
    int uSeed = 12; 
    uint64_t nHistories = 2.5e6;
    std::string macroFile = "";
    
    // Default setup
    std::string targetMaterial = "WaterCa0"; 
    std::string stepName = "PhaseSpace"; // Change to "Final" for Step 2

    int i = 0;
    while(++i < argc) {
        std::string opt(argv[i]);
        if (opt.compare("-ui") == 0) sessionType = std::stoi(argv[++i]);
        else if (opt.compare("-n") == 0) nThreads = std::stoi(argv[++i]);
        else if (opt.compare("-run") == 0) runNum = std::stoi(argv[++i]);
        else if (opt.compare("-seed") == 0) uSeed = std::stoi(argv[++i]);        
        else if (opt.compare("-p") == 0) nHistories = std::stoull(argv[++i]);
        else if (opt == "-macro") macroFile = argv[++i];
        else if (opt == "-mat") targetMaterial = argv[++i];
    }

    SharedMaterials::BuildAllMaterials();
    if (!SharedMaterials::GetTargetMaterial(targetMaterial)) {
        std::cerr << "FATAL: Material '" << targetMaterial << "' not found." << std::endl; return 1;
    }

    // NOTE: Keep the phase space integrity check here for Step 2, remove for Step 1.

    #ifdef G4MULTITHREADED
        if (nThreads == 0) nThreads = G4Thread::hardware_concurrency();
        G4MTRunManager *runManager = new G4MTRunManager();
        runManager->SetNumberOfThreads(nThreads);
    #else
        G4RunManager *runManager = new G4RunManager();
    #endif

    G4VModularPhysicsList* physicsList = new physics;
    physicsList->RegisterPhysics(new G4ParallelWorldPhysics("parallelWorld"));

    runManager->SetUserInitialization(new geometry(targetMaterial));
    runManager->SetUserInitialization(physicsList);
    runManager->SetUserInitialization(new actions(targetMaterial, stepName));
    runManager->Initialize();
    
    auto start = std::chrono::steady_clock::now();
    G4VisManager *visManager = new G4VisExecutive(); visManager->Initialize();
    G4UImanager  *uiManager  = G4UImanager::GetUIpointer();
    G4UIExecutive *uiExecutive = 0;

    if (sessionType == 1) {
        uiExecutive = new G4UIExecutive(argc, argv, "qt");
        if (!macroFile.empty()) uiManager->ApplyCommand("/control/execute " + macroFile);
        uiExecutive->SessionStart();
    } else {
        if (sessionType == 2) {
            uiExecutive = new G4UIExecutive(argc, argv, "tcsh");
            if (!macroFile.empty()) uiManager->ApplyCommand("/control/execute " + macroFile);
            uiExecutive->SessionStart(); // Added missing SessionStart for tcsh
        }

        const uint64_t MAX_PER_RUN = 2000000000ULL;
        if (nHistories > MAX_PER_RUN) {
            std::cerr << "Error: Number of histories (" << nHistories << ") exceeds the maximum allowed per run (" << MAX_PER_RUN << ")." << std::endl;
            return 1;
        }

        // Deterministic Seed Generation
        std::string seedString = targetMaterial + "_Seed" + std::to_string(uSeed) + "_Run" + std::to_string(runNum);
        std::hash<std::string> hasher;
        long deterministicSeed = static_cast<long>(hasher(seedString) % 900000000); 

        G4Random::setTheSeed(deterministicSeed);
        std::cout << "Starting run " << runNum << " with " << nHistories << " histories (seed: " << G4Random::getTheSeed() << ")" << std::endl;

        runManager->SetRunIDCounter(runNum);
        runManager->BeamOn(nHistories);
    }

    auto end = std::chrono::steady_clock::now();
    G4cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " s" << G4endl;

    delete runManager; 
    delete visManager; 
    if (uiExecutive) delete uiExecutive;

    std::cout << "Simulation completed!" << std::endl;
    return 0;
}