APPDIR = $(HOME)/cms
LIBHOME = $(HOME)/xprj/lib

CFLAGS = -I. \
         -I$(APPDIR)/inc \
         -I$(APPDIR)/lib/src \
         -I$(HOME)/xprj/inc \
         -I$(HOME)/ACE_wrappers




LFLAGS =  \
            -L${LIBHOME}



LIBS = CSTANotify.o mqueue.o callreport_agent_shm.o device_manage_shm.o \
	device_manage_monref_shm.o agent_status_shm.o vdn_status_shm.o \
	vdn_call_list_shm.o tracking_call.o group_status_shm.o \
	event_handler_base.o event_handler_vdn.o event_handler_extn.o \
	CSTANameString.o ICSTA.o -lrt -lpthread

all: $(LIBS) cms_ircv cms_anl cms_logger \
	dev_status dev_status2 agent_status vdn_status group_status


cms_ircv: cms_ircv.cpp
	g++ -g -o $@ $^ -D_REENTRANT $(CFLAGS) $(LFLAGS) -lACE -lXPRJACEDLLEx -lHUMLibEx $(LIBS) -L$(APPDIR)/lib -lcom_util

cms_anl: cms_anl.cpp
	g++ -g -o $@ $^ -D_REENTRANT $(CFLAGS) $(LFLAGS) -lACE -lXPRJACEDLLEx -lHUMLibEx $(LIBS) -L$(APPDIR)/lib -lcom_util

cms_logger: cms_logger.cpp
	g++ -g -o $@ $^ -D_REENTRANT $(CFLAGS) $(LFLAGS) -lACE -lXPRJACEDLLEx -lHUMLibEx $(LIBS) -L$(APPDIR)/lib -lcom_util

time_test: time_test.cpp
	g++ -g -o $@ $^ $(CFLAGS)  $(LIBS) -L$(APPDIR)/lib -lcom_util

t: t.cpp
	g++ -o $@ $^ -lrt -lpthread

t2: t2.cpp
	g++ -o $@ $^ callreport_agent_shm.o -lrt -lpthread

t3: t3.cpp
	g++ -o $@ $^ device_manage_shm.o $(CFLAGS) -lrt -lpthread

t4: t4.cpp
	g++ -o $@ $^ agent_status_shm.o $(CFLAGS) -lrt -lpthread

dev_status: dev_status.cpp
	g++ -o $@ $^ device_manage_shm.o device_manage_monref_shm.o $(CFLAGS) -lrt -lpthread

dev_status2: dev_status2.cpp
	g++ -o $@ $^ device_manage_monref_shm.o $(CFLAGS) -lrt -lpthread

agent_status: agent_status.cpp
	g++ -o $@ $^ agent_status_shm.o group_status_shm.o $(CFLAGS) -lrt -lpthread

group_status: group_status.cpp
	g++ -o $@ $^ group_status_shm.o $(CFLAGS) -lrt -lpthread

vdn_status: vdn_status.cpp
	g++ -o $@ $^ vdn_status_shm.o vdn_call_list_shm.o agent_status_shm.o group_status_shm.o tracking_call.o $(CFLAGS) -lrt -lpthread -L$(APPDIR)/lib -lcom_util

mq_sample: mq_sample.cpp
	g++ -g -o $@ $^ $(CFLAGS) mqueue.o -lrt -lpthread


CSTANameString.o : CSTANameString.cpp
ICSTA.o : ICSTA.cpp
mqueue.o : mqueue.cpp
stat_ipc_shm.o : stat_ipc_shm.cpp
callreport_agent_shm.o : callreport_agent_shm.cpp
device_manage_shm.o : device_manage_shm.cpp
device_manage_monref_shm.o : device_manage_monref_shm.cpp
agent_status_shm.o : agent_status_shm.cpp
group_status_shm.o : group_status_shm.cpp
vdn_status_shm.o : vdn_status_shm.cpp
group_status_shm.o : group_status_shm.cpp
vdn_call_list_shm.o : vdn_call_list_shm.cpp
tracking_call.o : tracking_call.cpp
event_handler_base.o : event_handler_base.cpp
event_handler_vdn.o : event_handler_vdn.cpp
event_handler_extn.o : event_handler_extn.cpp
CSTANotify.o : CSTANotify.cpp

.cpp.o :
	g++ -g -c $^ -D_REENTRANT $(CFLAGS)

#----------------------------------------------------------------------
clean:
	rm -f *.o *.lis

#install:
#	mv *.so $(APPLIBHOME)/.	
