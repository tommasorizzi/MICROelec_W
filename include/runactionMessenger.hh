#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "runaction.hh"

class RunActionMessenger : public G4UImessenger
{
public:
    RunActionMessenger(MyRunAction*);
    virtual ~RunActionMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);

private:
    MyRunAction* action;
    G4UIcmdWithAString* setRunNameCmd;
};
