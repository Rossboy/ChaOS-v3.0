#ifndef MEMORYMANAGER__H
#define MEMORYMANAGER__H
#include "../Headers/Process.h"
#include <list>
#include <iostream>
#include <queue>

using namespace std;

class PCB;

struct Page;

struct FIFO_entry
{
	PCB * pcb;
	int pageNumber;

	FIFO_entry(PCB * pcb, int pageNumber)
	{
		this->pcb = pcb;
		this->pageNumber = pageNumber;
	}
};

enum class CharTable : char
{
	CTL = 218, //Corner Top Left
	CTR = 191, //Corner Top Right
	CBL = 192, //Corner Bottom Left
	CBR = 217, //Corner Bottom Right
	VLR = 195, //Vertical Line
	VLL = 180,
	VL = 179,
	HLT = 193,
	HLB = 194,
	HL = 196,
	Cross = 197
};

class MemoryManager
{
private:

	enum class MemoryManagerErrorCodes
	{
		OUT_OF_RANGE = 9,
		OUT_OF_MEMORY = 10
	};

	const int MEM_SIZE = 128; //rozmiar pamieci
	const int FRAME_SIZE = 16; //rozmiar ramki oraz stronicy
	const int FRAME_COUNT = MEM_SIZE / FRAME_SIZE; //ilosc ramek
	const int SWAP_FILE_FRAME_COUNT = 1024; //liosc ramek w pliku wymiany
	const int SWAP_FILE_SIZE = SWAP_FILE_FRAME_COUNT * FRAME_SIZE; //rozmiar plku wymiany
	char * RAM; //tablica symulujaca RAM
	char * swapFile; //tablica symulujaca plik wymiany
	list<int> freeMemoryFrames; //lista wolnych ramek w RAM
	list<int> freeSwapFileFrames; //lista wolnych ranek w pliku wymiany
	list<FIFO_entry> FIFO; //trzyma informacje ktory PCB wszedl pierwszy
	void swapPageFromSwapFileToMemory(PCB * pcb, int pageNr);
	void swapPageFromMemoryToSwapFile(PCB * pcb, int pageNr);
	void moveMemoryContentToSwapFile(int MemoryFrame, int SwapFileFrame);
	void moveSwapFileContentToMemory(int SwapFileFrame, int MemoryFrame);
	int calculateOffset(int addr_l);
	int calculatePageNumber(int addr_l);
	int calculatePhysicalAddress(PCB* pcb, int addr_l);
	int calculatePageTableSize(int sizeInBytes);
	void ensureFreeMemoryFrame();
	void addEntryToFIFO(FIFO_entry entry);
	char readMemory(PCB * pcb, int l_Addr);
	void writeMemory(PCB * pcb, int l_Addr, char element);
	void writePageContentToSwapFile(int frameNumber, string pageContent);
	int getFreeSwapFileFrame();
	int getFreeMemoryFrame();
	void ensurePageInMemory(PCB * pcb, int logicalAddress);
	bool isOutOfAddressSpace(PCB * pcb, int logicalAddress);
	void clearSwapFileFrame(int frameNumber);
public:
	void allocateMemory(PCB * pcb, string program, int size);
	string readString(PCB * pcb, int l_Addr);
	void writeString(PCB * pcb, int l_Addr, string content);
	void printMemoryConnetent(int nrToPrint = 0);
	void printPCBframes(PCB * pcb, bool onlyInRam = false);
	void printFIFO();
	void printFrame(int frameNr, int pageNr = -1);
	void printSFframe(int frameNr, int pageNr = -1);
	void deallocateMemory(PCB * pcb);
	MemoryManager();
	~MemoryManager();
};

#endif MEMORYMANAGER__H