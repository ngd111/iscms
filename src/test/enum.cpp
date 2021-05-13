#include <stdio.h>
#include <iostream>

enum RELEASING_SIDE
{
	NONE = -1,  
	CALLING_PTY = 0,
	CALLED_PTY  = 1
};

using namespace std;

int main()
{	
	RELEASING_SIDE first;

	std::cout << "first : [" << first << "]" << std::endl;

	first = CALLED_PTY;
	std::cout << "first : [" << first << "]" << std::endl;

	return 0;
}

