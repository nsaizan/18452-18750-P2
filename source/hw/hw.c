#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "interface.h"

int main() {
	interface_init();

	while(1){
		file_interface_send();
		file_interface_receive();
	}

	return 0;
}
