//===============================================================================================
// std libraries
//===============================================================================================

//===============================================================================================
// geant libraries
//===============================================================================================

//===============================================================================================
// user libraries
//===============================================================================================

#include "physics.h"

//===============================================================================================
// de/constructor
//===============================================================================================

physics::physics():
G4VModularPhysicsList(){
    SetVerboseLevel(uVerbose);

    plEM = new G4EmStandardPhysics_option4(uVerbose);
    plEMextra = new G4EmExtraPhysics(uVerbose);

    plDecay = new G4DecayPhysics(uVerbose);
    plRadioactiveDecay = new G4RadioactiveDecayPhysics(uVerbose);

    plHadronElastic = new G4HadronElasticPhysicsHP(uVerbose);
    plHadronInelastic = new G4HadronPhysicsQGSP_BIC_HP(uVerbose);
    // Arguments: (Name, QuasiElastic, NeutronHP, FTFP)
    // plHadronInelastic = new G4HadronPhysicsINCLXX("hInelastic INCLXX", true, true, false);

    plStoppping = new G4StoppingPhysics(uVerbose);

    plIonElastic = new G4IonElasticPhysics(uVerbose);
    plIonInelastic = new G4IonPhysics(uVerbose);
}

physics::~physics() {
    delete plEM;
    delete plEMextra;
    delete plDecay;
    delete plRadioactiveDecay;
    delete plHadronElastic;
    delete plHadronInelastic;
    delete plStoppping;
    delete plIonElastic;
    delete plIonInelastic;
}

//===============================================================================================
// particles constructors
//===============================================================================================

void physics::ConstructParticle(){
    G4BosonConstructor      pcBosons;       pcBosons.ConstructParticle();
    G4LeptonConstructor     pcLeptons;      pcLeptons.ConstructParticle();
    G4MesonConstructor      pcMesons;       pcMesons.ConstructParticle();
    G4BaryonConstructor     pcBaryons;      pcBaryons.ConstructParticle();
    G4IonConstructor        pcIons;         pcIons.ConstructParticle();
    G4ShortLivedConstructor pcShortLived;   pcShortLived.ConstructParticle();
}

//===============================================================================================
// processes constructors
//===============================================================================================

void physics::ConstructProcess(){        
    AddTransportation(); 

    if (plEM != nullptr)               plEM->ConstructProcess();
    if (plEMextra != nullptr)          plEMextra->ConstructProcess();
    if (plDecay != nullptr)            plDecay->ConstructProcess();
    if (plRadioactiveDecay != nullptr) plRadioactiveDecay->ConstructProcess();
    if (plHadronElastic != nullptr)    plHadronElastic->ConstructProcess();
    if (plHadronInelastic != nullptr)  plHadronInelastic->ConstructProcess();
    if (plStoppping != nullptr)        plStoppping->ConstructProcess();
    if (plIonElastic != nullptr)       plIonElastic->ConstructProcess();
    if (plIonInelastic != nullptr)     plIonInelastic->ConstructProcess();

    //===============================================================================
    // Electromagnetic Processes
    //===============================================================================
        
    G4EmParameters* params = G4EmParameters::Instance();    
    params->SetFluo(true);
    params->SetAuger(false);
    params->SetPixe(false);

    // Set Region
    params->SetDeexActiveRegion("rtarget", true, false, false);   // <region name>, <fluo>, <auger>, <pixe>

    // PIXE & Flourescence cross section (Empirical, ECPSSR_FormFactor, ECPSSR_Analytical, ECPSSR_ANSTO)
    params->SetPIXECrossSectionModel("ECPSSR_ANSTO");
    params->SetPIXEElectronCrossSectionModel("ECPSSR_ANSTO");

    params->SetDeexcitationIgnoreCut(true);
}

/*-----------------*/
/* simulation Cuts */
/*-----------------*/

void physics::SetCuts()
{
    // default cuts of 1.*mm
    SetCutsWithDefault();
  
    // user cuts for world    
    G4double ucut = 1.*mm;
    
    SetCutValue(ucut, "e-");
    SetCutValue(ucut, "e+");
    SetCutValue(ucut, "proton");
}
