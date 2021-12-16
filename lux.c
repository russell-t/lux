#include <fcntl.h> // open()
#include <unistd.h> // close()
#include <sys/ioctl.h> // ioctl()
#include <linux/i2c-dev.h> // i2c
#include <linux/i2c.h> // i2c_msg
#include <stdio.h>
#include <time.h>
#include <stdlib.h> //exit()

/* See https://www.kernel.org/doc/html/latest/i2c/dev-interface.html */
/*
struct i2c_rdwr_ioctl_data {
	struct i2c_msg *msgs;
	int nmsgs;
};
*/

typedef struct i2c {
	int fd;
	unsigned char addr;
	unsigned char reg;
	unsigned char *buf; //allocate max. number of bytes you want to read/write in a single transaction
} i2c;


i2c* init_i2c(char* i2c_devicename, unsigned char addr, int nbytes) {
	i2c * new = (i2c *) malloc(sizeof(i2c));
	if (new == NULL) {
		printf("Error initializing i2c struct.\n");
		exit(1);
	}
	new->buf = (unsigned char *) malloc(sizeof(unsigned char) * nbytes);
	if (new->buf == NULL) {
		printf("Error allocating byte buffer in i2c struct.\n");
		exit(1);
	}
	if ((new->fd = open(i2c_devicename, O_RDWR)) < 0) {
		printf("Error opening i2c device.\n");
		exit(1);
	}
	new->addr = addr;

	return new;
}

/* this function taken from https://gist.github.com/JamesDunne/9b7fbedb74c22ccc833059623f47beb7 */
typedef unsigned char u8;

int i2c_read(i2c* aa, int nbytes) {

	u8 outbuf[1], inbuf[2];
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	msgs[0].addr = aa->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = outbuf;

	msgs[1].addr = aa->addr;
	msgs[1].flags = I2C_M_RD | I2C_M_NOSTART; 
	msgs[1].len = nbytes; 
	msgs[1].buf = aa->buf;

	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 2;

	outbuf[0] = aa->reg;

	inbuf[0] = 0;
	inbuf[1] = 0;

	aa->buf[0] = 0;
	aa->buf[1] = 0;
	if (ioctl(aa->fd, I2C_RDWR, &msgset) < 0) {
		printf("error reading 2 bytes\n");
		return -1;
	}

	//aa->buf[0] = inbuf[0];
	//aa->buf[1] = inbuf[1];


	return 0;
} 


int main(int argc, char **argv) {

	char i2c_device[]="/dev/i2c-1";
	unsigned char buf[10];
	unsigned char addr = 0x10;
	unsigned char comm = 0x04;

	u8 a, b;
	i2c* bb = init_i2c(i2c_device, addr, 50);

	/* open the i2c device 
	file = open(i2c_device, O_RDWR);

	
	if (file < 0) {
		printf("File did not open.\n");
		exit(1);
	}
	*/

	/* set i2c slave address */
	if (ioctl(bb->fd, I2C_SLAVE, 0x10) < 0) {
		printf("Failed to set i2c address.\n");
		exit(1);
	}

	buf[0] = 0x00; // Command code 0x00: ALS_CONF 0

	// set up config, 1/8 gain 25 ms integration time
	buf[1] = 0x00;
	buf[2] = 0x18;

	write(bb->fd, buf, 3); // write 1 byte: 0000 0000 (command 0x00)

	buf[0] = 0x01;	
	buf[1] = 0x00;
	buf[2] = 0x00;
	write(bb->fd, buf, 3);

	buf[0] = 0x02;
	buf[1] = 0x00;
	buf[2] = 0x00;
	write(bb->fd, buf, 3);
	
	/*	
	buf[0] = 0x03;
	buf[1] = 0x00;
	buf[2] = 0x00;
	result = write(file, buf, 3);
	*/
	bb->reg = 0x04; // set register to 0x04
	while(1) {		
		sleep(1);
	
		/* read 2 bytes from sensor after sending 1 byte command */
		i2c_read(bb, 2); 	
		printf("%x %x\n", bb->buf[1], bb->buf[0]);
	}

	close(bb->fd);
	
	return 0;
}
