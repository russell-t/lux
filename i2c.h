#ifndef __I2C_H
#define __I2C_H

typedef struct i2c {
	int fd;
	unsigned char addr;
} i2c;

i2c* init_i2c(char* i2c_devicename, unsigned char addr);
void destroy_i2c(i2c* aa);
int i2c_write(i2c* aa, unsigned char *data_bytes, int nbytes);
int i2c_read(i2c* aa, unsigned char* data_bytes, int nbytes);

#endif
