//===============================================================================================
// Std libraries
//===============================================================================================

#include <iostream>
#include <set>
#include <chrono>

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

//===============================================================================================
// Main code
//===============================================================================================

int main(int argc, char** argv) {
    //-------------------------------------------------
    // Ackowledge input commands
    //-------------------------------------------------
    
    uint16_t sessionType = 0;
    uint16_t nThreads = 1;
    uint16_t nRuns = 0;
    int uSeed = 0;
    uint64_t nHistories = 2.5e6;

    std::string macroFile = "";

    int i = 0;
    while(++i < argc) {
        std::string opt(argv[i]);
        if (opt.compare("-ui") == 0) sessionType = std::stoi(argv[++i]);

        else if (opt.compare("-n") == 0) nThreads = std::stoi(argv[++i]);
        else if (opt.compare("-runs") == 0) nRuns = std::stoi(argv[++i]);
        else if (opt.compare("-seed") == 0) uSeed = std::stoi(argv[++i]);        
        else if (opt.compare("-p") == 0) nHistories = std::stoull(argv[++i]);

        else if (opt == "-macro") macroFile = argv[++i];
    }

    //-------------------------------------------------
    // Run manager
    //-------------------------------------------------

    #ifdef G4MULTITHREADED
        if (nThreads == 0) nThreads = G4Thread::hardware_concurrency();
        G4MTRunManager *runManager = new G4MTRunManager();
        runManager->SetNumberOfThreads(nThreads);
    #else
        G4RunManager *runManager = new G4RunManager();
    #endif

    //-------------------------------------------------
    // Define physical and parallel worlds
    //-------------------------------------------------

    geometry* realWorld = new geometry;

    G4VModularPhysicsList* physicsList = new physics;
    physicsList->RegisterPhysics(new G4ParallelWorldPhysics("parallelWorld"));

    //-------------------------------------------------
    // Initialization (state aa)
    //-------------------------------------------------

    runManager->SetUserInitialization(realWorld);
    runManager->SetUserInitialization(physicsList);
    runManager->SetUserInitialization(new actions);
    runManager->Initialize();
    
    //-------------------------------------------------
    // Visualization & user interface manager
    //-------------------------------------------------
    
    auto start = std::chrono::steady_clock::now();

    G4VisManager  *visManager  = new G4VisExecutive(); visManager->Initialize();
    G4UImanager   *uiManager   = G4UImanager::GetUIpointer();
    G4UIExecutive *uiExecutive = 0;

    if (sessionType == 1) {
        //-------------------------------------------------
        // Graphical mode
        //-------------------------------------------------

        uiExecutive = new G4UIExecutive(argc, argv, "qt");
        if (!macroFile.empty()) uiManager->ApplyCommand("/control/execute " + macroFile);

        uiExecutive->SessionStart();
    } else {
                        
        //-------------------------------------------------
        // Interactive session
        //-------------------------------------------------

        if (sessionType == 2) {
            uiExecutive = new G4UIExecutive(argc, argv, "tcsh");
            if (!macroFile.empty()) uiManager->ApplyCommand("/control/execute " + macroFile);
        }

        //-------------------------------------------------
        // Start simulation
        //-------------------------------------------------

        const uint64_t MAX_PER_RUN = 1000000000ULL;

        for (size_t irun = 0; irun < nRuns; ++irun) {
            uint64_t remainingHistories = nHistories;
            uint64_t chunkIndex = 0;
            
            while (remainingHistories > 0) {
                uint64_t chunk = (remainingHistories > MAX_PER_RUN) ? MAX_PER_RUN : remainingHistories;
                
                G4Random::setTheSeed(irun + 1 + uSeed*nRuns + chunkIndex);
                runManager->SetRunIDCounter((irun * 10000) + chunkIndex);
                runManager->BeamOn(chunk);
                
                remainingHistories -= chunk;
                chunkIndex++;
            }
        }
    }

    //-------------------------------------------------
    // Free-up space
    //-------------------------------------------------    

    auto end = std::chrono::steady_clock::now();
    G4cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " s" << G4endl;

    // 1. Delete RunManager to release all file locks
    delete runManager;       
    delete visManager;
    if (uiExecutive) delete uiExecutive;

    // 2. Automate the merging process
    std::cout << "\nMerging chunked ROOT files..." << std::endl;
    
    // The -f flag forces an overwrite if merged_results.root already exists
    int sysReturn = std::system("hadd -f merged_results.root results_run*.root");
    
    // Optional: Clean up the individual chunk files if the merge was successful
    if (sysReturn == 0) {
        std::cout << "Merge successful. Removing temporary chunk files..." << std::endl;
        int status = std::system("rm results_run*.root");
        if (status != 0) {
            // Handle the failure (e.g., print a warning or exit)
            std::cerr << "Warning: Failed to clear old ROOT files." << std::endl;
}
    } else {
        std::cerr << "Warning: 'hadd' merge failed. Check if ROOT is sourced in your environment." << std::endl;
    }

    

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
}