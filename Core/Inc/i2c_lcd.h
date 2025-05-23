/*
 * i2c_lcd.h
 *
 *  Created on: 2019. 9. 4.
 *      Author: k
 */

#ifndef SRC_I2C_LCD_H_
#define SRC_I2C_LCD_H_

#define I2C_LCD_ADDRESS (0x27<<1)
#define BACKLIGHT_ON 0x08

/* LCD command   */
#define DISPLAY_ON 0x0C
#define DISPLAY_OFF 0x08
#define CLEAR_DISPLAY 0x01  //Delay 2msec
#define RETURN_HOME 0x02

//GPIO 핀의 i2c 핀설정
#define GPIO_SCL_Pin GPIO_PIN_8
#define GPIO_SCL_GPIO_Port GPIOB
#define GPIO_SDA_Pin GPIO_PIN_9
#define GPIO_SDA_GPIO_Port GPIOB

//GPIO제어 매크로
#define SDA_HIGH() HAL_GPIO_WritePin(GPIO_SDA_GPIO_Port, GPIO_SDA_Pin, GPIO_PIN_SET)
#define SDA_LOW()  HAL_GPIO_WritePin(GPIO_SDA_GPIO_Port, GPIO_SDA_Pin, GPIO_PIN_RESET)
#define SCL_HIGH() HAL_GPIO_WritePin(GPIO_SCL_GPIO_Port, GPIO_SCL_Pin, GPIO_PIN_SET)
#define SCL_LOW()  HAL_GPIO_WritePin(GPIO_SCL_GPIO_Port, GPIO_SCL_Pin, GPIO_PIN_RESET)
#define I2C_DELAY()  for(volatile int i = 0; i < 10; i++) // 약간의 딜레이

void i2c_start(void);
void i2c_stop(void);
void i2c_write_bit(uint8_t bit);
uint8_t i2c_write_byte(uint8_t byte);
void i2c_write_data(uint8_t addr, uint8_t *data, uint8_t len);
void i2c_lcd_main(void);
void i2c_lcd_init(void);
void i2c_lcd_dis_line0(void);
void i2c_lcd_dis_line1(void);
void lcd_command(uint8_t command);
void lcd_data(uint8_t data);
void lcd_string(uint8_t *str);
void move_cursor(uint8_t row, uint8_t column);

#endif /* SRC_I2C_LCD_H_ */
