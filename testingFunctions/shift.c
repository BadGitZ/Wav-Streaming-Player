/*
 * shift.c
 * 
 * Copyright 2019 Badgitz <badgitz@thinkgitz>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


int main(int argc, char **argv)
{
	//unifying 4 integers into one 32Bit Integer
	uint8_t a = 255;
	uint8_t b = 128;
	uint8_t c = 255;
	uint8_t d = 128;
	
	uint32_t result = (a << 24) | (b << 16) | (c << 8) | d;
	printf("%u\n", result);
	
	//Splitting 32Bit Integer into 4 8Bit Integers
	uint8_t a2 = (result >> 24);
	uint8_t b2 = (result >> 16);
	uint8_t c2 = (result >> 8);
	uint8_t d2 = result;
	printf("%u\n%u\n%u\n%u\n\n", a2, b2, c2, d2);
	
	
	//Converting Integers to hexadecimal String
	char data[8];
	
	sprintf(data, "%X%X%X%X", a, b, c, d);
	printf(data); 
	printf("\n");
	
	//Converting Info hexadecimal String back to single Integers
	int j = 0;
	char extract[3];
	uint8_t myArray[4];
	for (int i = 0; i <= 6; i+=2) {
		strncpy(extract, data + i, 2);
		int16_t inject = (int16_t)strtol(extract, NULL, 16);
		myArray[j] = inject;
		j++;
	}
	printf("%d\n%d\n%d\n%d\n", myArray[0], myArray[1], myArray[2], myArray[3]);
	
	return 0;
}

