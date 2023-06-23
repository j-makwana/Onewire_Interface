#include "xparameters.h"

#include "xil_printf.h"

#include "xuartlite.h"

#include "xil_io.h"

#include <stdio.h>

#include <unistd.h>

class onewireOS
{
private:
	/* data */

	XUartLite UartLite;
	unsigned char rst_mode = 0xC1;
	unsigned char presence_pulse = 0;
	unsigned char data_mode = 0xE1;
	unsigned char set_pull_dur = 0x3B;
	unsigned char pull_dur_response = 0;
	unsigned char command_mode = 0xE3;
	unsigned char terminate_pulse = 0xF1;
	unsigned char convert_temp = 0x44;
	unsigned char ARM_PULLUP = 0xEF;
	unsigned char disarm_pullup = 0xED;
	unsigned char READ_SCRATCHPAD = 0xBE;

	unsigned char UART1WIRE_READ_SEQ = 0xFF;

	static uint8_t calculateCRC(uint8_t *data_buf, uint8_t len)
	{
		uint8_t crc = 0x00;

		for (uint8_t i = 0; i < len; i++)
		{
			uint8_t in_byte = data_buf[i];
			for (uint8_t j = 0; j < 8; j++)
			{
				uint8_t mix = (crc ^ in_byte) & 0x01;
				crc >>= 1;
				if (mix != 0)
				{
					crc ^= 0x8C;
				}
				in_byte >>= 1;
			}
		}
		return crc;
	}

	void enterDataMode()
	{

		XUartLite_Send(&UartLite, &data_mode, sizeof(char));

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}

		//xil_printf("entered data mode\r\n");
		usleep(1000);
	}
	void enterCommandMode()
	{

		XUartLite_Send(&UartLite, &command_mode, sizeof(char));

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}

		//xil_printf("entered command mode\r\n");
		usleep(1000);
	}
	int setPullDuration()
	{
		unsigned char pull_dur_response;
		XUartLite_Send(&UartLite, &set_pull_dur, sizeof(char));

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}
		unsigned char buf[20];
		int ret;
		while ((ret = XUartLite_Recv(&UartLite, buf, sizeof(buf))) == 0)
		{
		}
		pull_dur_response = buf[0];

		if (pull_dur_response != 0x3A)
		{
			xil_printf("error configuring pull duration\r\n");
			return -1;
		}
		return 1;
	}

	int SkipROM()
	{
		unsigned char skip_ROM = 0xCC;
		unsigned char skip_response = 0;
		XUartLite_Send(&UartLite, &skip_ROM, sizeof(char));

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}
		while (XUartLite_Recv(&UartLite, &skip_response, sizeof(char)) == 0)
		{
		}

		if (skip_response != 0xCC)
		{
			return -1;
		}

		return 1;
	}

	int startTemperatureConversion()
	{
		setPullDuration();
		resetPulse();
		enterDataMode();
		SkipROM();
		enterCommandMode();

		// arm strong pullup
		XUartLite_Send(&UartLite, &ARM_PULLUP, sizeof(char));

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}

		// send terminate pulse
		XUartLite_Send(&UartLite, &terminate_pulse, sizeof(char));
		unsigned char terminate_pulse_response = 0;

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}
		while (XUartLite_Recv(&UartLite, &terminate_pulse_response, sizeof(char)) == 0)
		{
		}

		// terminate pulse over
		clear_uart_buffer(&UartLite);
		enterDataMode();
		// convert temp

		// convert temperature;
		clear_uart_buffer(&UartLite);
		XUartLite_Send(&UartLite, &convert_temp, sizeof(char));
		unsigned char temp_response = 0;

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}
		while (XUartLite_Recv(&UartLite, &temp_response, sizeof(char)) == 0)
		{
		}

		if (temp_response != 0x44)
		{
			return -1;
		}

		unsigned char end_pulse;
		while (XUartLite_Recv(&UartLite, &end_pulse, sizeof(char)) == 0)
		{
		}
		if (end_pulse != 0xF6)
		{
			if (end_pulse != 0x76)
			{
				xil_printf("Arm pullup end not signaled properly: %x\r\n", end_pulse);
				return -1;
			}
		}

		clear_uart_buffer(&UartLite);
		enterCommandMode();
		// disarming

		XUartLite_Send(&UartLite, &disarm_pullup, sizeof(char));

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}

		XUartLite_Send(&UartLite, &terminate_pulse, sizeof(char));
		unsigned char terminate_pulse_response2 = 0;

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}
		while (XUartLite_Recv(&UartLite, &terminate_pulse_response2, sizeof(char)) == 0)
		{
		}

		resetPulse();
		return 1;
	}

	int readScratchpad(uint8_t *data_out)
	{

		unsigned char readResponse = 0;
		// Send command

		XUartLite_Send(&UartLite, &READ_SCRATCHPAD, sizeof(char));
		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}

		while (XUartLite_Recv(&UartLite, &readResponse, sizeof(char)) == 0)
		{
		}

		if (readResponse != READ_SCRATCHPAD)
		{
			return - 1;
		}
		//xil_printf(" reading temperature...: %x\r\n", readResponse);

		int32_t rx_size;
		uint8_t rx_cnt = 0;
		clear_uart_buffer(&UartLite);
		usleep(10000);

		for (uint8_t cnt = 0; cnt < 9; cnt++)
		{
			XUartLite_Send(&UartLite, &UART1WIRE_READ_SEQ, sizeof(char));
			while (XUartLite_IsSending(&UartLite))
			{
				// wait
			}
			// usleep(10000);
			while ((rx_size = XUartLite_Recv(&UartLite, &data_out[cnt], 1)) == 0)
			{
			}
			if (rx_size > 0)
			{
				rx_cnt++;
			}
		}

		if (rx_cnt != 9)
		{
			return - 1;
		}

		enterCommandMode();
		return 1;
	}

	int readTemperature(float *temperature)
	{
		int16_t temp_word = 0;
		uint8_t rx_buf[9] = {0};

		clear_uart_buffer(&UartLite);
		resetPulse();
		// issue skip ROM command
		clear_uart_buffer(&UartLite);
		enterDataMode();


		clear_uart_buffer(&UartLite);
		SkipROM();

		clear_uart_buffer(&UartLite);
		readScratchpad(rx_buf);
		uint8_t crc;
		crc = calculateCRC(rx_buf, 8);


		if (crc != rx_buf[8])
		{
			xil_printf("Invalid Data\r\n");
			return -1;
		}

		temp_word = (rx_buf[1] << 8) | rx_buf[0];
		*temperature = (float)temp_word;
		*temperature /= (1 << (12 - 8));
		return 1;
	}
	void clear_uart_buffer(XUartLite *UartLite)
	{
		unsigned char dummy;
		while (XUartLite_Recv(UartLite, &dummy, 1) != 0)
		{

		}
	}

