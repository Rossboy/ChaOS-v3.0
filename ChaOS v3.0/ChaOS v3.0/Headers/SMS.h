#pragma once
#include<string>
class SMS
{
	int WID;
	std::string wiad;
public:
	SMS(std::string wiad);
	~SMS();
	int getID();
	std::string getwiad();
};

