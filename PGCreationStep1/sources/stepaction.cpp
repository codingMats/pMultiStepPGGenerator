#include "stepaction.h"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4EventManager.hh"
#include "G4NistManager.hh"
#include "G4Nucleus.hh"
#include "G4HadronicProcess.hh"
#include "G4AnalysisManager.hh"
#include "trackinfo.h"
#include "../../sharedFiles/shared_params.h"

stepaction::stepaction(eventaction *eventaction): 
G4UserSteppingAction(), ueventaction(eventaction), pOuterTarget(nullptr), pWorld(nullptr), pointersInitialized(false){}

stepaction::~stepaction(){}

void stepaction::UserSteppingAction(const G4Step *uStep){
    G4Track* track = uStep->GetTrack();

    // 15-minute time cutoff
    if (track->GetGlobalTime() > 900.0 * s) {
        track->SetTrackStatus(fStopAndKill);
        return;
    }

    if (!pointersInitialized) {
        pOuterTarget = G4LogicalVolumeStore::GetInstance()->GetVolume("lTarget", false);
        pWorld = G4LogicalVolumeStore::GetInstance()->GetVolume("lWorld", false);
        pointersInitialized = true;
    }

    G4StepPoint* preStepPoint  = uStep->GetPreStepPoint();
    G4StepPoint* postStepPoint = uStep->GetPostStepPoint();

    // --------------------------------------------------------------------------
    // 1. Tagging New Secondaries at Creation
    // --------------------------------------------------------------------------
    const std::vector<const G4Track*>* secondaries = uStep->GetSecondaryInCurrentStep();
    if (secondaries && secondaries->size() > 0) {
        G4VProcess* process = const_cast<G4VProcess*>(postStepPoint->GetProcessDefinedStep());
        if (process) {
            G4int tZ = 0;
            G4int tA = 0;
            G4int interactionID = 0;

            trackinfo* parentInfo = dynamic_cast<trackinfo*>(track->GetUserInformation());
            if (parentInfo) {
                tZ = parentInfo->GetTargetZ();
                tA = parentInfo->GetTargetA();
                interactionID = parentInfo->GetInteractionID();
            }

            G4HadronicProcess* hproc = dynamic_cast<G4HadronicProcess*>(process);
            if (hproc) {
                const G4Nucleus* nucleus = hproc->GetTargetNucleus();
                if (nucleus) {
                    interactionID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() * 100000 + track->GetTrackID() * 1000 + track->GetCurrentStepNumber();
                    tZ = nucleus->GetZ_asInt();
                    tA = nucleus->GetA_asInt();
                }
            }

            for (auto sec : *secondaries) {
                G4Track* modifiableTrack = const_cast<G4Track*>(sec);
                if (!modifiableTrack->GetUserInformation()) {
                    modifiableTrack->SetUserInformation(new trackinfo(tZ, tA, interactionID));
                }
            }
        }
    }

    // --------------------------------------------------------------------------
    // 2. Phase Space Scoring on Outer Boundary
    // --------------------------------------------------------------------------
    G4VPhysicalVolume* prePhys = preStepPoint->GetPhysicalVolume();
    G4VPhysicalVolume* postPhys = postStepPoint->GetPhysicalVolume();
    
    if (!prePhys || !postPhys) return;

    // This strictly captures particles the moment they cross the target's exterior boundary into the world.
    if (prePhys->GetLogicalVolume() == pOuterTarget && postPhys->GetLogicalVolume() == pWorld) {
        G4double zLimit = SharedParams::targetHalfLength;
        G4ThreeVector pos = postStepPoint->GetPosition();
    
        // If the absolute Z position is near the end caps, ignore it
        if (std::abs(pos.z()) > (zLimit - 0.01 * mm)) {
            return; // Exited through a cap, do not score
        }
        G4int pdg = track->GetDefinition()->GetPDGEncoding();
        
        // Score protons (2212), neutrons (2112), gammas (22)
        if (pdg == 2212 || pdg == 2112 || pdg == 22) {
            
            G4ThreeVector pos = postStepPoint->GetPosition();
            G4String processName = track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "Primary";
            G4double birthEnergy = track->GetVertexKineticEnergy() / MeV;
            G4int tZ = 0;
            G4int tA = 0;
            G4int interactionID = 0;

            trackinfo* info = dynamic_cast<trackinfo*>(track->GetUserInformation());
            if (info) {
                tZ = info->GetTargetZ();
                tA = info->GetTargetA();
                interactionID = info->GetInteractionID();
            }

            auto analysisManager = G4AnalysisManager::Instance();
            
            analysisManager->FillNtupleIColumn(0, G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID());
            analysisManager->FillNtupleIColumn(1, pdg);
            analysisManager->FillNtupleDColumn(2, track->GetKineticEnergy() / MeV);
            analysisManager->FillNtupleDColumn(3, pos.x() / cm);
            analysisManager->FillNtupleDColumn(4, pos.y() / cm);
            analysisManager->FillNtupleDColumn(5, pos.z() / cm);
            
            G4ThreeVector mom = track->GetMomentumDirection();
            analysisManager->FillNtupleDColumn(6, mom.x());
            analysisManager->FillNtupleDColumn(7, mom.y());
            analysisManager->FillNtupleDColumn(8, mom.z());
            analysisManager->FillNtupleDColumn(9, track->GetGlobalTime() / ns);
            
            analysisManager->FillNtupleDColumn(10, birthEnergy);
            analysisManager->FillNtupleSColumn(11, processName);
            analysisManager->FillNtupleIColumn(12, tZ);
            analysisManager->FillNtupleIColumn(13, tA);
            analysisManager->FillNtupleIColumn(14, interactionID);
            
            analysisManager->AddNtupleRow(0);
        }
        track->SetTrackStatus(fStopAndKill);
    }
}