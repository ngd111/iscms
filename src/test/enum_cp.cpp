#include <stdio.h>
#include <iostream>
#include <string.h>

enum RELEASING_SIDE
{
	NONE = -1,  
	CALLING_PTY = 0,
	CALLED_PTY  = 1
};

using namespace std;

typedef struct __state {
	RELEASING_SIDE side;
} tState;

void copy_data(tState &__out_data) {
	__out_data.side = CALLED_PTY;
}

int main()
{	
	tState data;

	memset(&data, 0x00, sizeof(tState));

	copy_data(data);

	std::cout << "data : [" << data.side << "]" << std::endl;

	return 0;
}

