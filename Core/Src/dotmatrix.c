#if 1
#include "main.h"
#include "button.h"
#include "dotmatrix.h"


void init_dotmatrix(void);
int dotmatrix_main_func(void);


uint8_t up_arrow_data[20][10] =
{
   {
      0b00011000,
      0b00111100,
      0b01111110,
      0b11111111,
      0b00111100,
      0b00111100,
      0b00111100,
      0b00000000
   },
   {
      0b00011000,
      0b00111100,
      0b01111110,
      0b11111111,
      0b00111100,
      0b00111100,
      0b00111100,
      0b00000000
   }
};

uint8_t down_arrow_data[20][10] =
{
   {
      0b00000000,
      0b00111100,
      0b00111100,
      0b00111100,
      0b11111111,
      0b01111110,
      0b00111100,
      0b00011000
   },
   {
      0b00000000,
      0b00111100,
      0b00111100,
      0b00111100,
      0b11111111,
      0b01111110,
      0b00111100,
      0b00011000
   }
};

uint8_t col[2] = {0, 0};


//shiftregister로 데이터 전송함수
void direct(uint8_t* data, uint8_t len)
{
    uint8_t temp;
    for (int j = 0; j < len; j++)
    {
        for (int k = 0; k < 8; k++)
        {
            temp = data[j];
            if (temp & (1 << k))
                HAL_GPIO_WritePin(GPIOB, SER_74HC595_Pin, 1);
            else
                HAL_GPIO_WritePin(GPIOB, SER_74HC595_Pin, 0);

            // CLK 펄스: 상승 -> 하강
            HAL_GPIO_WritePin(GPIOB, CLK_74HC595_Pin, 1);
            HAL_GPIO_WritePin(GPIOB, CLK_74HC595_Pin, 0);
        }
    }
}

unsigned char display_data[8];  // 최종 출력 buffer
unsigned char scroll_buffer[50][8] = {0};
int number_of_character = 2;  // 출력할 문자 갯수 up and down



void init_dotmatrix(void)
{
   for (int i=0; i < 8; i++)
   {
      display_data[i] = up_arrow_data[i];
   }
   for (int i=1; i < number_of_character+1; i++)
   {
      for (int j=0; j < 8; j++) // scroll_buffer[0] = blank
      {
         scroll_buffer[i][j] = up_arrow_data[i-1][j];
      }
   }
}

// scroll 문자 출력 프로그램
int dotmatrix_main_func(void)
{
    init_dotmatrix(); // 화살표 데이터 준비

    // 8줄을 반복하며 dot 출력
    for (int i = 0; i < 8; i++)
    {
        col[0] = ~(1 << i);          // 현재 행 선택
        col[1] = display_data[i];    // 해당 행에 표시할 패턴
        direct(col, 2);              // shift register로 전송

        // LATCH 신호 (출력 적용)
        GPIOB->ODR &= ~GPIO_PIN_13;
        GPIOB->ODR |= GPIO_PIN_13;

        osDelay(1); // 잠깐 delay로 표시 유지
    }

    return 0;
}

