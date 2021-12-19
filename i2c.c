#include <fcntl.h> // open()
#include <unistd.h> // close()
#include <sys/ioctl.h> // ioctl()
#include <linux/i2c-dev.h> // i2c
#include <linux/i2c.h> // i2c_msg
#include <stdio.h>
#include <stdlib.h> //exit()
#include "i2c.h"

/* See https://www.kernel.org/doc/html/latest/i2c/dev-interface.html */
/*
struct i2c_rdwr_ioctl_data_bytes {
	struct i2c_msg *msgs;
	int nmsgs;
};
*/

i2c* init_i2c(char* i2c_devicename, unsigned char addr) {

	i2c * new = (i2c *) malloc(sizeof(i2c));
	if (new == NULL) {
		printf("Error initializing i2c struct.\n");
		exit(1);
	}
	if ((new->fd = open(i2c_devicename, O_RDWR)) < 0) {
		printf("Error opening i2c device.\n");
		exit(1);
	}
	new->addr = addr;

	return new;
}

void destroy_i2c(i2c* aa) {

	close(aa->fd);
	free(aa);

}

int i2c_write(i2c* aa, unsigned char *data_bytes, int nbytes) {

	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];
	
	msgs[0].addr = aa->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1+nbytes;
	msgs[0].buf = data_bytes;

	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 1;

	/* Write */
	if (ioctl(aa->fd, I2C_RDWR, &msgset) < 0) {
		printf("Error writing to i2c device.\n");
		return -1;
	}

	return 0;
}

int i2c_read(i2c* aa, unsigned char* data_bytes, int nbytes) {

	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	msgs[0].addr = aa->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = data_bytes;

	msgs[1].addr = aa->addr;
	msgs[1].flags = I2C_M_RD | I2C_M_NOSTART; 
	msgs[1].len = nbytes; 
	msgs[1].buf = &data_bytes[1];

	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 2;

	if (ioctl(aa->fd, I2C_RDWR, &msgset) < 0) {
		printf("Failed to read %d bytes from i2c address 0x%x.\n", nbytes, aa->addr);
		return -1;
	}

	return 0;
} 

