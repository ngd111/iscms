/*******************************************************************************
** File Name  : hlcsipc.cpp
** Author     : 
** Date       : 
** Description: 
**              
*******************************************************************************/
#include "hlcsipc.hpp"


/*******************************************************************************
** Module Name :  MsgGet
** Description :  Create Queue & Get Queue ID
*******************************************************************************/
int IPCFunc::MessageQue::create_ipc(int key)
{
	int		qid;
	
	qid = msgget(key, 0666 | IPC_CREAT);
	if(qid == -1) {
//		Trc(ERRLOGPATH, __FILE__, __LINE__, func,
//			"msgget key[%x] error[%d]\n", key, errno);
		return  -1;
	}
	return qid;
}

#if	0
/*******************************************************************************
** Module Name :  MsgSnd
** Description :  Message Queue Send
*******************************************************************************/
//int MessageQue::MsgSnd(int qid, HTS_SEND *msgbuf, int msgsize, int flag)
int IPCFunc::MessageQue::write_data(int qid, char *msgbuf, int msgsize, int flag)
{
	int		rc;
	
	if(msgsize > MAX_MSG_SIZE) {
//		Trc(ERRLOGPATH, __FILE__, __LINE__, func,
//			"msgsnd qid[%d] mtext is greater than msgsz\n", qid);
		return -1;
	}
	
	if((rc = msgsnd(qid, msgbuf, msgsize, flag)) == -1) {
//		Trc(ERRLOGPATH, __FILE__, __LINE__, func,
//			"msgsnd qid[%d] error[%d][%s]\n", qid, errno, strerror(errno));
		return -1;
	}
	return 0;
}
#endif	/* MsgSnd() */


#if	0
/*******************************************************************************
** Module Name :  read_data
** Description :  Message Queue Receive
*******************************************************************************/
//int MessageQue::MsgRcv(int qid, HTS_RECV *msgbuf, int msgtype, int msgflag)
int IPCFunc::MessageQue::read_data(int qid, char *msgbuf, int msgtype, int msgflag)	
{
	int		rc;
	
	if((rc = msgrcv(qid, msgbuf, MAX_MSG_SIZE, msgtype, msgflag)) == -1) {
		if(errno == ENOMSG)
			return -1;
		else {
//			Trc(ERRLOGPATH, __FILE__, __LINE__, func,
//				"msgrcv qid[%d] error[%d][%s]\n", qid, errno, strerror(errno));
			return -1;
		}
	}
	return rc;
}
#endif	/* MsgRcv() */

/*******************************************************************************
** Module Name :  get_shm -> create_ipc
** Description :  
*******************************************************************************/
void *IPCFunc::SharedMem::create_ipc(char *keypath, int keyid)
{
	void	*MemBuf;
	int		MemId;
	key_t	MemKey;

	
	if((MemKey = ftok(keypath, keyid)) == -1) {
//		snprintf(Msg, sizeof(Msg), "shm path not found %d %s",
//			errno, strerror(errno));
//		Er( func, errno, Msg);
		return NULL;
	}
	if(mkShm(MemKey, &MemBuf) == -1) {
		return NULL;
	}
	return MemBuf;
}

/*******************************************************************************
** Module Name :  mkShm
** Description :  
*******************************************************************************/
int IPCFunc::SharedMem::mkShm(key_t keyShm, void **shmPt)
{
	size_t	shmSz = 256;
	int		shmId;

	shmId = shmget(keyShm, shmSz, 0660 | IPC_CREAT | SHM_R | SHM_W);
	if(shmId == -1) {
//		snprintf(Msg, sizeof(Msg), "shmget %d %s",
//			errno, strerror(errno));
//		Er( func, errno, Msg);
		return -1;
	}
	*shmPt = shmat(shmId, 0, 0);
	if((size_t)(*shmPt) == -1) {
//		snprintf(Msg, sizeof(Msg), "shmat %d %s",
//			errno, strerror(errno));
//		Er( func, errno, Msg);
		return -1;
	}
	return	0;
}

/*******************************************************************************
** Module Name :  dtShm
** Description :  
*******************************************************************************/
int IPCFunc::SharedMem::dtShm(char *shmPt)
{
	int		shmRtn;

	if((shmRtn = shmdt((char *)shmPt)) == -1) {
//		snprintf(Msg, sizeof(Msg), "shmdt %d %s", errno, strerror(errno));
//		Er( func, errno, Msg);
		return -1;
	}
	return 0;
}

/*******************************************************************************
** Module Name :  write_file -> write_data
** Description :  
*******************************************************************************/
int IPCFunc::SharedMem::write_data(S_CONFMEM *s_confmem)
{
	int	fd;

	if(( fd = open( SHM_FILEPATH, O_CREAT|O_WRONLY, 0666)) < 0){
//		Er(func, errno, "open file error");
		return -1;
	}
	write( fd, (S_CONFMEM *)s_confmem, sizeof(S_CONFMEM));
	close( fd );
	return 0;
}

/*******************************************************************************
** Module Name :  read_file -> read_data
** Description :  
*******************************************************************************/
int IPCFunc::SharedMem::read_data(S_CONFMEM *s_confmem) 
{
	int	fd;

	if(( fd = open( SHM_FILEPATH, O_RDONLY, 0666)) < 0) {
//		Er(func, errno, "open file error");
		return -1;
	}
	read( fd, (S_CONFMEM *)s_confmem, sizeof(S_CONFMEM));
	close( fd );
	return 0;
}

/***************************<<< End Of File >>>********************************/
