#pragma once

//===============================================================================================
// std libraries
//===============================================================================================

//===============================================================================================
// geant libraries
//===============================================================================================

#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"

//===============================================================================================
// user libraries
//===============================================================================================

//===============================================================================================
// Class Prototype
//===============================================================================================


class generator : public G4VUserPrimaryGeneratorAction {    
    public:
        generator();
        virtual ~generator() override;

        virtual void GeneratePrimaries(G4Event* anEvent) override;

        G4double getIEnergy() { return energy; }
        G4ThreeVector getIPosition() { return position; }
        G4ThreeVector getIDirection() { return direction; }

    private:
        G4GeneralParticleSource *uParticleGPS;
        G4double energy = -1;
        G4ThreeVector position, direction;
};