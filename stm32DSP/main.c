/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team and B. Voss
  * @version V1.1.1
  * @date    19.09.2017
  * @brief   This example code shows how to use the Codec with analog Input and Output to headphones
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fdacoefs.h"
#include "stlogo.h"
#include "menu_player.h"
#include "stm32f4xx_hal.h"


/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	BUFFER_OFFSET_NONE = 0,
	BUFFER_OFFSET_HALF = 1,
	BUFFER_OFFSET_FULL = 2,
}BUFFER_StateTypeDef;

/* Private define ------------------------------------------------------------*/
#define AUDIO_FREQUENCY AUDIO_FREQUENCY_8K
//#define AUDIO_INPUT INPUT_DEVICE_ANALOG_MIC
#define AUDIO_INPUT INPUT_DEVICE_INPUT_LINE_1
#define AUDIO_BLOCK_SIZE   ((uint32_t)2)
#define HEADBAND_HEIGHT    64
#define MAX_ITEMS ((uint8_t)1)
#define PUTCHAR_PROTOTYPE int __io_putchar(int buffer)
#define START_SIGNAL_LENGHT 8
#define SONG_NAME_LENGHT 11
#define DATA_LIST_LENGHT 5
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t TxBufferCOM[1];
uint8_t TxBuffferDN[1024];
uint8_t RxBufferCOM[START_SIGNAL_LENGHT];
uint8_t RxBufferDL[SONG_NAME_LENGHT];
uint8_t Vergleich[] = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
uint8_t Vergleich2[] = "00000000000000000000000000000000";
uint8_t desc[100];
//JOYState_TypeDef joykey;
int currentItem = 0;
Audio_Info_TypeDef Menu_List[2];
char *datalist[DATA_LIST_LENGHT];
int index_list = 0;
BOOL_TypeDef ListEnd = false;
int anz=0;
int vg;
uint8_t buffer[11];
int RxBufferLenght = 0;
char puffer1[11];
char puffer2[11];

extern UART_HandleTypeDef huart;
extern __IO ITStatus UartReady;

/* Global variables ----------------------------------------------------------*/
uint16_t rx_buffer[AUDIO_BLOCK_SIZE];
uint16_t tx_buffer[AUDIO_BLOCK_SIZE];
volatile int left_right_rx=0;
volatile int left_right_tx=0;
volatile int receive_ready = 0;
/* Global extern variables ---------------------------------------------------*/
#ifndef USE_FULL_ASSERT
uint16_t ErrorCounter = 0;
#endif
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Display_Start(void);
void BSP_AUDIO_OUT_TransferComplete_CallBack(void);
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void);
/* Private functions ---------------------------------------------------------*/
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  /* Configure the system clock to 100 Mhz */
  SystemClock_Config();
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  BSP_LCD_Init();
  MX_USART2_UART_Init();
  Display_Start();
  BSP_JOY_Init(JOY_MODE_EXTI);


//#define Tx
#define Rx
//#define TEST_OUTPUT
#ifdef TEST_OUTPUT
  TxBufferCOM[0] = 't';
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 15, (uint8_t *)"Revive UART", CENTER_MODE);
  sprintf((char *)desc,"Alt: %s", TxBufferCOM);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 30, (uint8_t *)desc, CENTER_MODE);
#endif //TEST_OUTPUT
#ifdef Tx
  for(;;){
	  BSP_LED_On(LED_GREEN);
	  if(HAL_UART_Transmit_IT(&huart, (uint8_t *)TxBufferCOM, sizeof(TxBufferCOM)) != HAL_OK){
		 Error_Handler();
	  }
	  while(UartReady != SET){
		  BSP_LED_On(LED_GREEN);
	  }
	  UartReady = RESET;
	  HAL_Delay(1000);
  }
#endif //Tx
#ifdef Rx
  do{
	  TxBufferCOM[0] = 't';
	  if(HAL_UART_Transmit_IT(&huart, (uint8_t *)&TxBufferCOM, sizeof(TxBufferCOM)) != HAL_OK){
		 Error_Handler();
	  }
	  while(UartReady != SET){
	  }
	  if(HAL_UART_Receive_IT(&huart, (uint8_t *)&RxBufferDL, 11) != HAL_OK){
		  Error_Handler();
	  }
	  //datalist[index_list+1] = (char *)RxBufferDL;
	  while(UartReady != SET){
	  }
	  UartReady = RESET;
	  if(index_list == 1){
	  		  strncpy(puffer1,(char *)RxBufferDL,10);
	  	  }
	  if(index_list == 2){
		  	  strcpy(puffer2,(char *)RxBufferDL);
	  	  }
	  index_list++;
	  sprintf((char *)desc,"Name2=%d", index_list);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 15, (uint8_t *)desc, CENTER_MODE);
  }while(index_list <= 3);
  sprintf((char *)desc,"Name1=%s", puffer1);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 30, (uint8_t *)desc, CENTER_MODE);
  sprintf((char *)desc,"Name2=%s", puffer2);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 45, (uint8_t *)desc, CENTER_MODE);
  sprintf((char *)desc,"Name3=%s", datalist[2]);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 60, (uint8_t *)desc, CENTER_MODE);
  sprintf((char *)desc,"Name4=%s", datalist[4]);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 75, (uint8_t *)desc, CENTER_MODE);
  while(1);
