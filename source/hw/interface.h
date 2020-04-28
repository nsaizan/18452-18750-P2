#include <stdio.h>
#include <math.h>
#include "audioDeviceInterface.h"
#include "quadratureModulation.h"

/* file interface globals */
char* rec_fifo;
char* send_fifo;
int rec_fd;
int send_fd;

void interface_init(void);

void interface_deinit(void);

void file_interface_deinit(void);

void file_interface_receive(void);

void file_interface_send(void);
