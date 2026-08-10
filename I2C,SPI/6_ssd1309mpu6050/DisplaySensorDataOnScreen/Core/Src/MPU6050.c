#include "MPU6050.h"

const float GyroTestLUT[32] = { 0.0f, 3275.00f, 3425.65f, 3583.23f, 3748.06f,
		3920.47f, 4100.81f, 4289.45f, 4486.76f, 4693.15f, 4909.04f, 5134.85f,
		5371.06f, 5618.13f, 5876.56f, 6146.88f, 6429.64f, 6725.40f, 7034.77f,
		7358.37f, 7696.86f, 8050.91f, 8421.25f, 8808.63f, 9213.83f, 9637.66f,
		10081.00f, 10544.72f, 11029.78f, 11537.15f, 12067.86f, 12622.98f };

const float AccTestLUT[32] = { 0.0f, 1392.64f, 1439.62f, 1488.19f, 1538.40f,
		1590.30f, 1643.96f, 1699.42f, 1756.75f, 1816.02f, 1877.29f, 1940.63f,
		2006.10f, 2073.78f, 2143.74f, 2216.07f, 2290.83f, 2368.12f, 2448.01f,
		2530.60f, 2615.98f, 2704.24f, 2795.47f, 2889.78f, 2987.28f, 3088.06f,
		3192.25f, 3299.94f, 3411.28f, 3526.36f, 3645.34f, 3768.32f };

void Read_Test_Values(I2C_HandleTypeDef *hi2c, SelfTestParameters *caller_struct){
	uint8_t TestValues[4];
	HAL_I2C_Mem_Read(hi2c, (MPU6050 << 1), TEST_VALUES_START, I2C_MEMADD_SIZE_8BIT, TestValues, 4, 20);
	caller_struct->xa_test = ((TestValues[0] & 0b11100000) >> 3) | ((TestValues[3] & 0b00110000) >> 4);
	caller_struct->ya_test = ((TestValues[1] & 0b11100000) >> 3) | ((TestValues[3] & 0b00001100) >> 2);
	caller_struct->za_test = ((TestValues[2] & 0b11100000) >> 3) | (TestValues[3] & 0b00000011);
	caller_struct->xg_test = (TestValues[0] & 0b00011111);
	caller_struct->yg_test = (TestValues[1] & 0b00011111);
	caller_struct->zg_test = (TestValues[2] & 0b00011111);
}
void MPU60X0_Reset(I2C_HandleTypeDef *hi2c){
	uint8_t pw_mgm_1 = PWR_MGMT_1_DEVICE_RESET;
	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &(pw_mgm_1), 1, 20);
	HAL_Delay(100);
}
static void MPU60X0_Sampling_Test(I2C_HandleTypeDef *hi2c, int16_t acc[static 3], int16_t gyro[static 3], uint16_t sample_size){
	uint8_t rawbuffer[14];
	uint8_t readings = 0, status = 0;
	int32_t gyro_avg[3] = {0};
	int32_t acc_avg[3] = {0};

	while (readings < sample_size) {
		HAL_I2C_Mem_Read(hi2c, (MPU6050 << 1), INT_STATUS, I2C_MEMADD_SIZE_8BIT, &status, 1, 10);
		if (status & 1) {

			HAL_I2C_Mem_Read(hi2c, (MPU6050 << 1), ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, rawbuffer, 14, 20);

			acc[0] = (int16_t) ((rawbuffer[0] << 8) | rawbuffer[1]);
			acc[1] = (int16_t) ((rawbuffer[2] << 8) | rawbuffer[3]);
			acc[2] = (int16_t) ((rawbuffer[4] << 8) | rawbuffer[5]);

			gyro[0] = (int16_t) ((rawbuffer[8] << 8) | rawbuffer[9]);
			gyro[1] = (int16_t) ((rawbuffer[10] << 8) | rawbuffer[11]);
			gyro[2] = (int16_t) ((rawbuffer[12] << 8) | rawbuffer[13]);

			for (uint8_t i = 0; i < 3; ++i){
				acc_avg[i] += acc[i];
				gyro_avg[i] += gyro[i];
			}

			readings++;
		}
	}
	for (uint8_t i = 0; i < 3; ++i){
		acc[i] = acc_avg[i] / 30;
		gyro[i] = gyro_avg[i] / 30;
	}
}
int Perform_Test(I2C_HandleTypeDef *hi2c, SelfTestParameters caller_struct){
	MPU60X0_Reset(hi2c);
	uint8_t pw_mgm_1 = PWR_MGMT_1_CLKSEL_PLL_X;
	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &(pw_mgm_1), 1, 20);

	uint8_t sensor_conf[3] = {CONFIG_DLPF_CFG_44Hz, GYRO_CONFIG_FS_SEL_250, ACCEL_CONFIG_AFS_SEL_8G};
	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), CONFIG, I2C_MEMADD_SIZE_8BIT, sensor_conf, 3, 20);

	int16_t acc[3] = {0};
	int16_t gyro[3] = {0};

	HAL_Delay(100);
	MPU60X0_Sampling_Test(hi2c, acc, gyro, 30);
	// perform STR test
	sensor_conf[1] |= (GYRO_CONFIG_XG_ST | GYRO_CONFIG_YG_ST | GYRO_CONFIG_ZG_ST);
	sensor_conf[2] |= (ACCEL_CONFIG_XA_ST | ACCEL_CONFIG_YA_ST | ACCEL_CONFIG_ZA_ST);

	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), GYRO_CONFIG, I2C_MEMADD_SIZE_8BIT, (sensor_conf+1), 2, 20);

	int16_t str_acc[3] = {0};
	int16_t str_gyro[3] = {0};

	HAL_Delay(250);
	MPU60X0_Sampling_Test(hi2c, str_acc, str_gyro, 30);
	for (int i = 0; i < 3; ++i){
		str_acc[i] -= acc[i];
		str_gyro[i] -= gyro[i];
	}

	float str_deltas[6] =
			{ (str_gyro[0] - GyroTestLUT[caller_struct.xg_test]) / GyroTestLUT[caller_struct.xg_test],
			(str_gyro[1] + GyroTestLUT[caller_struct.yg_test]) / -GyroTestLUT[caller_struct.yg_test],
			(str_gyro[2] - GyroTestLUT[caller_struct.zg_test]) / GyroTestLUT[caller_struct.zg_test],
			(str_acc[0] - AccTestLUT[caller_struct.xa_test]) / AccTestLUT[caller_struct.xa_test],
			(str_acc[1] - AccTestLUT[caller_struct.ya_test]) / AccTestLUT[caller_struct.ya_test],
			(str_acc[2] - AccTestLUT[caller_struct.za_test]) / AccTestLUT[caller_struct.za_test] };

	uint8_t response = 0b00000000; // response[2:0] gyroscope test results: 1 failed, 0 passed. response[5:3] accelaration test results: 1 failed, 0 passed.
	for (int i = 0; i < 6; ++i){
		if (str_deltas[i]*100.0f <= -14 || str_deltas[i]*100.0f >= 14){
			response |= (1 << i);
		}
	}

	MPU60X0_Reset(hi2c);
	return response;
}

