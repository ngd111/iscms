#include "XPRJACEDLLEx/ICSTA.h"
#include "XPRJACEDLLEx/LogConfig.h"
#include "XPRJACEDLLEx/PBXConfig.h"
#include "XPRJACEDLLEx/CSTANameString.h"

#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <queue>
#include "mqueue.hpp"
#include "common.h"
#include "hutil.hpp"
#include "ipc_res.hpp"
#include "ipc_def.hpp"

// 임시 로그 디렉토리 설정
static string LogDir= "/home/winddori/cms/log";
static string Log_prefix="SLOG_L";

static char *sAppName = (char *)"cms_logger";


#define WAIT_SECOND 5                 // used in timed_receive function
#define WAIT_JOINING WAIT_SECOND + 2  // used in termination function

#define LOG_SEPERATOR_BAR "=============================================================================="



struct __log_thread_params
{
	hcommon* pCommObj;
	char log_dir [1024+1];
};

typedef struct __log_thread_params    TH_PARAMS_LOG;

class LogHandler
{
	public:
		LogHandler() {
			pHandler = this;
		};
		~LogHandler() {};
		
		void do_logging() {
			string sCurrentDate;
			char log_dir  [1024+1]; // 로그파일명(full path)
			char cMessage [4096+1];
			int status;
			
			TH_PARAMS_LOG   *params_log;
			
			pCommObj = new hcommon;
			if (pCommObj == NULL) return;
				
			pCommObj->SetSystemLogFileName(LogDir, Log_prefix);
				
			remove_thread = 0;
			
			pthread_mutex_init(&rm_mutex, NULL);
			
			params_log = new TH_PARAMS_LOG;
			if (params_log == NULL) {
				delete pCommObj;
				return;
			}
			memset(params_log, 0x00, sizeof(TH_PARAMS_LOG));

			params_log->pCommObj = pCommObj;
			strcpy(params_log->log_dir, LogDir.c_str());

		    // Thread 1 : Thread for writing event & call log to DB
			pthread_create(&th_log, NULL, &loggerMain, (void*)params_log);
					
			pthread_join(th_log, (void **)&status);
			std::cout << "after pthread_join : th_log" << std::endl;					
			
		};
		
		
		static void FuncSIGTERM(int __signo) {
			char sMsg[1024];
			
			sprintf(sMsg, "Pid:[%d], SIGTERM signal caught[%d]", getpid(), __signo);
			pHandler->pCommObj->LoggingError(__FILE__, __func__, 0, sMsg);
			
			pHandler->WaitForJoiningMain();  //Exit scheduler & event handler
			
			pthread_mutex_destroy(&pHandler->rm_mutex);
			
			if (pHandler->pCommObj!=NULL) delete pHandler->pCommObj;
/* Clearing MySQL DB connection object
			if (pHandler->pOraObj!=NULL) {
				pHandler->pOraObj->TerminateConnectionPool();
				delete pHandler->pOraObj;
			}
*/
			exit(0);
		};		
		
