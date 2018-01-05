#include "..\Headers\SMS.h"
#include"..\Headers\Process.h"
extern PCB* ActiveProcess;


SMS::SMS(std::string wiad,int indeks)
{
	this->wiad = wiad;
	this->WID = ActiveProcess->GetPID();
	this->indeks = indeks;
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
int SMS::getCVindex()
{
	return indeks;
}