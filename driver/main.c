#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int keepRunning = 1;
struct termios tty;
char read_buf[256];

void intHandler(int dummy)
{
  keepRunning = 0;
}

int main(int argc, char* argv[])
{
  signal(SIGINT, intHandler);
  int serial_port = open("/dev/ttyUSB0", O_RDWR);
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
  }
  if (tcgetattr(serial_port, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)

  tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
  tty.c_cflag |= CS8;    // 8 bits per byte (most common)

  tty.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
  }

  Display* display = XOpenDisplay(NULL);
  if (!display) {
    fprintf(stderr, "Unable to open display\n");
    return 1;
  }

  Window root = DefaultRootWindow(display);

  XWindowAttributes windowAttributes;
  XGetWindowAttributes(display, root, &windowAttributes);

  int width = windowAttributes.width;
  int height = windowAttributes.height;

  while (keepRunning) {
    int x, y, win_x, win_y;
    unsigned int mask_return;

    Window root_return, child_return;

    XQueryPointer(display, root, &root_return, &child_return, &x, &y, &win_x, &win_y, &mask_return);

    // printf("X is %d Y is %d \n", x, y);
    XImage* image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);

    if (!image) {
      fprintf(stderr, "Unable to get image\n");
      XCloseDisplay(display);
      return 1;
    }

    unsigned long pixel = XGetPixel(image, x, y);
    XDestroyImage(image);

    //    printf("Red mask %d \n", image->red_mask);
    //   printf("green mask: %d\n", image->green_mask);
    //    printf("bue mask: %d\n", image->blue_mask);
    //
    //      printf("Pixel at (%d, %d): %lu\n", x, y, pixel);
    //
    printf("Coordinates %d - %d ", x, y);
    printf("RGB: %02lx", (pixel & image->red_mask) >> 16);
    printf("%02lx", (pixel & image->green_mask) >> 8);
    printf("%02lx\n", (pixel & image->blue_mask));

    unsigned char msg[] = {
      ((pixel & image->red_mask) >> 16),
      ((pixel & image->green_mask) >> 8),
      (pixel & image->blue_mask),
      '\r'
    };

    write(serial_port, msg, sizeof(msg));

    //int n = read(serial_port, &read_buf, sizeof(read_buf));

    //printf("Read %i bytes. Received message: %s", n, read_buf);

    usleep(50000); // Sleep for 100,000 microseconds (0.1 seconds)
  }

  // Cleanup
  printf("Stopping .. cleaning\n");
  XCloseDisplay(display);
  close(serial_port);

  return 0;
}
