#pragma once

//===============================================================================================
// geant libraries
//===============================================================================================

#include "G4UserStackingAction.hh"
#include "G4Track.hh"
#include "G4LogicalVolume.hh"

//===============================================================================================
// class prototype
//===============================================================================================

class stackaction: public G4UserStackingAction
{
    public:
        stackaction();
        virtual ~stackaction() override;
        
        virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* utrack) override;

    private:
        const G4ParticleDefinition* uparticle;
        
        G4LogicalVolume* pInnerTarget;
        G4LogicalVolume* pOuterTarget;
        bool pointersInitialized;
};