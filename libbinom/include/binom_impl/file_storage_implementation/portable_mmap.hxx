#ifndef PORTABLE_MMAP_HXX
#define PORTABLE_MMAP_HXX

namespace pmmap {

#include <cstddef>

#ifdef _WIN32
#include <windows.h>

/**
 * @brief FileDescriptor - alias for native handle of file
 */
typedef HANDLE FileDescriptor;
const FileDescriptor INVALID_FILE_DESCRIPTOR = INVALID_HANDLE_VALUE;
#else

/**
 * @brief FileDescriptor - alias for native handle of file
 */
typedef int FileDescriptor;
const FileDescriptor INVALID_FILE_DESCRIPTOR = -1;
#endif

/**
 * @brief The Protocol enum - describes the desired memory protection mode (it must not conflict with the file open mode)
 */
enum class Protocol : int {
  read        = 0x01, //< data can be read
  write       = 0x02, //< data can be write

#ifdef FILE_MAP_EXECUTE
  exec        = 0x04, //< data ca be executed
  rx          = 0x01 | 0x04,
  wx          = 0x02 | 0x04,
  rwx         = 0x01 | 0x02 | 0x04,
  rxt         = 0x01 | 0x04 | 0x08,
  wxt         = 0x02 | 0x04 | 0x08,
  rwxt        = 0x01 | 0x02 | 0x04 | 0x08,

#else
  exec        = 0x00, //< data ca be executed (FLAG IS DISABLED)
  rx          = 0x01 | 0x00,
  wx          = 0x02 | 0x00,
  rwx         = 0x01 | 0x02 | 0x00,
  rxt         = 0x01 | 0x00 | 0x08,
  wxt         = 0x02 | 0x00 | 0x08,
  rwxt        = 0x01 | 0x02 | 0x00 | 0x08,

#endif
  trunc       = 0x08, //< clear file on open
  rw          = 0x01 | 0x02,
  rt          = 0x01 | 0x08,
  wt          = 0x02 | 0x08,
  rwt         = 0x01 | 0x02 | 0x08

};

/**
 * @brief The MapFlag enum - The flags argument provides other information about the handling of the mapped data. The value of flags is the bitwise inclusive OR of these options
 */
enum class MapFlag : int {
  shared      = 0x01,
  priv        = 0x02,
  anonymous   = 0x04,
  anon        = anonymous,
#ifndef _WIN32 // Not implemented in windows
  denywrite   = 0x08,
  executable  = 0x10,
  noreserve   = 0x20,
  locked      = 0x30,
  growsdown   = 0x80,
  file        = 0x100
#endif
};

/**
 * @brief mmapPortable - portable analogue of Unix mmap, see: https://www.opennet.ru/man.shtml?topic=mmap&category=&submit=%F0%CF%CB%C1%DA%C1%D4%D8+man
 */
extern void* mmapPortable(void* start, size_t length, Protocol prot, MapFlag flags, FileDescriptor fd, std::size_t offset);
/**
 * @brief munmapPortable - portable analogue of Unix munmap, see: https://www.opennet.ru/man.shtml?topic=mmap&category=&submit=%F0%CF%CB%C1%DA%C1%D4%D8+man
 */
extern void munmapPortable(void *addr, std::size_t length);

}

#endif // PORTABLE_MMAP_HXX
