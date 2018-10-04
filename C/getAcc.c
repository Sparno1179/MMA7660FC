#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>

#define MICRO_SEC 1000
#define MILLI_SEC 1000000
#define SEC 1000000000

typedef struct accl_data {
	int x;
	int y;
	int z;
} accl;

int read(int handle, void *buf, unsigned n);
int write(int handle, void *buf, unsigned n);
unsigned int sleep(unsigned int second);
int nanosleep(const struct timespec *req, struct timespec *rem);

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
	char config[ 2] = {0};

	// モードレジスタを選択(0x07)
	// スタンバイモードに書き換え(0x00)
	config[0] = 0x07;
	config[1] = 0x00;
	write(file, config, 2);
	sleep(1);

	// サンプリングレートレジスタ(0x08)の書き換え
	// 64 Sample/second(0x01)
	config[0] = 0x08;
	config[1] = 0x01;
	write(file, config, 2);

	// モードレジスタを選択(0x07)
	// アクティブモードに書き換え(0x01)
	config[0] = 0x07;
	config[1] = 0x01;
	write(file, config, 2);
	sleep(1);

	// nanosleep用構造体
	struct timespec req = {0, 0.015625 * SEC};

	// データ保存用ファイルポインタ
	FILE *fp;

	if( (fp = fopen("acclCSVtest.csv", "w" )) == NULL ) {
        printf( "結果ファイルがオープンできませんでした\n" );
        exit( 1 );
    }
	// ヘッダ行を出力する
    fprintf( fp, "x,y,z\n" );

	// 加速度取得・出力ループ
	for(int i=0; i < 320; i++)
	{
		// 先頭3バイトを読み込む
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
			// データ保存用構造体
			struct accl_data accl;

			// データを-32~31に変換
			int xAccl = data[0] & 0x3F;
			if(xAccl > 31) xAccl -= 64;
			accl.x = xAccl;

			int yAccl = data[1] & 0x3F;;
			if(yAccl > 31) yAccl -= 64;
			accl.y = yAccl;

			int zAccl = data[2] & 0x3F;;
			if(zAccl > 31) zAccl -= 64;
			accl.z = zAccl;

			// 画面出力
			printf("Acc = (%2d, %2d, %2d)\n", accl.x, accl.y, accl.z);
			// CSV出力
			fprintf(fp, "%d,%d,%d\n", accl.x, accl.y, accl.z);
			// 待つ
			nanosleep(&req, NULL);
		}
	}
	// ファイルのクローズ
	fclose(fp);
	printf("クローズ\n");
	
}