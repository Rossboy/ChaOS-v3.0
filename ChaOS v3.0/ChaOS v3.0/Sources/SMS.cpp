#include "..\Headers\SMS.h"
#include"..\Headers\Process.h"
#include"..\Headers\ConditionVariable.h"
extern PCB* ActiveProcess;
extern std::list<ConditionVariable> cv;


SMS::SMS(std::string wiad)
{
	this->wiad = wiad;
	this->WID = ActiveProcess->GetPID();
}


SMS::~SMS()
{
}
int SMS::getID()
{
	return WID;
}
std::string SMS::getwiad()
{
	return wiad;
}