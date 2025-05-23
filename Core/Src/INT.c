#include "INT.h"
#include "stepmotor.h"

extern uint8_t stepmotor_state;

//move from stm32F4xx_HAL_Driver/src/stm32/stm32f4xx_hal_gpio.c to here
//external INT callback function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
  case GPIO_PIN_0: //BTN0과 연동
	stepmotor_state =(stepmotor_state + 1 ) % 3;
	//printf("GPIO_PIN_0\n");
       break;
  case GPIO_PIN_4: //BTN1과 연동
	stepmotor_state =(stepmotor_state + 1 ) % 3;
	// printf("GPIO_PIN_0\n");
      break;
  }

}
