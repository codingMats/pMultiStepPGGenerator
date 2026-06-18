//===============================================================================================
// User libraries
//===============================================================================================

#include "geometry.h"

//===============================================================================================
// De/constructor
//===============================================================================================

geometry::geometry() {
    //-------------------------------------------------
    // Native NIST Materials
    //-------------------------------------------------  
    matVacuum   = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    matAir      = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    matAl       = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
    matWater = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");

    //-------------------------------------------------
    // Custom Materials
    //-------------------------------------------------
    
    // Teflon
    matTeflon = new G4Material("teflon", 2.2*g/cm3, 2);
    matTeflon->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 2);
    matTeflon->AddElement(G4NistManager::Instance()->FindOrBuildElement("F"), 4);

    // Tungsten
    matTungsten = new G4Material("tungsten", 19.25*g/cm3, 1);
    matTungsten->AddElement(G4NistManager::Instance()->FindOrBuildElement("W"), 1);

    // CeBr3
    matCeBr3 = new G4Material("CeBr3", 5.10*g/cm3, 2);
    matCeBr3->AddElement(G4NistManager::Instance()->FindOrBuildElement("Ce"), 1);
    matCeBr3->AddElement(G4NistManager::Instance()->FindOrBuildElement("Br"), 3);

    // BGO
    matBGO = new G4Material("BGO", 7.13*g/cm3, 3);
    matBGO->AddElement(G4NistManager::Instance()->FindOrBuildElement("Bi"), 4);
    matBGO->AddElement(G4NistManager::Instance()->FindOrBuildElement("Ge"), 3);
    matBGO->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 12);

    //Polystyrene
    matPolystyrene = new G4Material("polystyrene", 1.05*g/cm3, 2);
    matPolystyrene->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 8);
    matPolystyrene->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 8);
    
    // Calcium Oxide
    matCaO = new G4Material("calcium_oxide", 1.22*g/cm3, 2);
    matCaO->AddElement(G4NistManager::Instance()->FindOrBuildElement("Ca"), 1);
    matCaO->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 1);
    
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

