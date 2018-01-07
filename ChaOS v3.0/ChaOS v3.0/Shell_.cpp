// Shell_.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <conio.h>
#include <cstring>
#include <Windows.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "Headers\Process.h"
#include "Headers\ProcessesManager.h"
#include "Headers\Interpreter.h"
#include "Headers\MemoryManager.h"
#include "Headers\ChaOS_filesystem.h"
#include "Headers\ConditionVariable.h"
#include "Headers\Siec.h"
#include "Headers\ProcessScheduler.h"
#include "Headers\table_maker.h"
#include "locale.h"
#include "Headers\rlutil.h"

using namespace std;

PCB* ActiveProcess;

ProcessesManager *pm;
ProcessScheduler *ps;
MemoryManager *mm;
Siec *s;
ChaOS_filesystem *fs;
Interpreter *i;

std::vector<std::string> ErrorsTab{

	//Proszę o dopisywanie następnika z własnym opisem błędu. W komentarzu piszcie nr żebyśmy się nie pogubili.
	"Brak bledów w obsludze.",//(0)
	"Przykladowy blad który opisuje w tym miejscu jakas osoba. Możecie go zmienic na wlasny.", //(1)
	"Brak wolnych sektorów na dysku.",//(2)
	"Obiekt o podanej nazwie juz istnieje.",//(3)
	"Obiekt o podanej nazwie nie zostal odnaleziony.",//(4)
	"Brak otwartego pliku do wykonania operacji.",//(5)
	"Podany plik musi byc otwarty, by wykonac operacje.", //(6)
	"Aktywny proces ma juz otwarty plik.", //(7)
	"Typ pliku, ktory zostal podany jest inny niz plik lub folder", //(8)
	"Odwolanie do pamieci spoza obszaru adresowego procesu.", //(9)
	"Brak wolnej pamieci.", //(10)
	"Usuwany folder musi być pusty.", //(11)
	"Podany tekst jest zbyt dlugi, by zapisac go do pliku.", //(12)
	"" //(13)
};

class main_loop {
private:
	vector<string> command;
	string line;
	void choose_function();
	void initialize();
	void prepare_string();
	void help();


public:
	main_loop();
	void run();
	void big_letter(vector<string> &pom);
};

void main_loop::big_letter(vector<string> &pom) {
	std::transform(pom[0].begin(), pom[0].end(), pom[0].begin(), ::toupper);
}

void main_loop::initialize() {
	//uruchmienie logo
	rlutil::locate(2, 10);
	cout << "        /\\\\\\\\\\\\\\\\\\  /\\\\\\                              /\\\\\\\\\          /\\\\\\\\\\\\\\\\\\\\\\           " << endl
		 << "      /\\\\\\////////  \\/\\\\\\                            /\\\\\\///\\\\\\      /\\\\\\/////////\\\\\\        " << endl
		 << "     /\\\\\\/           \\/\\\\\\                          /\\\\\\/  \\///\\\\\\   \\//\\\\\\      \\///        " << endl
		 << "     /\\\\\\             \\/\\\\\\          /\\\\\\\\\\\\\\\\\\     /\\\\\\      \\//\\\\\\   \\////\\\\\\              " << endl
		 << "     \\/\\\\\\             \\/\\\\\\\\\\\\\\\\\\\\  \\////////\\\\\\   \\/\\\\\\       \\/\\\\\\      \\////\\\\\\          " << endl
		 << "      \\//\\\\\\            \\/\\\\\\/////\\\\\\   /\\\\\\\\\\\\\\\\\\\\  \\//\\\\\\      /\\\\\\          \\////\\\\\\      "<<endl
		 << "        \\///\\\\\\          \\/\\\\\\   \\/\\\\\\  /\\\\\\/   \\\\\\   \\///\\\\\\  /\\\\\\     /\\\\\\      \\//\\\\\\    "<<endl
		 << "           \\////\\\\\\\\\\\\\\\\\\ \\/\\\\\\   \\/\\\\\\ \\//\\\\  \\\\\\/\\\\    \\///\\\\\\\\\\/     \\///\\\\\\\\\\\\\\\\\\\\\\/    "<<endl
		 << "               \\/////////  \\///    \\///   \\////////\\//       \\/////         \\///////////     " << endl<<endl<<endl;

	mm = new MemoryManager();
	ps = new ProcessScheduler();
	pm = new ProcessesManager();
	i = new Interpreter();
	fs = new ChaOS_filesystem();
	s = new Siec();
	/*
	rlutil::hidecursor();
	char a = 177, b = 219;
	cout << "\t\t\t\t\t";
	for (int i = 0; i <= 25; i++) 
		cout << a;
	rlutil::msleep(150);
	cout << "\r";
	cout << "\t\t\t\t\t";
	for (int i = 0; i <= 25; i++) {
		cout << b;
		rlutil::msleep(100);
	}
	system("cls");*/
	rlutil::locate(0, 0);
	rlutil::showcursor();
}


