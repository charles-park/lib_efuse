//------------------------------------------------------------------------------
/**
 * @file lib_efuse.c
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
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/fb.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include "lib_efuse.h"

//------------------------------------------------------------------------------
// Debug msg
//------------------------------------------------------------------------------
#if defined (__LIB_SINGLE_APP__)
    #define dbg_msg(fmt, args...)   printf(fmt, ##args)
#else
    #define dbg_msg(fmt, args...)
#endif

//------------------------------------------------------------------------------
const char *eFuseRWControl = "/sys/class/block/mmcblk0boot0/force_ro";
const char *eFuseRWFile = "/dev/mmcblk0boot0";

const char *M1S_MAC_START = "001E0653"; // 65536개
const char  M1S_MAC_BLOCK = 2;          // 2개의 block reserved (13만개)

//------------------------------------------------------------------------------
// function prototype
//------------------------------------------------------------------------------
static void tolowerstr  (char *p);
static void toupperstr  (char *p);
static int  efuse_lock  (char lock);

int  efuse_valid_check  (const char *efuse_data);
void efuse_get_mac      (const char *efuse_data, char *mac);
int  efuse_control      (char *efuse_data, char control);

//------------------------------------------------------------------------------
// 문자열 변경 함수. 입력 포인터는 반드시 메모리가 할당되어진 변수여야 함.
//------------------------------------------------------------------------------
static void tolowerstr (char *p)
{
    int i, c = strlen(p);

    for (i = 0; i < c; i++, p++)
        *p = tolower(*p);
}

//------------------------------------------------------------------------------
static void toupperstr (char *p)
{
    int i, c = strlen(p);

    for (i = 0; i < c; i++, p++)
        *p = toupper(*p);
}

//------------------------------------------------------------------------------
static int efuse_lock (char lock)
{
    int fd;

    if ((fd = open (eFuseRWControl, O_WRONLY)) < 0) {
        printf ("error, file write mode open (%s)\n", eFuseRWControl);
        return 0;
    }
    if (!write (fd, lock ? "1" : "0", 1))
        printf ("error, write size different.\n");

    close (fd);
    return 1;
}

//------------------------------------------------------------------------------
int efuse_valid_check (const char *efuse_data)
{
    char data[10], addr, mac;

    memset (data, 0, sizeof(data));
    strncpy (data, &efuse_data[EFUSE_MAC_OFFSET + 6], 2);
    mac = atoi(data);

    memset  (data, 0, sizeof(data));
    strncpy (data, &M1S_MAC_START[6], 2);
    addr = atoi(data);

    if ((mac >= addr) && (mac < addr + M1S_MAC_BLOCK)) {
        dbg_msg ("success, efuse data = %s, mac = %s\n",
            efuse_data, &efuse_data[EFUSE_MAC_OFFSET]);
        return 1;
    }

    printf ("error, ODROID-M1S mac range.\n");
    printf ("mac : %s, range %s0000 ~ (%d block)\n",
        &efuse_data[EFUSE_MAC_OFFSET], M1S_MAC_START, M1S_MAC_BLOCK);

    return 0;
}

//------------------------------------------------------------------------------
void efuse_get_mac (const char *efuse_data, char *mac)
{
    memset  (mac, 0, MAC_STR_SIZE);
    strncpy (mac, &efuse_data[EFUSE_MAC_OFFSET], MAC_STR_SIZE);
}

//------------------------------------------------------------------------------
int efuse_control (char *efuse_data, char control)
{
    int fd;
    char size;

    if (access (eFuseRWFile, F_OK) != 0) {
        dbg_msg ("error, eFuse read/write file not found.(%s)\n", eFuseRWFile);
        return 0;
    }
    if (access (eFuseRWControl, F_OK) != 0) {
        dbg_msg ("error, eFuse control file not found.(%s)\n", eFuseRWControl);
        return 0;
    }
    if (efuse_data == NULL) {
        dbg_msg ("error, eFuse data is NULL.\n");
        return 0;
    }

    toupperstr(efuse_data);

    switch (control) {
        case EFUSE_ERASE:
            memset (efuse_data, 0, EFUSE_SIZE_M1S);
        case EFUSE_WRITE:
            if (!efuse_lock(EFUSE_UNLOCK))
                return 0;

            if ((fd = open (eFuseRWFile, O_WRONLY)) < 0) {
                printf ("error, file write mode open (%s)\n", eFuseRWFile);
                return 0;
            }
            size = write (fd, efuse_data, EFUSE_SIZE_M1S);
            close (fd);

            if (!efuse_lock(EFUSE_LOCK))
                return 0;

            dbg_msg ("success, eFuse data write. efuse = %s\n", efuse_data);
            break;
        case EFUSE_READ:
            memset (efuse_data, 0, EFUSE_SIZE_M1S);
            if ((fd = open (eFuseRWFile, O_RDONLY)) < 0) {
                printf ("error, file read mode open (%s)\n", eFuseRWFile);
                return 0;
            }
            size = read (fd, efuse_data, EFUSE_SIZE_M1S);
            close (fd);
            dbg_msg ("success, eFuse data read. efuse = %s\n", efuse_data);
            break;
        default:
            dbg_msg ("Unknown control cmd.(%d)\n", control);
            return 0;
    }
    if (size != EFUSE_SIZE_M1S) {
        printf ("error, read/write size are different. (read/write size = %d, %d)\n",
            size, EFUSE_SIZE_M1S);
            return 0;
    }
	return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
