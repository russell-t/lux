#include <fcntl.h> // open()
#include <unistd.h> // close()
#include <sys/ioctl.h> // ioctl()
#include <linux/i2c-dev.h> // i2c
#include <linux/i2c.h> // i2c_msg
#include <stdio.h>
#include <time.h>
#include <stdlib.h> //exit()
#include "i2c.h"

/* See https://www.kernel.org/doc/html/latest/i2c/dev-interface.html */
/*
struct i2c_rdwr_ioctl_data_bytes {
	struct i2c_msg *msgs;
	int nmsgs;
};
*/
int main(int argc, char **argv) {

	char i2c_device[]="/dev/i2c-1";

	unsigned char cmd0[3] = {0x00, 0x00, 0x18};
	unsigned char cmd1[3] = {0x01, 0x00, 0x00};
	unsigned char cmd2[3] = {0x02, 0x00, 0x00};
	unsigned char buffer[3];
	unsigned char buff[2] = {0x0f, 0x00};

	i2c* bb = init_i2c(i2c_device, 0x10);
	i2c* cc = init_i2c(i2c_device, 0x1d);

	i2c_write(bb, cmd0, 2);
	i2c_write(bb, cmd1, 2);
	i2c_write(bb, cmd2, 2);

	buffer[0] = 0x04;
	i2c_read(cc, buff, 1);
	printf("Accelerometer reading: 0x%x\n", buff[1]);

	while(1) {		

		sleep(1);
		i2c_read(bb, buffer, 2); 	
		printf("0x%x 0x%x\n", buffer[2], buffer[1]);
	}

	close(bb->fd);
	free(bb);
	
	return 0;
}
