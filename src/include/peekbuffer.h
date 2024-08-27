#ifndef _LIBDESOCK_PEEKBUFFER_H
#define _LIBDESOCK_PEEKBUFFER_H

ssize_t peekbuffer_read (size_t len);
size_t peekbuffer_cp (char* dest, size_t len, size_t offset);
size_t peekbuffer_mv (char* dest, size_t len);
size_t peekbuffer_size (void);

#endif /* _LIBDESOCK_PEEKBUFFER_H */