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
/// \file B4dEventAction.cc
/// \brief Implementation of the B4dEventAction class

#include "B4dEventAction.hh"
#include "B4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4dEventAction::B4dEventAction()
 : G4UserEventAction(),
   fworldTrackLengthHCID(-1),
   fworldNofSecondaryHCID(-1),
   ffirstPlateChargeHCID(-1),
   fsecondPlateChargeHCID(-1),
   fsecondPlateTerminationHCID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4dEventAction::~B4dEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4THitsMap<G4double>* 
B4dEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<G4THitsMap<G4double>*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4dEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double B4dEventAction::GetSum(G4THitsMap<G4double>* hitsMap) const
{
  G4double sumValue = 0.;
  for ( auto it : *hitsMap->GetMap() ) {
    // hitsMap->GetMap() returns the map of std::map<G4int, G4double*>
    sumValue += *(it.second);
  }
  return sumValue;  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dEventAction::PrintEventStatistics( G4double firstPlateCharge, G4double secondPlateCharge, G4double secondPlateTermination, G4double worldTrackLength, G4double worldNofSecondary) const
{
  // Print event statistics
  //
  G4cout
     << "   Right Plate: Total Charge: " 
     << std::setw(7) << G4BestUnit(secondPlateCharge, "Electric charge")
     << G4endl 
     << "   Right Plate: Termations: " 
     << std::setw(7) << G4BestUnit(secondPlateTermination, "Amount of substance")
     << G4endl 
     << "   Left Plate: Total Charge: " 
     << std::setw(7) << G4BestUnit(firstPlateCharge, "Electric charge")
     << G4endl 
     << "   Middle Volume: Total Track Length: " 
     << std::setw(7) << G4BestUnit(worldTrackLength, "Length")
     << G4endl 
     << "   Middle Volume: Total Secondaries " 
     << std::setw(7) << G4BestUnit(worldNofSecondary, "Amount of substance")
     << G4endl ;




}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dEventAction::EndOfEventAction(const G4Event* event)
{  
   // Get hist collections IDs
  if ( fsecondPlateChargeHCID == -1 ) {
    fworldTrackLengthHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("World/TrackLength");
    fworldNofSecondaryHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("World/Secondaries");
    fsecondPlateChargeHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("secondPlate/Charge");
    fsecondPlateTerminationHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("secondPlate/Termination");
    ffirstPlateChargeHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("firstPlate/Charge");
  }
  
  // Get sum values from hits collections
  //
  auto firstPlateCharge = GetSum(GetHitsCollection(ffirstPlateChargeHCID, event));
  auto secondPlateCharge = GetSum(GetHitsCollection(fsecondPlateChargeHCID, event));
  auto secondPlateTermination = GetSum(GetHitsCollection(fsecondPlateTerminationHCID, event));
  auto worldTrackLength = GetSum(GetHitsCollection(fworldTrackLengthHCID, event));
  auto worldNofSecondary = GetSum(GetHitsCollection(fworldNofSecondaryHCID, event));
 
  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  //  
  analysisManager->FillH1(0, secondPlateCharge);
  analysisManager->FillH1(1, firstPlateCharge);
  analysisManager->FillH1(2, worldTrackLength);
  analysisManager->FillH1(3, worldNofSecondary);
  analysisManager->FillH1(4, secondPlateTermination);
  // fill ntuple
  //
  analysisManager->FillNtupleDColumn(0, secondPlateCharge);
  analysisManager->FillNtupleDColumn(1, firstPlateCharge);
  analysisManager->FillNtupleDColumn(2, worldTrackLength);
  analysisManager->FillNtupleDColumn(3, worldNofSecondary);
  analysisManager->FillNtupleDColumn(4, secondPlateTermination);
  analysisManager->AddNtupleRow();  
  
  //print per event (modulo n)
  //
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;     
    PrintEventStatistics(firstPlateCharge, secondPlateCharge, secondPlateTermination, worldTrackLength, worldNofSecondary);
  }
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
