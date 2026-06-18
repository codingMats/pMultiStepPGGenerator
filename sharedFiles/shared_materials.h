#pragma once
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

namespace SharedMaterials {
    inline void ConstructMaterials(
        G4Material*& matVacuum, G4Material*& matAir, G4Material*& matAl, 
        G4Material*& matTeflon, G4Material*& matTungsten, G4Material*& matCeBr3, 
        G4Material*& matBGO, G4Material*& matPolystyrene, G4Material*& matCaO, G4Material*& matWater) 
    {
        G4NistManager* nist = G4NistManager::Instance();
        matVacuum = nist->FindOrBuildMaterial("G4_Galactic");
        matAir    = nist->FindOrBuildMaterial("G4_AIR");
        matAl     = nist->FindOrBuildMaterial("G4_Al");
        matWater  = nist->FindOrBuildMaterial("G4_WATER");

        matTeflon = new G4Material("teflon", 2.2*g/cm3, 2);
        matTeflon->AddElement(nist->FindOrBuildElement("C"), 2);
        matTeflon->AddElement(nist->FindOrBuildElement("F"), 4);

        matTungsten = new G4Material("tungsten", 19.25*g/cm3, 1);
        matTungsten->AddElement(nist->FindOrBuildElement("W"), 1);

        matCeBr3 = new G4Material("CeBr3", 5.10*g/cm3, 2);
        matCeBr3->AddElement(nist->FindOrBuildElement("Ce"), 1);
        matCeBr3->AddElement(nist->FindOrBuildElement("Br"), 3);

        matBGO = new G4Material("BGO", 7.13*g/cm3, 3);
        matBGO->AddElement(nist->FindOrBuildElement("Bi"), 4);
        matBGO->AddElement(nist->FindOrBuildElement("Ge"), 3);
        matBGO->AddElement(nist->FindOrBuildElement("O"), 12);

        matPolystyrene = new G4Material("polystyrene", 1.05*g/cm3, 2);
        matPolystyrene->AddElement(nist->FindOrBuildElement("C"), 8);
        matPolystyrene->AddElement(nist->FindOrBuildElement("H"), 8);
        
        matCaO = new G4Material("calcium_oxide", 1.22*g/cm3, 2);
        matCaO->AddElement(nist->FindOrBuildElement("Ca"), 1);
        matCaO->AddElement(nist->FindOrBuildElement("O"), 1);
    }
}