//btn 초기화 주석처리
  /*  static GPIO_PinState lastBtnState = GPIO_PIN_RESET;
    HAL_Delay(50);
    lastBtnState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);

    static uint8_t up_arrow_disp[8];
    static int up_currentFrame = 0;
    static int up_nextRow = 0;

    static uint8_t down_arrow_disp[8];
    static int down_currentFrame = 0;
    static int down_nextRow = 0;

    while (1)
    {
        uint32_t now = HAL_GetTick();
       // GPIO_PinState btnState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0); 버튼상태읽기 주석처리

        switch (state)
        {
            case IDLE_STATE:

                if (now - past_time >= SCROLL_DELAY)
                {
                    past_time = now;
                    for (int i = 0; i < 8; i++)
                    {
                        display_data[i] = (scroll_buffer[index][i] >> count) |
                                          (scroll_buffer[index+1][i] << (8 - count));
                    }
                    if (++count == 8)
                    {
                        count = 0;
                        index++;
                        if (index == number_of_character + 1)
                            index = 0;
                    }
                }

                for (int i = 0; i < 8; i++)
                {
                    col[0] = ~(1 << i);
                    col[1] = display_data[i];
                    direct(col, 2);
                    GPIOB->ODR &= ~GPIO_PIN_13;
                    GPIOB->ODR |= GPIO_PIN_13;
                    HAL_Delay(1);
                }

                //버튼 누르면 화살표 상태로 전환제거
                if (btnState == GPIO_PIN_SET && lastBtnState == GPIO_PIN_RESET)
                {
                    state = UP_ARROW_STATE;
                    arrowStateStart = now;
                    arrowFrameTimer = now;

                    for (int i = 0; i < 8; i++)
                    {
                        up_arrow_disp[i] = up_arrow_data[up_currentFrame][i];
                    }
                    up_nextRow = 0;
                }
                break;

            case UP_ARROW_STATE:

                if (btnState == GPIO_PIN_SET && lastBtnState == GPIO_PIN_RESET)
                {
                    state = DOWN_ARROW_STATE;
                    arrowStateStart = now;
                    arrowFrameTimer = now;

                    for (int i = 0; i < 8; i++)
                    {
                        down_arrow_disp[i] = down_arrow_data[down_currentFrame][i];
                    }
                    down_nextRow = 0;
                }
                else if (now - arrowStateStart >= ARROW_DURATION)
                {
                    state = IDLE_STATE;
                    past_time = now;
                }
                else
                {
                    if (now - arrowFrameTimer >= ARROW_FRAME_PERIOD)
                    {
                        for (int i = 0; i < 7; i++)
                        {
                            up_arrow_disp[i] = up_arrow_disp[i+1];
                        }
                        up_arrow_disp[7] = up_arrow_data[(up_currentFrame + 1) % 2][up_nextRow];
                        up_nextRow++;

                        if (up_nextRow >= 8)
                        {
                            up_nextRow = 0;
                            up_currentFrame = (up_currentFrame + 1) % 2;
                        }
                        arrowFrameTimer = now;
                    }

                    for (int i = 0; i < 8; i++)
                    {
                        col[0] = ~(1 << i);
                        col[1] = up_arrow_disp[i];
                        direct(col, 2);
                        GPIOB->ODR &= ~GPIO_PIN_13;
                        GPIOB->ODR |= GPIO_PIN_13;
                        HAL_Delay(1);
                    }
                }
                break;

            case DOWN_ARROW_STATE:

                if (btnState == GPIO_PIN_SET && lastBtnState == GPIO_PIN_RESET)
                {
                    state = UP_ARROW_STATE;
                    arrowStateStart = now;
                    arrowFrameTimer = now;

                    for (int i = 0; i < 8; i++)
                    {
                        up_arrow_disp[i] = up_arrow_data[up_currentFrame][i];
                    }
                    up_nextRow = 0;
                }
                else if (now - arrowStateStart >= ARROW_DURATION)
                {
                    state = IDLE_STATE;
                    past_time = now;
                }
                else
                {
                    if (now - arrowFrameTimer >= ARROW_FRAME_PERIOD)
                    {
                        for (int i = 7; i > 0; i--)
                        {
                            down_arrow_disp[i] = down_arrow_disp[i-1];
                        }
                        down_arrow_disp[0] = down_arrow_data[(down_currentFrame + 1) % 2][7 - down_nextRow];
                        down_nextRow++;

                        if (down_nextRow >= 8)
                        {
                            down_nextRow = 0;
                            down_currentFrame = (down_currentFrame + 1) % 2;
                        }
                        arrowFrameTimer = now;
                    }

                    for (int i = 0; i < 8; i++)
                    {
                        col[0] = ~(1 << i);
                        col[1] = down_arrow_disp[i];
                        direct(col, 2);
                        GPIOB->ODR &= ~GPIO_PIN_13;
                        GPIOB->ODR |= GPIO_PIN_13;
                        HAL_Delay(1);
                    }
                }
                break;
        }

        lastBtnState = btnState;
        HAL_Delay(10);
    }
    return 0;*/




#endif