#endif //Rx
//#define AUDIOPLAYER
#ifdef AUDIOPLAYER
  /*Aufbau der Verbindung zum Server*/
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 15, (uint8_t *)"Wait for connection to server...", CENTER_MODE);
  TxBufferCOM[0] = 't';
  if(HAL_UART_Transmit_IT(&huart, (uint8_t *)TxBufferCOM, sizeof(TxBufferCOM)) != HAL_OK){
	  Error_Handler();
  }
  while(UartReady != SET){
  }
  UartReady = RESET;
  do{
	  if(HAL_UART_Receive_IT(&huart, (uint8_t *)RxBufferCOM, sizeof(RxBufferCOM)) != HAL_OK){
		  Error_Handler();
	  }
	  while(UartReady != SET){
	  }

	  if( strcmp ((char *)RxBufferCOM, "FFFFFFFF") == 0 ){
		  ListEnd = true;
	  }
	  UartReady = RESET;
  }while(ListEnd != true );
  ListEnd = false;
  BSP_LCD_ClearStringLine(BSP_LCD_GetYSize()/3 + 15);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 15, (uint8_t *)"Connected to Server", CENTER_MODE);
  HAL_Delay(500);
  BSP_LCD_ClearStringLine(BSP_LCD_GetYSize()/3 + 15);

  /*Uebertragung der Datenliste*/
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 15, (uint8_t *)"Get Data list...", CENTER_MODE);
  TxBufferCOM[0] = 'f';
  if(HAL_UART_Transmit_IT(&huart, (uint8_t *)TxBufferCOM, sizeof(TxBufferCOM)) != HAL_OK){
	  Error_Handler();
  }
  while(UartReady != SET){
  }
  UartReady = RESET;
  do{
	  if(HAL_UART_Receive_IT(&huart, (uint8_t *)RxBufferDL, SONG_NAME_LENGHT) != HAL_OK){
		  Error_Handler();
	  }
	  while(UartReady != SET){
	  };
	  UartReady = RESET;
//#define Debug1
#ifdef Debug1
	  int anz = strlen(buffer);
	  sprintf((char *)desc,"Name=%s", buffer);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 45, (uint8_t *)desc, CENTER_MODE);
	  vg = strcmp((char *)buffer,"00000000000000000000000000000000" );
	  sprintf((char *)desc,"Unterschied=%d", vg);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 30, (uint8_t *)desc, CENTER_MODE);
#endif //Debug
	  if(strcmp((char *)RxBufferDL, "00000000000000000000000000000000") == 0){
		  ListEnd = true;
	  }
	  else{
		  datalist[index_list] = (char *)RxBufferDL;
		  index_list++;
		  sprintf((char *)desc,"b%s", buffer);
		  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 90, (uint8_t *)desc, CENTER_MODE);
		  sprintf((char *)desc,"%s", (char *)RxBufferDL);
		  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 105, (uint8_t *)desc, CENTER_MODE);;
	  }
  }while(ListEnd != true);
  while(1);
  Display_Print_Item(datalist, &currentItem);
  BSP_LED_On(LED_GREEN);
  while(1){} // Debug
  while(1){
	  /*Userauswahl der Datei*/
	  switch(BSP_JOY_GetState()){
	  	  case JOY_SEL:
	  		  /*get music data and play*/
	  		  TxBufferCOM[0] = 'g';
	  		  if(HAL_UART_Transmit_IT(&huart, (uint8_t *)TxBufferCOM, 1) != HAL_OK){
	  			  Error_Handler();
	  		  }
	  		  while(UartReady != SET){
	  		  }
	  		  UartReady = RESET;
	  		  if(HAL_UART_Transmit_IT(&huart, (uint8_t *)currentItem, 1) != HAL_OK){
	  			  Error_Handler();
	  		  }
	  		  while(UartReady != SET){
	  		  }
	  		  UartReady = RESET;
	  		  break;
	  	  case JOY_UP:
	  		  if(BSP_JOY_GetState() == JOY_NONE){
		  		  Menu_Up(&currentItem);
		  		  Display_Print_Item(datalist, &currentItem);
	  		  }
	  		  break;
	  	  case JOY_DOWN:
	  		  if(BSP_JOY_GetState() == JOY_NONE){
		  		  Menu_Down(&currentItem);
		  		  Display_Print_Item(datalist, &currentItem);
	  		  }
	  		  break;
	  	  case JOY_NONE:
	  		  break;
	  	  case JOY_LEFT:
	  		  break;
	  	  case JOY_RIGHT:
	  		  break;
	  }
  }
#endif //ADUIOPLAYER
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 200
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
}

/**
  * @brief  Display main demo messages.
  * @param  None
  * @retval None
  */
static void Display_Start(void)
{
  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

  /* Clear the LCD */
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_Clear(LCD_COLOR_CYAN);

  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

  /* Display LCD messages */
  BSP_LCD_DisplayStringAt(0, 2, (uint8_t *)"Mikrorechnerentwurf SS19", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, 14, (uint8_t *)"Audioplayer", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, 28, (uint8_t *)"by Schmolke and Peuckert", CENTER_MODE);
  BSP_LCD_SetFont(&Font12);
}
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
  BSP_LED_On(LED1);
  //BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)&pcm[0], 44100);
}
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
  BSP_LED_Off(LED1);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
