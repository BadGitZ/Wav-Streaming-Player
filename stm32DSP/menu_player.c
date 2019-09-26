/*
 * menu_player.c
 *
 *  Created on: 29.08.2019
 *      Author: peuckert
 */
/* Includes ------------------------------------------------------------------*/
#include "menu_player.h"
/* Private define ------------------------------------------------------------*/
#define MENU_LIST_LENGHT 2
#define PI 3.1415
/* Private variables ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void Menu_Up(uint8_t *currentItem){
	*currentItem = (*currentItem) - 1;
}
void Menu_Down(uint8_t *currentItem){
	*currentItem = (*currentItem) + 1;
}

void Display_Print_Item(uint8_t *datalist[], int *currentItem){
	uint8_t desc[100];
	char bufferTitel[100];
	strcpy(bufferTitel, (char *)datalist[*currentItem]);

	BSP_LCD_ClearStringLine(BSP_LCD_GetYSize()/3);
	BSP_LCD_ClearStringLine(BSP_LCD_GetYSize()/3 + 15);
	BSP_LCD_ClearStringLine(BSP_LCD_GetYSize()/3 + 30);
	BSP_LCD_ClearStringLine(BSP_LCD_GetYSize()/3 + 45);

	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Clear the LCD */
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_Clear(LCD_COLOR_CYAN);

	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

	/* Display LCD messages */
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(0, BSP_LCD_GetYSize()/3 + 10, BSP_LCD_GetXSize(), 50);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_RED);
	BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 15, (uint8_t *)"Aktueller Titel:", CENTER_MODE);
	sprintf((char *)desc,"%s", bufferTitel);
	BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 30, (uint8_t *)desc, CENTER_MODE);
	if(currentItem > 0){
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3, (uint8_t *)"-", CENTER_MODE);
	}
	if(*currentItem < MENU_LIST_LENGHT){
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/3 + 45, (uint8_t *)"+", CENTER_MODE);
	}
}


