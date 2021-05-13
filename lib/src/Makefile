#-------------------------------------------------------------------------------------

PLATFORM = -D_LINUX
#-------------------------------------------------------------------------------------

APPDIR = $(HOME)/cms
APPLIBHOME = $(APPDIR)/lib

CFLAGS = -I. \
           -I$(APPDIR) 

LFLAGS =  \
            -L${APPLIBHOME}

#----------------------------------------------------------------------------
##g++ -shared -o $@ hutil.o htime.o /usr/lib/libstdc++.so.5
##g++ -shared -Wl,-soname,libNRcom.so.0 -o libNRcom.so hutil.o htime.o
#	@echo ">>>>> 모듈적용은 수동으로 복사하세요(또는 make install) <<<<<"
	
all: libcom_util.so

#libcom_util.so : hutil.cpp htime.cpp hlcsipc.cpp
#	g++ -D_REENTRANT -c -g -fPIC $^
#	g++ -shared -o $@ hutil.o htime.o hlcsipc.o
libcom_util.so : hutil.cpp htime.cpp hsock.cpp hstringutil.cpp
	g++ -D_REENTRANT -c -g -fPIC $^
	g++ -shared -o $@ hutil.o htime.o hsock.o hstringutil.o


#hlcsipc.o : hlcsipc.cpp
hsock.o : hsock.cpp
htime.o : htime.cpp
hutil.o : hutil.cpp
hstringutil.o : hstringutil.cpp

.cpp.o : 
	g++ -D_REENTRANT -c -fPIC $^

#-------------------------------------------------------------------------
clean:
	rm -f *.so *.o *.lis

install:
	mv *.so $(APPLIBHOME)/.
