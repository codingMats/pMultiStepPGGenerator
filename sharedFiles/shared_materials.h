#pragma once
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"


    


namespace SharedMaterials {
    inline void BuildAllMaterials(
        G4Material*& matVacuum, G4Material*& matAir, G4Material*& matAl, 
        G4Material*& matTeflon, G4Material*& matTungsten, G4Material*& matCeBr3, 
        G4Material*& matBGO, G4Material*& matPolystyrene, G4Material*& matCaO, G4Material*& matWater) 
    {
        G4NistManager* nist = G4NistManager::Instance();
        matVacuum = nist->FindOrBuildMaterial("G4_Galactic");
        matAir    = nist->FindOrBuildMaterial("G4_AIR");
        matAl     = nist->FindOrBuildMaterial("G4_Al");
        matWater  = nist->FindOrBuildMaterial("G4_WATER");
        
        // ----------------------------------------------------------------------
        // Elements
        // ----------------------------------------------------------------------
        G4Element* elH  = nist->FindOrBuildElement("H");
        G4Element* elC  = nist->FindOrBuildElement("C");
        G4Element* elN  = nist->FindOrBuildElement("N");
        G4Element* elO  = nist->FindOrBuildElement("O");
        G4Element* elF  = nist->FindOrBuildElement("F");
        G4Element* elMg = nist->FindOrBuildElement("Mg");
        G4Element* elAl = nist->FindOrBuildElement("Al");
        G4Element* elSi = nist->FindOrBuildElement("Si");
        G4Element* elP  = nist->FindOrBuildElement("P");
        G4Element* elCl = nist->FindOrBuildElement("Cl");
        G4Element* elCa = nist->FindOrBuildElement("Ca");
        G4Element* elMn = nist->FindOrBuildElement("Mn");
        G4Element* elGa = nist->FindOrBuildElement("Ga");
        G4Element* elGe = nist->FindOrBuildElement("Ge");
        G4Element* elBr = nist->FindOrBuildElement("Br");
        G4Element* elLa = nist->FindOrBuildElement("La");
        G4Element* elCe = nist->FindOrBuildElement("Ce");
        G4Element* elGd = nist->FindOrBuildElement("Gd");
        G4Element* elBi = nist->FindOrBuildElement("Bi");
        G4Element* elW  = nist->FindOrBuildElement("W");

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

        

        // ----------------------------------------------------------------------
        // Base NIST Materials
        // ----------------------------------------------------------------------
        G4Material* matCa    = nist->FindOrBuildMaterial("G4_Ca");

        // ----------------------------------------------------------------------
        // Detector & Basic Materials
        // ----------------------------------------------------------------------
        G4Material* teflon = new G4Material("teflon", 2.2*g/cm3, 2);
        teflon->AddElement(elC, 0.240183);
        teflon->AddElement(elF, 0.759817);

        G4Material* tungsten = new G4Material("tungsten", 19.25*g/cm3, 1);
        tungsten->AddElement(elW, 1.0);

        G4Material* cebr3 = new G4Material("CeBr3", 5.2*g/cm3, 2);
        cebr3->AddElement(elCe, 0.3714);
        cebr3->AddElement(elBr, 0.6286);

        G4Material* bgo = new G4Material("BGO", 7.12*g/cm3, 3);
        bgo->AddElement(elBi, 0.671);
        bgo->AddElement(elGe, 0.175);
        bgo->AddElement(elO, 0.154);

        G4Material* gaggce = new G4Material("GAGGCe", 6.63*g/cm3, 5);
        gaggce->AddElement(elGd, 0.5032);
        gaggce->AddElement(elAl, 0.0577);
        gaggce->AddElement(elGa, 0.2237);
        gaggce->AddElement(elO, 0.2053);
        gaggce->AddElement(elCe, 0.0100);

        G4Material* labr3ce = new G4Material("LaBr3Ce", 5.08*g/cm3, 3);
        labr3ce->AddElement(elLa, 0.3650);
        labr3ce->AddElement(elBr, 0.6300);
        labr3ce->AddElement(elCe, 0.0050);

        G4Material* polystyrene = new G4Material("polystyrene", 1.05*g/cm3, 2);
        polystyrene->AddElement(elC, 0.922582); // Approx mass fractions for CH
        polystyrene->AddElement(elH, 0.077418);

        G4Material* cao = new G4Material("calcium_oxide", 1.22*g/cm3, 2);
        cao->AddElement(elCa, 0.7147);
        cao->AddElement(elO, 0.2853);

        // ----------------------------------------------------------------------
        // Polymers and Resins
        // ----------------------------------------------------------------------
        G4Material* pmma = new G4Material("PMMA", 1.190*g/cm3, 3);
        pmma->AddElement(elC, 0.599848); pmma->AddElement(elH, 0.080538); pmma->AddElement(elO, 0.319614);

        G4Material* mma = new G4Material("MMA", 0.94*g/cm3, 3);
        mma->AddElement(elC, 0.599848); mma->AddElement(elH, 0.080538); mma->AddElement(elO, 0.319614);

        G4Material* bpo = new G4Material("BPO", 1.33*g/cm3, 3);
        bpo->AddElement(elC, 0.6941915); bpo->AddElement(elH, 0.0416133); bpo->AddElement(elO, 0.2641952);

        G4Material* clarofast = new G4Material("ClaroFast", 1.18*g/cm3, 3);
        clarofast->AddMaterial(mma, 0.007); clarofast->AddMaterial(bpo, 0.003); clarofast->AddMaterial(pmma, 0.99);

        // ----------------------------------------------------------------------
        // Bone Components
        // ----------------------------------------------------------------------
        G4Material* betatcp = new G4Material("betaTCP", 3.14*g/cm3, 3);
        betatcp->AddElement(elCa, 0.3876346); betatcp->AddElement(elP, 0.1997189); betatcp->AddElement(elO, 0.4126464);

        auto buildMix = [&](const G4String& name, G4double density, G4Material* m1, G4double f1, G4Material* m2, G4double f2) {
            G4Material* mat = new G4Material(name, density, (f1 > 0 && f2 > 0) ? 2 : 1);
            if (f1 > 0) mat->AddMaterial(m1, f1);
            if (f2 > 0) mat->AddMaterial(m2, f2);
        };

        buildMix("boneCa0",  1.180*g/cm3, betatcp, 0.000000, clarofast, 1.000000);
        buildMix("boneCa05", 1.205*g/cm3, betatcp, 0.012899, clarofast, 0.987101);
        buildMix("boneCa1",  1.231*g/cm3, betatcp, 0.025797, clarofast, 0.974203);
        buildMix("boneCa2",  1.281*g/cm3, betatcp, 0.051595, clarofast, 0.948405);
        buildMix("boneCa4",  1.382*g/cm3, betatcp, 0.103190, clarofast, 0.896810);
        buildMix("boneCa8",  1.585*g/cm3, betatcp, 0.206380, clarofast, 0.793620);
        buildMix("boneCa16", 1.989*g/cm3, betatcp, 0.412760, clarofast, 0.587240);
        buildMix("boneCa21", 2.242*g/cm3, betatcp, 0.541747, clarofast, 0.458253);

        buildMix("UACa0",  1.8*g/cm3, betatcp, 0.000000, clarofast, 1.000000);
        buildMix("UACa1",  1.8*g/cm3, betatcp, 0.025797, clarofast, 0.974203);
        buildMix("UACa5",  1.8*g/cm3, betatcp, 0.128985, clarofast, 0.871015);
        buildMix("UACa10", 1.8*g/cm3, betatcp, 0.257970, clarofast, 0.742030);
        buildMix("UACa20", 1.8*g/cm3, betatcp, 0.515940, clarofast, 0.484060);
        buildMix("UACa30", 1.8*g/cm3, betatcp, 0.773910, clarofast, 0.226090);
        buildMix("UACa38", 1.8*g/cm3, betatcp, 1.000000, clarofast, 0.000000);

        G4Material* boneT1 = new G4Material("BoneTissue1", 1.8*g/cm3, 4);
        boneT1->AddElement(elCa, 0.398940); boneT1->AddElement(elP, 0.184989); boneT1->AddElement(elO, 0.414064); boneT1->AddElement(elH, 0.002007);
        
        G4Material* boneT2 = new G4Material("BoneTissue2", 0.150*g/cm3, 4);
        boneT2->AddElement(elCa, 0.398940); boneT2->AddElement(elP, 0.184989); boneT2->AddElement(elO, 0.414064); boneT2->AddElement(elH, 0.002007);

        G4Material* hydroxy = new G4Material("Hydroxyapatite", 3.16*g/cm3, 4);
        hydroxy->AddElement(elCa, 0.398940); hydroxy->AddElement(elP, 0.184989); hydroxy->AddElement(elO, 0.414064); hydroxy->AddElement(elH, 0.002007);

        // ----------------------------------------------------------------------
        // Water/Calcium Mixes
        // ----------------------------------------------------------------------
        auto buildWaCa = [&](const G4String& name, G4double density, G4double fW, G4double fCa) {
            G4Material* mat = new G4Material(name, density, (fW > 0 && fCa > 0) ? 2 : 1);
            if (fW > 0) mat->AddMaterial(matWater, fW);
            if (fCa > 0) mat->AddMaterial(matCa, fCa);
        };

        // 1.0 g/cm3
        buildWaCa("WaterCa0", 1.0*g/cm3, 1.0, 0.0); buildWaCa("WaterCa1", 1.0*g/cm3, 0.99, 0.01);
        buildWaCa("WaterCa5", 1.0*g/cm3, 0.95, 0.05); buildWaCa("WaterCa10", 1.0*g/cm3, 0.90, 0.10);
        buildWaCa("WaterCa25", 1.0*g/cm3, 0.75, 0.25); buildWaCa("WaterCa50", 1.0*g/cm3, 0.50, 0.50);
        buildWaCa("WaterCa75", 1.0*g/cm3, 0.25, 0.75); buildWaCa("WaterCa100", 1.0*g/cm3, 0.0, 1.0);
        
        // 0.5 g/cm3
        buildWaCa("WaterCa0Dens05", 0.5*g/cm3, 1.0, 0.0); buildWaCa("WaterCa1Dens05", 0.5*g/cm3, 0.99, 0.01);
        buildWaCa("WaterCa5Dens05", 0.5*g/cm3, 0.95, 0.05); buildWaCa("WaterCa10Dens05", 0.5*g/cm3, 0.90, 0.10);
        buildWaCa("WaterCa25Dens05", 0.5*g/cm3, 0.75, 0.25); buildWaCa("WaterCa50Dens05", 0.5*g/cm3, 0.50, 0.50);
        buildWaCa("WaterCa75Dens05", 0.5*g/cm3, 0.25, 0.75); buildWaCa("WaterCa100Dens05", 0.5*g/cm3, 0.0, 1.0);

        // 2.0 g/cm3
        buildWaCa("WaterCa0Dens2", 2.0*g/cm3, 1.0, 0.0); buildWaCa("WaterCa1Dens2", 2.0*g/cm3, 0.99, 0.01);
        buildWaCa("WaterCa5Dens2", 2.0*g/cm3, 0.95, 0.05); buildWaCa("WaterCa10Dens2", 2.0*g/cm3, 0.90, 0.10);
        buildWaCa("WaterCa25Dens2", 2.0*g/cm3, 0.75, 0.25); buildWaCa("WaterCa50Dens2", 2.0*g/cm3, 0.50, 0.50);
        buildWaCa("WaterCa75Dens2", 2.0*g/cm3, 0.25, 0.75); buildWaCa("WaterCa100Dens2", 2.0*g/cm3, 0.0, 1.0);

        // ----------------------------------------------------------------------
        // Gammex Materials
        // ----------------------------------------------------------------------
        G4Material* ln300 = new G4Material("LN_300_lung", 0.29*g/cm3, 7);
        ln300->AddElement(elH, 0.08460); ln300->AddElement(elC, 0.59370); ln300->AddElement(elN, 0.01960); ln300->AddElement(elO, 0.18140);
        ln300->AddElement(elMg, 0.11190); ln300->AddElement(elSi, 0.00780); ln300->AddElement(elCl, 0.00100);

        G4Material* ln450 = new G4Material("LN_450_lung", 0.428*g/cm3, 7);
        ln450->AddElement(elH, 0.08470); ln450->AddElement(elC, 0.59560); ln450->AddElement(elN, 0.01970); ln450->AddElement(elO, 0.18110);
        ln450->AddElement(elMg, 0.11210); ln450->AddElement(elSi, 0.00580); ln450->AddElement(elCl, 0.00100);

        G4Material* ap6 = new G4Material("AP6_adipose", 0.946*g/cm3, 5);
        ap6->AddElement(elH, 0.09060); ap6->AddElement(elC, 0.72290); ap6->AddElement(elN, 0.02250); ap6->AddElement(elO, 0.16270); ap6->AddElement(elCl, 0.00130);

        G4Material* br12 = new G4Material("BR_12_breast", 0.981*g/cm3, 6);
        br12->AddElement(elH, 0.08590); br12->AddElement(elC, 0.70100); br12->AddElement(elN, 0.02330); br12->AddElement(elO, 0.17900); br12->AddElement(elCl, 0.00130); br12->AddElement(elCa, 0.00950);

        G4Material* wInsert = new G4Material("Water_insert", 0.998*g/cm3, 2);
        wInsert->AddElement(elH, 0.11190); wInsert->AddElement(elO, 0.88810);

        G4Material* ctSolid = new G4Material("CT_solid_water", 1.014*g/cm3, 6);
        ctSolid->AddElement(elH, 0.08000); ctSolid->AddElement(elC, 0.67290); ctSolid->AddElement(elN, 0.02390); ctSolid->AddElement(elO, 0.19870); ctSolid->AddElement(elCl, 0.00140); ctSolid->AddElement(elCa, 0.02310);

        G4Material* swM457 = new G4Material("Solid_water_M457", 1.045*g/cm3, 6);
        swM457->AddElement(elH, 0.08020); swM457->AddElement(elC, 0.67220); swM457->AddElement(elN, 0.02410); swM457->AddElement(elO, 0.19910); swM457->AddElement(elCl, 0.00140); swM457->AddElement(elCa, 0.02310);

        G4Material* alMgSi1 = new G4Material("Aluminium_AlMgSi1", 2.691*g/cm3, 4);
        alMgSi1->AddElement(elMg, 0.01000); alMgSi1->AddElement(elAl, 0.97200); alMgSi1->AddElement(elSi, 0.01000); alMgSi1->AddElement(elMn, 0.00800);

        G4Material* brn = new G4Material("BRN_SR2_brain", 1.051*g/cm3, 5);
        brn->AddElement(elH, 0.10830); brn->AddElement(elC, 0.72540); brn->AddElement(elN, 0.01690); brn->AddElement(elO, 0.14860); brn->AddElement(elCl, 0.00080);

        G4Material* lv1 = new G4Material("LV1_liver", 1.095*g/cm3, 6);
        lv1->AddElement(elH, 0.08060); lv1->AddElement(elC, 0.67010); lv1->AddElement(elN, 0.02470); lv1->AddElement(elO, 0.20010); lv1->AddElement(elCl, 0.00140); lv1->AddElement(elCa, 0.02310);

        G4Material* ib3 = new G4Material("IB3_inner_bone", 1.153*g/cm3, 7);
        ib3->AddElement(elH, 0.06670); ib3->AddElement(elC, 0.55650); ib3->AddElement(elN, 0.01960); ib3->AddElement(elO, 0.23520); ib3->AddElement(elP, 0.03230); ib3->AddElement(elCl, 0.00110); ib3->AddElement(elCa, 0.08860);

        G4Material* b200 = new G4Material("B200_bone_mineral", 1.159*g/cm3, 7);
        b200->AddElement(elH, 0.06650); b200->AddElement(elC, 0.55510); b200->AddElement(elN, 0.01980); b200->AddElement(elO, 0.23640); b200->AddElement(elP, 0.03240); b200->AddElement(elCl, 0.00110); b200->AddElement(elCa, 0.08870);

        G4Material* cb2_30 = new G4Material("CB2_30_CaCO3", 1.33*g/cm3, 6);
        cb2_30->AddElement(elH, 0.06680); cb2_30->AddElement(elC, 0.53470); cb2_30->AddElement(elN, 0.02120); cb2_30->AddElement(elO, 0.25610); cb2_30->AddElement(elCl, 0.00110); cb2_30->AddElement(elCa, 0.12010);

        G4Material* cb2_50 = new G4Material("CB2_50_CaCO3", 1.56*g/cm3, 6);
        cb2_50->AddElement(elH, 0.04770); cb2_50->AddElement(elC, 0.41620); cb2_50->AddElement(elN, 0.01520); cb2_50->AddElement(elO, 0.31990); cb2_50->AddElement(elCl, 0.00080); cb2_50->AddElement(elCa, 0.20020);

        G4Material* sb3 = new G4Material("SB3_cortical_bone", 1.823*g/cm3, 6);
        sb3->AddElement(elH, 0.03410); sb3->AddElement(elC, 0.31410); sb3->AddElement(elN, 0.01840); sb3->AddElement(elO, 0.36500); sb3->AddElement(elCl, 0.00040); sb3->AddElement(elCa, 0.26810);
    }

    inline G4Material* GetTargetMaterial(const G4String& matName) {
        G4Material* mat = G4Material::GetMaterial(matName, false);
        if (!mat) {
            mat = G4NistManager::Instance()->FindOrBuildMaterial(matName);
        }
        return mat;
    }
}
   