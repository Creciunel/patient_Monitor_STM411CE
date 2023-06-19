/*
 * bitMap.c
 *
 *  Created on: Apr 6, 2023
 *      Author: ICG
 */
#include "bitMap.h"
uint8_t stateLast;
uint8_t lastencoder;

void checkBTN(uint8_t state, uint8_t encoder) {
	stateLast = state;
	if (lastencoder != encoder) {
		if (state == 1) {
			ST7735_FillRectangle(3, 110, 8, 8, ST7735_GREEN);
			ST7735_FillRectangle(23, 110, 8, 8, ST7735_GREEN);
		}
	}
	switch (state) {
	case 0:
		ST7735_FillRectangle(3, 110, 8, 8, ST7735_GREEN); //temperatura
		break;
	case 1:
		ST7735_FillRectangle(23, 110, 8, 8, ST7735_GREEN); // umiditatea
		break;
	case 2:
		ST7735_FillRectangle(43, 110, 8, 8, ST7735_YELLOW); // 4Val GAz
		break;
	case 3:
		ST7735_FillRectangle(63, 110, 8, 8, ST7735_RED); //temp mana
		break;
	case 4:
		ST7735_FillRectangle(83, 110, 8, 8, ST7735_RED); //ecg
		break;
	case 5:
		ST7735_FillRectangle(3, 104, 150, 22, ST7735_BLACK);
		break;
	}

	lastencoder = encoder;

}

void Display1SHT3X_Init(uint16_t temperature, uint16_t humidity) {
	char str[20], str1[20];

	ST7735_FillScreen(ST7735_BLACK);

	sprintf(str, "%i", temperature);
	strcat(str, " 'C");

	ST7735_WriteString(100, 3, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);

	sprintf(str1, "%i", humidity);
	strcat(str1, " %");

	ST7735_WriteString(100, 25, str1, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);

	ST7735_WriteString(3, 6, "Temperatura", Font_7x10, ST7735_RED,
	ST7735_BLACK);

	ST7735_WriteString(3, 28, "Umiditate", Font_7x10, ST7735_RED, ST7735_BLACK);

	ST7735_WriteString(0, 55,
			"     Senzor SHT3X       citeste temperatura        si umiditatea",
			Font_7x10, ST7735_CYAN, ST7735_BLACK);

// contur
	ST7735_drawLine(0, 0, 0, ST7735_HEIGHT, ST7735_RED);
	ST7735_drawLine(0, 0, ST7735_WIDTH, 0, ST7735_RED);
	ST7735_drawLine(0, ST7735_HEIGHT - 1, ST7735_WIDTH, ST7735_HEIGHT - 1,
	ST7735_RED);
	ST7735_drawLine(ST7735_WIDTH - 1, ST7735_HEIGHT - 1, ST7735_WIDTH - 1, 0,
	ST7735_RED);
//text delimit
	ST7735_drawLine(0, 22, ST7735_WIDTH, 22, ST7735_RED);
	ST7735_drawLine(95, 0, 95, 45, ST7735_RED);
	ST7735_drawLine(0, 45, ST7735_WIDTH, 45, ST7735_RED);
	ST7735_drawLine(0, 100, ST7735_WIDTH, 100, ST7735_RED);
}

void Display1SHT3X(uint16_t temperature, uint16_t humidity) {
	char str[20], str1[20];

	sprintf(str, "%i", temperature);
	strcat(str, " 'C");

	ST7735_WriteString(100, 3, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);

	sprintf(str1, "%i", humidity);
	strcat(str1, " %");

	ST7735_WriteString(100, 25, str1, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);
}

