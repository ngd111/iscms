/**
libNRsock.so hsock.hpp header file -----------------------------------------

Description
socket handling�� class

�������� : 2014-08-31
�ۼ���   : ������
�����̷�
---------------------------------------------------------------------
[��������] [�������� ���]

**/

#ifndef _HSOCK_HPP
#define _HSOCK_HPP

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#if defined(_SOLARIS) // Solaris
	#include <sys/filio.h>	/* FIONBIO */
	#include <sys/fcntl.h>	/* F_GETFL, F_SETFL, O_NONBLOCK */
#else // Linux
	#include <fcntl.h>
#endif

#include <errno.h>


namespace SelectOption
{
	enum Type
	{
		READ,
		WRITE
	};
}

class hsocket
{		
    public:
		int sendn(int __sockfd, const void *__pbuff, size_t __nbytes, 
		          int __nflags);
       
		// __nflags �� 0���θ� ����Ѵ�. 
		// �� �� �ִ� ���� recv�� flag�̸� MSG_OOB, MSG_PEEK���� �ü��ִ�
		int recvn(int __sockfd, void *__pbuff, size_t __nbytes,
		          int __nflags, int __nsec);

		int connect_server(const char *__addr, int __port, 
		            		struct sockaddr_in *__conaddr);
       
        
		int sock_accept(int __listenfd);
        
		int connect_wait(int __sockfd, struct sockaddr *__addr, 
		                int __addrsize, int __nsec);
		                
	private:
		int sock_select(int __sockfd, struct timeval *__astv, 
		                SelectOption::Type __Option);
		                	
		int server_open(int __port);		                
};

#endif
