#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main() 
{
	// I2Cバスの生成
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// I2Cデバイスの取得, MMA7660FCのアドレスは0x4C(76)
	ioctl(file, I2C_SLAVE, 0x4C);

	// サンプリングレートレジスタ(0x08)の書き換え
	// 1 Sample/second(0x07)
	char config[2] = {0};
	config[0] = 0x08;
	config[1] = 0x07;
	write(file, config, 2);

	// Select mode register(0x07)
	// Active mode(0x01)
	config[0] = 0x07;
	config[1] = 0x01;
	write(file, config, 2);
	sleep(1);

	// Read 3 bytes of data
	// xAccl, yAccl, zAccl
	char reg[1] = {0x00};
	write(file, reg, 1);
	char data[3] = {0};
	if(read(file, data, 3) != 3)
	{
		printf("Error : Input/output Error \n");
	}
	else
	{
		for(int i=0; i < 100; i++)
		{
			// Convert the data to 6-bits
			int xAccl = data[0] & 0x3F;
			if(xAccl > 31)
			{
				xAccl -= 64;
			}

			int yAccl = data[1] & 0x3F;;
			if(yAccl > 31)
			{
				yAccl -= 64;
			}

			int zAccl = data[2] & 0x3F;;
			if(zAccl > 31)
			{
				zAccl -= 64;
			}

			// Output data to screen
			printf("Acc = (%2d, %2d, %2d)", xAccl, yAccl, zAccl);
		}
	}
}