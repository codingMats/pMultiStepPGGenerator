#pragma once
#include "G4SystemOfUnits.hh"

namespace SharedParams {
    // Target Geometry
    constexpr G4double targetRadius = 1.25 * cm;
    constexpr G4double targetHalfLength = 4.5 * mm;
    
    // Placement
    constexpr G4double detectorLateralDistance = 5.0 * cm; 

    // File Paths
    const std::string dataPath = "/omics/groups/OE0471/internal/m361e/master/pMultiStepPGGenerator/data/";
}