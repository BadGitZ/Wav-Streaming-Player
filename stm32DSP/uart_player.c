/*
 * uart_player.c
 *
 *  Created on: 19.09.2019
 *      Author: peuckert
 */
#include "uart_player.h"

UART_HandleTypeDef huart;

__IO ITStatus UartReady = RESET;

void Error_Handler(void){
	uint32_t error;
	char desc[100];
	error = HAL_UART_GetError(&huart);
	sprintf((char *)desc,"ERROR=%d", error);
	BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 130, (uint8_t *)desc, CENTER_MODE);
	while(1){
	}
}
void HAL_UART_MspInit(UART_HandleTypeDef *huart){
	GPIO_InitTypeDef GPIO_InitStruct;

    /* Peripheral clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PG9     ------> USART6_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
}
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart){
	/*Reset peripherals*/
	__HAL_RCC_USART6_FORCE_RESET();
	__HAL_RCC_USART6_RELEASE_RESET();

	/*Disable peripherals and clock*/
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);

	/*Disable NVIC for UART*/
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}
void MX_USART2_UART_Init(void){
  huart.Instance = USART6;
  huart.Init.BaudRate = 500000;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits = UART_STOPBITS_1;
  huart.Init.Parity = UART_PARITY_NONE;
  huart.Init.Mode = UART_MODE_TX_RX;
  huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart) != HAL_OK)
  {
    Error_Handler();
  }
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
  /* Turn LED_ORANGE on: Transfer error in reception/transmission process */
  BSP_LED_On(LED_ORANGE);
  Error_Handler();
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  /* Set transmission flag: transfer complete */
  UartReady = SET;
  BSP_LED_On(LED_BLUE);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  /* Set transmission flag: transfer complete */
  UartReady = SET;
  BSP_LED_On(LED_GREEN);
}




