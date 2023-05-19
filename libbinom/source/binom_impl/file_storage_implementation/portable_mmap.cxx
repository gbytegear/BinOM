#include "libbinom/include/binom_impl/file_storage_implementation/portable_mmap.hxx"

#ifdef _WIN32

#ifdef __USE_FILE_OFFSET64
# define DWORD_HI(x) (x >> 32)
# define DWORD_LO(x) ((x) & 0xffffffff)
#else
# define DWORD_HI(x) (0)
# define DWORD_LO(x) (x)
#endif

#else
#include <unistd.h>
#include <sys/mman.h>
#endif

using namespace pmmap;

extern void* mmapPortable(void* start, size_t length, Protocol prot, MapFlag flags, FileDescriptor fd, std::size_t offset) {
#ifndef _WIN32
  return ::mmap(
        start,
        length,
        ( ( (int)0x00 )
        | ( ( (int)prot & (int)Protocol::read ) ? PROT_READ : 0x00 )
        | ( ( (int)prot & (int)Protocol::write ) ? PROT_WRITE : 0x00 )
        | ( ( (int)prot & (int)Protocol::write ) ? PROT_EXEC : 0x00 ) ),
        ( ( (int)0x00 )
        | ( ( (int)flags & (int)MapFlag::anonymous ) ? MAP_ANONYMOUS : 0x00 )
        | ( ( (int)flags & (int)MapFlag::priv ) ? MAP_PRIVATE : 0x00 )
        | ( ( (int)flags & (int)MapFlag::shared ) ? MAP_SHARED : 0x00 )
        | ( ( (int)flags & (int)MapFlag::denywrite ) ? MAP_DENYWRITE : 0x00 )
        | ( ( (int)flags & (int)MapFlag::executable ) ? MAP_EXECUTABLE : 0x00 )
        | ( ( (int)flags & (int)MapFlag::noreserve ) ? MAP_NORESERVE : 0x00 )
        | ( ( (int)flags & (int)MapFlag::locked ) ? MAP_LOCKED : 0x00 )
        | ( ( (int)flags & (int)MapFlag::growsdown ) ? MAP_GROWSDOWN : 0x00 )
        | ( ( (int)flags & (int)MapFlag::file ) ? MAP_FILE : 0x00 ) ),
        fd,
        static_cast<off_t>(offset)
        );

#else
  if(fd == INVALID_FILE_DESCRIPTOR && (!((int)flags & (int)MapFlag::anon) || offset))
      return (void*) -1;

  DWORD fl_protect =
    ((int)prot & (int)Protocol::write)
    ? ((int)prot & (int)Protocol::exec)
      ? PAGE_EXECUTE_READWRITE
      : PAGE_READWRITE
    : ((int)prot & (int)Protocol::exec)
    ? ((int)prot & (int)Protocol::read)
      ? PAGE_EXECUTE_READ
      : PAGE_EXECUTE
    : PAGE_READONLY;

  std::size_t end = length + offset;
  HANDLE h = CreateFileMapping(fd, NULL, fl_protect, DWORD_HI(end), DWORD_LO(end), NULL);

  DWORD dwDesiredAccess =
    ((int)prot & (int)Protocol::write)
    ? FILE_MAP_WRITE
    : FILE_MAP_READ;
  if((int)prot & (int)Protocol::exec)
    dwDesiredAccess |= FILE_MAP_EXECUTE;
  if((int)flags & (int)MapFlag::priv)
    dwDesiredAccess |= FILE_MAP_COPY;

  void* ret = MapViewOfFile(h, dwDesiredAccess, DWORD_HI(offset), DWORD_LO(offset), length);
  if(ret == nullptr) {
    CloseHandle(h);
    ret = (void*) -1;
  }

  return ret;
#endif
}



extern void pmmf::munmapPortable(void *addr, std::size_t length) {
#ifndef _WIN32
  munmap(addr, length);
#else
  UnmapViewOfFile(addr);
#endif
}
