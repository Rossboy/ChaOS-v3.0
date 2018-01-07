#ifndef FILESYSTEM__H
#define FILESYSTEM__H
#include "../Headers/disk_drive.h"
#include "../Headers/file.h"
#include "../Headers/ConditionVariable.h"
#include "stack"

class ChaOS_filesystem final
{
public:
	enum class type { file = 'F', dir = 'D' };

	ChaOS_filesystem();
	~ChaOS_filesystem();

	// Tworzy obiekt w aktualnym folderze
	void create(const char* name, type t);

	// Usuwa obiekt z aktualnego folderu
	void remove(const char* name);

	// Zwraca string opisujący zawartość aktualnego folderu
	std::string listDirectory();

	// Zmiania aktualny folder na wskazany folder
	void changeDirectory(const char* name);

	// zmiania aktualny folder na wskazany folder
	void backDirectory();

	// Zmiania aktualny folder na wskazany folder
	void rootDirectory();

	// Zmienia nazwę obiektu w aktualnym folderze
	void rename(const char* name, const char* newname);

	// Szuka pliku/katalogu w aktualnym katalogu. Zwraca adres pierwszego sektora szukanego pliku/katalogu lub 0, gdy nie istnieje
	uShort search(const char* name, type t);

	// Otwiera plik o wskazanej nazwie
	void openFile(const char* filename);

	// Wpisanie nowej zawartości do pliku
	void writeFile(const std::string& text);

	// Odczytanie zawartości pliku i umieszczenie jej w currentFile
	std::string readFile();

	// Dopisanie tekstu na końcu pliku
	void appendFile(const std::string& text);

	// Zapisuje otwarty plik - użyte przy pisaniu do pliku
	void saveFile(const std::string& text);

	// Zamknięcie pliku
	void closeFile();

	// Zwraca string opisujący zadany sektor
	std::string printSector(const unsigned short number);

	// Zwraca string opisujący łańcuch sektorów zaczynając od 'first'
	std::string printSectorsChain(const unsigned short first);

	// Zwraca string opisujący stan dysku
	std::string printDiskStats();

	//file* currentFile;
	uShort getRootDir();
private:
	c_uShort allocateSector();
	void freeSector(uShort number);

	// Przechowują "wskaźniki" na plik/katalog
	uShort currentFileFirst;
	uShort currentFileSector;
	uShort currentDirFirst;
	uShort currentDirSector;
	uShort rootDirSector;

	disk_drive disk;

	ConditionVariable fileSynchronization[32];

	void addRow(char* sector, char* row);
	void getRow(char* sector, char* row);
	bool equalName(const char* n1, const char* n2);
	uShort charArrSize(const char* arr);
	void toChar5(const char* arr, char* result5);
	std::string asBitVector(const int vector);
	char check(const char& c);
	bool isDirEmpty(uShort dir);
	void clearDisk();
	std::string setBitNumber(const int vector);
};

#endif FILESYSTEM__H