void main_loop::prepare_string() {
	command.clear();
	size_t pos = 0;
	string delimiter = " ";
	string token;
	int pom = 0;
	while (((pos = line.find(delimiter)) != string::npos + 1) && pom < 3) {
		token = line.substr(0, pos);
		command.push_back(token);
		line.erase(0, pos + delimiter.length());
		pom++;
		if (token == "ap" || token == "write") {
			//cout << line << endl;
			command.push_back(line);
			break;
		}
	}
}


void main_loop::run() {
	initialize();
	while (true) {
		try {
			rlutil::setColor(rlutil::WHITE);
			cout << "> ";
			getline(cin, line);
			rlutil::setColor(rlutil::GREEN);
			if (line == "close")break;
			else {
				prepare_string();
				choose_function();
			}
		}
		catch (...) {
			rlutil::setColor(rlutil::RED);
			cout << "Bledna nazwa rozkazu. \nWpisz \"help\" zeby zobaczyc liste rozkazow\n";
			rlutil::setColor(rlutil::GREEN);
		}
	}
}

main_loop::main_loop() {

}



void main_loop::choose_function() {
	if (command[0] == "help") {
		help();
	}
	else if (command[0] == "xD") {
		rlutil::setColor(rlutil::YELLOW);
		cout << "           /$$$$$$$ " << endl
			<< "          | $$__  $$" << endl
			<< " /$$   /$$| $$  \\ $$" << endl
			<< "|  $$ /$$/| $$  | $$" << endl
			<< " \\  $$$$/ | $$  | $$" << endl
			<< "  >$$  $$ | $$  | $$" << endl
			<< " /$$/\\  $$| $$$$$$$/" << endl
			<< "|__/  \\__/|_______/ " << endl;
		cout << endl;
		rlutil::setColor(rlutil::GREEN);
	}
	else if (command[0] == "cf") {
		/*
		char tab[1024];
		strcpy_s(tab, command[1].c_str());
		if (command[2] == "plik") {
			fs->create(tab, ChaOS_filesystem::type::file);
		}
		else if (command[2] == "folder") {
			fs->create(tab, ChaOS_filesystem::type::dir);
		}
		else {
			throw 1;
		}
	*/
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "rf") {
		/*char tab[1024];
		strcpy_s(tab, command[1].c_str());
		fs->remove(tab);
		*/ 
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
		//nie wiemy czy wyrzuca błąd
	}
	else if (command[0] == "ld") {
		//fs->listDirectory(ActiveProcess);
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "cd") {
		char tab[1024];
		strcpy_s(tab, command[1].c_str());
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		//fs->changeDirectory(tab);
		cout << endl;
	}
	else if (command[0] == "rd") {
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "bd") {
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "ren") {
		/*
		char tab[1024];
		strcpy_s(tab, command[1].c_str());
		char tab2[1024];
		strcpy_s(tab2, command[2].c_str());
		fs->rename(tab, tab2);
		*/
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "of") {
		/*
		char tab[1024];
		strcpy_s(tab, command[1].c_str());
		fs->openFile(tab);
		*/		
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "clf") {
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "pds") {
		std::cout << fs->printDiskStats() << std::endl;
		cout << endl;
	}
	else if (command[0] == "pdss") {
		int pom = stoi(command[1]);
		std::cout << fs->printSectorsChain(pom) << std::endl<<endl;
	}
	else if (command[0] == "read") {
		//fs->readFile();
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "ap") {
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "write") {
		//fs->writeFile(command[1]);
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "cp") {
		//pm->createProcess(command[1], stoi(command[2]));
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "kp") {
		//pm->killProcess(stoi(command[1]));
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << endl;
	}
	else if (command[0] == "dap") {
		pm->displayAllProcesses();
		cout << endl;
	}
	else if (command[0] == "dwp") {
		pm->displayWaitingProcesses();
		cout << endl;
	}
	else if (command[0] == "drp") {
		pm->displayReadyProcesses();
		cout << endl;
	}
	else if (command[0] == "pap") {
		if (ActiveProcess == nullptr)
		{
			cout << "Albert, tu powinien byc proces bezczynnosci, a jest nullptr!\n";
		}
		else
		{
			cout << "Aktywny process to:\n";
			cout << "PID: " << ActiveProcess->GetPID() << "\n";
			cout << "GID: " << ActiveProcess->GetGID() << "\n";
			cout << "Program: " << ActiveProcess->GetFileName() << "\n";
			cout << "Ilosc stron: " << ActiveProcess->getPageTableSize() << "\n";
			cout << "Licznik rozkazow: " << ActiveProcess->GetInstructionCounter() << "\n";
			cout << "Burst Time: " << ActiveProcess->GetProcesBurstTime() << "\n";
			cout << "Stan: " << ActiveProcess->GetState() << "\n";
			cout << endl;
		}
	}
	else if (command[0] == "mem") {
		if (command[2] == "-a") {
			mm->printPCBframes(pm->findPCBbyPID(stoi(command[1])), false);
		}
		else if (command[2] == "-r") {
			mm->printPCBframes(pm->findPCBbyPID(stoi(command[1])), true);
		}
		else if (command[2] == "-t") {
			mm->printPageTable(pm->findPCBbyPID(stoi(command[1])));
		}
		else if (command[2] == "-f") {
			mm->printFIFO();
		}
		else {
			mm->printMemoryConnetent();
		}
		cout << endl;
	}
	else if (command[0] == "sms") {
		s->wyswietlwiad();
		cout << endl;
	}
	else if (command[0] == "go") {
		ps->RunProcess();
		cout << endl;
	}
	else if (command[0] == "cls") {
		system("cls");
	}
	else {
		throw 1;
	}


}


