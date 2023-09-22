//------------------------------------------------------------------------------
/**
 * @file lib_efuse.h
 * @author charles-park (charles.park@hardkernel.com)
 * @brief efuse library.
 * @version 0.2
 * @date 2023-09-22
 *
 * @package apt install cups cups-bsd
 *
 * @copyright Copyright (c) 2022
 *
 */
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef __LIB_EFUSE_H__
#define __LIB_EFUSE_H__

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ODROID-M1S
//------------------------------------------------------------------------------
#define EFUSE_READ          0
#define EFUSE_ERASE         1
#define EFUSE_WRITE         2

#define EFUSE_UNLOCK        0
#define EFUSE_LOCK          1

// mac string size aabbccddeeff
#define MAC_STR_SIZE        12

// uuid + mac
#define EFUSE_SIZE_M1S      36

// mac address start pos
#define EFUSE_MAC_OFFSET    (EFUSE_SIZE_M1S - MAC_STR_SIZE)

//------------------------------------------------------------------------------
//	function prototype
//------------------------------------------------------------------------------
extern int  efuse_valid_check   (const char *efuse_data);
extern void efuse_get_mac       (const char *efuse_data, char *mac);
extern int  efuse_control       (char *efuse_data, char control);

//------------------------------------------------------------------------------
#endif  // #ifndef __LIB_EFUSE_H__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
