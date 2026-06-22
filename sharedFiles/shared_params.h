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
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            std::string path(cwd);
            std::string target = "pMultiStepPGGenerator";
            size_t pos = path.find(target);
            
            if (pos != std::string::npos) {
                // Slice the path up to the end of the target folder name and append /data/
                return path.substr(0, pos + target.length()) + "/data/";
            }
        }
        // Fallback if the executable is run from completely outside the project tree
        return "./data/"; 
    }

    // Initialize the path once at runtime
    static const std::string dataPath = getDynamicDataPath();
}