/*
 * button.h
 *
 *  Created on: Mar 26, 2025
 *      Author: microsoft
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"   // HAL / GPIO config

#define BTN0  0   // PC0
#define BTN1  1   // PC1
#define BTN2  2   // PC2
#define BTN3  3   // PC3
#define BTN4  4   // PC13  demo'bd
#define BUTTON_NUMER  5

#define BUTTON_PRESS  0   // active low
#define BUTTON_RELEASE 1  // button을 뗀 상태
int get_button( GPIO_TypeDef *GPIO, int GPIO_Pin, int button_num);
#define GPIOC_PUPDR 0x4002080C
#endif /* INC_BUTTON_H_ */