public:
	onewireOS()  {}

	~onewireOS() {}

	// first command to be used
	int intializeBoard()
	{
		xil_printf("~~~~~~~~~~PROGRAM START~~~~~~~~~~\r\n");
		int Status;

		Status = XUartLite_Initialize(&UartLite, XPAR_AXI_UARTLITE_0_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			xil_printf("Transfer Failed.\r\n\n");
			return XST_FAILURE;
		}
		Status = XUartLite_SelfTest(&UartLite);
		if (Status != XST_SUCCESS)
		{
			xil_printf("Transfer Failed.\r\n\n");
			return XST_FAILURE;
		}
		xil_printf("will initialize the board now\r\n\n");
		if (resetPulse() != 1)
		{
			return -1;
		}
		return 1;
	}

	int resetPulse()
	{

		clear_uart_buffer(&UartLite);
		XUartLite_Send(&UartLite, &rst_mode, sizeof(char));

		while (XUartLite_IsSending(&UartLite))
		{
			// wait
		}

		unsigned char buf[20];
		int ret;
		usleep(1000);

		while ((ret = XUartLite_Recv(&UartLite, buf, sizeof(char))) == 0)
		{
		}

		presence_pulse = buf[0];
		switch (presence_pulse)
		{
		case 0b11001100:
			//xil_printf("One wire Shorted\r\n");
			return -1;
		case 0b11101100:
			//xil_printf("One wire Shorted\r\n");
			return -1;
		case 0b11001101:
			//xil_printf("Device detected\r\n");
			break;
		case 0b11101101:
			//xil_printf("Device detected\r\n");
			break;
		case 0b11001110:
			//xil_printf("ALARMING PRESENCE PULSE\r\n");
			return -1;
		case 0b11101110:
			//xil_printf("ALARMING PRESENCE PULSE\r\n");
			return -1;
		case 0b11001111:
			//xil_printf("No presence pulse\r\n");
			return -1;
		case 0b11101111:
			//xil_printf("No presence pulse\r\n");
			return -1;
		default:
			//xil_printf("Unknown error\r\n");
			return -1;
		}
		clear_uart_buffer(&UartLite);

		return 1;
	}
	float getTemperature()
	{
		float temperature = 0x00;
		if (readTemperature(&temperature) == -1)
		{
			return -1;
		}else
		{
			int temperature_int = (int)temperature;
			int temperature_frac = (temperature - temperature_int) * 1000; // scale fractional part
			xil_printf("Temperature: %d.%03d\r\n", temperature_int, temperature_frac);
		}
		return temperature;
	}
	void get5Readings(){
		for (int i=0; i<5; i++){
			getTemperature();
			sleep(3);
		}
	}


};

int main(void){
		onewireOS onewire;
		//Initialize Board
		if(onewire.intializeBoard()!=1){
			//exit(1);
			xil_printf("Couldn't initalize board\t\n");
		}
		//get temperature
		//onewire.getTemperature();
		onewire.get5Readings();
		return 0;
	}

