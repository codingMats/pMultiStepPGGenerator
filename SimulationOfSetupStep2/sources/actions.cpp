#include "actions.h"

actions::actions(G4String material, G4String step) : uMaterial(material), uStep(step) {}

void actions::BuildForMaster() const {
    // This forces the Master Thread to read the merged file (without appending _t0)
    // and loads it into your static sharedPhaseSpaceData vector.
    generator tempGen(uMaterial);

    runaction *uRunAction = new runaction(uMaterial, uStep);
    SetUserAction(uRunAction);
}

void actions::Build() const {        
    // In Step 1, generator takes no string, in Step 2 it takes uMaterial. 
    // If your Step 1 generator doesn't take an argument, it's safe to just use 'new generator()' here for Step 1 
    // and 'new generator(uMaterial)' for Step 2. Assuming you update Step 2 generator per the instructions below:

    generator   *uGenerator     = new generator(uMaterial);


    runaction   *uRunAction     = new runaction(uMaterial, uStep);
    eventaction *uEventAction   = new eventaction(uRunAction, uGenerator);
    stepaction  *uStepAction    = new stepaction(uEventAction);
    stackaction *uStackAction   = new stackaction();

    SetUserAction(uGenerator); SetUserAction(uRunAction);
    SetUserAction(uEventAction); SetUserAction(uStepAction); SetUserAction(uStackAction);
}