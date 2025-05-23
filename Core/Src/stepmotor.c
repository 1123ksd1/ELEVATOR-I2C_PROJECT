#include "stepmotor.h"
#include "button.h"
//btn0: idle btn1:정방향->역방향
//전역변수 선언 (init상태인지 정방향 역방향 상태인지)

uint8_t stepmotor_state=IDLE;

void set_rpm(int rpm);
void stepmotor_main(void);
int stepmotor_drive(int direction);


/*
RPM:분당 회전수
1분(60sec):1sec(1,000,000us) * 60sec = 60,000,000us
1초(1000ms):1sec(1,000us) * 1000ms = 1,000,000us
1바퀴 회전: 4096 step 필요
4096/8 -> 512 sequence : 360도 회전
1sequence는 0.70312도 회전

RPM조절 (회전수)
-1분에 13회전이 최대  속도
-13회전:60,000,000us(1분) /4096/13->step간의 간격 time :1126ms
-1126ms * 4096(1회전 하는데 필요 스텝)= 4.6초
60초/4.6초(1회전 하는데 소요 시간) : 13회전
*/

//RPM조절 함수
void set_rpm(int rpm)//1~13까지 가능
{
 delay_us(60000000/4096/rpm); //step과 step간격조절
 //최대 speed 조절기준 = rpm이 13 = delay_us(1126);
}


void stepmotor_main(void)
{
#if 1 //o/s방식

		GPIOC->PUPDR |= 0x55; // 하위 4개 핀(Pin 0~3) PUPDR 각각 01 설정 → Pull-up 활성화
		switch(stepmotor_state)
			{
				case  IDLE:
					if(get_button(GPIOC, BTN1_Pin, BTN1)==BUTTON_PRESS)
						stepmotor_state=FORWARD;
					break;

				case  FORWARD:
					stepmotor_drive(FORWARD);
					set_rpm(13); //최대 속도
					if(get_button(GPIOC, BTN0_Pin, BTN0)==BUTTON_PRESS)
						stepmotor_state=IDLE;
					if(get_button(GPIOC, BTN1_Pin, BTN1)==BUTTON_PRESS)
						stepmotor_state=BACKWARD;
					break;

				case  BACKWARD:
					stepmotor_drive( BACKWARD);
					set_rpm(13); //최대 속도
					if(get_button(GPIOC, BTN0_Pin, BTN0)==BUTTON_PRESS)
						stepmotor_state=IDLE;
					if(get_button(GPIOC, BTN1_Pin, BTN1)==BUTTON_PRESS)

						stepmotor_state=FORWARD;
					break;
			}
#endif
#if 0 //none o/s
	while(1)
	{
		GPIOC->PUPDR |= 0x55; // 하위 4개 핀(Pin 0~3) PUPDR 각각 01 설정 → Pull-up 활성화
		switch(stepmotor_state)
			{
				case  IDLE:
					if(get_button(GPIOC, BTN1_Pin, BTN1)==BUTTON_PRESS)
						stepmotor_state=FORWARD;
					break;

				case  FORWARD:
					stepmotor_drive(FORWARD);
					set_rpm(13); //최대 속도
					if(get_button(GPIOC, BTN0_Pin, BTN0)==BUTTON_PRESS)
						stepmotor_state=IDLE;
					if(get_button(GPIOC, BTN1_Pin, BTN1)==BUTTON_PRESS)
						stepmotor_state=BACKWARD;
					break;

				case  BACKWARD:
					stepmotor_drive( BACKWARD);
					set_rpm(13); //최대 속도
					if(get_button(GPIOC, BTN0_Pin, BTN0)==BUTTON_PRESS)
						stepmotor_state=IDLE;
					if(get_button(GPIOC, BTN1_Pin, BTN1)==BUTTON_PRESS)
						stepmotor_state=FORWARD;
					break;
			}
	}

#else
		for(int i=0;i<512;i++)//시계방향1바퀴회전
		{
			for(int j=0;j<8;j++)//1바퀴는 8스텝
			{
				stepmotor_drive(j);
				set_rpm(13); //최대 속도
			}
		}
		for(int i=0;i<512;i++)//시계반대방향1바퀴회전
		{
				for(int j=7  ;j>=0;j--)//1바퀴는 8스텝
				{
					stepmotor_drive(j);
					set_rpm(13); //최대 속도
				}
		}

#endif
}

int stepmotor_drive(int direction)
{
	static int step=0;
	switch(step)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOC,IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN4_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOC,IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN4_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOC,IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN4_Pin, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOC,IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN4_Pin, 0);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOC,IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN4_Pin, 1);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOC,IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN4_Pin, 1);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOC,IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN4_Pin, 1);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOC,IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC,IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC,IN4_Pin, 0);
		break;
	}
	if(direction == FORWARD)
	{
		step++;
		step %= 8; //다음 진행할 step = for(int i=0;i<8;i++)
	}
	else
	{
		step--;
		if(step<0) // =for(int j=7;j<0;j--)
			step=7;
	}
}
