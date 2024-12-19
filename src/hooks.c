/*  Hooks let you customize what happens
 *  when an application reads from / writes to
 *  a network connection.
 *  The default behavior is to read from stdin /
 *  write to stdout but this can easily be changed
 *  in the functions below.
 */

#include <unistd.h>

#include "util.h"
#include "hooks.h"
#include "syscall.h"

static struct {
    const unsigned char *buffer;
    size_t len;
    size_t offset;
} input_buffer;

VISIBLE
void libdesock_hooks_buffer_init(const unsigned char *buf, size_t len) {
    input_buffer.buffer = buf;
    input_buffer.len = len;
    input_buffer.offset = 0;
}

/*  This function is called whenever a read on a network
 *  connection occurs. It MUST return the number of bytes
 *  written to buf or -1 if an error occurs.
 */
ssize_t hook_input (char* buf, size_t size) {
    ssize_t available_len = input_buffer.len - (ssize_t) input_buffer.offset;
    if (UNLIKELY(available_len < 0)) return -1;
    size_t result_len = (size_t) available_len < size? available_len : size;
    __builtin_memcpy(buf, input_buffer.buffer + input_buffer.offset, result_len);
    input_buffer.offset += result_len;
    return result_len;
}

/*  This function is called whenever a write on a network
 *  connection occurs. It MUST return the number of bytes
 *  written or -1 if an error occurs.
 */
ssize_t hook_output (const char* buf, size_t size) {
#ifdef DEBUG
    return syscall_cp(SYS_write, 1, buf, size);
#else
    (void) buf;
    return (ssize_t) size;
#endif
}

/*  This function is called whenever libdesock internally
 *  searches through the input stream. It MUST behave like
 *  the lseek() function in the sense that on success, it
 *  must return the resulting offset and on error it 
 *  must return -1.
 *  The supplied offset always is relative to the current
 *  stream position.
 */
off_t hook_seek (off_t offset) {
    off_t new_offset = input_buffer.offset + offset;
    if (UNLIKELY(new_offset < 0 || (size_t) new_offset > input_buffer.len))
        return -1;
    input_buffer.offset = new_offset;
    return new_offset;
}
