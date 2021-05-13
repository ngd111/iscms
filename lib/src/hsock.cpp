/**
libNRsock.so hsock.cpp source file -----------------------------------------

Description
socket handling용 메소스를 구현한다.
현재는 NR에서 사용하는 TCP socket만을 구현했다.

개발일자 : 2010-10-31
작성자   : 정진각
개정이력
---------------------------------------------------------------------
[개정일자] [개정사항 기술]

**/

#include "hsock.hpp"

int hsocket::sock_accept(int __listenfd)
{ 
	char	Msg[256];
	int		len, sockfd;
	struct	sockaddr_in	un_cliaddr;
	
	len = sizeof(len);
	sockfd = accept(__listenfd, (struct sockaddr *)&un_cliaddr, (socklen_t *) &len );
	if ( sockfd < 0 ) {
/*
		sprintf (Msg, "unix socket accept error %d, %s",
		              errno, strerror(errno));
		Er(func, errno, Msg );
*/
		return -1;
	}
	else
		return sockfd;
}
		                	
int hsocket::sock_select(int __sockfd, struct timeval *__astv,
	                      SelectOption::Type __Option)
{
    int nRet;
    int selectsock;
    fd_set  readmask, writemask, exceptmask;

	FD_ZERO(&readmask);
    FD_ZERO(&writemask);
    FD_ZERO(&exceptmask);

	FD_SET(__sockfd, &readmask);
    FD_SET(__sockfd, &writemask);
    FD_SET(__sockfd, &exceptmask);

    selectsock = __sockfd + 1;


	if (__Option == SelectOption::READ)
		nRet = select(selectsock, &readmask, (fd_set *)0, &exceptmask, __astv);
	else if (__Option == SelectOption::WRITE)
		nRet = select(selectsock, (fd_set *)0, &writemask, &exceptmask, __astv);
	else // Invalid option
	{
		return -1;
	}
		
	if(nRet == -1) {
		/*
		sprintf(s_temp, "socket select error %d [%s]", errno, strerror(errno));
		WriteLog('A', _dLogFileName, s_temp);		
		*/
		return -1;
	}
	else if(nRet == 0) {
		/*
		sprintf(s_temp, "socket timeout error %d [%s]", errno, strerror(errno));
		WriteLog('A', _dLogFileName, s_temp);
		*/
		return 0;
	}

	if (FD_ISSET(__sockfd, &exceptmask)) {
		/*
		sprintf(s_temp, "%s : socket exceptmask error %d[%s]"
		              , func, errno, strerror(errno));
		*/
		return -1;
	}

    return 1;
}

int hsocket::sendn(int __sockfd, const void *__pbuff, size_t __nbytes, 
	                int __nflags)
{
	const char *func = "sendn()";
	struct timeval tv; 
	int nRet;
	int nleft;
	int nsend;
	const char *ptr;
	
	time_t curr_time,serch_time;
	time(&curr_time);
	
	ptr = (char *)__pbuff;
	nleft = __nbytes;
	
	while(nleft > 0) {
		time(&serch_time);
		
		tv.tv_sec = 0;
		tv.tv_usec = 100;
		
		if ( (nRet = sock_select(__sockfd, &tv, SelectOption::WRITE)) < 1 )
			continue;
		
		if( (nsend = send(__sockfd, ptr, nleft, __nflags) ) <= 0) {
			if(errno == EINTR)
			continue;
		
			return nsend;
		}
		
		nleft -= nsend;
		ptr += nsend;

		if((long)difftime(serch_time,curr_time) > 3)
			return -100; /*time out*/
	}

	return __nbytes;
}

int hsocket::recvn(int __sockfd, void *__pbuff, size_t __nbytes, 
	                int __nflags, int __nsec)
{
    const char *func = "recvn()";

    int nRet;
    int nleft;
    int nrecv;
    char *ptr;
    fd_set sds;
    struct timeval tv, *ptv;

    ptr = (char *)__pbuff;
    nleft = __nbytes;

    FD_ZERO(&sds);
    FD_SET(__sockfd, &sds);

    while(nleft > 0) {
        if(__nsec < 0)
            ptv = (struct timeval *)NULL;
        else {
            tv.tv_sec = __nsec;
            tv.tv_usec = 0;
            ptv = &tv;
        }

		if ( (nRet = sock_select(__sockfd, &tv, SelectOption::READ)) < 1 ) {
            if(nRet < 0)  {
            	//printf("select failed : %s\n", strerror(errno));
            }

            return nRet;
        }

        if((nrecv = recv(__sockfd, ptr, nleft, __nflags)) < 0) {
            if(errno == EINTR)
                continue;

            return nrecv;
        } else if(nrecv == 0)
            break;

        nleft -= nrecv;
        ptr += nrecv;
    }

    return (__nbytes - nleft);
}

