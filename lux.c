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

/* this function taken from https://gist.github.com/JamesDunne/9b7fbedb74c22ccc833059623f47beb7 */
typedef unsigned char u8;

int i2c_read(int file, u8 slave_addr, u8 reg, u8 *a, u8 *b) {

	//int retval;
	u8 outbuf[1], inbuf[2];
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	msgs[0].addr = slave_addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = outbuf;

	msgs[1].addr = slave_addr;
	msgs[1].flags = I2C_M_RD | I2C_M_NOSTART; 
	msgs[1].len = 2; 
	msgs[1].buf = inbuf;

	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 2;

	outbuf[0] = reg;

	inbuf[0] = 0;
	inbuf[1] = 0;

	*a = 0;
	*b = 0;
	if (ioctl(file, I2C_RDWR, &msgset) < 0) {
		printf("error reading 2 bytes\n");
		return -1;
	}

	*a = inbuf[0];
	*b = inbuf[1];


	return 0;
} 


int main(int argc, char **argv) {

	int file;
	char i2c_device[]="/dev/i2c-1";
	unsigned char buf[10];
	int result;
	int addr = 0x10;
	unsigned char comm = 0x04;
	unsigned char a, b;

	/* open the i2c device */
	file = open(i2c_device, O_RDWR);

	/* error handling */
	if (file < 0) {
		printf("File did not open.\n");
		exit(1);
	}

	/* set i2c slave address */
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		printf("Failed to set i2c address.\n");
		exit(1);
	}

	buf[0] = 0x00; // Command code 0x00: ALS_CONF 0

	// set up config, 1/8 gain 25 ms integration time
	buf[1] = 0x00;
	buf[2] = 0x18;

	result = write(file, buf, 3); // write 1 byte: 0000 0000 (command 0x00)

	buf[0] = 0x01;	
	buf[1] = 0x00;
	buf[2] = 0x00;
	result = write(file, buf, 3);

	buf[0] = 0x02;
	buf[1] = 0x00;
	buf[2] = 0x00;
	result = write(file, buf, 3);
	
	/*	
	buf[0] = 0x03;
	buf[1] = 0x00;
	buf[2] = 0x00;
	result = write(file, buf, 3);
	*/
	while(1) {		
		sleep(1);
	
		/* read 2 bytes from sensor after sending 1 byte command */
		i2c_read(file, (u8)addr, (u8)comm, &a, &b); 	
		printf("%x %x\n", b, a);
	}

	close(file);
	
	return 0;
}
