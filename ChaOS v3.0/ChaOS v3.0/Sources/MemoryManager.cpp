#include "../Headers/MemoryManager.h"

void MemoryManager::swapToFile(PCB * pcb)
{
	pair<int, bool> * PCBpages = pcb->getPages();
	int PCBpagesSize = pcb->getPagesSize();
	for (int i = 0; i < PCBpagesSize; i++)
	{
		//jezeli trafimy na strone w pamieci RAM to wysylamy ja do pliku wymiany
		if (PCBpages[i].second == true)
		{
			//uwalniamy ramke od PCB
			int freeRAMframe = PCBpages[i].first;
			//wybieramy wolna ramke z listy ramek pliku wymiany
			int newSFframe = freeSwapFileFrames.front();
			freeSwapFileFrames.pop_front();
			//przenosimy strone
			int Id_SF = newSFframe*FRAME_SIZE;
			int endIdSF = Id_SF + FRAME_SIZE;
			int Id_RAM = freeRAMframe*FRAME_SIZE;
			int endIdRAM = Id_RAM + FRAME_SIZE;
			while(Id_SF < endIdSF && Id_RAM < endIdRAM)
			{
				swapFile[Id_SF] = RAM[Id_RAM];
				RAM[Id_RAM] = ' ';
				Id_SF++;
				Id_RAM++;
			}
			//wolna ramke ram dodajemy do listy wolnych ramek
			freeRAMFrames.push_back(freeRAMframe);
			//podmieniamy informacje o stronie w PCB
			PCBpages[i].first = newSFframe; //nowa ramka w SwapFile
			PCBpages[i].second = false; //strona nie jest w RAM
		}
	}
}

void MemoryManager::swapToRAM(PCB * pcb, int pageNr)
{
	pair<int, bool> * PCBpages = pcb->getPages();

	//sprawdzamy czy mamy wolne ramki w RAM
	while (freeRAMFrames.empty())
	{
		//nie ma wolnych ramek wiec usuwamy metoda FIFO najstarszy PCB
		PCB * victim = FIFOlist.front();
		FIFOlist.pop_front();
		swapToFile(victim);
	}
	//wybieramy wolna ramke RAM
	int newRAMframe = freeRAMFrames.front();
	freeRAMFrames.pop_front();
	int freeSFframe = PCBpages[pageNr].first;
	//zmiana stronicy z SF do RAM
	int Id_SF = freeSFframe*FRAME_SIZE;
	int endIdSF = Id_SF + FRAME_SIZE;
	int Id_RAM = newRAMframe*FRAME_SIZE;
	int endIdRAM = Id_RAM + FRAME_SIZE;
	while (Id_SF < endIdSF && Id_RAM < endIdRAM)
	{
		RAM[Id_RAM] = swapFile[Id_SF];
		swapFile[Id_SF] = ' ';
		Id_SF++;
		Id_RAM++;
	}
	//ustawiamy indeks aktualnej strony w RAM
	PCBpages[pageNr].first = newRAMframe; //nowa ramka w RAM
	PCBpages[pageNr].second = true; //strona jest w RAM
	//dodajemy zwolniona ramke SF do listy pustych ramek SF
	freeSwapFileFrames.push_front(freeSFframe);
	//proces zajmuje teraz ostatnie miejsce w FIFO
	FIFOlist.push_back(pcb);
}

char MemoryManager::readMemory(PCB * pcb, int l_Addr)
{
	pair<int, bool> * PCBpages = pcb->getPages();
	int PCBpagesSize = pcb->getPagesSize();
	//liczymy numer strony i bajt w tej stronie
	int pageNr = l_Addr / FRAME_SIZE;
	int offset = l_Addr % FRAME_SIZE;
	//sprawdzamy czy strona jest w RAM
	if (PCBpages[pageNr].second == false)
	{
		//nie ma strony w ramie wiec trzeba ja sprowadzic
		swapToRAM(pcb, pageNr);
	}
	//sprawdzamy w ktorej ramce jest poszukiwana strona
	int page = PCBpages[pageNr].first;
	//obliczamy adres fizyczny szukanej komorki pamieci
	int p_Addr = page*FRAME_SIZE + offset;
	return RAM[p_Addr];
}

