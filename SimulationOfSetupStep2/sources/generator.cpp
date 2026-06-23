#include "generator.h"
#include "../../sharedFiles/shared_params.h"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RootAnalysisReader.hh"
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
        
        auto analysisReader = G4RootAnalysisReader::Instance();
        G4String psFilePath = SharedParams::dataPath + "materialPhaseSpaces/" + materialName + "/PhaseSpace_" + materialName + ".root";
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
                G4ThreeVector pos(pos_x_cm * cm, pos_y_cm * cm, pos_z_cm * cm);
                G4ThreeVector mom(mom_dir_x, mom_dir_y, mom_dir_z);

                // ---------------------------------------------------------
                // PHASE SPACE FOLDING LOGIC (36 Degree Window)
                // ---------------------------------------------------------
                G4double phi = pos.phi(); 
                G4double window = 36.0 * deg;
                
                // Find the sector (0 to 9) and calculate the necessary rotation 
                // to fold it into Sector 0 (facing +X)
                G4double sector = std::round(phi / window);
                G4double rotAngle = -sector * window;

                // Apply rotation to both position and momentum
                pos.rotateZ(rotAngle);
                mom.rotateZ(rotAngle);
                // ---------------------------------------------------------

                PhaseSpaceParticle p;
                p.pdg = pdg;
                p.energy = energy_MeV;
                p.position = pos;
                p.direction = mom;
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