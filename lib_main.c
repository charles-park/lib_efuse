//------------------------------------------------------------------------------
/**
 * @file lib_main.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief efuse library app.
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
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/fb.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include "lib_efuse.h"

//------------------------------------------------------------------------------
#if defined(__LIB_EFUSE_APP__)

//------------------------------------------------------------------------------
const char *OPT_BOARD_NAME  = NULL;
const char *OPT_ADD_CONTROL = NULL;
const char *OPT_EFUSE_DATA  = NULL;

static char OPT_EFUSE_CONTROL = 0;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void print_usage(const char *prog)
{
    puts("");
    printf("Usage: %s [-rwecbm]\n", prog);
    puts("");

    puts("  -r --efuse_read         efuse read.\n"
         "  -w --efuse_write        efuse write\n"
         "  -e --efuse_erase        efuse erase\n"
         "  -c --efuse_check        efuse data vaild check\n"
         "  -b --efuse_board        board name (default m1s)\n"
         "  -m --efuse_mac          Display the mac in the read data\n"
         "\n"
         "   e.g) lib_efuse -b m1s -w dcbaa404-91bd-4a63-b5f1-001e06520000\n"
         "        lib_efuse -b m1s -c \n"
         "        lib_efuse -e\n"
         "        lib_efuse -r\n"
         "        lib_efuse -m\n"
    );
    exit(1);
}

//-------------------e-----------------------------------------------------------
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
static void parse_opts (int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            { "efuse_read", 0, 0, 'r' },
            { "efuse_write",1, 0, 'w' },
            { "efuse_erase",0, 0, 'e' },
            { "efuse_check",0, 0, 'c' },
            { "efuse_board",1, 0, 'b' },
            { "efuse_mac",  0, 0, 'm' },
            { NULL, 0, 0, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "rw:ecb:m", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
        case 'r':
            OPT_EFUSE_CONTROL = EFUSE_READ;
            break;
        case 'w':
            OPT_EFUSE_CONTROL = EFUSE_WRITE;
            OPT_EFUSE_DATA    = optarg;
            break;
        case 'e':
            OPT_EFUSE_CONTROL = EFUSE_ERASE;
            break;
        case 'c':
            OPT_ADD_CONTROL   = "valid_check";
            break;
        case 'm':
            OPT_ADD_CONTROL   = "mac_read";
            break;
        case 'b':
            toupperstr(optarg);
            OPT_BOARD_NAME = optarg;
            break;
        default:
            print_usage(argv[0]);
            break;
        }
    }
}

//------------------------------------------------------------------------------
int main (int argc, char **argv)
{
    char efuse_data[EFUSE_UUID_SIZE+1];

    parse_opts (argc, argv);

    if (argc < 2)
        print_usage("argc count < 2");
    memset  (efuse_data, 0, sizeof(efuse_data));

    // default board is m1s
    efuse_set_board (eBOARD_ID_M1S);

    if (OPT_BOARD_NAME != NULL) {
        if (!strncmp (OPT_BOARD_NAME, "M2", sizeof("M2")))
            efuse_set_board (eBOARD_ID_M2);
    }

    switch (OPT_EFUSE_CONTROL) {
        case EFUSE_WRITE:
            if (OPT_EFUSE_DATA != NULL) {
                memcpy (efuse_data, OPT_EFUSE_DATA, EFUSE_UUID_SIZE);
                toupperstr(efuse_data);
            }
        case EFUSE_READ: case EFUSE_ERASE:
            efuse_control (efuse_data, OPT_EFUSE_CONTROL);
            break;
        default :
            break;
    }
    if (OPT_ADD_CONTROL != NULL) {
        efuse_control (efuse_data, EFUSE_READ);
        if (!strncmp (OPT_ADD_CONTROL, "mac_read", strlen("mac_read")-1)) {
            char mac[MAC_STR_SIZE];
            memset (mac, 0, sizeof(mac));
            efuse_get_mac (efuse_data, mac);
            printf ("mac : %c%c:%c%c:%c%c:%c%c:%c%c:%c%c\n",
                mac[0],mac[1], mac[2], mac[3],
                mac[4], mac[5], mac[6], mac[7],
                mac[8], mac[9], mac[10],mac[11]);
        }
        if (!strncmp (OPT_ADD_CONTROL, "valid_check", strlen("valid_check")-1)) {
            if (efuse_valid_check (efuse_data))
                printf("success, eFuse data is valid \n");
            else
                printf("error, eFuse data is not valid \n");
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
#endif  // #if defined(__LIB_EFUSE_APP__)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