void MPU60X0Init(I2C_HandleTypeDef *hi2c, MPU60X0ConfigRegisters *init_conf){
	uint8_t *conf_ptr = (uint8_t *)init_conf;
	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), CONFIG, I2C_MEMADD_SIZE_8BIT, conf_ptr, 3, 20);
	conf_ptr += 3;
	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), MOT_THR, I2C_MEMADD_SIZE_8BIT, conf_ptr, 1, 20);
	conf_ptr++;
	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), FIFO_EN, I2C_MEMADD_SIZE_8BIT, conf_ptr, 2, 20);
	conf_ptr += 2;
	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), INT_PIN_CFG, I2C_MEMADD_SIZE_8BIT, conf_ptr, 2, 20);
	conf_ptr += 2;
	HAL_I2C_Mem_Write(hi2c, (MPU6050 << 1), SIGNAL_PATH_RESET, I2C_MEMADD_SIZE_8BIT, conf_ptr, 5, 20);
}

void MPU60X0_Read_FIFO_All(I2C_HandleTypeDef *hi2c, int16_t acc[static 3], int16_t *temperature, int16_t gyro[static 3]) {
	uint8_t rawbuffer[14];
	HAL_I2C_Mem_Read(hi2c, (MPU6050 << 1), FIFO_R_W, I2C_MEMADD_SIZE_8BIT, rawbuffer, 14, 20);
	acc[0] = (int16_t) ((rawbuffer[0] << 8) | rawbuffer[1]);
	acc[1] = (int16_t) ((rawbuffer[2] << 8) | rawbuffer[3]);
	acc[2] = (int16_t) ((rawbuffer[4] << 8) | rawbuffer[5]);
	*temperature = (int16_t) ((rawbuffer[6] << 8) | rawbuffer[7]);
	gyro[0] = (int16_t) ((rawbuffer[8] << 8) | rawbuffer[9]);
	gyro[1] = (int16_t) ((rawbuffer[10] << 8) | rawbuffer[11]);
	gyro[2] = (int16_t) ((rawbuffer[12] << 8) | rawbuffer[13]);
}
