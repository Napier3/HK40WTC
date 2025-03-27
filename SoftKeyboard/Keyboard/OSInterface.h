#ifndef _os_interface_h__
#define _os_interface_h__

#ifdef _PSX_IDE_QT_
    #include "QT/stdafx.h"
    static const char*   _P_FOLDER_PATH_SEPARATOR_ =   "/"  ;
    static const char   _FOLDER_PATH_SEPARATOR_ =   '/'  ;

#ifndef _PSX_QT_LINUX_
	#define __uint64 unsigned __int64
#endif

#else
     static const char*   _P_FOLDER_PATH_SEPARATOR_ =   "\\" ;
     static const char   _FOLDER_PATH_SEPARATOR_ =   '\\'  ;
    #define __uint64 unsigned __int64
#endif

#ifdef _PSX_OS_64_
    #define PSX_OBJ_ADDR  __uint64
#else
     #define  PSX_OBJ_ADDR  unsigned long
#endif

#endif   //_os_interface_h__
