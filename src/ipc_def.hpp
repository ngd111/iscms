///////////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IPC_DEFINITION_INCLUDED
#define IPC_DEFINITION_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

///////////////////////////////////////////////////////////////////////////////////
// Define for shared memory realm
///////////////////////////////////////////////////////////////////////////////////
#define REPORT_AREA_SHM      "SHM_REPORT"   // Shared memory realm name for report

///////////////////////////////////////////////////////////////////////////////////
// Define for queue 
///////////////////////////////////////////////////////////////////////////////////
#define QUE_EVENT            "QUE_EVENT"    // Queue for push event
#define QUE_LOGGER           "QUE_LOGGER"   // Queue for push data to logger

///////////////////////////////////////////////////////////////////////////////////
// Define for report
///////////////////////////////////////////////////////////////////////////////////

// Report management definition

#define MTX_HR_AGENT         "MTX_HAGENT"   // Report mutex name
#define CONTAINER_HR_AGENT   "CNT_HAGENT"   // Report area address name in shared memory


//////////////////////////////////////////////////////////////////////////////
// Define for management
//////////////////////////////////////////////////////////////////////////////

#define MTX_MON_DEVICE       "MTX_MONDEVICE" // Monitoring device list mutex name
#define CONTAINER_MON_DEVICE "CNT_MONDEVICE" // Device list area address name in shared memory

#define MTX_MON_REF_DEVICE       "MTX_MONREF_DEVICE" // Monitoring device list(refid key) mutex name
#define CONTAINER_MON_REF_DEVICE "CNT_MONREF_DEVICE" // Device list(refid key) area address name in shared memory

#define MTX_RT_AGENT_STATUS        "MTX_RT_AGENT"  // Realtime agent status mutex name
#define CONTAINER_RT_AGENT_STATUS  "CNT_RT_AGENT"  // Agent status list area address name in shared memory

#define MTX_RT_VDN_STATUS        "MTX_RT_VDN"  // Realtime vdn status mutex name
#define CONTAINER_RT_VDN_STATUS  "CNT_RT_VDN"  // VDN status list area address name in shared memory

#define MTX_RT_VDN_CALL_LIST        "MTX_RT_CALL_LIST_VDN"  // Realtime vdn call list mutex name
#define CONTAINER_RT_VDN_CALL_LIST  "CNT_RT_CALL_LIST_VDN"  // VDN call list area address name in shared memory

#define MTX_RT_GROUP_STATUS        "MTX_RT_GROUP"  // Realtime skill group status mutex name
#define CONTAINER_RT_GROUP_STATUS  "CNT_RT_GROUP"  // Skill group status list area address name in shared memory

#define MTX_TRACKING_CALL       "MTX_TRACKING_CALL" // Call log for tracking call lifetime mutex name

#endif // IPC_DEFINITION_INCLUDED
