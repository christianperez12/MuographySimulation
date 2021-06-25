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
/// \file B4RunAction.cc
/// \brief Implementation of the B4RunAction class

#include "B4RunAction.hh"
#include "B4Analysis.hh"
#include <iostream>
#include <fstream>
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::B4RunAction()
 : G4UserRunAction()
{ 
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);     

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B4Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Create directories 
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple
  //
  
  // Creating histograms
  analysisManager->CreateH1("Charge_Deposited_in_Right_Plate","Charge in Plate", 100, -100000, 100000*eplus);
  analysisManager->CreateH1("Charge_Deposited_in_Left_Plate","Charge in Plate", 100, -100000, 100000*eplus);
  analysisManager->CreateH1("Alpha_Track_Length","Alpha Track Length", 100, 0., 10*cm);
  analysisManager->CreateH1("Total_Secondaries_Produced","NofSecondary", 100, 0., 100000);
  analysisManager->CreateH1("Termination Right Plate","NofTerminations", 100, 0., 100000);


  // Creating ntuple
  //
  analysisManager->CreateNtuple("B4", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("Charge_Deposited_in_Right_Plate");
  analysisManager->CreateNtupleDColumn("Charge_Deposited_in_Left_Plate");
  analysisManager->CreateNtupleDColumn("Alpha_Track_Length");
  analysisManager->CreateNtupleDColumn("Total_Secondaries_Produce");
  analysisManager->CreateNtupleDColumn("TerminationRightPlate");
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::~B4RunAction()
{
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "B4";
  analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 

void B4RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // print histogram statistics
  //
  auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(0) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl; 
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl; 
    }
    
    G4cout << " Right Plate : mean = " 
       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Electric charge") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Electric charge") << G4endl;
    G4cout << " Left Plate : mean = " 
       << G4BestUnit(analysisManager->GetH1(1)->mean(), "Electric charge") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Electric charge") << G4endl;
    G4cout << " Middle : mean = " 
       << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Length") << G4endl;
    G4cout << " Middle : mean = " 
       << G4BestUnit(analysisManager->GetH1(3)->mean(), "Amount of substance") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Amount of substance") << G4endl;
  G4cout << " Right Plate : mean = " 
       << G4BestUnit(analysisManager->GetH1(4)->mean(), "Amount of substance") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(4)->rms(),  "Amount of substance") << G4endl;
       
     
       string energy;
       fstream results;
       results.open("run.mac");
       getline(results, energy);
       results.close();
       
       string str = energy;
       
       size_t i = 0;
       for ( ; i < str.length(); i++ ){ if ( isdigit(str[i]) ) break; }
       // remove the first chars, which aren't digits
       str = str.substr(i, str.length() - i );
       // convert the remaining text to an integer
       int id = atoi(str.c_str());
       
       stringstream ss; 
       ss << id;
       string enrgy = ss.str();
       
       ofstream MyFile(enrgy);
       MyFile << (-1*analysisManager->GetH1(0)->mean()) << endl;
       MyFile.close();
       


    /*   results.open("results.txt", std::ios_base::app | std::ios_base::in);
       results  << id << ", " << (-1*analysisManager->GetH1(0)->mean()) << endl;
       results.close();
    */
  }

  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
