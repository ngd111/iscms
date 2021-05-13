//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
// Writer : Jin Kak, Jung
// Revision history :  
//          First released on Oct. 21, 2014
//
//////////////////////////////////////////////////////////////////////////////

#ifndef LENGTH_DEFINITION_INCLUDED
#define LENGTH_DEFINITION_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif


#define SZ_DOMAIN  100
#define SZ_DIALPLN  12
#define SZ_DATE      8
#define SZ_STTIME    4

#define MUTEX_NAME_LEN     100   // Mutex name length
#define CONTAINER_NAME_LEN 100   // Container name length
#define LOG_LEN           8096   // Log variable length

///////////////// For report variable ////////////////////////
#define WORKMODE_LEN    10
#define DIRECTION_LEN    3
#define AGSTATE_LEN     WORKMODE_LEN + DIRECTION_LEN


#define EXTENSION_LEN   15   // Except domain name
#define DOMAIN_LEN   100     // Except domain name

#define AGENT_GROUP_MAX 99   // Max skill limitation for assigned by agent

#define INAUX_MAX       99   // Agent AUX state max by reason

#endif // LENGTH_DEFINITION_INCLUDED
