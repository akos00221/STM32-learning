#include "SSD1309.h"
#include "main.h"

static void SSD1309UpdateScreenHOR(SPI_HandleTypeDef *hspi, uint8_t screen[static 8*128]){
	HAL_GPIO_WritePIN(pinmap->cs.GPIOx, pinmap->cs.GPIO_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_SPI_Transmit_DMA(hspi, screen, 8*128);
	HAL_GPIO_WritePIN(pinmap->cs.GPIOx, pinmap->cs.GPIO_Pin, GPIO_PIN_SET);
}
static void SSD1309UpdateScreenVER(SPI_HandleTypeDef *hspi, uint8_t screen[static 8*128]);
static void SSD1309UpdateScreenPAGE(SPI_HandleTypeDef *hspi, uint8_t screen[static 8*128]);
void SSD1309ResetDevice(SPI_HandleTypeDef *hspi, SSD1309_PinMap *pinmap){
	  HAL_GPIO_WritePin(pinmap->res.GPIOx, pinmap->res.GPIO_Pin, GPIO_PIN_RESET);
	  HAL_Delay(50);
	  HAL_GPIO_WritePin(pinmap->res.GPIOx, pinmap->res.GPIO_Pin, GPIO_PIN_SET);
	  HAL_Delay(50);
}
static void SSD1309SendCommand(SPI_HandleTypeDef *hspi, uint8_t *commands, uint8_t size, SSD1309_PinMap *pinmap){
	HAL_GPIO_WritePin(pinmap->dc.GPIOx, pinmap->dc.GPIO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(pinmap->cs.GPIOx, pinmap->cs.GPIO_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_SPI_Transmit(hspi, commands, size, 20);
	HAL_GPIO_WritePin(pinmap->dc.GPIOx, pinmap->dc.GPIO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(pinmap->cs.GPIOx, pinmap->cs.GPIO_Pin, GPIO_PIN_SET);
}
