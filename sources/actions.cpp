
//===============================================================================================
// std libraries
//===============================================================================================

//===============================================================================================
// geant libraries
//===============================================================================================

//===============================================================================================
// user libraries
//===============================================================================================

#include "actions.h"

//===============================================================================================
// build simulation for the master thread
//===============================================================================================

void actions::BuildForMaster() const{
    runaction *uRunAction = new runaction();
    SetUserAction(uRunAction);
}

//===============================================================================================
// build simulation for general threads
//===============================================================================================

void actions::Build() const
{        
    generator   *uGenerator     = new generator();
    runaction   *uRunAction     = new runaction();
    eventaction *uEventAction   = new eventaction(uRunAction, uGenerator);
    stepaction  *uStepAction    = new stepaction(uEventAction);
    stackaction *uStackAction   = new stackaction();

    SetUserAction(uGenerator);
    SetUserAction(uRunAction);
    SetUserAction(uEventAction);
    SetUserAction(uStepAction);
    SetUserAction(uStackAction);
}

