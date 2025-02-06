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
#define EFUSE_READ      0
#define EFUSE_ERASE     1
#define EFUSE_WRITE     2

#define EFUSE_UNLOCK    0
#define EFUSE_LOCK      1

// mac string size aabbccddeeff
#define MAC_STR_SIZE    12

// uuid + mac (00000000-0000-0000-0000-001e06xxxxxx)
#define EFUSE_UUID_SIZE 36

//------------------------------------------------------------------------------
#define IOC_WRITE       0x7673
#define IOC_DUMP        0x7674
#define IOC_ERASE       0x7675

#define IOC_MSTR_WRITE_M1   "HKM1"
#define IOC_MSTR_WRITE_C4   "HKN2"

#define IOC_MSTR_DUMP       "1234"

#define UUID_FLASH_SIZE     0x80
#define UUID_WRITE_SIZE     32

struct ioc_data {
    char            mstr[4];
    int             offset;
    int             len;
    unsigned char   cksum;
    char            uuid [32];
}   __attribute__((packed));

//------------------------------------------------------------------------------
enum {
    eBOARD_ID_M1 = 0,
    eBOARD_ID_M1S,
    eBOARD_ID_M2,
    eBOARD_ID_C4,
    eBOARD_ID_C5,
    eBOARD_ID_END
};

//------------------------------------------------------------------------------
//	function prototype
//------------------------------------------------------------------------------
extern int  efuse_set_board     (int board_id);
extern int  efuse_get_board     (void);
extern int  efuse_valid_check   (const char *efuse_data);
extern void efuse_get_mac       (const char *efuse_data, char *mac);
extern int  efuse_control       (char *efuse_data, char control);

//------------------------------------------------------------------------------
#endif  // #ifndef __LIB_EFUSE_H__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
