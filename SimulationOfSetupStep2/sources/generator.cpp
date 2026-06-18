#include "generator.h"
#include "../../sharedFiles/shared_params.h"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "G4AnalysisReader.hh"

// Initialize the static members
std::vector<PhaseSpaceParticle>* generator::sharedPhaseSpaceData = nullptr;
G4Mutex generator::mutex = G4MUTEX_INITIALIZER;

generator::generator(): G4VUserPrimaryGeneratorAction() {        
    uParticleGun = new G4ParticleGun(1);
    
    // Lock the thread to ensure only the first thread reads the file
    G4AutoLock lock(&mutex);
    
    if (sharedPhaseSpaceData == nullptr) {
        sharedPhaseSpaceData = new std::vector<PhaseSpaceParticle>();
        
        auto analysisReader = G4AnalysisReader::Instance();
        G4String psFilePath = SharedParams::dataPath + "merged_results.root";
        analysisReader->SetFileName(psFilePath);
        
        G4int ntupleId = analysisReader->GetNtuple("PhaseSpace");
        
        if (ntupleId >= 0) {
            G4int eventid, pdg;
            G4double energy_MeV, pos_x_cm, pos_y_cm, pos_z_cm, mom_dir_x, mom_dir_y, mom_dir_z, time_ns;
            
            analysisReader->SetNtupleIColumn("eventid", eventid);
            analysisReader->SetNtupleIColumn("pdg", pdg);
            analysisReader->SetNtupleDColumn("energy_MeV", energy_MeV);
            analysisReader->SetNtupleDColumn("pos_x_cm", pos_x_cm);
            analysisReader->SetNtupleDColumn("pos_y_cm", pos_y_cm);
            analysisReader->SetNtupleDColumn("pos_z_cm", pos_z_cm);
            analysisReader->SetNtupleDColumn("mom_dir_x", mom_dir_x);
            analysisReader->SetNtupleDColumn("mom_dir_y", mom_dir_y);
            analysisReader->SetNtupleDColumn("mom_dir_z", mom_dir_z);
            analysisReader->SetNtupleDColumn("time_ns", time_ns);
            
            while (analysisReader->GetNtupleRow(ntupleId)) {
                PhaseSpaceParticle p;
                p.pdg = pdg;
                p.energy = energy_MeV;
                p.position = G4ThreeVector(pos_x_cm * cm, pos_y_cm * cm, pos_z_cm * cm);
                p.direction = G4ThreeVector(mom_dir_x, mom_dir_y, mom_dir_z);
                p.time = time_ns;
                sharedPhaseSpaceData->push_back(p);
            }
        } else {
            G4Exception("generator::generator()", "TreeError", FatalException, "PhaseSpace tree not found.");
        }
    }
    
    lock.unlock(); // Release the lock so other threads can proceed

    // Offset thread start to avoid identical sequence overlapping
    currentEntry = (sharedPhaseSpaceData->size() > 0) ? (uint64_t)(G4UniformRand() * sharedPhaseSpaceData->size()) : 0;
}

generator::~generator() {
    delete uParticleGun;
    
    // Memory cleanup: only the last thread to finish should delete the shared data
    G4AutoLock lock(&mutex);
    if (sharedPhaseSpaceData != nullptr) {
        // In a strict MT environment, you might let the OS reclaim this at process exit,
        // or use an atomic counter. For simplicity, leaving it allocated until program exit is standard.
    }
    lock.unlock();
}

void generator::GeneratePrimaries(G4Event* anEvent) {
    if (sharedPhaseSpaceData->empty()) return;

    if (currentEntry >= sharedPhaseSpaceData->size()) {
        currentEntry = 0; 
    }

    const PhaseSpaceParticle& p = (*sharedPhaseSpaceData)[currentEntry++];

    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(p.pdg);
    if (!particle) {
        particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    }

    uParticleGun->SetParticleDefinition(particle);
    uParticleGun->SetParticleEnergy(p.energy * MeV);
    
    position = p.position;
    uParticleGun->SetParticlePosition(position);

    direction = p.direction;
    uParticleGun->SetParticleMomentumDirection(direction);
    uParticleGun->SetParticleTime(p.time * ns);

    uParticleGun->GeneratePrimaryVertex(anEvent);
    energy = p.energy * MeV;
}