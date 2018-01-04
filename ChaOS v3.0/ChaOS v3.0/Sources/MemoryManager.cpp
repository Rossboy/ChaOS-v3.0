#include "../Headers/MemoryManager.h"

void MemoryManager::swapPageFromMemoryToSwapFile(PCB * pcb, int pageNumber)
{
	Page * PageTable = pcb->getPageTable();

	if (freeSwapFileFrames.empty()) //je¿eli brak wolnych ramek w pliku wymiany, to mamy problem
	{
		pcb->errorCode = static_cast<int>(MemoryManagerErrorCodes::OUT_OF_MEMORY);; // nie powinno nigdy siê staæ
		return;
	}

	int newFreeMemoryFrame = PageTable[pageNumber].frameOccupied; //jezeli trafimy na strone w pamieci RAM to wysylamy ja do pliku wymiany
	freeMemoryFrames.push_back(newFreeMemoryFrame); //wolna ramke ram dodajemy do listy wolnych ramek

	int newOccupiedSwapFileFrame = getFreeSwapFileFrame();

	moveMemoryContentToSwapFile(newFreeMemoryFrame, newOccupiedSwapFileFrame); //przenosimy strone

	PageTable[pageNumber].frameOccupied = newOccupiedSwapFileFrame; //nowa ramka w SwapFile
	PageTable[pageNumber].inMemory = false; //strona nie jest w RAM
}

