import smbus
import time

# I2Cバスの取得
bus = smbus.SMBus(1)

# MMA7660FCのアドレス: 0x4C(76)
# モードレジスタの選択:  0x07(07)
# 0x01(01): Active mode
bus.write_byte_data(0x4C, 0x07, 0x01)

# サンプルレートレジスタの選択: 0x08(08)
# 0x07(07): 1 Sample/second active
bus.write_byte_data(0x4C, 0x08, 0x07)

time.sleep(0.5)

# MMA7660FCの0x00番地から3バイト分取得。1バイトずつ加速度が格納されている
# X-Axis Accl, Y-Axis Accl, Z-Axis Accl
data=bus.read_i2c_block_data(0x4C, 0x00, 3)

# 0~63を、-32~31に変換。
xAccl = data[0] & 0x3F
if xAccl > 31:
    xAccl -= 64

yAccl = data[1] & 0x3F
if yAccl > 31:
    yAccl -= 64

zAccl = data[2] & 0x3F
if zAccl > 31:
    zAccl -= 64

# 結果出力
print("Acceleration = (%d, %d, %d",  (xAccl, yAccl, zAccl))
