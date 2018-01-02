#include "../Headers/MemoryManager.h"

void MemoryManager::swapPageFromMemoryToSwapFile(PCB * pcb, int pageNumber)
{
	Page * PageTable = pcb->getPageTable();

	if (freeSwapFileFrames.empty()) //je¿eli brak wolnych ramek w pliku wymiany, to mamy problem
	{
		pcb->errorCode = 10; // nie powinno nigdy siê staæ
		return;
	}

	int newFreeMemoryFrame = PageTable[pageNumber].frameOccupied; //jezeli trafimy na strone w pamieci RAM to wysylamy ja do pliku wymiany
	freeMemoryFrames.push_back(newFreeMemoryFrame); //wolna ramke ram dodajemy do listy wolnych ramek
	int newOccupiedSwapFileFrame = freeSwapFileFrames.front(); //wybieramy wolna ramke z listy ramek pliku wymiany
	freeSwapFileFrames.pop_front();
	moveMemoryContentToSwapFile(newFreeMemoryFrame, newOccupiedSwapFileFrame); //przenosimy strone

	PageTable[pageNumber].frameOccupied = newOccupiedSwapFileFrame; //nowa ramka w SwapFile
	PageTable[pageNumber].inMemory = false; //strona nie jest w RAM
}

void MemoryManager::swapPageFromSwapFileToMemory(PCB * pcb, int pageNumber)
{
	Page * PageTable = pcb->getPageTable();
	ensureFreeMemoryFrame(); //sprawiamy, ¿e na pewno pojawi siê wolna ramka pamiêci

	int newOccupiedMemoryFrame = freeMemoryFrames.front(); //wybieramy wolna ramke RAM
	freeMemoryFrames.pop_front();
	int newFreeSwapFileFrame = PageTable[pageNumber].frameOccupied; //zmiana stronicy z SF do RAM
	freeSwapFileFrames.push_front(newFreeSwapFileFrame);
	moveSwapFileContentToMemory(newFreeSwapFileFrame, newOccupiedMemoryFrame); //ustawiamy indeks aktualnej strony w RAM

	PageTable[pageNumber].frameOccupied = newOccupiedMemoryFrame; //nowa ramka w RAM
	PageTable[pageNumber].inMemory = true; //strona jest w RAM
	
	addEntryToFIFO(FIFO_entry(pcb, pageNumber)); //dodanie procesu do FIFO
}

void MemoryManager::moveMemoryContentToSwapFile(int MemoryFrame, int SwapFileFrame)
{
	int swapFileIndex = SwapFileFrame * FRAME_SIZE;
	int memoryIndex = MemoryFrame * FRAME_SIZE;
	int swapFileEndIndex = swapFileIndex + FRAME_SIZE;
	int memoryEndIndex = memoryIndex + FRAME_SIZE;

	while (memoryIndex < memoryEndIndex && swapFileIndex < swapFileEndIndex)
	{
		swapFile[swapFileIndex] = RAM[memoryIndex];
		RAM[memoryIndex] = ' ';
		memoryIndex++;
		swapFileIndex++;
	}
}

void MemoryManager::moveSwapFileContentToMemory(int SwapFileFrame, int MemoryFrame)
{
	int swapFileIndex = SwapFileFrame * FRAME_SIZE;
	int memoryIndex = MemoryFrame * FRAME_SIZE;
	int swapFileEndIndex = swapFileIndex + FRAME_SIZE;
	int memoryEndIndex = memoryIndex + FRAME_SIZE;

	while (memoryIndex < memoryEndIndex && swapFileIndex < swapFileEndIndex)
	{
		RAM[memoryIndex] = swapFile[swapFileIndex];
		swapFile[swapFileIndex] = ' ';
		memoryIndex++;
		swapFileIndex++;
	}
}

int MemoryManager::calculateOffset(int addr_l)
{
	return addr_l % FRAME_SIZE;
}

int MemoryManager::calculatePageNumber(int addr_l)
{
	return addr_l / FRAME_SIZE;
}

