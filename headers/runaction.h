#pragma once

//===============================================================================================
// std libraries
//===============================================================================================

//===============================================================================================
// geant libraries
//===============================================================================================

#include "G4RunManager.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"

#include "G4AnalysisManager.hh"

//===============================================================================================
// user libraries
//===============================================================================================

#include "run.h"
#include "geometry.h"

class G4Run;

//===============================================================================================
// class prototype
//===============================================================================================

class runaction: public G4UserRunAction {
    public:
        runaction();
        virtual ~runaction() override;
        
        virtual G4Run* GenerateRun() override;
        virtual void BeginOfRunAction(const G4Run* urun) override;
        virtual void EndOfRunAction(const G4Run* urun) override;
};