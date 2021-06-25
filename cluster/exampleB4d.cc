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
/// \file exampleB4d.cc
/// \brief Main program of the B4d example

using namespace std;

#include <iostream>
#include <fstream>

#include "G4SystemOfUnits.hh"
#include "G4EmParameters.hh"
#include "G4PhysListFactory.hh"
#include "QGSP_BIC.hh"
#include "G4StepLimiter.hh"
#include "G4StepLimiterPhysics.hh"


#include "G4EmDNAPhysics.hh"
#include "G4EmDNAPhysics_option1.hh"
#include "G4EmDNAPhysics_option2.hh"
#include "G4EmDNAPhysics_option3.hh"
#include "G4EmDNAPhysics_option4.hh"
#include "G4EmDNAPhysics_option5.hh"
#include "G4EmDNAPhysics_option6.hh"
#include "G4EmDNAPhysics_option7.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4GeneralParticleSource.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4DecayPhysics.hh"


#include "B4dDetectorConstruction.hh"
#include "B4dActionInitialization.hh"
#include "B4Analysis.hh"
#include "G4ScoringManager.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "PhysicsList.hh"
#include "QGSP_BIC.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
#include "G4TScoreNtupleWriter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " exampleB4d [-m macro ] [-u UIsession] [-t nThreads]" << G4endl;
    G4cerr << "   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{  
  // Evaluate arguments
  //
  if ( argc > 7 ) {
    PrintUsage();
    return 1;
  }
  
  G4String macro;
  G4String session;
/*#ifdef G4MULTITHREADED
  G4int nThreads = 4;
#endif 
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
#ifdef G4MULTITHREADED
    else if ( G4String(argv[i]) == "-t" ) {
      nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
#endif
    else {
      PrintUsage();
      return 1;
    }
  }  */
  
  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv, session);
  }

  // Optionally: choose a different Random engine...
  //
  // G4Random::setTheEngine(new CLHEP::MTwistEngine);
  
  // Construct the MT run manager
  //
/* #ifdef G4MULTITHREADED
  auto runManager = new G4MTRunManager;
  if ( nThreads > 0 ) { 
    runManager->SetNumberOfThreads(nThreads);
  }  
#else */
  auto runManager = new G4RunManager;
  //G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
//#endif


  // Set mandatory initialization classes
  //
  auto detConstruction = new B4dDetectorConstruction();
  runManager->SetUserInitialization(detConstruction);
 

/* #include "G4PhysListFactory.hh"
 G4String plname = "QGSP_BIC_EMZ"; 
 G4PhysListFactory plfactory;
 G4VModularPhysicsList* physList = plfactory.GetReferencePhysList(plname);    
 physList->SetVerboseLevel(2); 
 physList->RegisterPhysics(new G4StepLimiterPhysics());
 G4EmParameters* param = G4EmParameters::Instance();
 param->SetMinEnergy(1*eV);
 param->SetMaxEnergy(1*GeV); 
 param->SetLowestElectronEnergy(20*eV);
 physList-> SetDefaultCutValue(1*nanometer);
 runManager->SetUserInitialization(physList); */


 auto physList = new PhysicsList();
 runManager->SetUserInitialization(physList); 


  auto actionInitialization = new B4dActionInitialization();
  runManager->SetUserInitialization(actionInitialization);
  
  // Initialize visualization
  auto visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Activate score ntuple writer
  // The Root output type (Root) is selected in B3Analysis.hh.
  G4TScoreNtupleWriter<G4AnalysisManager> scoreNtupleWriter;
  // The verbose level can be set via UI commands
  // /score/ntuple/writerVerbose level
  // or via the score ntuple writer function:
  // scoreNtupleWriter.SetVerboseLevel(1);

  // Process macro or start UI session
  //
  
 int stepSize = 100000;
 int energy = 16900000;
 int maxEnergy = 50000000;
 static int energyArray[10000];
 static string dataArray[10000];
 int counter = 0;

   if ( macro.size() ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  {  
    // interactive mode : define UI session
    UImanager->ApplyCommand("/control/verbose 2");
    UImanager->ApplyCommand("/control/saveHistory");
    UImanager->ApplyCommand("/run/verbose 2");
    UImanager->ApplyCommand("/run/initialize");
    UImanager->ApplyCommand("/run/printProgress 100");
    UImanager->ApplyCommand("/control/execute gps.mac");
       fstream results;
       results.open("results.txt", std::ofstream::out | std::ofstream::trunc);
       results.close();
     
for(int e = energy; e <= maxEnergy; e += stepSize) {
	ofstream run;
	run.open("run.mac");
        run  << "/gps/energy " << e << " eV" << endl;
        run  << "/run/beamOn 1" << endl; 
        run.close();
        UImanager->ApplyCommand("/control/execute run.mac");
        
      energyArray[counter] = e;
        
       stringstream ss; 
       ss << e;
       string enrgy = ss.str();
        
      ifstream dataRead(enrgy);
      string data;
       getline(dataRead, data);
       dataArray[counter] = data;
       counter+=1;
    }
    ofstream file("wow.txt");
    for(int i = 0; i < counter; i++) {
	file << energyArray[i] << ", " << dataArray[i] << endl;
}
	file.close();
	//delete[] energyArray;
	//delete[] dataArray; 
    
    ui->SessionStart();
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    UImanager->ApplyCommand("/control/execute gui.mac");
     UImanager->ApplyCommand("/control/execute vis.mac");
    //delete ui; 
    
  } 

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
