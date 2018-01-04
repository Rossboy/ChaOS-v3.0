#include "../Headers/ChaOS_filesystem_exception.h"

// WYJĄTKI
outOfMemory::outOfMemory()
	: exception("EXCEPTION: Brak wolnych sektorow!")
{
}

objectNotFound::objectNotFound()
	: exception("EXCEPTION: Podany obiekt nie istnieje!")
{
}

objectExists::objectExists()
	: exception("EXCEPTION: Podany obiekt juz istnieje!")
{
}

fileToLong::fileToLong()
	: exception("EXCEPTION: Plik jest zbyt dlugi! Maksymalna dlugosc pliku to 255. Tekst zostal zmniejszony do dlugosci 255 znakow.")
{
}
