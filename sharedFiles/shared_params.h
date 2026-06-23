#pragma once
#include "G4SystemOfUnits.hh"
#include <string>
#include <unistd.h> // Required for getcwd()

namespace SharedParams {
    // Target Geometry
    constexpr G4double targetRadius =  3.0 * cm;
    constexpr G4double targetHalfLength = 4.5 * mm;
    
    // Placement
    constexpr G4double detectorLateralDistance = 5.0 * cm; 

    // Dynamic Path Resolution
    inline std::string getDynamicDataPath() {
        // Hardcoded absolute path for the new simulation project directory
        if (chdir("/omics/groups/OE0471/internal/m361e/master/qPaperSimulations/data/") != 0) {
            return "/home/mats/projects/geant4/pMultiStepPGGenerator/data/";
        }
        else{

            return "/omics/groups/OE0471/internal/m361e/master/qPaperSimulations/data/";
        }
    }

    // Initialize the path once at runtime
    static const std::string dataPath = getDynamicDataPath();
}