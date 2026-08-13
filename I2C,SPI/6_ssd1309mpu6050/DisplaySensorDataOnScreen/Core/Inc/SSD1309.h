#ifndef SSD1309_H
#define SSD1309_H

#include "stm32f4xx_hal.h"
#define COL_S 128
#define PAGE_S 8
#define SCREEN_SIZE (PAGE_S*COL_S)

#define CONTRAST_CTRL 0x81
typedef enum {
	RAM = 0xA4, ALLON
} entire_display;
typedef enum {
	NOMRAL = 0xA6, INVERSE
}display_mode;
typedef enum {
	ON = 0xAE, OFF = 0xAF
}set_display;
#define NOP 0xE3

#define SET_COMMAND_LOCK 0xFD
typedef enum {
	UNLOCKED = 0b00010010,
	LOCKED = 0b00010110
} lock_states;

#define MEM_ADDR_MODE 0x20
typedef enum  {
	HOR = 0x00,
	VER = 0x01,
	PAGE = 0x02
} mem_addr_modes;
#define COL_ADDR 0x21
#define PAGE_ADDR 0x22
#define GDDRAM_PAGE_START_1 0xB0
#define DISPLAY_START_LINE 0x40
typedef enum {
	seg0 = 0xA0, col127
}segment_remap;
#define MUX_RATIO 0xA8
typedef enum{
	com0 = 0xC0, com_n = 0xC8
}com_scan_dir;
#define DISPLAY_OFFSET 0xD3
#define COM_PINS_HW_CONF 0xDA
typedef enum {
	seq_dis_remap = 0x00,
	seq_en_remap = (1 << 5),
	alt_dis_remap = (1 << 4),
	alt_en_remap = ((1 << 5) | (1 << 4))
} com_hw_conf;
#define SET_GPIO 0xDC
typedef enum {
	hiz_dis = 0x00,
	hiz_en = 0x01,
	pin_low = 0x02,
	pin_high = 0x03
} set_gpio;
#define DISPLAY_CLK_OSC_FRQ 0xD5
#define PRE_CHRG_PERIOD 0xD9
#define VCOMH_DESLCT_LVL 0xDB
typedef enum {
	min = 0x00,
	res = 0x34,
	max = 0x3C
} vcomh_lvl;

typedef struct {
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
} GPIO_PinConfig;

typedef struct {
    GPIO_PinConfig res, dc, cs;
} SSD1309_PinMap;

typedef struct {
	uint8_t contrast;
	entire_display entire_disp;
	display_mode mode;
	set_display display_switch;
	lock_states display_lock;
	mem_addr_modes mem_address_mode;
	uint8_t col_address_start, col_address_end, page_address_start,
			page_address_end, page_start, display_start_line;
	segment_remap seg_remap;
	uint8_t mux_ratio;
	com_scan_dir com_output_scan_direction;
	uint8_t display_offset;
	com_hw_conf com_pin_hardware_config;
	set_gpio gpio_conf;
	uint8_t display_clock_div_oscillator_freq, pre_charge_period;
	vcomh_lvl common_deselect_voltage_level;
}SSD1309_Config;

static void SSD1309SendCommand(SPI_HandleTypeDef *hspi, uint8_t *commands,
		uint8_t size, SSD1309_PinMap *pinmap);
void SSD1309ResetDevice(SPI_HandleTypeDef *hspi, SSD1309_PinMap *pinmap);

typedef void (*Upload_function)(SPI_HandleTypeDef *, uint8_t *);
Upload_function SSD1309Init(SPI_HandleTypeDef *hspi, SSD1309_PinMap *pinmap, SSD1309_Config *);
#endif
