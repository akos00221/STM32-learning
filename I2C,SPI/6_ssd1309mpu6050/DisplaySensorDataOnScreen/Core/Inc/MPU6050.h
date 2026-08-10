#include "stm32f4xx_hal.h"

#ifndef MPU6050_H
#define MPU6050_H

#define MPU6050 0x68
#define TEST_VALUES_START 0x0D
#define SMPRT_DIV 0x19

#define CONFIG 0x1A
#define CONFIG_EXT_SYNC_SET_DIS (0x00)
#define CONFIG_EXT_SYNC_SET_TEMP_OUT_L (1 << 3)
#define CONFIG_EXT_SYNC_SET_GYRO_XOUT_L (1 << 4)
#define CONFIG_EXT_SYNC_SET_GYRO_YOUT_L ((1 << 4) | (1 << 3))
#define CONFIG_EXT_SYNC_SET_GYRO_ZOUT_L (1 << 5)
#define CONFIG_EXT_SYNC_SET_ACCEL_XOUT_L ((1 << 5) | (1 << 3))
#define CONFIG_EXT_SYNC_SET_ACCEL_YOUT_L ((1 << 5) | (1 << 4))
#define CONFIG_EXT_SYNC_SET_ACCEL_ZOUT_L ((1 << 5) | (1 << 4) | (1 << 3))
#define CONFIG_DLPF_CFG_260Hz (0x00)
#define CONFIG_DLPF_CFG_184Hz (0x01)
#define CONFIG_DLPF_CFG_94Hz (0x02)
#define CONFIG_DLPF_CFG_44Hz (0x03)
#define CONFIG_DLPF_CFG_21Hz (0x04)
#define CONFIG_DLPF_CFG_10Hz (0x05)
#define CONFIG_DLPF_CFG_5Hz (0x06)

#define GYRO_CONFIG 0x1B
#define GYRO_CONFIG_XG_ST (1 << 7)
#define GYRO_CONFIG_YG_ST (1 << 6)
#define GYRO_CONFIG_ZG_ST (1 << 5)
#define GYRO_CONFIG_FS_SEL_250 (0x00)
#define GYRO_CONFIG_FS_SEL_500 (1 << 3)
#define GYRO_CONFIG_FS_SEL_1000 (1 << 4)
#define GYRO_CONFIG_FS_SEL_2000 ((1 << 3) | (1 << 4))

#define ACCEL_CONFIG 0x1C
#define ACCEL_CONFIG_XA_ST (1 << 7)
#define ACCEL_CONFIG_YA_ST (1 << 6)
#define ACCEL_CONFIG_ZA_ST (1 << 5)
#define ACCEL_CONFIG_AFS_SEL_2G (0x00)
#define ACCEL_CONFIG_AFS_SEL_4G (1 << 3)
#define ACCEL_CONFIG_AFS_SEL_8G (1 << 4)
#define ACCEL_CONFIG_AFS_SEL_16G ((1 << 3) | (1 << 4))

#define MOT_THR 0x1F
#define FIFO_EN 0x23
#define FIFO_EN_TEMP (1 << 7)
#define FIFO_EN_XG (1 << 6)
#define FIFO_EN_YG (1 << 5)
#define FIFO_EN_ZG (1 << 4)
#define FIFO_EN_ACCEL 0x08
#define FIFO_EN_SLV2 0x04
#define FIFO_EN_SLV1 0x02
#define FIFO_EN_SLV0 0x01

#define I2C_MST_CTRL 0x24

#define INT_PIN_CFG 0x37

#define INT_ENABLE 0x38
#define INT_ENABLE_MOT_EN (1 << 6)
#define INT_ENABLE_FIFO_OFLOW_EN (1 << 4)
#define INT_ENABLE_I2C_MST_INT_EN (1 << 3)
#define INT_ENABLE_DATA_RDY_EN (0x01)

#define SIGNAL_PATH_RESET 0x68

#define USER_CTRL
#define USER_CTRL_FIFO_EN (1 << 6)
#define USER_CTRL_I2C_MST_EN (1 << 5)
#define USER_CTRL_I2C_IF_DIS (1 << 4)
#define USER_CTRL_FIFO_RESET (1 << 2)
#define USER_CTRL_I2C_MST_RESET 0x02
#define USER_CTRL_SIG_COND_RESET 0x01

#define PWR_MGMT_1 0x6B
#define PWR_MGMT_1_DEVICE_RESET (1 << 7)
#define PWR_MGMT_1_SLEEP (1 << 6)
#define PWR_MGMT_1_CYCLE (1 << 5)
#define PWR_MGMT_1_TEMP_DIS (1 << 3)
#define PWR_MGMT_1_CLKSEL_8MHz (0x00)
#define PWR_MGMT_1_CLKSEL_PLL_X 0x01
#define PWR_MGMT_1_CLKSEL_PLL_Y (1 << 1)
#define PWR_MGMT_1_CLKSEL_PLL_Z ((1 << 1) | 1)
#define PWR_MGMT_1_CLKSEL_PLL_32kHz_EXT (1 << 2)
#define PWR_MGMT_1_CLKSEL_RES ((1 << 2) | (1 << 1) | 1)

#define PWR_MGMT_2 0x6C
#define PWR_MGMT_2_LP_WAKE_CTRL_1Hz 0
#define PWR_MGMT_2_LP_WAKE_CTRL_5Hz (1 << 6)
#define PWR_MGMT_2_LP_WAKE_CTRL_20Hz (1 << 7)
#define PWR_MGMT_2_LP_WAKE_CTRL_40Hz ((1 << 7) | (1 << 6))
#define PWR_MGMT_2_STBY_XA (1 << 5)
#define PWR_MGMT_2_STBY_YA (1 << 4)
#define PWR_MGMT_2_STBY_ZA (0x08)
#define PWR_MGMT_2_STBY_XG (0x04)
#define PWR_MGMT_2_STBY_YG (0x02)
#define PWR_MGMT_2_STBY_ZG (0x01)

#define INT_STATUS 0x3A
#define INT_STATUS_MOT_INT (1 << 6)
#define INT_STATUS_FIFO_OFLOW_INT (1 << 4)
#define INT_STATUS_I2C_MST_INT (1 << 3)
#define INT_STATUS_DATA_RDY_INT (0x01)

#define GYRO_XOUT_H 0x43
#define ACCEL_XOUT_H 0x3B

#define FIFO_R_W 0x74

extern const float GyroTestLUT[32];

typedef struct {
	uint8_t xa_test, xg_test, ya_test, yg_test, za_test, zg_test;
} __attribute__((packed)) SelfTestParameters;

typedef struct {
	uint8_t config, gyro_config, accel_config, mot_thr, fifo_en, i2c_mst_ctrl,
			int_pin_cfg, int_enable, signal_path_reset, mot_detect_ctrl, user_ctrl, pwr_mgmt_1, pwr_mgmt_2;
}__attribute__((packed)) MPU60X0ConfigRegisters;

void Read_Test_Values(I2C_HandleTypeDef *hi2c, SelfTestParameters *caller_struct);
int Perform_Test(I2C_HandleTypeDef *hi2c, SelfTestParameters caller_struct);
void MPU60X0Init(I2C_HandleTypeDef *hi2c, MPU60X0ConfigRegisters *init_conf);
void MPU60X0_Reset(I2C_HandleTypeDef *hi2c);
void MPU60X0_Read_FIFO_All(I2C_HandleTypeDef *hi2c, int16_t acc[static 3], int16_t *temperature, int16_t gyro[static 3]);

#endif
