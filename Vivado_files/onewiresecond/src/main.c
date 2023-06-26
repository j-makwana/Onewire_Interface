#include "xparameters.h"
#include "xil_printf.h"
#include "xuartlite.h"
#include "xil_io.h"

#include <stdio.h>
#include <unistd.h>

XUartLite UartLite;
unsigned char rst_mode= 0xC1;
unsigned char presence_pulse=0;
unsigned char data_mode= 0xE1;
unsigned char set_pull_dur= 0x3B;
unsigned char pull_dur_response=0;
unsigned char skip_ROM=0xCC;
unsigned char skip_response=0;;
unsigned char command_mode=0xE3;
unsigned char terminate_pulse= 0xF1;
unsigned char convert_temp= 0x44;
unsigned char ARM_PULLUP= 0xEF;
unsigned char disarm_pullup =0xED;
unsigned char READ_SCRATCHPAD=0xBE;

unsigned char  UART1WIRE_READ_SEQ=  0xFF;

void clear_uart_buffer(XUartLite *UartLite) {
    unsigned char dummy;
    while (XUartLite_Recv(UartLite, &dummy, 1) != 0) {
        // This will keep reading one byte at a time until the buffer is empty.
    }
}
int initializeBoard(){

	clear_uart_buffer(&UartLite);
	//sleep(1);
//	usleep(1000);
	XUartLite_Send(&UartLite, &rst_mode, sizeof(char));

	//xil_printf("init start\r\n");
	while(XUartLite_IsSending(&UartLite)){
		//wait
	}
	//usleep(1000); // wait for 1ms

	//usleep(20000); // wait for 1ms
	//xil_printf("sent byte: %x\r\n", rst_mode);

	unsigned char buf[20];
	int ret;
	usleep(1000);
//	ret = XUartLite_Recv(&UartLite, buf, sizeof(buf));
	while((ret = XUartLite_Recv(&UartLite, buf, sizeof(char)))==0){

		}

//	for(int i=0 ; i< ret; i++){
//		xil_printf("data in buf: %x\r\n",buf[i]);
//	}
	presence_pulse= buf[0];
	//xil_printf("returned %d bytes\r\n", ret);
	xil_printf("%d\r\n", presence_pulse);
	switch(presence_pulse){
		case 0b11001100:
			xil_printf("One wire Shorted\r\n");
			break;
		case 0b11101100:
			xil_printf("One wire Shorted\r\n");
			break;
		case 0b11001101:
			xil_printf("Device detected\r\n");
			break;
		case 0b11101101:
			xil_printf("Device detected\r\n");
			break;
		case 0b11001110:
			xil_printf("ALARMING PRESENCE PULSE\r\n");
			break;
		case 0b11101110:
			xil_printf("ALARMING PRESENCE PULSE\r\n");
			break;
		case 0b11001111:
			xil_printf("No presence pulse\r\n");
			break;
		case 0b11101111:
			xil_printf("No presence pulse\r\n");
			break;
		default:
			xil_printf("Unknown error\r\n");
			return -1;

	}
	clear_uart_buffer(&UartLite);
	//usleep(10000); // wait for 1ms

	return 1;
}



void setPullDuration(){
	//usleep(10000);
	unsigned char pull_dur_response;
		XUartLite_Send(&UartLite, &set_pull_dur, sizeof(char));

		while(XUartLite_IsSending(&UartLite)){
			//wait
		}
		unsigned char buf[20];
		int ret;
		while((ret = XUartLite_Recv(&UartLite, buf, sizeof(buf)))==0){

			}

		for(int i=0 ; i< ret; i++){

			xil_printf("data in buf: %d\r\n",buf[i]);
		}

		xil_printf("returned %d bytes\r\n", ret);
		pull_dur_response = buf[0];


		if(pull_dur_response != 0x3A){
			xil_printf("error configuring pull duration\r\n");
		}
		xil_printf("pull response: %x\r\n", pull_dur_response);


}
void enterDataMode(){
	//usleep(10000);
	XUartLite_Send(&UartLite, &data_mode, sizeof(char));


		while(XUartLite_IsSending(&UartLite)){
			//wait
		}
	//usleep(10000);
	xil_printf("entered data mode\r\n");
	usleep(1000);


}
void enterCommandMode(){
	//usleep(10000);
		XUartLite_Send(&UartLite, &command_mode, sizeof(char));


			while(XUartLite_IsSending(&UartLite)){
				//wait
			}

		xil_printf("entered command mode\r\n");
		usleep(1000);

}

