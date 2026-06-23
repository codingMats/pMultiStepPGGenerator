#include "stepaction.h"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"

stepaction::stepaction(eventaction *eventaction): 
G4UserSteppingAction(), ueventaction(eventaction), pDetectorAssembly(nullptr),pOuterTarget(nullptr), pWorld(nullptr), pointersInitialized(false){
}

stepaction::~stepaction(){    
}

void stepaction::UserSteppingAction(const G4Step *uStep){
    G4Track* track = uStep->GetTrack();

    // ----------------------------------------------------------------
    // 1. Hardware Cutoff
    // ----------------------------------------------------------------
    if (track->GetGlobalTime() > 900.0 * s) {
        track->SetTrackStatus(fStopAndKill);
        return;
    }

    if (!pointersInitialized) {
        pDetectorAssembly = G4LogicalVolumeStore::GetInstance()->GetVolume("lDetectorAssembly", false);
        pOuterTarget = G4LogicalVolumeStore::GetInstance()->GetVolume("lOuterTarget", false);
        pWorld = G4LogicalVolumeStore::GetInstance()->GetVolume("lWorld", false);
        pointersInitialized = true;
    }

    G4StepPoint* preStepPoint  = uStep->GetPreStepPoint();
    G4StepPoint* postStepPoint = uStep->GetPostStepPoint();

    G4VPhysicalVolume* prePhys = preStepPoint->GetPhysicalVolume();
    G4VPhysicalVolume* postPhys = postStepPoint->GetPhysicalVolume();
    
    if (!prePhys || !postPhys) return;

    G4LogicalVolume *preVolume = prePhys->GetLogicalVolume();
    G4LogicalVolume *postVolume = postPhys->GetLogicalVolume();
    G4int trackID = track->GetTrackID();

    // ----------------------------------------------------------------
    // Target Exit Directional Culling (Universal)
    // ----------------------------------------------------------------
    if (preVolume == pOuterTarget && postVolume == pWorld) {
        G4ThreeVector momentumDir = track->GetMomentumDirection();
        G4ThreeVector detAxis(1.0, 0.0, 0.0);
        
        if (momentumDir.angle(detAxis) > 45.0 * deg) {
            track->SetTrackStatus(fStopAndKill);
            return;
        }
    }

    // ----------------------------------------------------------------
    // 2. Incident Particle Boundary Detection
    // ----------------------------------------------------------------
    G4bool preInDetector = false;
    G4bool postInDetector = false;

    auto preTouchable = preStepPoint->GetTouchable();
    for (G4int i = 0; i <= preTouchable->GetHistoryDepth(); ++i) {
        if (preTouchable->GetVolume(i)->GetLogicalVolume() == pDetectorAssembly) {
            preInDetector = true; break;
        }
    }

    auto postTouchable = postStepPoint->GetTouchable();
    for (G4int i = 0; i <= postTouchable->GetHistoryDepth(); ++i) {
        if (postTouchable->GetVolume(i)->GetLogicalVolume() == pDetectorAssembly) {
            postInDetector = true; break;
        }
    }

    if (postInDetector && !preInDetector) {
        if (!ueventaction->HasIncidentMap(trackID)) {
            G4String procName = track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "Primary";
            ueventaction->AddIncidentParticle(
                trackID, track->GetDefinition()->GetPDGEncoding(), procName, 
                track->GetKineticEnergy() / MeV, track->GetGlobalTime() / ns
            );
        }
    }

    // ----------------------------------------------------------------
    // 3. Secondary Inheritance Mapping
    // ----------------------------------------------------------------
    G4int parentID = track->GetParentID();
    
    // If this is a secondary track and it isn't in our map yet, 
    // inherit the root incident ID from its parent.
    if (parentID > 0 && !ueventaction->HasIncidentMap(trackID)) {
        ueventaction->LinkSecondary(parentID, trackID);
    }

    // ----------------------------------------------------------------
    // 4. Energy Scoring
    // ----------------------------------------------------------------
    G4double edep = uStep->GetTotalEnergyDeposit() / MeV;
    if (edep > 0) {
        static const G4LogicalVolume* lCeBr3_ptr = G4LogicalVolumeStore::GetInstance()->GetVolume("lCeBr3", false);
        static const G4LogicalVolume* lBGO_ptr = G4LogicalVolumeStore::GetInstance()->GetVolume("lBGO", false);

        G4double hitTime = track->GetGlobalTime() / ns; // Get the time of the hit

        if (preVolume == lCeBr3_ptr) {
            ueventaction->AddEnergy(trackID, preStepPoint->GetTouchable()->GetCopyNumber(), edep, true, hitTime);
        } 
        else if (preVolume == lBGO_ptr) {
            ueventaction->AddEnergy(trackID, preStepPoint->GetTouchable()->GetCopyNumber(), edep, false, hitTime);
        }
    }
}