int MemoryManager::calculatePhysicalAddress(PCB* pcb, int addr_l)
{
	Page * PageTable = pcb->getPageTable();
	int pageNumber = calculatePageNumber(addr_l);
	int offset = calculateOffset(addr_l);
	int frameNumber = PageTable[pageNumber].frameOccupied;
	return frameNumber * FRAME_SIZE + offset;
}

void MemoryManager::ensureFreeMemoryFrame()
{
	while (freeMemoryFrames.empty())
	{
		//nie ma wolnych ramek wiec usuwamy metoda FIFO najstarszy PCB
		FIFO_entry victim = FIFO.front();
		FIFO.pop_front();
		swapPageFromMemoryToSwapFile(victim.pcb, victim.pageNumber);
	}
}

void MemoryManager::addEntryToFIFO(FIFO_entry entry)
{
	FIFO.push_back(entry);
}

char MemoryManager::readMemory(PCB * pcb, int l_Addr)
{
	Page * PageTable = pcb->getPageTable();
	int PCBpagesSize = pcb->getPageTableSize();
	//liczymy numer strony i bajt w tej stronie
	int pageNumber = l_Addr / FRAME_SIZE;
	int offset = l_Addr % FRAME_SIZE;
	//sprawdzamy czy strona jest w RAM
	if (PageTable[pageNumber].inMemory == false)
	{
		//nie ma strony w ramie wiec trzeba ja sprowadzic
		swapPageFromSwapFileToMemory(pcb, pageNumber);
	}
	//sprawdzamy w ktorej ramce jest poszukiwana strona
	int page = PageTable[pageNumber].frameOccupied;
	//obliczamy adres fizyczny szukanej komorki pamieci
	int p_Addr = page * FRAME_SIZE + offset;
	return RAM[p_Addr];
}

bool MemoryManager::allocateMemory(PCB * pcb, string program, int size)
{
	//ilosc stronic potrzebynch do alokacji
	int PCBpagesSize = ceil((double)size / (double)FRAME_SIZE);
	Page * PCBpages = new Page[PCBpagesSize];
	int base = 0;
	for (int i = 0; i < PCBpagesSize; i++) {
		//jezeli plik wymiany jest pelny
		if (freeSwapFileFrames.empty())
		{
			//blad, nie da sie zaalokowac
			return false;
		}
		int freeFrame = freeSwapFileFrames.front();
		freeSwapFileFrames.pop_front();
		PCBpages[i].frameOccupied = freeFrame;
		PCBpages[i].inMemory = false;
		string page = program.substr(base, FRAME_SIZE);
		program.erase(base, FRAME_SIZE);
		for (int j = freeFrame * FRAME_SIZE, k = 0; j < j + FRAME_SIZE && k < page.size(); j++, k++)
		{
			swapFile[j] = page[k];
		}
	}
	pcb->setPages(PCBpages);
	pcb->setPagesSize(PCBpagesSize);
	return true;
}

string MemoryManager::readUntilSpace(PCB * pcb, int & l_Addr)
{
	int PCBpagesSize = pcb->getPageTableSize();
	string result = "";
	char byte = '0';
	bool quote = false;
	//powtarzej dopoki adres logiczny nie wskazuje na ' ' i nie przekroczyl logicznej pamieci
	while (l_Addr < PCBpagesSize*FRAME_SIZE)
	{
		byte = readMemory(pcb, l_Addr);
		l_Addr++;
		if (byte == '"' && quote == false)
		{
			quote = true;
		}
		else if (byte == '"' && quote == true)
		{
			quote = false;
		}
		if (byte == ' ' && quote == false)
			break;
		result += byte;
	}
	return result;
}

void MemoryManager::printMemoryConnetent(int nrToPrint)
{
	if (nrToPrint <= 0 || nrToPrint > FRAME_COUNT)
		nrToPrint = FRAME_COUNT;

	for (int i = 0; i < nrToPrint; i++)
	{
		printFrame(i);
	}
}