void SkipROM(){
	//usleep(1000);
	XUartLite_Send(&UartLite, &skip_ROM, sizeof(char));

			while(XUartLite_IsSending(&UartLite)){
				//wait
			}
			while(XUartLite_Recv(&UartLite, &skip_response, sizeof(char))==0){

					}

			if(skip_response != 0xCC){
				xil_printf("skip command not issued\r\n");
				xil_printf("skip response: %d\r\n", skip_response);
			}
			xil_printf("SKIP ROM RESPONSE: %x\r\n", skip_response);


}

void startTemperatureConversion(){
	setPullDuration();
	initializeBoard();
	enterDataMode();
	SkipROM();
	enterCommandMode();


	//arm strong pullup
	XUartLite_Send(&UartLite, &ARM_PULLUP, sizeof(char));


	while(XUartLite_IsSending(&UartLite)){
								//wait
					}


	//send terminate pulse
	XUartLite_Send(&UartLite, &terminate_pulse, sizeof(char));
	unsigned char terminate_pulse_response= 0;

	while(XUartLite_IsSending(&UartLite)){
				//wait
			}
	while(XUartLite_Recv(&UartLite, &terminate_pulse_response, sizeof(char))==0){

	}
	xil_printf("Terminate Pulse Response 1: %x\r\n", terminate_pulse_response);



	//terminate pulse over
	clear_uart_buffer(&UartLite);
	enterDataMode();
	//convert temp


	//convert temperature;
	clear_uart_buffer(&UartLite);
			XUartLite_Send(&UartLite, &convert_temp, sizeof(char));
			unsigned char temp_response= 0;


			while(XUartLite_IsSending(&UartLite)){
							//wait
						}
			while(XUartLite_Recv(&UartLite, &temp_response, sizeof(char))==0){

						}

			if(temp_response != 0x44){
				xil_printf("temp didn't convert\r\n");
				xil_printf("temp_response: %x\r\n", temp_response);

			}
			xil_printf("temp_response: %x\r\n", temp_response);
			//clear_uart_buffer(&UartLite);

			unsigned char end_pulse;
			while(XUartLite_Recv(&UartLite, &end_pulse, sizeof(char))==0){

							}
			if(end_pulse != 0xF6 ){
				if(end_pulse != 0x76){
				xil_printf("Arm pullup end not signaled properly: %x\r\n", end_pulse);}

			}
			xil_printf("End Pulse Returned: %x\r\n", end_pulse);
			//usleep(5240);
			 clear_uart_buffer(&UartLite);
			 enterCommandMode();
			 //disarming

			 XUartLite_Send(&UartLite, &disarm_pullup, sizeof(char));
			 //	char unsigned disarm_pullup_response=0;


			 		while(XUartLite_IsSending(&UartLite)){
		 						//wait
			 					}
			 		//usleep(1000);
			 	//terminate pulse

			 		//clear_uart_buffer(&UartLite);
			 		XUartLite_Send(&UartLite, &terminate_pulse, sizeof(char));
			 				unsigned char terminate_pulse_response2= 0;

			 						while(XUartLite_IsSending(&UartLite)){
			 							//wait
			 						}
			 						while(XUartLite_Recv(&UartLite, &terminate_pulse_response2, sizeof(char))==0){

			 								}

			 						xil_printf("terminate pulse 2: %x\r\n", terminate_pulse_response2);

			 						initializeBoard();
			 						xil_printf("Success\r\n\n\n");


}