G4VPhysicalVolume* geometry::Construct(){

    lPrimary.clear();
    lACShield.clear();
    lCollimator.clear();
    nDetectors = 0;

    //-------------------------------------------------
    // World Definition
    //-------------------------------------------------  
    const G4double worldSize[3] = {1.*m, 1.*m, 1.*m};
    G4Box* sWorld = new G4Box("sWorld", .5*worldSize[0], .5*worldSize[1], .5*worldSize[2]);
    lWorld = new G4LogicalVolume(sWorld, matVacuum, "lWorld");
    G4VPhysicalVolume* pWorld = new G4PVPlacement(nullptr, {0, 0, 0}, lWorld, "pWorld", 0, false, 0, true);


    //-------------------------------------------------
    // Primary Detector
    //-------------------------------------------------
    G4double dPrimary = 3.81 * cm;
    G4double lengthPrimary = 7.62 * cm;


    const G4double tTeflonFront = 0.5 * mm;
    const G4double tTeflonLat   = 0.5 * mm;
    
    const G4double tAirFront    = 2.45 * mm;
    const G4double tAirLat      = 2.25 * mm;
    
    const G4double tAlFront     = 0.8 * mm;
    const G4double tAlLat       = 1.25 * mm;

    // Calculate hierarchical radii
    G4double rCore   = 0.5 * dPrimary;
    G4double rTeflon = rCore + tTeflonLat;
    G4double rAir    = rTeflon + tAirLat;
    G4double rAl     = rAir + tAlLat;

    // Calculate hierarchical lengths
    G4double lCore   = lengthPrimary;
    G4double lTeflon = lCore + (2.0 * tTeflonFront); // Multiplied by 2 to cover front and back
    G4double lAir    = lTeflon + (2.0 * tAirFront);  // Multiplied by 2 to cover front and back
    G4double lAl     = lAir + tAlFront;              // Additive only on the front

    // --- Primary Mother Volume: Aluminum Casing ---
    G4Tubs* sAlCase = new G4Tubs("sAlCase", 0.*cm, rAl, 0.5 * lAl, 0.*deg, 360.*deg);
    G4LogicalVolume* lAlCase = new G4LogicalVolume(sAlCase, matAl, "lAlCase");
    G4VisAttributes *vAlCase = new G4VisAttributes(true, G4Colour(0.7, 0.7, 0.7, 1));
    vAlCase->SetForceSolid(true);
    visAttributes.push_back(vAlCase);
    lAlCase->SetVisAttributes(vAlCase);

    // --- Daughter 1: Air ---
    G4Tubs* sAir = new G4Tubs("sAir", 0.*cm, rAir, 0.5 * lAir, 0.*deg, 360.*deg);
    G4LogicalVolume* lAirVol = new G4LogicalVolume(sAir, matAir, "lAir");
    G4VisAttributes* vAir = new G4VisAttributes(false); // Change to this
    visAttributes.push_back(vAir);                      // Add this
    lAirVol->SetVisAttributes(vAir);
    
    // CORRECTED: Shift backward by half the front thickness
    new G4PVPlacement(nullptr, {0., 0., -tAlFront / 2.0}, lAirVol, "pAir", lAlCase, false, 0, true);

    // --- Daughter 2: Teflon ---
    G4Tubs* sTeflon = new G4Tubs("sTeflon", 0.*cm, rTeflon, 0.5 * lTeflon, 0.*deg, 360.*deg);
    G4LogicalVolume* lTeflonVol = new G4LogicalVolume(sTeflon, matTeflon, "lTeflon");
    G4VisAttributes* vTeflon = new G4VisAttributes(true, G4Colour(1.0, 1.0, 1.0, 1)); // Change to this
    visAttributes.push_back(vTeflon);                                                 // Add this
    lTeflonVol->SetVisAttributes(vTeflon); 

    new G4PVPlacement(nullptr, {0., 0., 0.*cm}, lTeflonVol, "pTeflon", lAirVol, false, 0, true);

    // --- Daughter 3: CeBr3 Core ---
    G4Tubs* sCeBr3 = new G4Tubs("sCeBr3", 0.*cm, rCore, 0.5 * lCore, 0.*deg, 360.*deg);
    G4LogicalVolume* lCeBr3 = new G4LogicalVolume(sCeBr3, matCeBr3, "lCeBr3");
    G4VisAttributes *vPrimary = new G4VisAttributes(true, G4Colour(0.0, 0.5, 1.0, 1));
    vPrimary->SetForceSolid(true);
    visAttributes.push_back(vPrimary);
    lCeBr3->SetVisAttributes(vPrimary);

    new G4PVPlacement(nullptr, {0., 0., 0.*cm}, lCeBr3, "pCeBr3", lTeflonVol, false, nDetectors, true);

    lPrimary.push_back(lCeBr3); 
    nDetectors++;

    //-------------------------------------------------
    // Anti Coincidence Shield (BGO Ring)
    //-------------------------------------------------
    //Aluminium Ring
    G4double dBGO = 3 * cm;
    G4double lBGO = 10.0 * cm;
    G4int nBGO = 8;
    
    G4double bgoClearance = 0.1 * mm;
    G4double bgoAlSideThick = 1.25 * mm;
    G4double bgoAlFrontThick = 0.8 * mm;
    G4double bgoAirFrontGap = 2.45 * mm;

    // The center of the BGO ring must be pushed outward to make room for the inner aluminum wall (bgoAlSideThick) and the clearance gap.
    G4double ringRadius = rAl + (2.0 * bgoClearance) + bgoAlSideThick + (.5 * dBGO); 

    // The inner radius of the BGO air cavity now perfectly aligns outside the primary detector
    G4double bgoInnerRadius = ringRadius - (.5 * dBGO) - bgoClearance;
    G4double bgoOuterRadius = ringRadius + (.5 * dBGO) + bgoClearance;

    // They do not include the thickness of the front cover itself.
    G4double sideWallLength = lBGO + bgoAirFrontGap;
    
    // Offset for the side walls so their back aligns with the back of the BGO (-0.5 * lBGO)
    G4double sideWallZOffset = bgoAirFrontGap / 2.0; 

    // Define the absolute local Z position where both the AC and CeBr3 front faces will align
    G4double bgoFrontZ = 0.5 * lBGO + bgoAirFrontGap + bgoAlFrontThick;

    //-------------------------------------------------
    // Create the Main Detector Assembly Volume
    //-------------------------------------------------
    G4double detectorRadius = bgoOuterRadius + bgoAlSideThick + 1.0 * mm;
    
    // Set half-length based on the furthest Z limits (bgoFrontZ and the back of the BGO crystal)
    G4double detectorHalfLength = std::max(bgoFrontZ, 0.5 * lBGO) + 1.0 * mm; 
    
    G4Tubs* sDetectorAssembly = new G4Tubs("sDetectorAssembly", 0.*cm, detectorRadius, detectorHalfLength, 0.*deg, 360.*deg);
    G4LogicalVolume* lDetectorAssembly = new G4LogicalVolume(sDetectorAssembly, matVacuum, "lDetectorAssembly");
    G4VisAttributes* vDetAssembly = new G4VisAttributes(false); // Change to this
    visAttributes.push_back(vDetAssembly);                      // Add this
    lDetectorAssembly->SetVisAttributes(vDetAssembly);
    
    //-------------------------------------------------
    // Place Components into the Assembly
    //-------------------------------------------------
    
    // Place Primary Detector (CeBr3) aligned to the front face
    G4double primaryZ = bgoFrontZ - 0.5 * lAl;
    new G4PVPlacement(nullptr, {0., 0., primaryZ}, lAlCase, "pAlCase", lDetectorAssembly, false, 0, true);
    
    // Create and place BGO crystals at Z = 0 inside the assembly
    G4Tubs* sBGO = new G4Tubs("sBGO", 0.*cm, .5 * dBGO, .5 * lBGO, 0.*deg, 360.*deg);
    G4LogicalVolume* lBGOVol = new G4LogicalVolume(sBGO, matBGO, "lBGO");
    
    G4VisAttributes *vBGO = new G4VisAttributes(true, G4Colour(1.0, 0.5, 0.0, 1));
    vBGO->SetForceSolid(true);
    visAttributes.push_back(vBGO);
    lBGOVol->SetVisAttributes(vBGO);
    
    for (int i = 0; i < nBGO; ++i) {
        G4double angle = i * (360. * deg / nBGO);
        G4double xPos = ringRadius * std::cos(angle);
        G4double yPos = ringRadius * std::sin(angle);
        
        new G4PVPlacement(nullptr, {xPos, yPos, 0.*cm}, lBGOVol, "pBGO", lDetectorAssembly, false, nDetectors, true);
        nDetectors++;
    }
    lACShield.push_back(lBGOVol);
    
    // Place BGO Casings
    G4VisAttributes* vCasingAl = new G4VisAttributes(true, G4Colour(0.4, 0.4, 0.4, 1));
    vCasingAl->SetForceSolid(true);
    visAttributes.push_back(vCasingAl);
    
    // Outer Side Wall
    G4Tubs* sOuterAl = new G4Tubs("sOuterAl", bgoOuterRadius, bgoOuterRadius + bgoAlSideThick, .5 * sideWallLength, 0.*deg, 360.*deg);
    G4LogicalVolume* lOuterAl = new G4LogicalVolume(sOuterAl, matAl, "lOuterAl");
    lOuterAl->SetVisAttributes(vCasingAl);
    new G4PVPlacement(nullptr, {0., 0., sideWallZOffset}, lOuterAl, "pOuterAl", lDetectorAssembly, false, 0, true);
    
    // Inner Side Wall
    G4Tubs* sInnerAl = new G4Tubs("sInnerAl", bgoInnerRadius - bgoAlSideThick, bgoInnerRadius, .5 * sideWallLength, 0.*deg, 360.*deg);
    G4LogicalVolume* lInnerAl = new G4LogicalVolume(sInnerAl, matAl, "lInnerAl");
    lInnerAl->SetVisAttributes(vCasingAl);
    new G4PVPlacement(nullptr, {0., 0., sideWallZOffset}, lInnerAl, "pInnerAl", lDetectorAssembly, false, 0, true);
    
    // Front Cover
    G4double frontCoverZCenter = (.5 * lBGO) + bgoAirFrontGap + (.5 * bgoAlFrontThick);
    G4Tubs* sFrontAl = new G4Tubs("sFrontAl", bgoInnerRadius - bgoAlSideThick, bgoOuterRadius + bgoAlSideThick, .5 * bgoAlFrontThick, 0.*deg, 360.*deg);
    G4LogicalVolume* lFrontAl = new G4LogicalVolume(sFrontAl, matAl, "lFrontAl");
    lFrontAl->SetVisAttributes(vCasingAl);
    new G4PVPlacement(nullptr, {0., 0., frontCoverZCenter}, lFrontAl, "pFrontAl", lDetectorAssembly, false, 0, true);
    
    //-------------------------------------------------
    // 5. Tungsten Collimator
    //-------------------------------------------------
    G4double collX = 12.5 * cm;
    G4double collY = 9.0 * cm;
    G4double collZ = 7.0 * cm;
    G4double collGap = 12.0 * mm;
    
    // The mother volume encapsulates the thickness (X), height (Y), and total width including the gap (Z).
    G4Box* sCollCase = new G4Box("sCollCase", collX / 2.0, collY / 2.0, (2.0 * collZ + collGap) / 2.0);
    G4LogicalVolume* lCollCase = new G4LogicalVolume(sCollCase, matVacuum, "lCollCase");
    G4VisAttributes* vCollCase = new G4VisAttributes(false); // Change to this
    visAttributes.push_back(vCollCase);                      // Add this
    lCollCase->SetVisAttributes(vCollCase);

    G4Box* sCollimator = new G4Box("sCollimator", collX / 2.0, collY / 2.0, collZ / 2.0);
    G4LogicalVolume* lCollimatorVol = new G4LogicalVolume(sCollimator, matTungsten, "lCollimator");
    
    G4VisAttributes *vCollimator = new G4VisAttributes(true, G4Colour(0.83, 0.83, 0.83, 1.0));
    vCollimator->SetForceSolid(true);
    visAttributes.push_back(vCollimator);
    lCollimatorVol->SetVisAttributes(vCollimator);

    // Place the tungsten blocks INSIDE lCollCase, separated along the Z-axis to form the slit.
    G4double collZOffset = (collGap / 2.0) + (collZ / 2.0);
    new G4PVPlacement(nullptr, {0.*cm, 0.*cm, -collZOffset}, lCollimatorVol, "pCollimatorLeft", lCollCase, false, nDetectors++, true);
    new G4PVPlacement(nullptr, {0.*cm, 0.*cm, collZOffset}, lCollimatorVol, "pCollimatorRight", lCollCase, false, nDetectors++, true);
    

    //-------------------------------------------------
    // 6. Target
    //--------------------------------------------------
    G4double targetZ = 36.4*mm;
    G4double targetY = 125.4*mm;
    G4double targetX = 80.0*mm;
    
    G4double wallThickness = 1*mm;
    
    // Outer Target (Polystyrene shell)
    G4Box* sOuterTarget = new G4Box("sOuterTarget", 0.5*targetX, 0.5*targetY, 0.5*targetZ);
    G4LogicalVolume* lOuterTarget = new G4LogicalVolume(sOuterTarget, matPolystyrene, "lOuterTarget");
    
    // Inner Target (CaO core)
    G4Box* sInnerTarget = new G4Box("sInnerTarget", 0.5*(targetX - 2*wallThickness), 0.5*(targetY - 2*wallThickness), 0.5*(targetZ - 2*wallThickness));
    G4LogicalVolume* lInnerTarget = new G4LogicalVolume(sInnerTarget, matCaO, "lInnerTarget");
    
    // Visualization Attributes
    G4VisAttributes* vInnerTarget = new G4VisAttributes(true, G4Colour(1.0, 0.0, 0.0, 1.0)); // Change to this
    visAttributes.push_back(vInnerTarget);                                                   // Add this
    lInnerTarget->SetVisAttributes(vInnerTarget);
    
    // Adjusted alpha channel to 0.3 for transparency
    G4VisAttributes* vOuterTarget = new G4VisAttributes(true, G4Colour(0.0, 1.0, 0.0, 0.3)); 
    vOuterTarget->SetForceSolid(true);
    visAttributes.push_back(vOuterTarget);
    lOuterTarget->SetVisAttributes(vOuterTarget);
    
    // 1. Place inner target INSIDE the outer target
    new G4PVPlacement(nullptr, {0., 0., 0.*mm}, lInnerTarget, "pInnerTarget", lOuterTarget, false, 0, true);
    
    //-------------------------------------------------
    // Global Centering and Placement Calculations (X-Axis)
    //-------------------------------------------------
    G4double gapTargetToColl = 12.2 * cm;
    G4double gapCollToDetector = 9.5 * cm;

    G4double lenTargetX = targetX; 
    G4double lenCollX = collX;     
    G4double lenDetectorX = bgoFrontZ + (0.5 * lBGO); 

    // Total length of the setup along X
    G4double totalLengthX = lenTargetX + gapTargetToColl + lenCollX + gapCollToDetector + lenDetectorX;
    G4double startX = -totalLengthX / 2.0;

    // Calculate global X coordinates
    G4double posX_Target = startX + (lenTargetX / 2.0);
    G4double posX_Coll = (startX + lenTargetX) + gapTargetToColl + (lenCollX / 2.0);
    G4double globalDetFrontX = (posX_Coll + lenCollX / 2.0) + gapCollToDetector;
    G4double posX_Detector = globalDetFrontX + bgoFrontZ;

    //-------------------------------------------------
    // Final World Placements
    //-------------------------------------------------
    
    // Place Target
    new G4PVPlacement(nullptr, {posX_Target, 0., 0.}, lOuterTarget, "pOuterTarget", lWorld, false, 0, true);

    // Place Collimator Mother Volume
    new G4PVPlacement(nullptr, {posX_Coll, 0., 0.}, lCollCase, "pCollCase", lWorld, false, 0, true);

    // Place Detector Assembly (Rotated -90 deg around Y to point face at the target)
    rotDetector = new G4RotationMatrix();
    rotDetector->rotateY(+90. * deg);
    new G4PVPlacement(rotDetector, {posX_Detector, 0., 0.}, lDetectorAssembly, "pDetectorAssembly", lWorld, false, 0, true);
    
    return pWorld;
}

void geometry::ConstructSDandField(){
}