void main_loop::help() {
	fstream file;
	std::string line;

	TextTable t(196, 179, 197);

	t.add("KOMENDA");
	t.add("OPIS");
	t.endOfRow();
	t.add("help");
	t.add("Wyswietla liste komend");
	t.endOfRow();
	t.add("close");
	t.add("Konczy prace systemu");
	t.endOfRow();
	t.add("cf [nazwa] [typ(plik/folder)]");
	t.add("Tworzy plik/folder");
	t.endOfRow();
	t.add("rf [nazwa]");
	t.add("Usuwa plik/folder");
	t.endOfRow();
	t.add("ld");
	t.add("Wypisuje zawartosc aktualnego folderu");
	t.endOfRow();
	t.add("cd [nazwa]");
	t.add("Przechodzi do podanego folderu(nizszego w hierarchii)");
	t.endOfRow();
	t.add("rd");
	t.add("Przechodzi do folderu ROOT");
	t.endOfRow();
	t.add("bd");
	t.add("Wraca do folderu poprzedniego w hierarchii");
	t.endOfRow();
	t.add("ren [nazwa1] [nazwa2]");
	t.add("Zmienia nazwę podanego pliku/folderu");
	t.endOfRow();
	t.add("of [nazwa]");
	t.add("Otwiera plik");
	t.endOfRow();
	t.add("clf");
	t.add("Zamyka otwarty plik");
	t.endOfRow();
	t.add("pds");
	t.add("Wyswietla statystyki dysku");
	t.endOfRow();
	t.add("pdss [nr_sektora]");
	t.add("Wyswietla zawartosc sektora");
	t.endOfRow();
	t.add("read");
	t.add("Wyświetla zawartość pliku");
	t.endOfRow();
	t.add("write [tekst]");
	t.add("Zapisuje tekst do pliku");
	t.endOfRow();
	t.add("ap");
	t.add("Dodaje tekst do pliku");
	t.endOfRow();
	t.add("cp [nazwa_pliku] [nr_grupy]");
	t.add("Tworzy proces");
	t.endOfRow();
	t.add("kp [numer]");
	t.add("Zabija proces");
	t.endOfRow();
	t.add("dap");
	t.add("Wyswietla wszystkie procesy(blok kontrolny)");
	t.endOfRow();
	t.add("dwp");
	t.add("Wyswietla oczekujace procesy");
	t.endOfRow();
	t.add("drp");
	t.add("Wyswietla gotowe procesy");
	t.endOfRow();
	t.add("mem [proces] -r");
	t.add("Wypisze strony procesu ktore sa tylko w ramkach RAM");
	t.endOfRow();
	t.add("mem [proces] -a");
	t.add("Wypisze wszystkie ramki");
	t.endOfRow();
	t.add("mem");
	t.add("Wyswietla RAM");
	t.endOfRow();
	t.add("sms");
	t.add("Wyswietla liste wszystkich wiadomosci");
	t.endOfRow();
	t.add("go");
	t.add("Wykonuje kolejny rozkaz z pliku z aktywnego procesu");
	t.endOfRow();
	t.add("pap");
	t.add("Pokazuje informacje o aktywnym procesie");
	t.endOfRow();

	t.setAlignment(2, TextTable::Alignment::RIGHT);
	std::cout << t<<endl;
}


int main() {
	rlutil::setColor(rlutil::GREEN); 
	rlutil::setConsoleTitle("ChaOS - BAMBO DEJ NA PRZEPIS PLOX");
	main_loop m;
	m.run();
	delete ps;
	delete pm;
	delete mm;
	delete s;
	delete fs;
	delete i;
	return EXIT_SUCCESS;
}