void readScratchpad( uint8_t *data_out){
	//unsigned char command = READ_SCRATCHPAD; // Use appropriate value for READ_SCRATCHPAD
	unsigned char readResponse=0;
	// Send command
	//clear_uart_buffer(&UartLite);
	XUartLite_Send(&UartLite, &READ_SCRATCHPAD, sizeof(char));
	while(XUartLite_IsSending(&UartLite)) {
		//wait
	}
//	usleep(10000);
	while(XUartLite_Recv(&UartLite, &readResponse, sizeof(char))==0){

	}
	//usleep(1000);
	if(readResponse != READ_SCRATCHPAD){
		xil_printf("wrong scratchpad reading: %x\r\n", readResponse);

	}
	xil_printf(" scratchpad reading...: %x\r\n", readResponse);
	//usleep(10000); // wait for 10ms

	 int32_t rx_size;
	 uint8_t rx_cnt = 0;
	 clear_uart_buffer(&UartLite);
	usleep(10000);

	  for ( uint8_t cnt = 0; cnt < 9; cnt++ )
	     {
		  	 XUartLite_Send(&UartLite, &UART1WIRE_READ_SEQ, sizeof(char) );
		  	 while(XUartLite_IsSending(&UartLite)) {
		  			//wait
		  		}
	        // usleep(10000);
	         while((rx_size = XUartLite_Recv(&UartLite, &data_out[ cnt ], 1 ))==0){

	         }
	           xil_printf("data: %x\r\n", data_out[cnt]);
	         if ( rx_size > 0 )
	         {
	             rx_cnt++;
	         }
	     }

	     if ( rx_cnt != 9 )
	     {
	         xil_printf("PROBLEM");
	     }




	enterCommandMode();
}


static uint8_t calculateCRC(uint8_t *data_buf, uint8_t len){
	  uint8_t crc = 0x00;

	    for ( uint8_t i = 0; i < len; i++ )
	    {
	        uint8_t in_byte = data_buf[ i ];
	        for ( uint8_t j = 0; j < 8; j++ )
	        {
	            uint8_t mix = ( crc ^ in_byte ) & 0x01;
	            crc >>= 1;
	            if ( mix != 0 )
	            {
	                crc ^= 0x8C;
	            }
	            in_byte >>= 1;
	        }
	    }
	    return crc;

}

void readTemperature(float *temperature){
	int16_t temp_word = 0;
	uint8_t rx_buf[ 9 ] = { 0 };

	clear_uart_buffer(&UartLite);
	initializeBoard();
	//issue skip ROM command
	clear_uart_buffer(&UartLite);
	enterDataMode();
	//usleep(10000);
	char unsigned skip_response2= 0;
	//usleep(10000);
	clear_uart_buffer(&UartLite);
	XUartLite_Send(&UartLite, &skip_ROM, sizeof(char));
	xil_printf("skip_ROM: %x\r\n",skip_ROM);
				while(XUartLite_IsSending(&UartLite)){
					//wait
				}
				while(XUartLite_Recv(&UartLite, &skip_response2, sizeof(char))==0){

						}

				if(skip_response2 != 0xCC){
					xil_printf("skip command not issued\r\n");
					xil_printf("skip response: %d\r\n", skip_response2);
				}
				xil_printf("SKIP ROM RESPONSE: %x\r\n", skip_response2);


	clear_uart_buffer(&UartLite);
	 readScratchpad(rx_buf);
	 uint8_t crc;
	    crc = calculateCRC(rx_buf, 8 );
	    xil_printf("CRC: %x \r\n rx_buf[8]: %x \r\n", crc, rx_buf[8]);

	    if ( crc != rx_buf[8] )
	    {
	        return XST_FAILURE;
	    }


	 temp_word = ( rx_buf[ 1 ] << 8 ) | rx_buf[ 0 ];
	 *temperature = ( float ) temp_word;
	 *temperature /= ( 1 << ( 12 - 8 ) );



}


int main(void){
	xil_printf("~~~ Program Start ~~~~\r\n");
		int Status;
		float temperature=0;
		Status= XUartLite_Initialize(&UartLite,XPAR_AXI_UARTLITE_0_DEVICE_ID);
		if(Status != XST_SUCCESS){
			xil_printf("Transfer Failed.\r\n\n");
			return XST_FAILURE;
		}
		Status = XUartLite_SelfTest(&UartLite);
		if(Status != XST_SUCCESS){
			xil_printf("Transfer Failed.\r\n\n");
			return XST_FAILURE;
		}
		xil_printf("will initialize the board now\r\n\n");

		if(initializeBoard()!= 1){
			exit(1);
		}
		startTemperatureConversion();
		readTemperature(&temperature);

		int temperature_int = (int)temperature;
		int temperature_frac = (temperature - temperature_int) * 1000;  // scale fractional part
		xil_printf("Temperature: %d.%03d\r\n", temperature_int, temperature_frac);


		return 0;
}
