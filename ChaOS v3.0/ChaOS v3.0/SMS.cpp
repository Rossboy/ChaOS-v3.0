#include "SMS.h"
#include"Headers\Process.h"
extern PCB* ActiveProcess;


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