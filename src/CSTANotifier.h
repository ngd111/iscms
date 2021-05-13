#pragma once

#include "XPRJACEDLLEx/IcstaNotifier.h"
#include "XPRJACEDLLEx/CSTADefine.h"
#include <vector>

using namespace IS;



class CSTANotifier : public ICSTANotifier
{
public:
	CSTANotifier(void);
	~CSTANotifier(void);

	void	NotifyEvent(CCSTAEvent& event);


	string	GetSessionID()		{	return sessionID_;	};
	void    SetSessionStatus(int __sessionStatus) { sessionStatus = __sessionStatus; };
	int     GetSessionStatus() { return sessionStatus; };
	
	vector<int> vmonlist;   // manage monitorCrossRefID
	vector<int>::iterator vmonIter;
		
	vector<int> vmonVDNlist; // manage 
private:
	
	void	OnConfirmationEvent(tConfirmationEventType conf, CSTAConfirmationEvent confEvent, tPrivateData privData);
	void	OnUnsolicitedEvent(tUnsolicitedEventType unsol, CSTAUnsolicitedEvent unsolEvent, tPrivateData privData);
//	void	OnSystemEvent(tSystemEventType sys, CSTASystemEvent sysEvent, tPrivateData privData);
	

	void	AddLocalConnState2List(tLocalConnectionState localstate);
	void	AddCause2List(tEventCause cause);
	void	AddLastRedirectDevice2List(tDeviceID lastRedirectDevice);

	string	sessionID_;

	int     sessionStatus;  /* 1 : connected, 0 : closed */
};



