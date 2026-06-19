#pragma once

//===============================================================================================
// std libraries
//===============================================================================================
#include <vector>

//===============================================================================================
// geant libraries
//===============================================================================================

#include "G4VUserDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"

#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4ProductionCutsTable.hh"
#include "G4UserLimits.hh"
#include "G4SystemOfUnits.hh"

//===============================================================================================
// class prototype
//===============================================================================================
class geometry : public G4VUserDetectorConstruction {
    public:
        geometry(G4String targetMatName);
        virtual ~geometry() override;

        virtual G4VPhysicalVolume* Construct() override;
        virtual void ConstructSDandField() override;

        G4LogicalVolume* getLogicalWorld() const { return lWorld; }
        std::vector<G4LogicalVolume*> getLogicalPrimary() const { return lPrimary; }
        std::vector<G4LogicalVolume*> getLogicalACShield() const { return lACShield; }
        G4int getNDetectors() const { return nDetectors; }

    private:
        G4String uTargetMatName; // ADD THIS

        // Material pointers
        G4Material *matVacuum; G4Material *matAir; G4Material *matAl;
        G4Material *matTeflon; G4Material *matTungsten; G4Material *matCeBr3;
        G4Material *matBGO; G4Material *matPolystyrene; G4Material *matCaO; G4Material *matWater;
        
        G4LogicalVolume *lWorld; G4VPhysicalVolume *pWorld;
        std::vector<G4LogicalVolume*> lPrimary; std::vector<G4LogicalVolume*> lACShield; std::vector<G4LogicalVolume*> lCollimator;
        G4int nDetectors; G4RotationMatrix* rotDetector; std::vector<G4VisAttributes*> visAttributes;
};