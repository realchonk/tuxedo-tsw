#define _POSIX_C_SOURCE 700

#include <linux/hidraw.h>
#include <stdnoreturn.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#define HIDRAW_DEVICE "/dev/hidraw0"

static noreturn void panic(const char* fmt, ...) {
   va_list ap;
   va_start(ap, fmt);

   fputs("tuxedo-tsw: ", stderr);
   vfprintf(stderr, fmt, ap);

   if (errno) {
      fprintf(stderr, ": %s\n", strerror(errno));
   } else fputc('\n', stderr);

   va_end(ap);
   exit(1);
}

int main(int argc, char** argv) {
   if (argc < 2) {
   print_usage:
      puts("Usage: tuxedo-tsw <on/off>");
      return 1;
   }

   bool enabled;
   if (!strcmp(argv[1], "on")) enabled = true;
   else if (!strcmp(argv[1], "off")) enabled = false;
   else goto print_usage;

   const int fd = open(HIDRAW_DEVICE, O_WRONLY | O_NONBLOCK);
   if (fd < 0)
      panic("failed to open " HIDRAW_DEVICE);

   char buffer[2] = { 0x07, enabled ? 0x03 : 0x00 };

   if (ioctl(fd, HIDIOCSFEATURE(sizeof(buffer)), buffer) < 0)
      panic("failed to do ioctl() on " HIDRAW_DEVICE);

   close(fd);
   return 0;
}