bool MemoryManager::allocateMemory(PCB * pcb, string program, int size)
{
	//ilosc stronic potrzebynch do alokacji
	int PCBpagesSize = ceil((double)size / (double)FRAME_SIZE);
	pair<int, bool> * PCBpages = new pair<int,bool>[PCBpagesSize];
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
		PCBpages[i].first = freeFrame;
		PCBpages[i].second = false;
		string page = program.substr(base, FRAME_SIZE);
		program.erase(base, FRAME_SIZE);
		for (int j = freeFrame*FRAME_SIZE, k = 0; j < j + FRAME_SIZE && k < page.size(); j++, k++)
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
	int PCBpagesSize = pcb->getPagesSize();
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
	pair<int, bool> * PCBpages = pcb->getPages();
	int PCBpagesSize = pcb->getPagesSize();
	
	cout << " --- Pamiec zarezerwowana przez proces: " << pcb->getName() << " ---\n";
	for (int i = 0; i < PCBpagesSize; i++)
	{
		if (PCBpages[i].second)
		{
			//Jezeli strona znajduje sie w pamieci
			printFrame(PCBpages[i].first, i);
		}
		//jezeli nie chcemy wypisywac ramek w SF
		else if(!onlyInRam)
		{
			//Jezeli strona znajduje sie w SF
			printSFframe(PCBpages[i].first, i);
		}
	}
}

void MemoryManager::printFrame(int frameNr, int pageNr)
{
	int addr = frameNr*FRAME_SIZE;
	string space = "        ";
	cout << "Ramka nr: " << frameNr;
	if (pageNr >= 0)
	{
		cout << ", zawiera strone nr: " << pageNr;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyzwietla gore ramki
	cout << space <<(char)CharTable::CTL;
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
		if(addr + i <10)
			printf(" %d ", addr + i);
		else
			printf("%3d", addr + i);
		cout << (char)CharTable::VL;
	}
	cout << endl;
	// ----------------------------------------------------------------------
	// wyzwietla srodek ramki
	cout << space << (char)CharTable::VLR;
	for (int i = 0; i < FRAME_SIZE-1; i++)
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
	cout << (char)CharTable::VLL <<endl;
	// ----------------------------------------------------------------------
	// wyzwietla zawartosc RAMu
	addr = frameNr*FRAME_SIZE;
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

void MemoryManager::printSFframe(int frameNr, int pageNr)
{
	int addr = frameNr*FRAME_SIZE;
	string space = "        ";
	cout << "Ramka pliku wymiany nr: " << frameNr;
	if (pageNr >= 0)
	{
		cout << ", zawiera strone nr: " << pageNr;
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
		if (addr + i <10)
			printf("  %d  ", addr + i);
		else if(addr + i <100)
			printf("  %d ", addr + i);
		else if (addr + i <1000)
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
	addr = frameNr*FRAME_SIZE;
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
	int PCBpagesSize = pcb->getPagesSize();
	//liczymy numer strony i bajt w tej stronie
	int pageNr = l_Addr / FRAME_SIZE;
	int offset = l_Addr % FRAME_SIZE;
	//sprawdzamy czy adres logiczny jest w przestrzeni adresowej procesu
	if (pageNr >= PCBpagesSize)
	{
		//adres jest z poza przestrzni adresowej procesu
		return false;
	}
	int p_addr = pageNr + offset;
	return true;
}

bool MemoryManager::deallocateMemory(PCB * pcb)
{
	pair<int, bool> * PCBpages = pcb->getPages();
	int PCBpagesSize = pcb->getPagesSize();
	//usuwanie informacji z pamieci i stronic
	for (int i = 0; i < PCBpagesSize; i++)
	{
		int frame = PCBpages[i].first;
		int begFrameId = frame*FRAME_SIZE;
		int endFrameId = begFrameId + FRAME_SIZE;
		//wysylamy wszystkie ramki do SF
		swapToFile(pcb);
		//usuwamy z kolejki FIFO pcb, nie mozemy juz z niego kozystac
		FIFOlist.remove(pcb);
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
	for (int i = 0; i<FRAME_COUNT; i++)
		freeRAMFrames.push_back(i);
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
