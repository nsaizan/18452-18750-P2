#include <stdio.h>
#include <math.h>
#include "audioDeviceInterface.h"
#include "quadratureModulation.h"

/* file interface globals */
char* rec_fifo, send_fd;
int rec_fd, send_fd;

void file_interface_init(void);

void file_interface_deinit(void);

void file_interface_receive(void);

void file_interface_send(void);
