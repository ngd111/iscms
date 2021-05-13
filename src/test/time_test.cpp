#include <stdio.h>
#include <stdlib.h>
#include "htime.hpp"

int main(int argc, char **argv)
{
	hdatetime dttm;
	time_t tCurrentTime;

	if (argc < 2) {
		std::cout << "Need interval value(min)" << std::endl;
		return -1;
	}

	long nWait = dttm.get_batchwaitingtime(atoi(argv[1]));
	std::cout << "Wait time : " << nWait << std::endl;

	time(&tCurrentTime);
	
	string lststarttime = dttm.get_previntervalstarttime(atoi(argv[1]), tCurrentTime);
	std::cout << "Last time : " << lststarttime << std::endl;

	string lststarttimebyday = dttm.get_lastintervalstarttime(atoi(argv[1]));
	std::cout << "Last time by day: " << lststarttimebyday << std::endl;	
	

	return 0;
}