void MemoryManager::printPCBframes(PCB * pcb, bool onlyInRam)
{
	Page * PCBpages = pcb->getPageTable();
	int PCBpagesSize = pcb->getPageTableSize();

	cout << " --- Pamiec zarezerwowana przez proces: " << pcb->GetPID() << " ---\n";
	for (int i = 0; i < PCBpagesSize; i++)
	{
		if (PCBpages[i].inMemory)
		{
			//Jezeli strona znajduje sie w pamieci
			printFrame(PCBpages[i].frameOccupied, i);
		}
		//jezeli nie chcemy wypisywac ramek w SF
		else if (!onlyInRam)
		{
			//Jezeli strona znajduje sie w SF
			printSFframe(PCBpages[i].frameOccupied, i);
		}
	}
}

void MemoryManager::printFrame(int frameNr, int pageNumber)
{
	int addr = frameNr * FRAME_SIZE;
	string space = "        ";
	cout << "Ramka nr: " << frameNr;
	if (pageNumber >= 0)
	{
		cout << ", zawiera strone nr: " << pageNumber;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyzwietla gore ramki
	cout << space << (char)CharTable::CTL;
	for (int i = 0; i < FRAME_SIZE - 1; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << (char)CharTable::HL;
		}
		cout << (char)CharTable::HLB;
	}
	for (int j = 0; j < 3; j++)
	{
		cout << (char)CharTable::HL;
	}
	cout << (char)CharTable::CTR << endl;
	// ----------------------------------------------------------------------
	// wyzwietla indeksy RAM
	cout << space << (char)CharTable::VL;
	for (int i = 0; i < FRAME_SIZE; i++)
	{
		if (addr + i < 10)
			printf(" %d ", addr + i);
		else
			printf("%3d", addr + i);
		cout << (char)CharTable::VL;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyzwietla srodek ramki
	cout << space << (char)CharTable::VLR;
	for (int i = 0; i < FRAME_SIZE - 1; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << (char)CharTable::HL;
		}
		cout << (char)CharTable::Cross;
	}
	for (int j = 0; j < 3; j++)
	{
		cout << (char)CharTable::HL;
	}
	cout << (char)CharTable::VLL << endl;
	// ----------------------------------------------------------------------
	// wyzwietla zawartosc RAMu
	addr = frameNr * FRAME_SIZE;
	cout << space << (char)CharTable::VL;
	for (int i = 0; i < FRAME_SIZE; i++)
	{
		printf(" %c ", RAM[addr + i]);
		cout << (char)CharTable::VL;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyzwietla spod ramki
	cout << space << (char)CharTable::CBL;
	for (int i = 0; i < FRAME_SIZE - 1; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << (char)CharTable::HL;
		}
		cout << (char)CharTable::HLT;
	}
	for (int j = 0; j < 3; j++)
	{
		cout << (char)CharTable::HL;
	}
	cout << (char)CharTable::CBR << endl;
}

void MemoryManager::printSFframe(int frameNr, int pageNumber)
{
	int addr = frameNr * FRAME_SIZE;
	string space = "        ";
	cout << "Ramka pliku wymiany nr: " << frameNr;
	if (pageNumber >= 0)
	{
		cout << ", zawiera strone nr: " << pageNumber;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyzwietla gore ramki
	cout << space << (char)CharTable::CTL;
	for (int i = 0; i < FRAME_SIZE - 1; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			cout << (char)CharTable::HL;
		}
		cout << (char)CharTable::HLB;
	}
	for (int j = 0; j < 5; j++)
	{
		cout << (char)CharTable::HL;
	}
	cout << (char)CharTable::CTR << endl;
	// ----------------------------------------------------------------------
	// wyzwietla indeksy RAM
	cout << space << (char)CharTable::VL;
	for (int i = 0; i < FRAME_SIZE; i++)
	{
		if (addr + i < 10)
			printf("  %d  ", addr + i);
		else if (addr + i < 100)
			printf("  %d ", addr + i);
		else if (addr + i < 1000)
			printf(" %d ", addr + i);
		else
			printf("%5d", addr + i);
		cout << (char)CharTable::VL;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyzwietla srodek ramki
	cout << space << (char)CharTable::VLR;
	for (int i = 0; i < FRAME_SIZE - 1; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			cout << (char)CharTable::HL;
		}
		cout << (char)CharTable::Cross;
	}
	for (int j = 0; j < 5; j++)
	{
		cout << (char)CharTable::HL;
	}
	cout << (char)CharTable::VLL << endl;
	// ----------------------------------------------------------------------
	// wyzwietla zawartosc RAMu
	addr = frameNr * FRAME_SIZE;
	cout << space << (char)CharTable::VL;
	for (int i = 0; i < FRAME_SIZE; i++)
	{
		printf("  %c  ", swapFile[addr + i]);
		cout << (char)CharTable::VL;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyzwietla spod ramki
	cout << space << (char)CharTable::CBL;
	for (int i = 0; i < FRAME_SIZE - 1; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			cout << (char)CharTable::HL;
		}
		cout << (char)CharTable::HLT;
	}
	for (int j = 0; j < 5; j++)
	{
		cout << (char)CharTable::HL;
	}
	cout << (char)CharTable::CBR << endl;
}