void Display2Gas_Init(uint16_t MQ3, uint16_t MQ4, uint16_t MQ7, uint16_t MQ135) {
	char str[20];
	ST7735_FillScreen(ST7735_BLACK);
// MQ3
	ST7735_WriteString(3, 3, "Etanol", Font_11x18, ST7735_RED,
	ST7735_BLACK);
	sprintf(str, "%i", (uint16_t) ((MQ3 * 100) / 3500));
	strcat(str, " %");

	ST7735_WriteString(100, 3, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);
//MQ4
	ST7735_WriteString(3, 30, "Gas CH4", Font_11x18, ST7735_RED,
	ST7735_BLACK);
	sprintf(str, "%i", (uint16_t) ((MQ4 * 100) / 3500));
	strcat(str, " %");

	ST7735_WriteString(100, 30, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);
//MQ7
	ST7735_WriteString(3, 55, "CO2", Font_11x18, ST7735_RED,
	ST7735_BLACK);
	sprintf(str, "%i", (uint16_t) ((MQ7 * 100) / 3500));
	strcat(str, " %");

	ST7735_WriteString(100, 55, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);
//MQ135
	ST7735_WriteString(3, 80, "Nocive", Font_11x18, ST7735_RED,
	ST7735_BLACK);
	sprintf(str, "%i", (uint16_t) ((MQ135 * 100) / 3500));
	strcat(str, " %");

	ST7735_WriteString(100, 80, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);

//Contur
	ST7735_drawLine(0, 0, 0, ST7735_HEIGHT, ST7735_RED);
	ST7735_drawLine(0, 0, ST7735_WIDTH, 0, ST7735_RED);
	ST7735_drawLine(ST7735_WIDTH - 1, ST7735_HEIGHT - 1,
	ST7735_WIDTH - 1, 0,
	ST7735_RED);
	ST7735_drawLine(0, ST7735_HEIGHT - 1, ST7735_WIDTH,
	ST7735_HEIGHT - 1,
	ST7735_RED);
// text delimit
	ST7735_drawLine(0, 100, ST7735_WIDTH, 100, ST7735_RED);
	ST7735_drawLine(95, 0, 95, 100, ST7735_RED);
}

void Display2Gas(uint16_t MQ3, uint16_t MQ4, uint16_t MQ7, uint16_t MQ135) {
	char str[20];
//MQ3
	sprintf(str, "%i", (uint16_t) ((MQ3 * 100) / 3500));
	strcat(str, " %");

	ST7735_WriteString(100, 3, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);
//MQ4
	sprintf(str, "%i", (uint16_t) ((MQ4 * 100) / 3500));
	strcat(str, " %");

	ST7735_WriteString(100, 30, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);
//MQ7
	sprintf(str, "%i", (uint16_t) ((MQ7 * 100) / 3500));
	strcat(str, " %");

	ST7735_WriteString(100, 55, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);
//MQ135
	sprintf(str, "%i", (uint16_t) ((MQ135 * 100) / 3500));
	strcat(str, " %");

	ST7735_WriteString(100, 80, str, Font_11x18, ST7735_GREEN,
	ST7735_BLACK);
}

void Display3MLX_Init(float temp) {
	char str[20];
	ST7735_FillScreen(ST7735_BLACK);
	// MQ3
	ST7735_WriteString(3, 3, "Corp T'", Font_11x18, ST7735_RED,
	ST7735_BLACK);
	sprintf(str, "%.1f", temp);
	strcat(str, " 'C");
	ST7735_WriteString(100, 10, str, Font_7x10, ST7735_GREEN,
	ST7735_BLACK);

	//descrip
	ST7735_WriteString(3, 40,
			"   Trebuie luata in      calcul temperatura     din mediu si partea         corpului",
			Font_7x10, ST7735_WHITE,
			ST7735_BLACK);
	//Contur
	ST7735_drawLine(0, 0, 0, ST7735_HEIGHT, ST7735_RED);
	ST7735_drawLine(0, 0, ST7735_WIDTH, 0, ST7735_RED);
	ST7735_drawLine(ST7735_WIDTH - 1, ST7735_HEIGHT - 1,
	ST7735_WIDTH - 1, 0,
	ST7735_RED);
	ST7735_drawLine(0, ST7735_HEIGHT - 1, ST7735_WIDTH,
	ST7735_HEIGHT - 1,
	ST7735_RED);
	// text delimit
	ST7735_drawLine(0, 100, ST7735_WIDTH, 100, ST7735_RED);
	ST7735_drawLine(0, 28, ST7735_WIDTH, 28, ST7735_RED);
	ST7735_drawLine(95, 0, 95, 28, ST7735_RED);
}

void Display3MLX(float temp) {
	char str[20];

	sprintf(str, "%.1f", temp);
	strcat(str, " 'C");
	ST7735_WriteString(100, 10, str, Font_7x10, ST7735_GREEN,
	ST7735_BLACK);
}

void Display4ECGInit(float temp) {

}
