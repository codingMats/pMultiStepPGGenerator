//===============================================================================================
// User libraries
//===============================================================================================

#include "geometry.h"
#include "../../sharedFiles/shared_materials.h"
#include "../../sharedFiles/shared_params.h"
//===============================================================================================
// De/constructor
//===============================================================================================
geometry::geometry(G4String targetMatName) : uTargetMatName(targetMatName) {
    G4NistManager* nist = G4NistManager::Instance();
    matVacuum = nist->FindOrBuildMaterial("G4_Galactic");
    matAir    = nist->FindOrBuildMaterial("G4_AIR");
    matAl     = nist->FindOrBuildMaterial("G4_Al");
    matWater  = nist->FindOrBuildMaterial("G4_WATER");
    
    // Fetch custom materials that were built by SharedMaterials::BuildAllMaterials()
    matTeflon      = G4Material::GetMaterial("teflon");
    matTungsten    = G4Material::GetMaterial("tungsten");
    matCeBr3       = G4Material::GetMaterial("CeBr3");
    matBGO         = G4Material::GetMaterial("BGO");
    matPolystyrene = G4Material::GetMaterial("polystyrene");
    matCaO         = G4Material::GetMaterial("calcium_oxide");
}
geometry::~geometry() {    
    if (rotDetector) {
        delete rotDetector;
    }
    
    for (auto vis : visAttributes) {
        delete vis;
    }
    visAttributes.clear();
}

//===============================================================================================
// define simulation geometry
//===============================================================================================

G4VPhysicalVolume* geometry::Construct() {
    G4double margin = 1.5 * cm;
    G4double worldRadius = SharedParams::targetRadius + margin;
    G4double worldHL = SharedParams::targetHalfLength + 5.0 * cm; 

    G4Tubs* sWorld = new G4Tubs("sWorld", 0, worldRadius, worldHL, 0.*deg, 360.*deg);
    lWorld = new G4LogicalVolume(sWorld, matVacuum, "lWorld");
    G4VPhysicalVolume* pWorld = new G4PVPlacement(nullptr, {0, 0, 0}, lWorld, "pWorld", 0, false, 0, true);

    G4Material* targetMaterial = SharedMaterials::GetTargetMaterial(uTargetMatName);

    G4Tubs* sTarget = new G4Tubs("sTarget", 0, SharedParams::targetRadius, SharedParams::targetHalfLength, 0.*deg, 360.*deg);
    G4LogicalVolume* lTarget = new G4LogicalVolume(sTarget, targetMaterial, "lTarget");
    
    G4VisAttributes *vTarget = new G4VisAttributes(true, G4Colour(0.7, 0.7, 0.7, 1.0));
    vTarget->SetForceSolid(true);
    visAttributes.push_back(vTarget);
    lTarget->SetVisAttributes(vTarget);

    // Placed at origin. Beam will be offset by -5cm.
    new G4PVPlacement(nullptr, {0., 0., 0.}, lTarget, "pOuterTarget", lWorld, false, 0, true);

    return pWorld;
}

void geometry::ConstructSDandField(){
}