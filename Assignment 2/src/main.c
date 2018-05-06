#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"

#include "joystick.h"
#include "pca9532.h"
#include "acc.h"
#include "light.h"
#include "temp.h"
#include "oled.h"
#include "rgb.h"
#include "led7seg.h"
#include "rotary.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define TEMP_NUM_HALF_PERIODS 340


volatile uint32_t msTicks = 0;
volatile uint32_t timer0_msTicks = 0;
volatile uint32_t sw3_ticks = 0;
volatile uint32_t rotary_ticks = 0;

bool rotary_flag = true;

uint32_t lightLoLimit = 0, lightHiLimit = 3000; // change accordingly
uint32_t temp_time1 = 0;
uint32_t temp_time2 = 0;
int temp_counter = 0;
int32_t currentTemperatureReading;
int8_t passcode = 0;
uint32_t curLValue = 0;

uint8_t rev_buf[6];
uint32_t rev_cnt = 0;

uint32_t isReceived = 0;
#define rpt "RPT \\r"
#define code "EE2024"
bool checkPasscode = false;

void SysTick_Handler(void) {
	msTicks++;
}

uint32_t getTicks(void) {
	return msTicks;
}

// delay in Ms
int isTimeup(uint32_t start, int delay) {
	return (getTicks() - start) >= delay;
}

static int countdown_char[] = {0x24, 0x7D, 0xE0, 0x70, 0x39, 0x32, 0x23, 0x7C, 0x20, 0x38,
		0x28, 0x23, 0xA6, 0x61, 0xA2, 0xAA};

int8_t x = 0, y = 0, z = 0, xoff = 0, yoff = 0, zoff = 0, xLast = 0, yLast = 0, zLast = 0;
float x_g, y_g, TEMP_THRESHOLD = 30.0;

typedef struct LEDS {
	int port;
	int pin;
} LED;

LED blue_led = {0, 26};
LED red_led = {2,0};

typedef enum {
	STATIONARY, LAUNCH, RETURN
} system_mode;

volatile system_mode mode;
volatile system_mode previous_mode;

typedef enum {
	NO_ERROR, TEMP_ERROR, ACC_ERROR, BOTH_ERROR, LIGHT_ERROR
} error_enum;
volatile error_enum error_flag;
volatile error_enum previous_flag;


typedef enum {
	START_COUNTDOWN, STOP_COUNTDOWN
} countdown_enum;
volatile countdown_enum countdown_flag;

/* LIGHT */
typedef enum {
	LIGHT_NORMAL, LIGHT_LOW
} light_enum;
volatile light_enum light_flag;
uint32_t luminance = 0;

uint8_t blink_blue = 0, blink_red = 0, sw3_pressed = 0;

void led_on (LED led)
{
	GPIO_SetValue(led.port,(1<<led.pin));
}

void led_off (LED led)
{
	GPIO_ClearValue(led.port,(1<<led.pin));
}


void led_invert (LED led)
{
	int ledstate;
	ledstate = GPIO_ReadValue(led.port);
	GPIO_ClearValue(led.port,(ledstate & (1 << led.pin )));
	GPIO_SetValue(led.port,((~ledstate) & (1 << led.pin)));
}

void led_init (void)
{
	PINSEL_CFG_Type PinCfg;

	/* Set P0_26 to 00 - GPIO */
	PinCfg.Funcnum = 0;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	/* Set P2_0 to 00 - GPIO */
	PinCfg.Funcnum = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	// Set GPIO - P0_26 - to be output - blue
	GPIO_SetDir(0,(1<<26),1);
	// Set GPIO - P2_0 - to be output - red
	GPIO_SetDir(2,(1<<0),1);

}

static void init_i2c(void)
{
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 2; // when 10
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize I2C peripheral
	I2C_Init(LPC_I2C2, 100000); // clock rate 100000

	/* Enable I2C1 operation */
	I2C_Cmd(LPC_I2C2, ENABLE);
}

