/*
 * i2c_lcd.c
 *
 *  Created on: 2019. 9. 4.
 *      Author: k
 */
#include "main.h"

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "i2c_lcd.h"
#include "extern.h"
#include "ds1302.h"
//extern I2C_HandleTypeDef hi2c1;
unsigned char lcd_test[4] = { '4','3', 0};
extern t_ds1302 ds1302;


//HAL->I2C
//start->addr->data->stop

void i2c_start(void)
{
    SDA_HIGH(); SCL_HIGH(); I2C_DELAY();
    SDA_LOW(); I2C_DELAY();
    SCL_LOW(); I2C_DELAY();
}


void i2c_stop(void)
{
    SCL_LOW(); SDA_LOW(); I2C_DELAY();
    SCL_HIGH(); I2C_DELAY();
    SDA_HIGH(); I2C_DELAY();
}

void i2c_write_bit(uint8_t bit) {
    if (bit) SDA_HIGH(); else SDA_LOW();
    I2C_DELAY();
    SCL_HIGH(); I2C_DELAY();
    SCL_LOW(); I2C_DELAY();
}

uint8_t i2c_write_byte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        i2c_write_bit(byte & 0x80);
        byte <<= 1;
    }

    // ACK 비트 읽기
    SDA_HIGH(); // 릴리즈
    I2C_DELAY();
    SCL_HIGH();
    uint8_t ack = HAL_GPIO_ReadPin(GPIO_SDA_GPIO_Port, GPIO_SDA_Pin);
    SCL_LOW();
    return ack == 0; // 0이면 ACK 받은 것
}

void i2c_write_data(uint8_t addr, uint8_t *data, uint8_t len)
{
    i2c_start();
    if (!i2c_write_byte(addr)) return; // 주소 전송 + ACK 확인

    for (uint8_t i = 0; i < len; i++) {
        if (!i2c_write_byte(data[i])) return; // 데이터 전송 + ACK 확인
    }

    i2c_stop();
}


void i2c_lcd_dis_line0(void)//mutex의 thread1
{
	static int value = 0;
    static char time_buf[32];

    if(line0_timer >= 1000)//1초마다
    {
    	line0_timer=0;

    	read_time_ds1302(); //ds1302에서 현재 시간 읽어오기

		move_cursor(0,0);
		sprintf(time_buf, "time:%02d-%02d-%02d", ds1302.hours, ds1302.minutes, ds1302.seconds);
		lcd_string(time_buf);
   /*lcd_data(value + '0');
		value++;
		if(value>9)value=0;*/
    }
}


void i2c_lcd_dis_line1(void)//mutex의 thread1
{
	static int value = 0;
    static char date_buf[32];

    if(line1_timer >= 1000)//1초마다
    {
    	line1_timer=0;

    	read_date_ds1302(); //ds1302에서 현재 날짜 읽어오기

		move_cursor(1,0);
		sprintf(date_buf, "Date:25-04-11");
		lcd_string(date_buf);
   /*lcd_data(value + '0');
		value++;
		if(value>9)value=0;*/
    }
}


/*
void i2c_lcd_dis_line1(void)//mutex의 thread2
{
	static int value = 0;
    if(line1_timer >= 100)//100ms
    {
    	line1_timer=0;
		move_cursor(1,0);
		lcd_data(value + '0');
		value++;
		if(value>9)value=0;
    }
}

*/

/*void i2c_lcd_main(void)
{
//  while (1)
//  {
//	   while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
//			 lcd_test, 2, 100)!=HAL_OK){
//		  //HAL_Delay(1);
//	   }
//	   HAL_Delay(1000);
//  }


#if 1
	uint8_t value=0;
	i2c_lcd_init();


	while(1)
	{
		move_cursor(0,0);
		lcd_string("Hello World!!!");
		move_cursor(1,0);
		lcd_data(value + '0');
		value++;
		if(value>9)value=0;
		HAL_Delay(500);
	}
#endif
}
*/

void lcd_command(uint8_t command){

	uint8_t high_nibble, low_nibble;
	uint8_t i2c_buffer[4];
	high_nibble = command & 0xf0;
	low_nibble = (command<<4) & 0xf0;
	i2c_buffer[0] = high_nibble | 0x04 | 0x08; //en=1, rs=0, rw=0, backlight=1
	i2c_buffer[1] = high_nibble | 0x00 | 0x08; //en=0, rs=0, rw=0, backlight=1
	i2c_buffer[2] = low_nibble  | 0x04 | 0x08; //en=1, rs=0, rw=0, backlight=1
	/*while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
				i2c_buffer, 4, 100)!=HAL_OK){*/
	i2c_write_data(I2C_LCD_ADDRESS,i2c_buffer,4);
			//HAL_Delay(1);

	return;
}

void lcd_data(uint8_t data){

	uint8_t high_nibble, low_nibble;
	uint8_t i2c_buffer[4];
	high_nibble = data & 0xf0;
	low_nibble = (data<<4) & 0xf0;
	i2c_buffer[0] = high_nibble | 0x05 | 0x08; //en=1, rs=1, rw=0, backlight=1
	i2c_buffer[1] = high_nibble | 0x01 | 0x08; //en=0, rs=1, rw=0, backlight=1
	i2c_buffer[2] = low_nibble  | 0x05 | 0x08; //en=1, rs=1, rw=0, backlight=1
	i2c_buffer[3] = low_nibble  | 0x01 | 0x08; //en=0, rs=1, rw=0, backlight=1
	/*while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
			i2c_buffer, 4, 100)!=HAL_OK){*/
	i2c_write_data(I2C_LCD_ADDRESS,i2c_buffer,4);
		//HAL_Delay(1);

	return;
}
void i2c_lcd_init(void){

	lcd_command(0x33);//초기화
	lcd_command(0x32);//4bit모드 설정
	lcd_command(0x28);	//Function Set 4-bit mode 2라인
	lcd_command(DISPLAY_ON);
	lcd_command(0x06);	//Entry mode set
	lcd_command(CLEAR_DISPLAY);//화면 클리어
	HAL_Delay(2);
}
void lcd_string(uint8_t *str)
{
	while(*str)lcd_data(*str++);

}
void move_cursor(uint8_t row, uint8_t column)
{
	lcd_command(0x80 | row<<6 | column);
	return;
}











