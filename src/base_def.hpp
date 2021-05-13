//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Hansol inticube 2014
//
//////////////////////////////////////////////////////////////////////////////


#ifndef BASE_DEFINITION_INCLUDED
#define BASE_DEFINITION_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#define NOUPDATE "no_update"


namespace DUP_OPTION
{
    enum ProcType
    {
        ACCUMULATE,
        UPDATE,
        REPLACE
    };
}

enum DEVICE_TYPE {
    STATION = 1,
    VDN = 2,

    UNDEFINED = 0
};

#endif // BASE_DEFINITION_INCLUDED
