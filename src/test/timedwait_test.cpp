#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <iostream>
#include <errno.h>


int main()
{
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	struct timeval now;
	struct timespec ts;

	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	while(1) {
		gettimeofday(&now, NULL);
		//ts.tv_sec = now.tv_sec + 5;
		ts.tv_sec = now.tv_sec + (60 * 15);
		ts.tv_nsec = now.tv_usec * 1000;

std::cout << "before : " << now.tv_sec << ";" << now.tv_usec << std::endl;

		int retval = pthread_cond_timedwait(&cond, &mutex, &ts);
		if (retval == ETIMEDOUT)
			std::cout << "Timeout" << std::endl;
		else
			std::cout << "Signal caught" << std::endl;

		gettimeofday(&now, NULL);
std::cout << "after  : " << now.tv_sec << ";" << now.tv_usec << std::endl;
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return 0;
}
