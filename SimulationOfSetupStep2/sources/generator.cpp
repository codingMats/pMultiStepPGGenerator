#include "generator.h"
#include "../../sharedFiles/shared_params.h"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4AnalysisReader.hh"
#include "G4RunManager.hh" // ADD THIS LINE

// Initialize static members
std::shared_ptr<std::vector<PhaseSpaceParticle>> generator::sharedPhaseSpaceData = nullptr;
std::atomic<uint64_t> generator::globalAtomicIndex{0};
G4Mutex generator::mutex = G4MUTEX_INITIALIZER;

generator::generator(G4String materialName): G4VUserPrimaryGeneratorAction() {        
    uParticleGun = new G4ParticleGun(1);
    
    G4AutoLock lock(&mutex);
    
    if (sharedPhaseSpaceData == nullptr) {
        sharedPhaseSpaceData = std::make_shared<std::vector<PhaseSpaceParticle>>();
        
        auto analysisReader = G4AnalysisReader::Instance();
        G4String psFilePath = SharedParams::dataPath + "PhaseSpace_" + materialName + ".root";
        analysisReader->SetFileName(psFilePath);
        
        G4int ntupleId = analysisReader->GetNtuple("PhaseSpace");
        // ... (Keep the rest of your Tree-reading logic exactly as it is) ...
        
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
    lock.unlock();
}

generator::~generator() {
    delete uParticleGun;
    // sharedPhaseSpaceData automatically cleans up when the last thread instance is destroyed.
}

void generator::GeneratePrimaries(G4Event* anEvent) {
    if (!sharedPhaseSpaceData || sharedPhaseSpaceData->empty()) return;

    // Fetch next index atomically and increment
    uint64_t myIndex = globalAtomicIndex.fetch_add(1, std::memory_order_relaxed);

    if (myIndex >= sharedPhaseSpaceData->size()) {
        G4RunManager::GetRunManager()->AbortRun();
        return;
    }

    const PhaseSpaceParticle& p = (*sharedPhaseSpaceData)[myIndex];

    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(p.pdg);
    if (!particle) particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");

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