#pragma once

//===============================================================================================
// geant libraries
//===============================================================================================

#include "G4RunManager.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

#include "G4Gamma.hh"
#include "G4Proton.hh"
#include "G4LogicalVolume.hh"

//===============================================================================================
// user libraries
//===============================================================================================

#include "eventaction.h"

//===============================================================================================
// class prototype
//===============================================================================================

class stepaction: public G4UserSteppingAction
{
    public:
        stepaction(eventaction *eventaction);
        virtual ~stepaction() override;

        virtual void UserSteppingAction(const G4Step*) override;

    private:
        eventaction *ueventaction;
        
        G4LogicalVolume* pDetectorAssembly;
        G4LogicalVolume* pOuterTarget;
        G4LogicalVolume* pWorld;
        bool pointersInitialized;
};