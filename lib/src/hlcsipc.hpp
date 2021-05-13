#ifndef _HLCSIPC_HPP
#define _HLCSIPC_HPP

#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//#include "shm_struct.h" -> ���� ����ü ���� �� ó��


typedef	struct {
	char	LogLevel	;
	char	OraUserID	[64];
//	struct	S_FAXSERV	pt;
	char	UniqKey		[ 4];
	char	filler		[19];
} S_CONFMEM;


/* shared memory environment */
//#define	CONFSHM_KEYPATH	"/home/cmsnr/test/libxml2/lib/shm/confshm"
//#define	CONFSHM_KEYID	0x63		// 'c'
#define	SHM_FILEPATH	"/home/cmsnr/test/libxml2/lib/shm/shmfile"




namespace IPCFunc
{
	class MessageQue
	{
		public:
			int create_ipc(int key);
			int write_data(int qid, char *msgbuf, int msgsize, int flag);
			int read_data(int qid, char *msgbuf, int msgtype, int msgflag);
			
		private:
		
	};
	
	class SharedMem
	{
		public:
			void *create_ipc(char *keypath, int keyid);
			
			// �� ��Ȳ�� ���� ����ü�� �ٸ��� �ֵ��� ó��(size �Ķ���� �߰�)
			int write_data(S_CONFMEM *s_confmem);
			int read_data(S_CONFMEM *s_confmem);
			
		private:
			int mkShm(key_t keyShm, void **shmPt);
			int dtShm(char *shmPt);			
	};

}

#endif