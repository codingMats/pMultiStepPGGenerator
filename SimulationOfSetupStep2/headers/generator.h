#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4AutoLock.hh"
#include <vector>

struct PhaseSpaceParticle {
    G4int pdg;
    G4double energy;
    G4ThreeVector position;
    G4ThreeVector direction;
    G4double time;
};

class generator : public G4VUserPrimaryGeneratorAction {    
    public:
        generator();
        virtual ~generator() override;

        virtual void GeneratePrimaries(G4Event* anEvent) override;

        G4double getIEnergy() { return energy; }
        G4ThreeVector getIPosition() { return position; }
        G4ThreeVector getIDirection() { return direction; }

    private:
        G4ParticleGun* uParticleGun;
        G4double energy = -1;
        G4ThreeVector position, direction;
        uint64_t currentEntry;

        // Static members shared across all threads
        static std::vector<PhaseSpaceParticle>* sharedPhaseSpaceData;
        static G4Mutex mutex;
};