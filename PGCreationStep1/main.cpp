//===============================================================================================
// Std libraries
//===============================================================================================
#include <iostream>
#include <set>
#include <chrono>
#include <fstream>

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
#include "../../sharedFiles/shared_materials.h"
#include "../../sharedFiles/shared_params.h"

//===============================================================================================
// Main code
//===============================================================================================
int main(int argc, char** argv) {
    uint16_t sessionType = 0; uint16_t nThreads = 1; uint16_t nRuns = 0;
    int uSeed = 0; uint64_t nHistories = 2.5e6;
    std::string macroFile = "";
    
    // Default setup
    std::string targetMaterial = "WaterCa0"; 
    std::string stepName = "PhaseSpace";

    int i = 0;
    while(++i < argc) {
        std::string opt(argv[i]);
        if (opt.compare("-ui") == 0) sessionType = std::stoi(argv[++i]);
        else if (opt.compare("-n") == 0) nThreads = std::stoi(argv[++i]);
        else if (opt.compare("-runs") == 0) nRuns = std::stoi(argv[++i]);
        else if (opt.compare("-seed") == 0) uSeed = std::stoi(argv[++i]);        
        else if (opt.compare("-p") == 0) nHistories = std::stoull(argv[++i]);
        else if (opt == "-macro") macroFile = argv[++i];
        else if (opt == "-mat") targetMaterial = argv[++i];
    }

    // Build and verify material
    SharedMaterials::BuildAllMaterials();
    if (!SharedMaterials::GetTargetMaterial(targetMaterial)) {
        std::cerr << "FATAL: Material '" << targetMaterial << "' not found." << std::endl; return 1;
    }

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
        }
        const uint64_t MAX_PER_RUN = 1000000000ULL;
        for (size_t irun = 0; irun < nRuns; ++irun) {
            uint64_t remainingHistories = nHistories; uint64_t chunkIndex = 0;
            while (remainingHistories > 0) {
                uint64_t chunk = (remainingHistories > MAX_PER_RUN) ? MAX_PER_RUN : remainingHistories;
                G4Random::setTheSeed(irun + 1 + uSeed*nRuns + chunkIndex);
                runManager->SetRunIDCounter((irun * 10000) + chunkIndex);
                runManager->BeamOn(chunk);
                remainingHistories -= chunk; chunkIndex++;
            }
        }
    }

    auto end = std::chrono::steady_clock::now();
    G4cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " s" << G4endl;

    delete runManager; delete visManager; if (uiExecutive) delete uiExecutive;

    // DYNAMIC MERGING
    std::cout << "\nMerging chunked ROOT files in data directory..." << std::endl;
    std::string baseName = stepName + "_" + targetMaterial;
    std::string mergeCmd = "hadd -f " + SharedParams::dataPath + baseName + ".root " + SharedParams::dataPath + baseName + "_run*.root";
    if (std::system(mergeCmd.c_str()) == 0) {
        std::cout << "Merge successful. Removing temporary files..." << std::endl;
        int rmStatus = std::system(("rm " + SharedParams::dataPath + baseName + "_run*.root").c_str());
        (void)rmStatus; // Suppress the unused-result warning
    } else {
        std::cerr << "Warning: 'hadd' merge failed." << std::endl;
    }
    std::cout << "Press Enter to exit..." << std::endl; std::cin.get();
}