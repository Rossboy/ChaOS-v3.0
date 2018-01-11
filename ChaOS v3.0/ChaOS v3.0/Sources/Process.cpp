#include "../Headers/Process.h"
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include "../Headers/ChaOS_filesystem.h"
#include "../Headers/ProcessScheduler.h"

extern PCB* ActiveProcess;
int PCB::processesCounter = 0;
extern ChaOS_filesystem* fs;
extern ProcessesManager* pm;
extern ProcessScheduler* ps;
PCB::PCB(std::string programName, int GID) :points()
{
	this->PID = processesCounter++;
	this->GID = GID;
	this->state = State::New;
	this->instructionCounter = 0;
	this->registers[0] = 0;
	this->registers[1] = 0;
	this->registers[2] = 0;
	this->registers[3] = 0;
	this->burstTime = 10;
	this->programName = programName;
	this->state = State::Ready;
	this->errorCode = 0;
	this->zero = 0;
	//this->currentDir = fs->getRootDir();
	this->currentDir = 1;
}

PCB::~PCB()
{
	if (this->currentFile)
	{
		std::cout << "----- destruktor PCB zrobił signal() na zmiennej niezamkniętego pliku -----" << std::endl;
		fs->signalByID(this->currentFile->getID());
		delete currentFile;
		currentFile = nullptr;
	}
}

void PCB::setStateAndMoveToRespectiveList(State newState)
{
	if (newState == State::Ready && this->GetState() != State::Ready)
	{
		this->state = newState;
		pm->RemoveProcessFromWaiting(this);
		pm->AddProcessToReady(this);
	}
	else if (newState == State::Waiting && this->GetState() != State::Waiting)
	{
		this->state = newState;
		pm->RemoveProcessFromReady(this);
		pm->AddProcessToWaiting(this);


	}
	else if (newState == State::Terminated && this->GetState() != State::Terminated)
	{
		this->state = newState;
		if (this->currentFile != nullptr)fs->closeFile();
		if (ActiveProcess->GetPID() == this->GetPID())ActiveProcess = pm->findPCBbyPID(1);
		pm->killProcess(this->PID);
	}
}
void PCB::SetProcesBurstTime(int newBurstTime)
{
	this->burstTime = newBurstTime;
}
int PCB::GetProcesBurstTime()
{
	return burstTime;
}
int PCB::GetPID()
{
	return PID;
}

int PCB::GetGID()
{
	return GID;
}
std::string PCB::GetFileName()
{
	return programName;
}
State PCB::GetState()
{
	return state;
}
void PCB::setPageTable(Page* newpageTable)
{
	this->pageTable = newpageTable;
}
Page* PCB::getPageTable()
{
	return this->pageTable;
}
void PCB::setPageTableSize(int num)
{
	this->pageTableSize = num;
}
int PCB::getPageTableSize()
{
	return this->pageTableSize;
}
void PCB::displayProcess()
{
	// New, Ready, Waiting, Running, Terminated
	std::cout << "PID: ";
	rlutil::setColor(rlutil::LIGHTCYAN);
	std::cout << GetPID();
	rlutil::setColor(rlutil::LIGHTGREEN);
	std::cout << ", GID: ";
	rlutil::setColor(rlutil::LIGHTCYAN);
	std::cout << GetGID();
	rlutil::setColor(rlutil::LIGHTGREEN);
	std::cout << ", File name: ";
	rlutil::setColor(rlutil::LIGHTCYAN);
	std::cout << GetFileName();
	rlutil::setColor(rlutil::LIGHTGREEN);
	std::cout << ", Burst time: ";
	rlutil::setColor(rlutil::YELLOW);
	std::cout << GetProcesBurstTime();
	rlutil::setColor(rlutil::LIGHTGREEN);
	std::cout << ", Page Table Size: ";
	rlutil::setColor(rlutil::YELLOW);
	std::cout << getPageTableSize();
	rlutil::setColor(rlutil::LIGHTGREEN);
	std::cout << ", State: ";
	rlutil::setColor(rlutil::WHITE);
	switch (this->GetState())
	{
	case 0:
		std::cout << "New";
		break;
	case 1:
		std::cout << "Ready";
		break;
	case 2:
		std::cout << "Waiting";
		break;
	case 3:
		std::cout << "Running";
		break;
	case 4:
		std::cout << "Terminated";
		break;
	default:
		std::cout << "WEIRDO! LOL";
		break;
	}
	rlutil::setColor(rlutil::LIGHTGREEN);
	std::cout << std::endl;
}
void PCB::SetInstructionCounter(int counter)
{
	this->instructionCounter = counter;
}
int PCB::GetInstructionCounter()
{
	return this->instructionCounter;
}
void PCB::addToMessages(SMS message)
{
	this->messages.push_back(message);
}
SMS PCB::getMessage()
{
	// return this->messages.pop_front();
	auto popped = this->messages.front();
	return popped;
}
void PCB::deleteMessage()
{
	this->messages.pop_front();
}
std::list<SMS> PCB::getMessages()
{
	return messages;
}