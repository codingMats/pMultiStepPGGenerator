//===============================================================================================
// std libraries
//===============================================================================================

//===============================================================================================
// geant libraries
//===============================================================================================

//===============================================================================================
// user libraries
//===============================================================================================

#include "generator.h"
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
//===============================================================================================
// de/constructor
//===============================================================================================
generator::generator(): 
G4VUserPrimaryGeneratorAction() {        
    uParticleGPS = new G4GeneralParticleSource();
    
    // 1. Particle Type
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("proton");
    uParticleGPS->GetCurrentSource()->SetParticleDefinition(particle);
    uParticleGPS->SetNumberOfParticles(1);

    // 2. Beam Energy
    uParticleGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    uParticleGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(100.2098319 * MeV);

    // 3. Beam Position & Spatial Distribution
// 3. Beam Position & Spatial Distribution
G4double fwhm = 15.9 * mm;
G4double sigma = fwhm / 2.3548; 

// Dynamically retrieve target position
G4double targetX = 0.0;
G4double targetY = 0.0;
G4double targetZ = 0.0;
G4VPhysicalVolume* targetVol = G4PhysicalVolumeStore::GetInstance()->GetVolume("pOuterTarget");
if (targetVol) {
    targetX = targetVol->GetTranslation().x();
    targetY = targetVol->GetTranslation().y();
    targetZ = targetVol->GetTranslation().z();
}

G4SPSPosDistribution* posDist = uParticleGPS->GetCurrentSource()->GetPosDist();

// Offset the source 10 cm upstream of the target's center along the Z-axis
posDist->SetCentreCoords(G4ThreeVector(targetX, targetY, targetZ - 10.0 * cm));

// Define the 2D distribution type
posDist->SetPosDisType("Beam");
posDist->SetPosDisShape("Circle"); 

// --------------------------------------------------------------------------
// Align the GPS local XY distribution plane to the global XY plane
// --------------------------------------------------------------------------
posDist->SetPosRot1(G4ThreeVector(1., 0., 0.)); // GPS Local X maps to Global X
posDist->SetPosRot2(G4ThreeVector(0., 1., 0.)); // GPS Local Y maps to Global Y

// Apply the symmetric 2D sigma in the new XY plane
posDist->SetBeamSigmaInR(sigma);

// 4. Momentum Direction (Propagate along Z)
uParticleGPS->GetCurrentSource()->GetAngDist()->SetAngDistType("planar");
uParticleGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
}
generator::~generator() {
    delete uParticleGPS;
}

//===============================================================================================
// generate primaries
//===============================================================================================

void generator::GeneratePrimaries(G4Event* anEvent) {
    uParticleGPS->GeneratePrimaryVertex(anEvent);
    G4PrimaryVertex* vertex = anEvent->GetPrimaryVertex();

    if (vertex) {
        G4PrimaryParticle* particle = vertex->GetPrimary();

        // Get the particle information.
        if (particle) {
            energy = particle->GetKineticEnergy();
            position = vertex->GetPosition();
            direction = particle->GetMomentumDirection();

        // If for some reason there is no particle, set energy to -1.
        } else { energy = -1; }
    }
}