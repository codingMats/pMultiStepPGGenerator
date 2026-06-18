#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "G4RunManager.hh"
#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4AutoLock.hh"

#include "geometry.h"
#include "runaction.h"
#include "generator.h"

// Structure to hold data for a single incident particle and its entire resulting shower
struct IncidentRecord {
    G4int pdg;
    std::string process;
    G4double creationEnergy;
    G4double globalTime;
    G4double firstBGOHitTime;
    G4double eDepCeBr3;
    std::vector<G4double> eDepBGO;
    
    IncidentRecord() : pdg(0), creationEnergy(0), globalTime(0),firstBGOHitTime(-1.0), eDepCeBr3(0) {}
};

class eventaction: public G4UserEventAction {
    public:
        eventaction(runaction *runaction, generator *generator);
        virtual ~eventaction() override;

        virtual void BeginOfEventAction(const G4Event* uevent) override;
        virtual void EndOfEventAction(const G4Event* uevent) override;

        // Anti-coincidence logic mapping
        void AddIncidentParticle(G4int trackID, G4int pdg, std::string process, G4double energy, G4double time);
        void LinkSecondary(G4int parentTrackID, G4int secondaryTrackID);
        void AddEnergy(G4int trackID, G4int copyNo, G4double edep, G4bool isPrimary, G4double hitTime);
        bool HasIncidentMap(G4int trackID);

    private:
        runaction *urunaction;
        generator *ugenerator;
        G4int eventID;

        // Maps any TrackID (primary or N-th generation secondary) back to the original Incident ID
        std::unordered_map<G4int, G4int> trackToIncidentMap;
        // Stores the physical data for each unique Incident ID
        std::unordered_map<G4int, IncidentRecord> incidentData;
};