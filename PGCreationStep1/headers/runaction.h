#pragma once
#include "G4RunManager.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4String.hh"
#include "run.h"
#include "geometry.h"

class G4Run;

class runaction: public G4UserRunAction {
    public:
        runaction(G4String material, G4String step);
        virtual ~runaction() override;
        
        virtual G4Run* GenerateRun() override;
        virtual void BeginOfRunAction(const G4Run* urun) override;
        virtual void EndOfRunAction(const G4Run* urun) override;
    private:
        G4String uMaterial;
        G4String uStep;
};