
#ifndef	_LINUX_SHIMS_H
#define	_LINUX_SHIMS_H	1

#if !__APPLE__
#   error for Darwin compilation of dump_syms_elf
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "elf.h" // Local Breakpad Android ELF additions
#include <unistd.h>
#include <sys/stat.h>
    
#define sys_readlink readlink
#define sys_open open
#define sys_fstat fstat
#define sys_fstat64 fstat
#define sys_close close
#define sys_mmap mmap
#define sys_mmap2 mmap
#define sys_munmap munmap
#define kernel_stat stat
#define kernel_stat64 stat


/* ------------------------------------------------------------------------- */
/* Elf stuff-n-things gleaned from <elf.h> on Linux */

#if defined __x86_64__ && !defined __ILP32__
# define __WORDSIZE 64
#else
# define __WORDSIZE 32
#endif

#define __ELF_NATIVE_CLASS __WORDSIZE

/* We use this macro to refer to ELF types independent of the native wordsize.
   `ElfW(TYPE)' is used in place of `Elf32_TYPE' or `Elf64_TYPE'.  */
#define ElfW(type)	_ElfW (Elf, __ELF_NATIVE_CLASS, type)
#define _ElfW(e,w,t)	_ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)	e##w##t

/* ------------------------------------------------------------------------- */
/* Apparently Darwin does not have some needful things */

#define N_UNDF 0

void *memrchr(const void *s, int c, size_t n);
    
#ifdef __cplusplus
}
#endif

#endif /* whole file */