void MemoryManager::swapPageFromSwapFileToMemory(PCB * pcb, int pageNumber)
{
	Page * PageTable = pcb->getPageTable();
	ensureFreeMemoryFrame(); //sprawiamy, ¿e na pewno pojawi siê wolna ramka pamiêci

	int newOccupiedMemoryFrame = getFreeMemoryFrame();
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

int MemoryManager::calculatePageTableSize(int sizeInBytes)
{
	double frameSize = static_cast<double>(FRAME_SIZE);
	double entireSize = static_cast<double>(sizeInBytes);
	double result = ceil(entireSize / frameSize);
	return static_cast<int>(result);
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
	if (isOutOfAddressSpace(pcb, l_Addr))
	{
		pcb->errorCode = static_cast<int>(MemoryManagerErrorCodes::OUT_OF_RANGE);
		return ' ';
	}
	ensurePageInMemory(pcb, l_Addr);
	int p_Addr = calculatePhysicalAddress(pcb, l_Addr); //obliczamy adres fizyczny szukanej komorki pamieci
	return RAM[p_Addr];
}

void MemoryManager::allocateMemory(PCB * pcb, string program, int size)
{
	//ilosc stronic potrzebynch do alokacji
	int PageTableSize = calculatePageTableSize(size);
	Page * PageTable = new Page[PageTableSize];
	int stringBegin = 0;

	for (int pageNumber = 0; pageNumber < PageTableSize; pageNumber++) {
		//jezeli plik wymiany jest pelny
		if (freeSwapFileFrames.empty())
		{
			pcb->errorCode = static_cast<int>(MemoryManagerErrorCodes::OUT_OF_MEMORY);;//blad, nie da sie zaalokowac
			return;
		}
		int newOccupiedSwapFileFrame = getFreeSwapFileFrame();

		PageTable[pageNumber].frameOccupied = newOccupiedSwapFileFrame;
		PageTable[pageNumber].inMemory = false;

		string pageContent = program.substr(stringBegin, FRAME_SIZE);
		program.erase(stringBegin, FRAME_SIZE);
		writePageContentToSwapFile(newOccupiedSwapFileFrame, pageContent);
	}

	pcb->setPageTable(PageTable);
	pcb->setPageTableSize(PageTableSize);
}

string MemoryManager::readString(PCB * pcb, int l_Addr)
{
	int pageTableSize = pcb->getPageTableSize();
	string result = "";
	char byte;
	int addressLimiter = pageTableSize * FRAME_SIZE;
	//powtarzej dopoki adres logiczny nie wskazuje na ' ' i nie przekroczyl logicznej pamieci
	while (l_Addr < addressLimiter)
	{
		byte = readMemory(pcb, l_Addr);
		if (byte == ' ')
			break;
		result += byte;
		l_Addr++;
	}
	return result;
}

void MemoryManager::writeString(PCB * pcb, int l_Addr, string content)
{
	for (int contentIndex = 0; contentIndex < content.size(); contentIndex++)
	{
		writeMemory(pcb, l_Addr + contentIndex, content[contentIndex]);
	}
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
	if (pcb == nullptr)
	{
		cout << "Nie ma takiego procesu.\n";
		return;
	}

	Page * PCBpages = pcb->getPageTable();
	int PCBpagesSize = pcb->getPageTableSize();

	cout << " --- Pamiec zarezerwowana przez proces: " << pcb->GetPID() << " ---\n";
	for (int i = 0; i < PCBpagesSize; i++)
	{
		if (PCBpages[i].inMemory)
		{
			printFrame(PCBpages[i].frameOccupied, i); //Jezeli strona znajduje sie w pamieci
		}
		else if (!onlyInRam) //jezeli nie chcemy wypisywac ramek w SF
		{
			printSFframe(PCBpages[i].frameOccupied, i); //Jezeli strona znajduje sie w SF
		}
	}
}

void MemoryManager::printFIFO()
{
	cout << "Stan algorytmu FIFO. Element po lewej zostanie usuniety, w przypadku braku miejsca w pamieci." << endl
		<< "Format: [{PID_procesu},{numer strony}]" << endl;
	for (auto entry : FIFO)
	{
		cout << "[" << entry.pcb->GetPID() << "," << entry.pageNumber << "] ";
	}
	cout << endl;
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

bool MemoryManager::isAddressInAddressSpace(PCB * pcb, int logicalAddress)
{
	int pageNumber = calculatePageNumber(logicalAddress);
	if (pageNumber < pcb->getPageTableSize())
		return true;
	else
		return false;
}

bool MemoryManager::isAddressRangeInAddressSpace(PCB * pcb, int logicalAddress, int range)
{
	int endAddress = range + logicalAddress;
	while (logicalAddress < endAddress)
	{
		if (isOutOfAddressSpace(pcb, logicalAddress))
			return false;

		logicalAddress++;
	}
	return true;
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
	// wyswietla srodek ramki
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
	// wyswietla zawartosc RAMu
	addr = frameNr * FRAME_SIZE;
	cout << space << (char)CharTable::VL;
	for (int i = 0; i < FRAME_SIZE; i++)
	{
		printf("  %c  ", swapFile[addr + i]);
		cout << (char)CharTable::VL;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyswietla spod ramki
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

void MemoryManager::writeMemory(PCB * pcb, int l_Addr, char element)
{
	if (isOutOfAddressSpace(pcb, l_Addr))
	{
		pcb->errorCode = static_cast<int>(MemoryManagerErrorCodes::OUT_OF_RANGE);
		return;
	}
	ensurePageInMemory(pcb, l_Addr);
	int p_Addr = calculatePhysicalAddress(pcb, l_Addr);
	RAM[p_Addr] = element;
}

void MemoryManager::writePageContentToSwapFile(int frameNumber, string pageContent)
{
	int swapFileIndex = frameNumber * FRAME_SIZE;
	int stringIndex = 0;

	while (stringIndex < pageContent.size())
	{
		swapFile[swapFileIndex] = pageContent[stringIndex];
		swapFileIndex++;
		stringIndex++;
	}
}

int MemoryManager::getFreeSwapFileFrame()
{
	int freeFrame = freeSwapFileFrames.front();
	freeSwapFileFrames.pop_front();
	return freeFrame;
}

int MemoryManager::getFreeMemoryFrame()
{
	int freeFrame = freeMemoryFrames.front();
	freeMemoryFrames.pop_front();
	return freeFrame;
}

void MemoryManager::ensurePageInMemory(PCB * pcb, int logicalAddress)
{
	Page * PageTable = pcb->getPageTable();

	int pageNumber = calculatePageNumber(logicalAddress); //liczymy numer strony i bajt w tej stronie

	if (PageTable[pageNumber].inMemory == false) //sprawdzamy czy strona jest poza RAM
	{
		swapPageFromSwapFileToMemory(pcb, pageNumber); //nie ma strony w ramie wiec trzeba ja sprowadzic
	}
}

bool MemoryManager::isOutOfAddressSpace(PCB * pcb, int logicalAddress)
{
	int pageNumber = calculatePageNumber(logicalAddress);
	if (pageNumber >= pcb->getPageTableSize())
		return true;
	else
		return false;
}

void MemoryManager::clearSwapFileFrame(int frameNumber)
{
	int frameIndex = frameNumber * FRAME_SIZE;
	int frameEndIndex = frameIndex + FRAME_SIZE;
	while (frameIndex < frameEndIndex)
	{
		swapFile[frameIndex] = ' ';
		frameIndex++;
	}
}

void MemoryManager::deallocateMemory(PCB * pcb)
{
	Page * pageTable = pcb->getPageTable();
	int pageTableSize = pcb->getPageTableSize();
	int pageNumber = 0;

	while (pageNumber < pageTableSize) //usuwanie informacji z pamieci i stronic
	{
		if (pageTable[pageNumber].inMemory) //wysylamy ramki z pamieci do pliku wymiany
		{
			swapPageFromMemoryToSwapFile(pcb, pageNumber);
		}

		FIFO.remove_if([pcb](const FIFO_entry &victim) {return victim.pcb == pcb; }); //usuwamy z kolejki FIFO pcb

		int frameToFree = pageTable[pageNumber].frameOccupied;
		clearSwapFileFrame(frameToFree);
		freeSwapFileFrames.push_back(frameToFree);

		pageNumber++;
	}

	delete[] pageTable;

	pcb->setPageTable(nullptr);
	pcb->setPageTableSize(0);
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
