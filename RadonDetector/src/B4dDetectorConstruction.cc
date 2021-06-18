//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B4dDetectorConstruction.cc
/// \brief Implementation of the B4dDetectorConstruction class

#include "B4dDetectorConstruction.hh"
#include "F02ElectricFieldSetup.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4StepLimiter.hh"
#include "G4SDManager.hh"
#include "G4SDChargedFilter.hh"
#include "G4SDParticleFilter.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSNofSecondary.hh"
#include "G4PSTermination.hh"
#include "G4PSTrackLength.hh"
#include "G4PSCellCharge.hh"
#include "G4UserLimits.hh"
#include "G4StepLimiter.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B4dDetectorConstruction::fMagFieldMessenger = 0; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4dDetectorConstruction::B4dDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fCheckOverlaps(true)
{
}

//F02ElectricFieldSetup* field = new F02ElectricFieldSetup(); 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4dDetectorConstruction::~B4dDetectorConstruction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4dDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dDetectorConstruction::DefineMaterials()
{ 
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Pb");
  
  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4dDetectorConstruction::DefineVolumes()
{
  G4NistManager* nist = G4NistManager::Instance();
       
  // World
  G4double world_sizeX = 2.6*cm;
  G4double world_sizeY = 2.6*cm;
  G4double world_sizeZ  = 5.5*mm;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       world_sizeX, world_sizeY, world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      fCheckOverlaps);        //overlaps checking
                      
                         G4Material* granite_mat = nist->FindOrBuildMaterial("G4_AIR");
                    

 
  G4Box* solidGranite =    
    new G4Box("Granite",                       //its name
      2.5*cm, 2.5*cm, 4.5*mm);     //its size
      
  G4LogicalVolume* logicGranite =                         
    new G4LogicalVolume(solidGranite,          //its solid
                        granite_mat,           //its material
                        "Granite");            //its name
                                   
  G4VPhysicalVolume* physGranite = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0, 5*cm, 0),       //at (0,0,0)
                      logicGranite,            //its logical volume
                      "Granite",               //its name
                      logicWorld,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      fCheckOverlaps);        //overlaps checking 
                      
                    
 
                      
  //     
  // Envelope
  //  
 /* G4Box* solidEnv =    
    new G4Box("Envelope",                    //its name
        env_sizeXY, env_sizeXY, env_sizeZ); //its size
      
  G4LogicalVolume* logicEnv =                         
    new G4LogicalVolume(solidEnv,            //its solid
                        env_mat,             //its material
                        "Envelope");         //its name
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking */
 
  //     
  // Shape 1
  //  
	
  G4Material* Cu = nist->FindOrBuildMaterial("G4_Cu");
  G4ThreeVector pos1 = G4ThreeVector(0, 0, -5.0*mm);

     //Create the shape of the first plate, relative to the gun 
		G4double target_sizeX=2.5*cm;
		G4double target_sizeY=2.5*cm;
		G4double target_sizeZ=0.5*mm;
		G4Box* solidfirstPlate = 
			new G4Box("firstPlate",target_sizeX, target_sizeY, target_sizeZ);
                
                G4LogicalVolume* logicfirstPlate = 
			new G4LogicalVolume(solidfirstPlate, Cu, "firstPlate");


              
			new G4PVPlacement(0,                       //no rotation
					  pos1,       //at (0,0,0)
					  logicfirstPlate,           //its logical volume
					  "firstPlate",               //its name
					  logicWorld,             //its mother  volume
					  false,                 //no boolean operation
					  0,                     //copy number
					  true);         //overlaps checking                     

 //shape 2
  G4ThreeVector pos2 = G4ThreeVector(0, 0, 5.0*mm);
  G4Material* Cu2 = nist->FindOrBuildMaterial("G4_Cu");

     //Create the shape of the first plate, relative to the gun 
		G4Box* solidsecondPlate = 
			new G4Box("secondPlate",target_sizeX, target_sizeY, target_sizeZ);
                
                G4LogicalVolume* logicsecondPlate = 
			new G4LogicalVolume(solidsecondPlate, Cu2, "secondPlate");


					new G4PVPlacement(0,                       //no rotation
					  pos2,       //at (0,0,0)
					  logicsecondPlate,           //its logical volume
					  "secondPlate",               //its name
					  logicWorld,             //its mother  volume
					  false,                 //no boolean operation
					  0,                     //copy number
					  true);         //overlaps checking 
					  
					  
					
  // 
  // Scorers
  //

  // declare Absorber as a MultiFunctionalDetector scorer
  //  
 /* auto secondPlateDetector = new G4MultiFunctionalDetector("secondPlate");
  G4SDManager::GetSDMpointer()->AddNewDetector(secondPlateDetector);

  G4VPrimitiveScorer* primitive;
  primitive = new G4PSCellCharge("Charge");
  //auto charged = new G4SDChargedFilter("chargedFilter");
  //primitive ->SetFilter(charged);
  secondPlateDetector->RegisterPrimitive(primitive);  

  SetSensitiveDetector("secondPlate",secondPlateDetector); */
  

   

	// shape Spacer1
	G4ThreeVector pos3 = G4ThreeVector(2.4*cm, 2.4*cm, 0*mm);
        G4Material* PLA = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
	
	G4Tubs* solidSpacer1 = new G4Tubs("Spacer1", 0, 0.75*mm, 4.5*mm, 0, twopi);
	G4LogicalVolume* logicSpacer1 = new G4LogicalVolume(solidSpacer1, PLA, "Spacer1");

	new G4PVPlacement(0,                       //no rotation
					  pos3,       //at (0,0,0)
					  logicSpacer1,           //its logical volume
					  "Spacer1",               //its name
					  logicWorld,             //its mother  volume
					  false,                 //no boolean operation
					  0,                     //copy number
					  true);         //overlaps checking 
	//shape Spacer2
	G4ThreeVector pos4 = G4ThreeVector(-2.4*cm, 2.4*cm, 0*mm);
	G4Tubs* solidSpacer2 = new G4Tubs("Spacer2", 0, 0.75*mm, 4.5*mm, 0, twopi);
	G4LogicalVolume* logicSpacer2 = new G4LogicalVolume(solidSpacer2, PLA, "Spacer2");

	new G4PVPlacement(0,                       //no rotation
					  pos4,       //at (0,0,0)
					  logicSpacer2,           //its logical volume
					  "Spacer2",               //its name
					  logicWorld,             //its mother  volume
					  false,                 //no boolean operation
					  0,                     //copy number
					  true);         //overlaps checking 
	//shape Spacer3
	G4ThreeVector pos5 = G4ThreeVector(-2.4*cm, -2.4*cm, 0*mm);
	G4Tubs* solidSpacer3 = new G4Tubs("Spacer3", 0, 0.70*mm, 4.5*mm, 0, twopi);
	G4LogicalVolume* logicSpacer3 = new G4LogicalVolume(solidSpacer3, PLA, "Spacer3");

	new G4PVPlacement(0,                       //no rotation
					  pos5,       //at (0,0,0)
					  logicSpacer3,           //its logical volume
					  "Spacer3",               //its name
					  logicWorld,             //its mother  volume
					  false,                 //no boolean operation
					  0,                     //copy number
					  true);         //overlaps checking 

	G4ThreeVector pos6 = G4ThreeVector(2.4*cm, -2.4*cm, 0*mm);
	G4Tubs* solidSpacer4 = new G4Tubs("Spacer4", 0, 0.75*mm, 4.5*mm, 0, twopi);
	G4LogicalVolume* logicSpacer4 = new G4LogicalVolume(solidSpacer4, PLA, "Spacer4");

	new G4PVPlacement(0,                       //no rotation
					  pos6,       //at (0,0,0)
					  logicSpacer4,           //its logical volume
					  "Spacer4",               //its name
					  logicWorld,             //its mother  volume
					  false,                 //no boolean operation
					  0,                     //copy number
					  true);         //overlaps checking */ 
	





    
  // Set Shape2 as scoring volume
  //
  //
  //always return the physical World
  if (!fEmFieldSetup.Get()) { 
    F02ElectricFieldSetup* emFieldSetup = new F02ElectricFieldSetup();

    fEmFieldSetup.Put(emFieldSetup);
    G4AutoDelete::Register(emFieldSetup); //Kernel will delete the messenger
  }  
  // Set local field manager and local field in radiator and its daughters:
  G4bool allLocal = true;
  logicGranite->SetFieldManager(fEmFieldSetup.Get()->GetLocalFieldManager(),
                                  allLocal );
  //
  return physWorld;
	

  //Major Payne
  // Always return the physical World
  //
 // return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  // 
  // Scorers
  //


  //Second Detector
  auto secondDetector = new G4MultiFunctionalDetector("secondPlate");
  G4SDManager::GetSDMpointer()->AddNewDetector(secondDetector);

  G4VPrimitiveScorer* primitive;
  primitive = new G4PSCellCharge("Charge");
  G4SDParticleFilter* eMinusFilter = new G4SDParticleFilter("eMinusFilter ");
  eMinusFilter->add("e-");
 G4SDParticleFilter* ePlusFilter = new G4SDParticleFilter("ePlusFilter ");
  ePlusFilter->add("e+");
  
  secondDetector->RegisterPrimitive(primitive);


  primitive = new G4PSTermination("Termination");
  secondDetector->RegisterPrimitive(primitive);
  
  SetSensitiveDetector("secondPlate",secondDetector);


  //First Detector

  auto firstDetector = new G4MultiFunctionalDetector("firstPlate");
  G4SDManager::GetSDMpointer()->AddNewDetector(firstDetector);

  primitive = new G4PSCellCharge("Charge");
  
  firstDetector->RegisterPrimitive(primitive);
  SetSensitiveDetector("firstPlate",firstDetector);

  //World Detector
  auto worldDetector = new G4MultiFunctionalDetector("World");
  G4SDManager::GetSDMpointer()->AddNewDetector(worldDetector);

  primitive = new G4PSTrackLength("TrackLength");
  G4SDParticleFilter* alphaFilter = new G4SDParticleFilter("alphaFilter");
  alphaFilter->add("alpha");
  primitive ->SetFilter(alphaFilter);
  
  worldDetector->RegisterPrimitive(primitive);
  G4VPrimitiveScorer* prime = new G4PSNofSecondary("Secondaries");
  worldDetector->RegisterPrimitive(prime);
  
  SetSensitiveDetector("World",worldDetector);



  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