static void init_ssp(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Funcnum = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);

}

static void init_GPIO(void) {

	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 31;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(1, 1<<31, 0);

	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(2, 1 << 10, 0);
}

static void sendToCloud(unsigned char *string) {
	UART_Send(LPC_UART3, (uint8_t *) string, strlen(string), BLOCKING);
}

static void passcodeJoystick(uint8_t joyState)
{
	if ((joyState & JOYSTICK_CENTER) != 0) {
		passcode = 0;
		TEMP_THRESHOLD = 30.0;
		lightHiLimit = 3000;
		oled_clearScreen(OLED_COLOR_WHITE);
		return;
	}

	if ((joyState & JOYSTICK_UP) != 0) {
		if(passcode <= 2) {
			passcode++;
			return;
		}
		else if(passcode == 6) {
			if(mode == STATIONARY || mode == LAUNCH) {
				TEMP_THRESHOLD += 0.5;
				char tempString[50];
				sprintf(tempString, "New temp: %.2f", TEMP_THRESHOLD);
				oled_putString(2,40,(uint8_t*)tempString, OLED_COLOR_BLACK, OLED_COLOR_WHITE);

			} else if(mode == RETURN) {
				lightHiLimit = (lightHiLimit < 4000)? lightHiLimit + 25 : 4000;
				char tempString[50];
				sprintf(tempString, "New light: %d", lightHiLimit);
				oled_putString(2,40,(uint8_t*)tempString, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			}
		} else {
			//passcode = 0;
		}
	}

	if ((joyState & JOYSTICK_DOWN) != 0) {
		if((passcode == 2 || passcode == 3)) {
			passcode++;
			return;
		}
		else if(passcode == 6) {
			if(mode == STATIONARY || mode == LAUNCH) {
				TEMP_THRESHOLD = (TEMP_THRESHOLD > 0 ) ? TEMP_THRESHOLD - 0.5 : 0;
				char tempString[50];
				sprintf(tempString, "New temp: %.2f", TEMP_THRESHOLD);
				if(TEMP_THRESHOLD < 10.0)
					oled_clearScreen(OLED_COLOR_WHITE);
				oled_putString(2,40,(uint8_t*)tempString, OLED_COLOR_BLACK, OLED_COLOR_WHITE);

			} else if(mode == RETURN) {
				lightHiLimit = (lightHiLimit > 0)? lightHiLimit - 25 : 0;
				char tempString[50];
				sprintf(tempString, "New light: %d", lightHiLimit);
				if(lightHiLimit < 1000)
					oled_clearScreen(OLED_COLOR_WHITE);
				oled_putString(2,40,(uint8_t*)tempString, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
				light_setHiThreshold(lightHiLimit);

			}
		} else {
			//passcode = 0;
		}
	}


	if ((joyState & JOYSTICK_RIGHT) != 0) {
		if(passcode == 5) {
			passcode++;
			oled_putString(2,40,(uint8_t*)"Correct code", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			if (mode == STATIONARY || mode == LAUNCH) {
				sendToCloud("\r\nTemp threshold can be changed \r\n");
			} else {
				sendToCloud("\r\nLight threshold can be changed \r\n");
			}
			return;
		} else {
			//passcode = 0;
		}

	}

	if ((joyState & JOYSTICK_LEFT) != 0) {
		if((passcode == 4)) {
			passcode++;
			return;
		}
		else{
			//			passcode = 0;
		}

	}
}




static void light_and_temp_configuration(void) {
	light_setRange(LIGHT_RANGE_4000);
	light_setLoThreshold(lightLoLimit);
	light_setHiThreshold(lightHiLimit);
	light_clearIrqStatus();
	LPC_GPIOINT->IO2IntClr = 1 << 5;
	LPC_GPIOINT->IO2IntEnF |= 1 << 5;
	LPC_GPIOINT->IO0IntEnF |= (1 << 2);
	LPC_GPIOINT->IO0IntEnR |= (1 << 2);
}

void lightToLedArray(uint16_t lumi) {
	uint16_t lum = lumi/250;
	lum = (int)pow(2, lum) - 1;
	pca9532_setLeds(lum,0xffff);
}

static void readAcc() {
	acc_read(&x, &y, &z);
	x = x + xoff;
	y = y + yoff;
	z = z + zoff;
	x_g = x/64.0;
	y_g = y/64.0;
	if(x_g < 0) {
		x_g = x_g * -1;
	}
	if(y_g < 0) {
		y_g = y_g * -1;
	}
	if (x_g > 0.4 || y_g > 0.4) {
		if (error_flag == NO_ERROR) {
			error_flag = ACC_ERROR;
		} else if (error_flag == TEMP_ERROR) {
			error_flag = BOTH_ERROR;
		}
	}
}

void UART_Receive_Customised(void)
{
	/* Read the received data */
	if(UART_Receive(LPC_UART3, &rev_buf[rev_cnt], 1, NONE_BLOCKING) == 1) {

		if (rev_buf[rev_cnt] == '\n' || rev_buf[rev_cnt] == '\r') {
			rev_cnt = -1;
		}
		//UART_RcvMsgHandling();
		if (strcmp(rev_buf, rpt) == 0) {
			if (mode == LAUNCH) {
				char tempString[50];
				sprintf(tempString, "\r\nTemp : %.2f; ACC X : %.2f, Y : %.2f \r\n",currentTemperatureReading/10.0, x_g, y_g);
				sendToCloud(tempString);
			} else if (mode == RETURN) {
				char tempString[50];
				sprintf(tempString, "\r\nLux: %.2f \r\n", light_read()/1.0);
				sendToCloud(tempString);
			}
			rev_cnt = 0;
			rev_buf[0] = '\0';
			rev_buf[1] = '\0';
			rev_buf[2] = '\0';
			rev_buf[3] = '\0';
			rev_buf[4] = '\0';
			rev_buf[5] = '\0';
		} else if(strcmp(rev_buf, code) == 0) {
			if(mode == STATIONARY && !rotary_flag) {
				checkPasscode = true;
				rev_cnt = 0;
				rev_buf[0] = '\0';
				rev_buf[1] = '\0';
				rev_buf[2] = '\0';
				rev_buf[3] = '\0';
				rev_buf[4] = '\0';
				rev_buf[5] = '\0';
			}
		}
		rev_cnt++;
		if(rev_cnt == 6) rev_cnt = 0;
	}
}

void pinsel_uart3(void){
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
}

void init_uart(void){
	UART_CFG_Type uartCfg;
	UART_ConfigStructInit(&uartCfg);
	UART_FIFO_CFG_Type uartFifoCfg;
	UART_FIFOConfigStructInit(&uartFifoCfg);
	uartCfg.Baud_rate = 115200;
	uartCfg.Databits = UART_DATABIT_8;
	uartCfg.Parity = UART_PARITY_NONE;
	uartCfg.Stopbits = UART_STOPBIT_1;
	pinsel_uart3();
	UART_Init(LPC_UART3, &uartCfg);
	UART_FIFOConfig(LPC_UART3, &uartFifoCfg);
	UART_TxCmd(LPC_UART3, ENABLE);
	UART_SetupCbs(LPC_UART3, 0, (void *)UART_Receive_Customised);
	// Enable UART Transmit
	UART_TxCmd(LPC_UART3, ENABLE);
}

//UART3 interrupt handler
void UART3_IRQHandler(void)
{
	UART3_StdIntHandler();
}

void TIMER0_IRQHandler(void) {
	timer0_msTicks++;
	LPC_TIM0->IR |= 1 << 0;
}

uint32_t getTimer0ticks(void) {
	return timer0_msTicks;
}

static void init_TIM(void) {
	LPC_SC->PCONP |= (1<<1) ;
	LPC_SC->PCLKSEL0 = (01<<1);
	LPC_TIM0->MCR  = (1<<0) | (1<<1);
	LPC_TIM0->PR   = 0;
	LPC_TIM0->MR0  = 25000000;
	LPC_TIM0->TCR  = (1 <<0);
}

void eint_init(void) {
	NVIC_SetPriority(SysTick_IRQn, 1);

	LPC_SC->EXTINT = 1;
	LPC_SC->EXTMODE |= 1;
	LPC_SC->EXTPOLAR &= 0;
	NVIC_ClearPendingIRQ(EINT0_IRQn);
	NVIC_SetPriority(EINT0_IRQn, 2);
	NVIC_EnableIRQ(EINT0_IRQn);

	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_SetPriority(EINT3_IRQn, 3);
	NVIC_EnableIRQ(EINT3_IRQn);

	UART_IntConfig(LPC_UART3, UART_INTCFG_RBR, ENABLE);
	NVIC_SetPriority(UART3_IRQn, 4);
	NVIC_EnableIRQ(UART3_IRQn);

	NVIC_SetPriority(TIMER0_IRQn, 5);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

// EINT3 Interrupt Handler
void EINT3_IRQHandler(void) {
	// Determine whether GPIO Interrupt P2.5 has occurred (LIGHT SENSOR) by checking pin
	if ((LPC_GPIOINT->IO2IntStatF>>5)& 0x1) {
		LPC_GPIOINT->IO2IntClr = (1<<5);
		light_clearIrqStatus();
		if (mode == RETURN){
			if (error_flag != LIGHT_ERROR) {
				sendToCloud("\r\nObstacle Near. \r\n");
				error_flag = LIGHT_ERROR;
			}
		}
	}
	if (((LPC_GPIOINT->IO0IntStatF >> 2) & 0x1) ||
			((LPC_GPIOINT->IO0IntStatR >> 2) & 0x1)) {
		if (temp_time1 == 0 && temp_time2 == 0) {
			temp_time1 = getTicks();
		}
		else if (temp_time1 != 0 && temp_time2 == 0) {
			temp_counter++;
			if (temp_counter == TEMP_NUM_HALF_PERIODS) {
				temp_time2 = getTicks();
				if (temp_time2 > temp_time1) {
					temp_time2 = temp_time2 - temp_time1;
				}
				else {
					temp_time2 = (0xFFFFFFFF - temp_time1 + 1) + temp_time2;
				}
				currentTemperatureReading = ((2*1000*temp_time2) / (TEMP_NUM_HALF_PERIODS) - 2731 );
				if (currentTemperatureReading/10.0 > TEMP_THRESHOLD) {
					if (mode != RETURN) {
						if (error_flag == NO_ERROR) {
							error_flag = TEMP_ERROR;
						} else if (error_flag == ACC_ERROR){
							error_flag = BOTH_ERROR;
						}
					}
				}
				temp_time2 = 0;
				temp_time1 = 0;
				temp_counter = 0;
			}
		}
		LPC_GPIOINT->IO0IntClr |= (1 << 2);
	}

}

void EINT0_IRQHandler(void)
{
	if (mode == STATIONARY && error_flag == NO_ERROR && checkPasscode == true) {
		countdown_flag = START_COUNTDOWN;
	} else if (mode == LAUNCH) {
		if (sw3_pressed == 0) {
			sw3_pressed ++;
			sw3_ticks = getTicks();
		} else if (msTicks - sw3_ticks <= 1000){
			sw3_pressed ++;
		} else if (msTicks - sw3_ticks > 1000) {
			sw3_pressed = 1;
			sw3_ticks = getTicks();
		}
	} else if (mode == RETURN) {
		mode = STATIONARY;
		error_flag = NO_ERROR;
		checkPasscode = false;
		rotary_flag = true;
		sendToCloud("\r\nEntering STATIONARY MODE \r\n");
	}
	LPC_SC->EXTINT = (1<<0);
}

void init() {
	SysTick_Config(SystemCoreClock / 1000);
	init_i2c();
	init_ssp();
	init_GPIO();
	init_TIM();

	acc_init();
	acc_read(&x, &y, &z);
	xoff = 0 - x;
	yoff = 0 - y;
	zoff = 0 - z;

	light_enable();
	light_and_temp_configuration();

	temp_init(getTicks);
	pca9532_init();
	led_init();
	joystick_init();
	rgb_init();
	oled_init();
	rotary_init();
	led7seg_init();
	init_uart();
	eint_init();
}

int main (void) {
	init();
	uint8_t state    = 0;
	uint8_t rotary_value = 0;
	uint8_t countdown = 14, sw4_clicked = 1;
	uint32_t initial_time = getTicks();
	uint32_t sevenSegTime = initial_time, sw4_check = initial_time, led_time = initial_time, uart_time = initial_time;

	mode = STATIONARY;
	previous_mode = STATIONARY;
	error_flag = NO_ERROR;
	previous_flag = NO_ERROR;
	countdown_flag = STOP_COUNTDOWN;

	oled_clearScreen(OLED_COLOR_WHITE); // Initial clear screen

	led_off(red_led);
	led_off(blue_led);

	sendToCloud("\r\nEntering STATIONARY MODE \r\n");

	while (1) {

		if (mode == RETURN && error_flag == LIGHT_ERROR) {
			curLValue = light_read();
			if (curLValue < lightHiLimit) {
				sendToCloud("\r\nObstacle Avoided. \r\n");
				error_flag = NO_ERROR;
			}
		}

		if (isTimeup(sw4_check, 500)) {
			sw4_clicked = GPIO_ReadValue(1) >> 31 & 0x01;
			if (sw4_clicked == 0) {
				error_flag = NO_ERROR;
				led_off(red_led);
				led_off(blue_led);
			}
			sw4_check = getTicks();
		}

		state = joystick_read();
		if(state != 0)
			passcodeJoystick(state);

		if(!checkPasscode && (rotary_read() == ROTARY_RIGHT) && mode == STATIONARY) {
			if(rotary_flag) {
				sendToCloud("\r\nTrying to enter launch mode. \r\n Please enter password to continue \r\n");
				rotary_ticks = getTimer0ticks();
				rotary_flag = false;
			}
		}

		if(!rotary_flag && (timer0_msTicks - rotary_ticks) > 10 && checkPasscode != true) {
			sendToCloud("\r\nAttempt to launch mode aborted. \r\n");
			rotary_flag = true;
			checkPasscode = false;
		}

		if ((previous_flag == TEMP_ERROR || previous_flag == ACC_ERROR) && error_flag == BOTH_ERROR) {
			led_off(red_led);
			led_on(blue_led);
		}
		if (error_flag != previous_flag) {
			oled_clearScreen(OLED_COLOR_WHITE);
			if (error_flag == TEMP_ERROR || error_flag == BOTH_ERROR ){
				sendToCloud("\r\nTemp. too high. \r\n");
			} else if (error_flag == ACC_ERROR || error_flag ==  BOTH_ERROR) {
				sendToCloud("\r\nVeer off course.\r\n");
			}
			previous_flag = error_flag;
		}
		if (previous_mode != mode) {
			oled_clearScreen(OLED_COLOR_WHITE);
			error_flag = NO_ERROR;
			previous_mode = mode;
		}

		switch (mode) {

		case STATIONARY:
			oled_putString(2,10, (uint8_t*)"STATIONARY", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			if (countdown_flag == STOP_COUNTDOWN || error_flag == TEMP_ERROR) {
				led7seg_setChar(countdown_char[15], TRUE);
			}
			if (error_flag == NO_ERROR) {
				char tempString[50];
				sprintf(tempString, "Temp: %.2f", currentTemperatureReading/10.0);
				oled_putString(2,20,(uint8_t*)tempString, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			} else if (error_flag == TEMP_ERROR) {
				oled_putString(2,20, (uint8_t*)"Temp. too high", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
				checkPasscode = false;
				rotary_flag = true;
				passcode = 0;
			}

			if (countdown_flag == START_COUNTDOWN && error_flag == NO_ERROR) {
				if (isTimeup(sevenSegTime, 1000)) {
					led7seg_setChar(countdown_char[countdown], TRUE);
					if (countdown == 0) {
						countdown = 14;
						countdown_flag = STOP_COUNTDOWN;
						mode = LAUNCH;
						checkPasscode = false;
						rotary_flag = true;
						sendToCloud("\r\nEntering LAUNCH MODE \r\n");
						passcode = 0;
					}
					countdown --;
					sevenSegTime = getTicks();
				}
			} else  if (error_flag == TEMP_ERROR) {
				countdown = 14;
				countdown_flag = STOP_COUNTDOWN;
			}
			break;

		case LAUNCH:
			readAcc();
			oled_putString(2,10, (uint8_t*)"LAUNCH", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			led7seg_setChar(countdown_char[0], TRUE);
			if (error_flag == NO_ERROR || error_flag == ACC_ERROR) {
				char tempString[50];
				sprintf(tempString, "Temp: %.2f", currentTemperatureReading/10.0);
				oled_putString(2,20,(uint8_t*)tempString, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			} else if (error_flag == TEMP_ERROR || error_flag == BOTH_ERROR) {
				oled_putString(2,20, (uint8_t*)"Temp. too high", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			}
			if (error_flag == NO_ERROR || error_flag == TEMP_ERROR) {
				char tempString[50];
				sprintf(tempString, "X: %.2f Y: %.2f", x_g, y_g);
				oled_putString(2,30,(uint8_t*)tempString, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			} else if (error_flag == ACC_ERROR || error_flag == BOTH_ERROR) {
				oled_putString(2,30, (uint8_t*)"Veer off course", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			}
			if (msTicks - sw3_ticks <= 1000 && sw3_pressed == 2) {
				mode = RETURN;
				sw3_pressed = 0;
				passcode = 0;
				checkPasscode = false;
				rotary_flag = true;
				sendToCloud("\r\nEntering RETURN MODE \r\n");
			}
			if (isTimeup(uart_time, 10000)) {
				char tempString[50];
				sprintf(tempString, "\r\nTemp : %.2f; ACC X : %.2f, Y : %.2f \r\n",currentTemperatureReading/10.0, x_g, y_g);
				sendToCloud(tempString);
				uart_time = getTicks();
			}
			break;
		case RETURN:
			oled_putString(2,10, (uint8_t*)"RETURN", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			led7seg_setChar(countdown_char[0], TRUE);
			lightToLedArray(curLValue);
			if (error_flag == LIGHT_ERROR) {
				oled_putString(2,20,(uint8_t*)"Obstacle near", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			} else if (error_flag == NO_ERROR) {
				char tempString[50];
				sprintf(tempString, "Lux: %.2f", curLValue/1.0);
				oled_putString(2,20,(uint8_t*)tempString, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			}
			if (isTimeup(uart_time, 10000)) {
				char tempString[50];
				sprintf(tempString, "\r\nLux: %.2f\r\n", curLValue/1.0);
				sendToCloud(tempString);
				uart_time = getTicks();
			}
			break;
		}
		if (error_flag == TEMP_ERROR) {
			if (isTimeup(led_time, 333)) {
				led_invert(red_led);
				led_time = getTicks();
			}
		} else if (error_flag == ACC_ERROR) {
			if (isTimeup(led_time, 333)) {
				led_invert(blue_led);
				led_time = getTicks();
			}
		} else if (error_flag == BOTH_ERROR) {
			if (isTimeup(led_time, 333)) {
				led_invert(red_led);
				led_invert(blue_led);
				led_time = getTicks();
			}
		} else {
			led_off(red_led);
			led_off(blue_led);
		}
	}
}

void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
