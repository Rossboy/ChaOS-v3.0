#ifndef SMS__H
#define SMS__H
#include<string>
class SMS
{
	int WID;
	std::string wiad;
	int indeks;
public:
	SMS(std::string wiad);
	~SMS();
	int getID();
	std::string getwiad();
};

#endif SMS__H