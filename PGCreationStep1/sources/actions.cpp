#include "actions.h"

actions::actions(G4String material, G4String step) : uMaterial(material), uStep(step) {}

void actions::BuildForMaster() const{
    runaction *uRunAction = new runaction(uMaterial, uStep);
    SetUserAction(uRunAction);
}

void actions::Build() const {        
    // Step 1 Generator takes no arguments
    generator   *uGenerator     = new generator(); 

    runaction   *uRunAction     = new runaction(uMaterial, uStep);
    eventaction *uEventAction   = new eventaction(uRunAction, uGenerator);
    stepaction  *uStepAction    = new stepaction(uEventAction);
    stackaction *uStackAction   = new stackaction();

    SetUserAction(uGenerator); 
    SetUserAction(uRunAction);
    SetUserAction(uEventAction); 
    SetUserAction(uStepAction); 
    SetUserAction(uStackAction);
}