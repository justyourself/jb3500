#ifndef __LITECORE_H__
#define __LITECORE_H__


#ifdef __cplusplus
extern "C" {
#endif



//Header Includes
#include <def.h>

#include "bsp_cfg.h"

#include <lib/lib.h>

#include <mtd/flash.h>

#if OS_TYPE
#include <fs/fs.h>

#include <chl/chl.h>

#include <reg/reg.h>

#include <cp/cp.h>
#endif

#include <dbg/dbg.h>

#if MODEM_ENABLE
#include <drivers/modem.h>
#endif



#if TCPPS_ENABLE
#include <net/net.h>
#endif



//Warning
#if DEBUG_ENABLE
#warning "DebugMode is enabled........................."
#endif
#if WDG_ENABLE == 0
#warning "WatchDog is disabled........................."
#endif




#ifdef __cplusplus
}
#endif


#endif

