#include "stepaction.h"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4EventManager.hh"
#include "G4NistManager.hh"

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
            G4String processname = process->GetProcessName();
            G4String reactionString = "Unknown";
            G4int interactionID = 0;

            // Inherit information from parent if it exists
            trackinfo* parentInfo = dynamic_cast<trackinfo*>(track->GetUserInformation());
            if (parentInfo) {
                reactionString = parentInfo->GetOriginReaction();
                interactionID = parentInfo->GetInteractionID();
            }

            // Check if this step is a hadronic inelastic interaction
            G4HadronicProcess* hproc = dynamic_cast<G4HadronicProcess*>(process);
            if (hproc) {
                const G4Nucleus* nucleus = hproc->GetTargetNucleus();
                if (nucleus) {
                    // Create a unique ID for this specific nuclear interaction
                    interactionID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() * 100000 + track->GetTrackID() * 1000 + track->GetCurrentStepNumber();

                    G4int targetZ = nucleus->GetZ_asInt();
                    G4int targetA = nucleus->GetA_asInt();
                    G4String elemSymbol = G4NistManager::Instance()->FindOrBuildElement(targetZ)->GetSymbol();
                    G4String targetStr = elemSymbol + std::to_string(targetA);

                    std::map<G4String, G4int> productCounts;
                    for (auto sec : *secondaries) {
                        G4String pName = sec->GetDefinition()->GetParticleName();
                        
                        productCounts[pName]++;
                        }

                    G4String projName = track->GetDefinition()->GetParticleName();
                    if (projName == "proton") projName = "p"; 

                    reactionString = projName + " + " + targetStr + " -> ";
                    G4int countOutput = 0;
                    for (auto const& [name, count] : productCounts) {
                        if (countOutput > 0) reactionString += " + ";
                        if (count == 1) reactionString += name;
                        else reactionString += std::to_string(count) + name;
                        countOutput++;
                    }
                }
            } else if (!parentInfo) {
                reactionString = processname; // Fallback for non-hadronic primaries
            }

            // Apply the custom info to all secondaries created in this step
            for (auto sec : *secondaries) {
                G4Track* modifiableTrack = const_cast<G4Track*>(sec);
                if (!modifiableTrack->GetUserInformation()) {
                    modifiableTrack->SetUserInformation(new trackinfo(reactionString, interactionID));
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

    if (prePhys->GetLogicalVolume() == pOuterTarget && postPhys->GetLogicalVolume() == pWorld) {
        G4int pdg = track->GetDefinition()->GetPDGEncoding();
        
        // Score protons (2212), neutrons (2112), gammas (22)
        if (pdg == 2212 || pdg == 2112 || pdg == 22) {
            
            G4ThreeVector pos = postStepPoint->GetPosition();
            G4double radius = std::sqrt(pos.x()*pos.x() + pos.y()*pos.y());
            
            if (radius >= 1.249 * cm) {
                
                // Extract track meta-data
                G4String processName = track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "Primary";
                G4double birthEnergy = track->GetVertexKineticEnergy() / MeV;
                G4String reaction = "Primary";
                G4int interactionID = 0;

                trackinfo* info = dynamic_cast<trackinfo*>(track->GetUserInformation());
                if (info) {
                    reaction = info->GetOriginReaction();
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
                
                // Added parameters
                analysisManager->FillNtupleDColumn(10, birthEnergy);
                analysisManager->FillNtupleSColumn(11, processName);
                analysisManager->FillNtupleSColumn(12, reaction);
                analysisManager->FillNtupleIColumn(13, interactionID);
                
                analysisManager->AddNtupleRow(0);
            }
        }
    }
}