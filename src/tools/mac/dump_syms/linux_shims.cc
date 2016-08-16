
#include "linux_shims.h"

#ifdef __cplusplus
extern "C" {
#endif

void *memrchr(const void *buf, int c, size_t n) {
    const char *p = (const char *)buf + n;
    while (--p >= (const char *) buf) {
        if (*p == (const char)c) {
            return (void *)p;
        }
    }
    return NULL;
}

#ifdef __cplusplus
}
#endif