int hsocket::connect_server(const char *__addr, int __port,
	                  struct sockaddr_in *__conaddr)
{
    const char *func = "connect_server()";

    int sd, one;
    struct linger linger;

    // Socket을 생성한다
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  {
/*     	printf("socket failed : %s\n", strerror(errno)); */
        return -1;
    }

    __conaddr->sin_family = AF_INET;
    __conaddr->sin_port = htons(__port);
    __conaddr->sin_addr.s_addr = inet_addr(__addr);

//    bzero(&(__conaddr->sin_zero), 8);
    memset(&(__conaddr->sin_zero), 0x00, 8);
    
	if( setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, 
		( char *)&one, sizeof( one)) < 0) {
		//printf("socket setsockopt error %d [%s]", errno, strerror(errno));
	
		close(sd);
		return -1;
	}
	linger.l_onoff = 1;
	linger.l_linger = 1;
	if( setsockopt(sd, SOL_SOCKET, SO_LINGER, 
		(char *)&linger, sizeof( linger)) < 0) {
		//printf("socket setsockopt error %d [%s]",errno, strerror(errno));
	
		close(sd);
		return -1;
	}

    if(connect(sd, (struct sockaddr *)__conaddr, 
    	       (socklen_t)sizeof(struct sockaddr))== -1) {
    	//printf("connect failed : %s\n", strerror(errno));
        close(sd);
        return -1;
    }

    return sd;
}

int hsocket::server_open(int __port)
{
	char   sMessage[128];
	int    listenfd, optval, optlen;
	struct sockaddr_in	svraddr;
	
	if((listenfd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		snprintf(sMessage, sizeof(sMessage), "socket create error %d [%s]",
				errno, strerror(errno));
		return -1;
	}

	memset((char *)&svraddr, 0x00, sizeof(svraddr));
	
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svraddr.sin_port = htons(__port);
	
	optval = 1;
	optlen = sizeof(optval);
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) == -1) {
		close(listenfd);
		snprintf(sMessage, sizeof(sMessage), "setsockopt error %d [%s]",
				errno, strerror(errno));
		return -1;
	}
	
	if(bind(listenfd, (struct sockaddr *)&svraddr, sizeof(struct sockaddr)) < 0) {
        close(listenfd);
        snprintf(sMessage, sizeof(sMessage), "socket bind error %d [%s]",
                 errno, strerror(errno));
        return -1;
    }
	
	if(listen(listenfd, SOMAXCONN) == -1) { /* <sys/socket.h>에 define */
		close(listenfd);
		snprintf(sMessage, sizeof(sMessage), "socket listen error %d [%s]",
				errno, strerror(errno));
		return -1;
	}
	
	return listenfd;
}

int hsocket::connect_wait(int __sockfd, struct sockaddr *__addr,
	                      int __addrsize, int __nsec)
{
	int newSockStat;
	int orgSockStat;
	int res, n;
	fd_set  wset;
	struct timeval tval;
	
	int error = 0;
	int esize;
	
	if ( (newSockStat = fcntl(__sockfd, F_GETFL, NULL)) < 0 )
		return -101;
	
	orgSockStat = newSockStat;
	newSockStat |= O_NONBLOCK;
	
	// Non blocking 상태로 만든다. 
	if(fcntl(__sockfd, F_SETFL, newSockStat) < 0)
		return -102;
	
	// 연결을 기다린다.
	// Non blocking 상태이므로 바로 리턴한다.
	if((res = connect(__sockfd, __addr, __addrsize)) < 0) {
		if (errno != EINPROGRESS)
			return -103;
	}
	
	// 즉시 연결이 성공했을 경우 소켓을 원래 상태로 되돌리고 리턴한다. 
	if (res == 0) {
		/* printf("Connect Success\n"); */
		fcntl(__sockfd, F_SETFL, orgSockStat);
		return 0;
	}
	
	FD_ZERO(&wset);
	FD_SET(__sockfd, &wset);
	
	tval.tv_sec  = __nsec;    
	tval.tv_usec = 0;
	
	if ( (n = select(__sockfd+1, NULL, &wset, NULL, &tval)) == 0) {
		// timeout and no fds changed
		errno = ETIMEDOUT;
		return -104;
	}
	
	// 쓴 데이터가 있는지 검사한다. 
	if (FD_ISSET(__sockfd, &wset)) {
		esize = sizeof(int);
		n = getsockopt(__sockfd, SOL_SOCKET, SO_ERROR, &error, 
		       (socklen_t *)&esize);
		if (n < 0)
		return -105;
	}
	else
		return -106;
	
	fcntl(__sockfd, F_SETFL, orgSockStat);
	if(error) {
		errno = error;
		return -107;
	}
	
	return 0;
}
