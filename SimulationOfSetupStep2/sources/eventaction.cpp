#include "eventaction.h"

namespace {
    G4Mutex threadLocker = G4MUTEX_INITIALIZER;
}

eventaction::eventaction(runaction *runaction, generator *generator):
G4UserEventAction(), urunaction(runaction), ugenerator(generator) {
}

eventaction::~eventaction(){
}

void eventaction::BeginOfEventAction(const G4Event* uEvent){
    eventID = uEvent->GetEventID();
    
    // Clear the tracking maps and force memory deallocation
    std::unordered_map<G4int, G4int>().swap(trackToIncidentMap);
    std::unordered_map<G4int, IncidentRecord>().swap(incidentData);
}



void eventaction::EndOfEventAction(const G4Event* uEvent) {
    G4int evtShow = 1e7;
    if(eventID % evtShow == 0) {
        G4cout << "event: " << eventID << G4endl;
    }

    auto analysisManager = G4AnalysisManager::Instance();

    for (const auto& pair : incidentData) {
        const IncidentRecord& record = pair.second;
        
        G4double totalBGO = 0;
        for (G4double bgoEdep : record.eDepBGO) totalBGO += bgoEdep;
        if (record.eDepCeBr3 == 0 && totalBGO == 0) continue;

        analysisManager->FillNtupleIColumn(0, 0, eventID);
        analysisManager->FillNtupleIColumn(0, 1, record.pdg);
        analysisManager->FillNtupleSColumn(0, 2, record.process);
        analysisManager->FillNtupleDColumn(0, 3, record.creationEnergy);
        analysisManager->FillNtupleDColumn(0, 4, record.globalTime);
        analysisManager->FillNtupleDColumn(0, 5, record.eDepCeBr3);
        
       // Ensure 8 columns are filled for the NTuple, padding with 0 if necessary
        for (size_t i = 0; i < 8; ++i) {
            G4double energy = (i < record.eDepBGO.size()) ? record.eDepBGO[i] : 0.0;
            analysisManager->FillNtupleDColumn(0, 6 + i, energy);
        }

        // Fill the new BGO time column at index 14
        analysisManager->FillNtupleDColumn(0, 14, record.firstBGOHitTime);

        analysisManager->AddNtupleRow(0);
    }
}

void eventaction::AddIncidentParticle(G4int trackID, G4int pdg, std::string process, G4double energy, G4double time) {
    // Traverse the ancestry tree to find the root particle
    G4int rootID = trackID;
    while (trackToIncidentMap.find(rootID) != trackToIncidentMap.end() && trackToIncidentMap[rootID] != rootID) {
        rootID = trackToIncidentMap[rootID];
    }

    // Set the root map
    trackToIncidentMap[trackID] = rootID;
    
    // Only create a new physical record if the root isn't already tracked
    if (incidentData.find(rootID) == incidentData.end()) {
        IncidentRecord record;
        record.pdg = pdg;
        record.process = process;
        record.creationEnergy = energy;
        record.globalTime = time;
        
        incidentData[rootID] = record;
    }
}

void eventaction::LinkSecondary(G4int parentTrackID, G4int secondaryTrackID) {
    // If the parent has an incident root, link the secondary to that root.
    // Otherwise, link the secondary to the parent directly, establishing an ancestry chain.
    if (trackToIncidentMap.find(parentTrackID) != trackToIncidentMap.end()) {
        trackToIncidentMap[secondaryTrackID] = trackToIncidentMap[parentTrackID];
    }
}

bool eventaction::HasIncidentMap(G4int trackID) {
    return trackToIncidentMap.find(trackID) != trackToIncidentMap.end();
}
void eventaction::AddEnergy(G4int trackID, G4int copyNo, G4double edep, G4bool isPrimary, G4double hitTime) {
    if (trackToIncidentMap.find(trackID) == trackToIncidentMap.end()) return;
    
    G4int incidentID = trackToIncidentMap[trackID];
    
    if (isPrimary) {
        incidentData[incidentID].eDepCeBr3 += edep;
    } else {
        G4int bgoIndex = copyNo - 1; 
        if (bgoIndex >= 0) {
            // Dynamically resize vector if a new BGO copy number is found
            if (bgoIndex >= incidentData[incidentID].eDepBGO.size()) {
                incidentData[incidentID].eDepBGO.resize(bgoIndex + 1, 0.0);
            }
            incidentData[incidentID].eDepBGO[bgoIndex] += edep;

            // Track the earliest hit time in the BGO
            if (incidentData[incidentID].firstBGOHitTime < 0.0 || hitTime < incidentData[incidentID].firstBGOHitTime) {
                incidentData[incidentID].firstBGOHitTime = hitTime;
            }
        }
    }
}