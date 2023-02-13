#include "log.h"
// #include "stm32f1xx.h"
#include "HardwareSerial.h"

#if (LOG_SIZE_DEFAULT > 0)
static char log_buff_mem[LOG_SIZE_DEFAULT] = {0};
char* LOG_BUFF = log_buff_mem;
int LOG_SIZE = LOG_SIZE_DEFAULT;
#else
char* LOG_BUFF = null;
int LOG_SIZE = 0;
#endif

void log_init()
{
    Serial.begin(9600);
}

// void log_deInit()
// {
//     NVIC_DisableIRQ(USART2_IRQn);
//     NVIC_ClearPendingIRQ(USART2_IRQn);
// }

// void debug_sendChar(char c)
// {
//     USART2->DR = c;
//     while(!(USART2->SR & (1 << 6)));
// }

// void debug_sendBuffer(uint8_t* buffer, uint16_t size)
// {
//     for(uint16_t i = 0; i < size; i++)
//     {
//         debug_sendChar(buffer[i]);
//     }
// }

