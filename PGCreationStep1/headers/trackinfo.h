#pragma once

#include "G4VUserTrackInformation.hh"
#include "G4String.hh"

class trackinfo : public G4VUserTrackInformation {
    public:
        trackinfo(const G4String& reaction, G4int intID)
            : originReaction(reaction), interactionID(intID) {}
        virtual ~trackinfo() override {}

        virtual void Print() const override {}

        G4String GetOriginReaction() const { return originReaction; }
        G4int GetInteractionID() const { return interactionID; }

    private:
        G4String originReaction;
        G4int interactionID;
};