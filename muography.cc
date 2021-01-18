#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

int main()
{
  // construct the default run manager
  auto runManager = G4RunManagerFactory::CreateRunManager();

  // set mandatory initialization classes
  runManager->SetUserInitialization(new DetectorConstruction);
  runManager->SetUserInitialization(new PhysicsList);
  runManager->SetUserInitialization(new ActionInitialization);

  // initialize G4 kernel
  runManager->Initialize();

  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/verbose 1");
  UI->ApplyCommand("/event/verbose 1");
  UI->ApplyCommand("/tracking/verbose 1");

  // start a run
  int numberOfEvent = 1;
  runManager->BeamOn(numberOfEvent);

  // job termination
  delete runManager;
  return 0;
}