bool MemoryManager::writeMemory(PCB * pcb, int l_Addr, char element)
{
	int PCBpagesSize = pcb->getPageTableSize();
	Page * PCBpages = pcb->getPageTable();
	//liczymy numer strony i bajt w tej stronie
	int pageNumber = l_Addr / FRAME_SIZE;
	int offset = l_Addr % FRAME_SIZE;
	//sprawdzamy czy adres logiczny jest w przestrzeni adresowej procesu
	if (pageNumber >= PCBpagesSize)
	{
		//adres jest z poza przestrzni adresowej procesu
		return false;
	}
	if (PCBpages[pageNumber].inMemory == false)
	{
		//nie ma strony w ramie wiec trzeba ja sprowadzic
		swapPageFromSwapFileToMemory(pcb, pageNumber);
	}
	int page = PCBpages[pageNumber].frameOccupied;
	int p_Addr = page * FRAME_SIZE + offset;
	RAM[p_Addr] = element;
	return true;
}

bool MemoryManager::deallocateMemory(PCB * pcb)
{
	Page * PCBpages = pcb->getPageTable();
	int PCBpagesSize = pcb->getPageTableSize();
	//usuwanie informacji z pamieci i stronic
	for (int i = 0; i < PCBpagesSize; i++)
	{
		int frame = PCBpages[i].frameOccupied;
		int begFrameId = frame * FRAME_SIZE;
		int endFrameId = begFrameId + FRAME_SIZE;
		//wysylamy wszystkie ramki do SF
		for (int j = 0; j < pcb->getPageTableSize(); j++)
		{
			if (pcb->getPageTable()[j].inMemory == true)
			{
				swapPageFromMemoryToSwapFile(pcb, j);
			}
		}
		//usuwamy z kolejki FIFO pcb, nie mozemy juz z niego kozystac
		FIFO.remove_if([pcb](const FIFO_entry &victim) {return victim.pcb == pcb; });
		//FIFO.remove(pcb);
		for (begFrameId; begFrameId < endFrameId; begFrameId++)
		{
			swapFile[begFrameId] = ' ';
		}
		freeSwapFileFrames.push_back(frame);
	}

	delete[] PCBpages;

	pcb->setPages(nullptr);
	pcb->setPagesSize(0);
	return false;
}

MemoryManager::MemoryManager()
{
	RAM = new char[MEM_SIZE];
	swapFile = new char[SWAP_FILE_SIZE];
	for (int i = 0; i < MEM_SIZE; i++)
	{
		RAM[i] = ' ';
	}
	for (int i = 0; i < FRAME_COUNT; i++)
		freeMemoryFrames.push_back(i);
	for (int i = 0; i < SWAP_FILE_SIZE; i++)
	{
		swapFile[i] = ' ';
	}
	for (int i = 0; i < SWAP_FILE_FRAME_COUNT; i++)
	{
		freeSwapFileFrames.push_back(i);
	}
}

MemoryManager::~MemoryManager()
{
	delete[] RAM;
	delete[] swapFile;
}
