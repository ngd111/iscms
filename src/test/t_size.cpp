#include <iostream>
#include <stdio.h>
#include <time.h>

using namespace std;

int main()
{
	cout << "size_t : " << sizeof(size_t) << endl;
	cout << "short  : " << sizeof(short) << endl;
	cout << "unsigned int : " << sizeof(unsigned int) << endl;
	cout << "int    : " << sizeof(int) << endl;
	cout << "long   : " << sizeof(long) << endl;
	cout << "unsigned long : " << sizeof(unsigned long) << endl;
	cout << "float  : " << sizeof(float) << endl;
	cout << "double : " << sizeof(double) << endl;
	cout << "time_t : " << sizeof(time_t) << endl;

	unsigned int i; i++;
	cout << "i : " << i << endl;
	return 0;
}
