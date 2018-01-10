#ifndef DISKDIRVE__H
#define DISKDRIVE__H
using uShort = unsigned short;
using c_uShort = const uShort;

// Klasa disk_drive reprezentuje fizyczny dysk twardy
// Dysk widoczny jest jako pamiec ciagla
// Dysk moze odczytac dowolny sektor
// Dysk moze zapisac dowolny sektor
// Volume Contol Block zawiera informacje potrzebne do odczytu istniejacych sektorow oraz alokacji nowych

class disk_drive final
{
public:
	disk_drive();
	~disk_drive();

	void readSector(c_uShort sectorNumber, char sector[]);
	void writeSector(c_uShort sectorNumber, char sector[]);
	void clear();
	void loadFromFile();

	c_uShort diskSize;
	c_uShort sectorSize;
	c_uShort numberOfSectors;

private:
	char* diskSpace;
};

#endif DISKDIRIVE__H