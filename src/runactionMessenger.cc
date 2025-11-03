#include "runactionMessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"

RunActionMessenger::RunActionMessenger(MyRunAction* a)
: action(a)
{
    setRunNameCmd = new G4UIcmdWithAString("/run/setRunName", this);
    setRunNameCmd->SetGuidance("Set a descriptive name for output files.");
}

RunActionMessenger::~RunActionMessenger()
{
    delete setRunNameCmd;
}

void RunActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == setRunNameCmd) {
        MyRunAction::SetRunName(newValue);
    }
}
