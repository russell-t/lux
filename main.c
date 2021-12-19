#include <fcntl.h> // open()
#include <unistd.h> // close()
#include <sys/ioctl.h> // ioctl()
#include <linux/i2c-dev.h> // i2c
#include <linux/i2c.h> // i2c_msg
#include <stdio.h>
#include <time.h>
#include <stdlib.h> //exit()
#include "i2c.h"

i2c* veml7700_init(void) {

	/* veml7700 is located at device address 0x10 */
	i2c *lux = init_i2c("/dev/i2c-1", 0x10);	

	unsigned char init[3][3] = {
					{0x00, 0x00, 0x18},
					{0x01, 0x00, 0x00},
					{0x02, 0x00, 0x00}
	};

	int i;
	for (i = 0; i<3; i++) {
		i2c_write(lux, init[i], 2);
	}

	return lux;
}

int main(int argc, char **argv) {


	unsigned char buffer[3];
	i2c* lux = veml7700_init();
	buffer[0] = 0x04;

	while(1) {		
		sleep(1);
		i2c_read(lux, buffer, 2); 	
		printf("0x%.2x 0x%.2x\n", buffer[2], buffer[1]);
	}

	destroy_i2c(lux);
	
	return 0;
}
