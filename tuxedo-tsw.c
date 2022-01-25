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

#define HID_NAME "UNIW0001:00 093A:0255"

static const char* tmpfile_path = "/tmp/tuxedo-tsw.tmp";
static char buffer[10];

static int find_device(void) {
   char path_uevent[50];
   char line[100];
   for (int i = 0; i < 10; ++i) {
      snprintf(path_uevent, sizeof(path_uevent), "/sys/class/hidraw/hidraw%d/device/uevent", i);
      FILE* file = fopen(path_uevent, "r");
      if (!file)
         continue;
      while (!feof(file)) {
         fgets(line, sizeof(line), file);
         if (!strcmp(line, "HID_NAME=" HID_NAME "\n")) {
            fclose(file);
            return i;
         }
      }
      fclose(file);
   }
   return -1;
}

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

static const char* read_tmpfile(void) {
   FILE* file = fopen(tmpfile_path, "r");
   if (!file) {
      fprintf(stderr, "tuxedo-tsw: failed to read '%s': %s\n",
            tmpfile_path, strerror(errno));
      return "on";
   }
   fgets(buffer, sizeof(buffer), file);
   fclose(file);
   return buffer;
}

static void write_tmpfile(const char* text) {
   FILE* file = fopen(tmpfile_path, "w");
   if (!file) {
      fprintf(stderr, "tuxedo-tsw: failed to update '%s': %s\n",
            tmpfile_path, strerror(errno));
      return;
   }
   fprintf(file, "%s\n", text);
   fprintf(stderr, "tuxedo-tsw: new state: %s\n", text);
   fclose(file);
}

int main(int argc, char** argv) {
   if (argc < 2) {
   print_usage:
      puts("Usage: tuxedo-tsw on|off|toggle");
      return 1;
   }

   bool enabled;
   if (!strcmp(argv[1], "toggle"))
      enabled = !!strcmp(read_tmpfile(), "on\n");
   else if (!strcmp(argv[1], "on")) enabled = true;
   else if (!strcmp(argv[1], "off")) enabled = false;
   else goto print_usage;

   int n = find_device();
   if (n < 0) {
      errno = 0;
      panic("failed to find device");
   }

   char device[30];
   snprintf(device, sizeof(device), "/dev/hidraw%d", n);

   const int fd = open(device, O_WRONLY | O_NONBLOCK);
   if (fd < 0)
      panic("failed to open '%s'", device);

   char buffer[2] = { 0x07, enabled ? 0x03 : 0x00 };

   if (ioctl(fd, HIDIOCSFEATURE(sizeof(buffer)), buffer) < 0)
      panic("failed to do ioctl() on '%s'", device);

   close(fd);

   write_tmpfile(enabled ? "on" : "off");

   return 0;
}
