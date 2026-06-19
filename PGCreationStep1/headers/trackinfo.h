#pragma once
#include "G4VUserTrackInformation.hh"

class trackinfo : public G4VUserTrackInformation {
    public:
        trackinfo(G4int tz, G4int ta, G4int intID)
            : targetZ(tz), targetA(ta), interactionID(intID) {}
        virtual ~trackinfo() override {}
        virtual void Print() const override {}

        G4int GetTargetZ() const { return targetZ; }
        G4int GetTargetA() const { return targetA; }
        G4int GetInteractionID() const { return interactionID; }

    private:
        G4int targetZ;
        G4int targetA;
        G4int interactionID;
};