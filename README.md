# libdesock

Network applications are hard to fuzz with traditional fuzzers because they a) 
expect their input over network connections, and b) process multiple inputs (multiple packets) in a single run.   

libdesock solves this problem by a) redirecting network I/O to stdin and stdout, and b) treating an input
as a sequence of chunks that get individually fed to the application.

It functions as a library that when preloaded with `LD_PRELOAD` replaces the network stack of the
libc with its own implementation that emulates all network operations in user-space.   
This has multiple advantages for fuzzing:

1. It reduces the syscalls of the target
2. It automatically synchronizes multi-threaded programs
3. No extra harnessing is needed to get the fuzz input to the application

## Usage
Prepend
```sh
LD_PRELOAD=libdesock.so
```
to the invocation of any network application or
set the environment variable
```sh
AFL_PRELOAD=libdesock.so
```
when using AFL++.

## Building
Libdesock uses `meson` as its build system.

```sh
meson setup ./build
cd ./build
```

You can configure the build using
```sh
meson configure -D <optname>=<optvalue>
```

You can get an overview over all options with
```sh
meson configure
```

The following options are specific to libdesock:

| Option           | Description                                                                                | Default |
|------------------|--------------------------------------------------------------------------------------------|---------|
| `arch`           | The CPU architecture for which you are compiling libdesock.so                              | x86_64  |
| `multiple_requests`| If this is true, a delimiter will be used to return different data from subsequent read calls     | false   |
| `request_delimiter` | The delimiter that separates multiple requests | `-=^..^=-` |
| `desock_client`  | If this is true, calls to `connect()` get hooked. This enables the desocketing of clients | false   |
| `desock_server`  | If this is true, calls to `bind()` get hooked. This enables the desocketing of servers    | true    |
| `allow_dup_stdin`| If this is true, calls to `dup*()` are allowed for stdin. This enables stdin redirection when running under gdb    | false   |
| `debug_desock`   | If this is true, calls to functions in libdesock.so get logged to stderr                  | false   |
| `fd_table_size`  | Only fds < `fd_table_size` can be desocked                                                | 128     |
| `interpreter`    | Path to ld.so (will be determined dynamically if not set)                                  |         |
| `symbol_version` | If this is set, every exported symbol has this version |  |
| `max_conns` | The number of simulatenous connections that can be desocketed. Any value > 1 doesn't really make sense in the default configuration | 1 |

If configuration is done compile with
```sh
meson compile
```

This creates a shared library `build/libdesock.so` and a static library `build/libdesock.a`.

## Examples
If you are using libdesock and AFL++ for fuzzing, the programs under test
usually require a special setup to work with AFL++. Checkout our [examples](./examples) 
directory for some examples on how to properly setup network applications for fuzzing.

## Known Bugs
- TCP servers using [libuv](https://libuv.org/) cannot be de-socket-ed (yet). De-socketing of libuv currently only works with UDP servers. It only takes a small change to fix this though, if anyone needs this create an issue.
- `ioctl()` is not supported. Make sure your target does not rely on `ioctl` requests
