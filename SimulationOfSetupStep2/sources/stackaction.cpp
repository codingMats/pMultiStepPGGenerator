#include "stackaction.h"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SystemOfUnits.hh"

stackaction::stackaction() : 
pInnerTarget(nullptr), pOuterTarget(nullptr), pointersInitialized(false) {    
}

stackaction::~stackaction(){    
}

G4ClassificationOfNewTrack stackaction::ClassifyNewTrack(const G4Track* utrack) {

    return fUrgent;    
}