#pragma once

//===============================================================================================
// std libraries
//===============================================================================================

//===============================================================================================
// geant libraries
//===============================================================================================

#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

#include "G4UserLimits.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmParameters.hh"

#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsINCLXX.hh"

#include "G4StoppingPhysics.hh"

#include "G4IonElasticPhysics.hh"
#include "G4IonPhysics.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4StepLimiter.hh"
#include "G4ProcessManager.hh"

#include "G4SystemOfUnits.hh"

//===============================================================================================
// user libraries
//===============================================================================================

#pragma once

//===============================================================================================
// Class Prototype
//===============================================================================================

class physics : public G4VModularPhysicsList {
    public:
        physics();
        virtual ~physics() override;
            
        virtual void  SetCuts () override;          // setting a range cut value for all particles
        virtual void  ConstructProcess() override;  // construct processes and register them to particles
        virtual void  ConstructParticle() override; // construction of particles

    private:    
        const G4int uVerbose = 0;

        G4VPhysicsConstructor *plEM = nullptr;
        G4VPhysicsConstructor *plDecay = nullptr;
        G4VPhysicsConstructor *plRadioactiveDecay = nullptr;
        G4VPhysicsConstructor *plHadronElastic = nullptr;
        G4VPhysicsConstructor *plHadronInelastic = nullptr;    
        G4VPhysicsConstructor *plEMextra = nullptr;
        G4VPhysicsConstructor *plStoppping = nullptr;
        G4VPhysicsConstructor *plIonElastic = nullptr;
        G4VPhysicsConstructor *plIonInelastic = nullptr;    
};