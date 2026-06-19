

#pragma once
#include "G4VUserActionInitialization.hh"
#include "generator.h"
#include "runaction.h"
#include "eventaction.h"
#include "stepaction.h"
#include "stackaction.h"
#include "G4String.hh"

class actions : public G4VUserActionInitialization {
    public:
        actions(G4String material, G4String step);
        virtual void Build() const override;
        virtual void BuildForMaster() const override;
    private:
        G4String uMaterial;
        G4String uStep;
};