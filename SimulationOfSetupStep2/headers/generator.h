#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4AutoLock.hh"
#include <vector>
#include <memory>
#include <atomic>

struct PhaseSpaceParticle {
    G4int pdg;
    G4double energy;
    G4ThreeVector position;
    G4ThreeVector direction;
    G4double time;
};

class generator : public G4VUserPrimaryGeneratorAction {    
    public:
        generator(G4String materialName);
        virtual ~generator() override;

        virtual void GeneratePrimaries(G4Event* anEvent) override;

        G4double getIEnergy() { return energy; }
        G4ThreeVector getIPosition() { return position; }
        G4ThreeVector getIDirection() { return direction; }

    private:
        G4ParticleGun* uParticleGun;
        G4double energy = -1;
        G4ThreeVector position, direction;

        // Static members shared across all threads
        static G4Mutex mutex;
        static std::shared_ptr<std::vector<PhaseSpaceParticle>> sharedPhaseSpaceData;
        static std::atomic<uint64_t> globalAtomicIndex;
};