		// Write data to write message queue using event list
		static void *loggerMain(void* __params){
			// list에서 데이터를 읽어 message queue에 등록한다.
			// cms_logger process 가 읽어서 DB에 등록처리한다.
			// parameter 1 : Common lib object pointer
			// parameter 2 : Log directory
			TH_PARAMS_LOG *params = (TH_PARAMS_LOG *) __params;
			hcommon *pCommObj = (hcommon *)params->pCommObj;
			
			mqueue *pMQ_recv = NULL;  // Message queue to send event and call log to cms_logger process
			char log_dir[1024+1], current_date[8+1], message_text[8096];
			char log_filename[2048+1];
			LOGGINGDATA_t logging_data;

			
			struct timeval now;
			struct timespec ts;
			string sCurrentDate;
			
	
			// Open message queue to write event & call log to cms_logger
			pMQ_recv = new mqueue;
			if (pMQ_recv == NULL) {
	            strcpy(message_text, "Message queue object memory allocation failed");
	            pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

				if (params != NULL) { delete params; params = NULL; }
				pthread_exit((void *) 0);
			}
			
			try {
				pMQ_recv->open_message_queue(QUE_LOGGER);
			}
			catch (std::string err_msg)
			{
	            sprintf(message_text, "Message queue open failed with %s : [%s]", QUE_LOGGER, err_msg.c_str());
	            pCommObj->LoggingError(__FILE__, __func__, -1, message_text);

				if (pMQ_recv != NULL) { delete pMQ_recv; pMQ_recv = NULL; }
				if (params != NULL) { delete params; params = NULL; }
				pthread_exit((void *) 0);
			}
			
            sprintf(message_text, "Message queue creation success with %s", QUE_LOGGER);
            pCommObj->LoggingError(__FILE__, __func__, 0, message_text);

			while(1) {
				memset(&logging_data, 0x00, sizeof(LOGGINGDATA_t));
				
				bool bRet = pMQ_recv->message_recv_timed(&logging_data, sizeof(LOGGINGDATA_t), WAIT_SECOND);
				if (bRet == false) {  // Timeout
					if (pHandler->remove_thread == 1) // exit thread
					{
						break;
					}

					pCommObj->SetSystemLogFileName(LogDir, Log_prefix);
					
					continue;
				}
				else { // Data received
					// DB writing...
					if (logging_data.data_type == 1) // Event data
					{
						EVENTDATA_t eventData;
						memset(&eventData, 0x00, sizeof(EVENTDATA_t));						
						memcpy(&eventData, &logging_data.u.eventData, sizeof(EVENTDATA_t));
						
						std::cout << "event_type        : " << eventData.event_type << std::endl;
						std::cout << "event_type_detail : " << eventData.event_type_detail << std::endl;
					}
					else if (logging_data.data_type == 2) // Calllog data
					{
						CALLLOG_SET_t callDataSet;
						
						memset(&callDataSet, 0x00, sizeof(CALLLOG_SET_t));						
						memcpy(&callDataSet, &logging_data.u.callData, sizeof(CALLLOG_SET_t));
/*						
std::cout << __FILE__ << " log_data.u.callData.key.callID  : " << logging_data.u.callData.key.callID << std::endl;
std::cout << __FILE__ << " log_data.u.callData.key.segment : " << logging_data.u.callData.key.segment << std::endl;
std::cout << __FILE__ << " callDataSet.key.callID          : " << callDataSet.key.callID << std::endl;
std::cout << __FILE__ << " callDataSet.key.segment         : " << callDataSet.key.segment << std::endl;
*/
			            sprintf(message_text, "callID      : %d \n"
			                                  "segment     : %d \n"
			                                  "domain      : %s \n"
			                                  "tenant      : %d \n"
			                                  "acwtime     : %ld \n"
			                                  "ansholdtime : %ld \n"
			                                  "anslogin    : %s \n"
			                                  "agt_released: %d \n"
			                                  "call_disp   : %d \n"
			                                  "calling_pty : %s \n"
			                                  "conference  : %d \n"
			                                  "consulttime : %d \n"
			                                  "dialed_num  : %s \n"
			                                  "dispsklevel : %d \n"
			                                  "dispsplit   : %s \n"
			                                  "disptime    : %d \n"
			                                  "dispvdn     : %s \n"
			                                  "held        : %d \n"
			                                  "holdabn     : %d \n"
			                                  "origlogin   : %s \n"
			                                  "queuetime   : %ld \n"
			                                  "ringtime    : %ld \n"
			                                  "segstart    : %ld \n"
			                                  "segstop     : %ld \n"
			                                  "talktime    : %ld \n"
			                                  "transferred : %ld \n"
			                                  "UCID        : %s \n"
			                                , callDataSet.key.callID
			                                , callDataSet.key.segment
			                                , callDataSet.data.domain
			                                , callDataSet.data.tenant
			                                , callDataSet.data.acwtime
			                                , callDataSet.data.ansholdtime			                                
			                                , callDataSet.data.anslogin
			                                , callDataSet.data.agt_released
			                                , callDataSet.data.call_disp
			                                , callDataSet.data.calling_pty
			                                , callDataSet.data.conference
			                                , callDataSet.data.consulttime
			                                , callDataSet.data.dialed_num
			                                , callDataSet.data.dispsklevel
			                                , callDataSet.data.dispsplit			                                
			                                , callDataSet.data.disptime
			                                , callDataSet.data.dispvdn
			                                , callDataSet.data.held
			                                , callDataSet.data.holdabn
			                                , callDataSet.data.origlogin
			                                , callDataSet.data.queuetime
			                                , callDataSet.data.ringtime
			                                , callDataSet.data.segstart
			                                , callDataSet.data.segstop
			                                , callDataSet.data.talktime
			                                , callDataSet.data.transferred
			                                , callDataSet.data.ucid
			                                );
			            pCommObj->LoggingData(__FILE__, __func__, message_text);
			            
			            pCommObj->LoggingData(__FILE__, __func__, LOG_SEPERATOR_BAR);
			            pCommObj->LoggingData(__FILE__, __func__, "\n");
					}
					
				}
			}

			if (pMQ_recv != NULL) { delete pMQ_recv; pMQ_recv = NULL; }
			
			pthread_exit((void *) 0);
			return ((void*) 0);			
		};
				
		
	private:
		static LogHandler *pHandler;
		
		pthread_t th_log;
		
		pthread_mutex_t rm_mutex;

		queue<LOGGINGDATA_t> logqueue;  // Event & Call log
			
		hcommon *pCommObj = NULL;
		//HOCIHandler *pOraObj; -> MySQL 처리
		int remove_thread;
		
		void WaitForJoiningMain() {
			int nRet, status;
			
			pthread_mutex_lock(&rm_mutex);
			remove_thread = 1;       // To use end thread gracefully.
			pthread_mutex_unlock(&rm_mutex);
			
			sleep(WAIT_JOINING);
			
			nRet = pthread_join(th_log, (void **)&status);
			if (status == 0) {
				std::cout << "Completed join with event logger thread:" << th_log 
					<< " => status=" << status << std::endl;
			}
			else {
				std::cout << "ERROR return(" << nRet << ") join with thread:" << th_log 
					<< " => status=" << status << std::endl;
			}
		};		
		
};

LogHandler* LogHandler::pHandler = 0;

int main()
{
	int pid, chpid;
	LogHandler logh;

	if ((pid = fork()) < 0) {
		perror("fork() error:");
		exit(0);
	}
	else if (pid != 0) {
		perror("Parent process end:");
		exit(0);
	}
	
	setsid();
	
	signal(SIGPIPE, SIG_IGN);
	Signal(SIGTERM, (void *)LogHandler::FuncSIGTERM);

	logh.do_logging();
}
