#define F_CPU 8000000UL
#define USART_BAUDRATE 9600 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "LCD16x2_4bit.h"
#define DHT11_PIN 6

void init(void);
void readKeypad(int, char[]);
void displaySoil(void);
//dht11
void RequestDHT11(void);
void ResponseDHT11(void);
char Receive_DHT11(void);
char DHT11[5];//display DHT11 data
void displayDHT11(void);
//send massege from GSM
void uart_init(void);
void send_uart(unsigned char gsmMsg);
void send_string(char *s);
void sendMessage(void);
int gsmSoilMoisture=0;
int gsmAirTemperature=0;
int gsmMaxHumidity=0;
int gsmMinHumidity=0;
//float mois_val;
unsigned int val,num;
char valueSetArray[10];//dispaly data
char keypadValue[20];//get keypad number
int soilMoisture=0;
int airTemperature=0;
int maxHumidity=0;
int minHumidity=0;
int c=0,humidityDig1,humidityDig2,TemparatureDig1,TemparatureDig2,CheckSum;//dht11
//soil moisture
float curMoisValue;
unsigned int curMoisDigValue;
char soilMois[10];

int initializedParams=0;

int main(void){
	init();
	uart_init();
	while(1){
	
		if(initializedParams == 0){
			ADMUX = 1<<REFS0|1<<MUX1;
		    ADCSRA = ADCSRA|(0<<ADSC); //end convertion
			if (soilMoisture==0){
				readKeypad(1,"Soil Moisture");
			
			}else if(airTemperature==0){
				readKeypad(2,"air Temparature");
			
			}else if(maxHumidity==0){
				readKeypad(3,"max Humidity");
			
			}else if(minHumidity==0){
				readKeypad(4,"min Humidity");
			//set values for send sms
			}else if(gsmSoilMoisture==0){
				readKeypad(5,"SoilMoistureSMS");
				
			}else if(gsmAirTemperature==0){
				readKeypad(6,"TemparatureSMS");
				
			}else if(gsmMaxHumidity==0){
				readKeypad(7,"MaxHumiditySMS");
				
			}else if(gsmMinHumidity==0){
				readKeypad(8,"MinHumiditySMS");
				
			}
			else{
				initializedParams = 1;
				ADMUX = 1<<REFS0|0<<MUX1;
				
				lcd_gotoxy(2,0);
				lcd_print("SetControlling");
				lcd_gotoxy(2,1);
				lcd_print("Values");
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);
				
				itoa(soilMoisture,valueSetArray,10);
				lcd_gotoxy(2,0);
				lcd_print("Soil Moisture");
				lcd_gotoxy(5,1);
				strcat(valueSetArray,"%");
				lcd_print(valueSetArray);
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);
				
				itoa(airTemperature,valueSetArray,10);
				lcd_gotoxy(2,0);
				lcd_print("air Temp-");
				lcd_print(valueSetArray);
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);
				
				itoa(maxHumidity,valueSetArray,10);
				lcd_gotoxy(0,0);
				lcd_print("max Humidity-");
				lcd_print(valueSetArray);
				
				itoa(minHumidity,valueSetArray,10);
				lcd_gotoxy(0,1);
				lcd_print("min Humidity-");
				lcd_print(valueSetArray);
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);	
				//display SMS values
				lcd_gotoxy(2,0);
				lcd_print("Set Values For");
				lcd_gotoxy(2,1);
				lcd_print("Warning SMS");
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);
				
				itoa(gsmSoilMoisture,valueSetArray,10);
				lcd_gotoxy(2,0);
				lcd_print("Soil Moisture");
				lcd_gotoxy(5,1);
				strcat(valueSetArray,"%");
				lcd_print(valueSetArray);
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);
				
				itoa(gsmAirTemperature,valueSetArray,10);
				lcd_gotoxy(2,0);
				lcd_print("air Temparature");
				lcd_gotoxy(5,1);
				//strcat(valueSetArray,"%");
				lcd_print(valueSetArray);
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);
				
				itoa(gsmMaxHumidity,valueSetArray,10);
				lcd_gotoxy(2,0);
				lcd_print("Max Humidity");
				lcd_gotoxy(5,1);
				//strcat(valueSetArray,"%");
				lcd_print(valueSetArray);
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);
				
				itoa(gsmMinHumidity,valueSetArray,10);
				lcd_gotoxy(2,0);
				lcd_print("Min Humidity");
				lcd_gotoxy(5,1);
				//strcat(valueSetArray,"%");
				lcd_print(valueSetArray);
				_delay_ms(1500);
				lcd_clear();
				_delay_ms(100);
			}
		}//end if initializedParams
		else{
		/////////////////////////////////////////////////////////////////
			// read sensors
			// dislplay values on LCD
			////////////////////////////////////////////////////////////////////
			ADMUX = 1<<REFS0;
			displaySoil();
			_delay_ms(2000);
			lcd_clear();
			displayDHT11();
			_delay_ms(2000);
			lcd_clear();
			////////////////////////////////////////////////////////////////////////
			// do control if required
			////////////////////////////////////////////////////////////////////
			//control soil
			DDRD=1<<2|1<<3|1<<4|1<<5;
			
			PORTD=1<<3;
			if(curMoisValue<=soilMoisture){
				PORTD=1<<2;
			}else{
				if(humidityDig1>=minHumidity){
					PORTD=0<<2;
					}
				}
				
			
			//control minHumidyt
			if(humidityDig1<=minHumidity){
				PORTD=1<<2|1<<4|0<<5;
			}else{
				if(curMoisValue>=soilMoisture){
					PORTD=0<<2;
				}
				
			}
			
			//control maxHumidyt
			if(humidityDig1>=maxHumidity){
				PORTD=1<<5|0<<4;
			}else{ }
			
			//control temparature
			if(TemparatureDig1>=airTemperature){
				PORTD=1<<4|0<<5;
			}else{ }
			///////////////////////////////////////////////////////////////////////////////////
			// if params out of range send sms 
			/////////////////////////////////////////////////////////////////////////////////////
			if(curMoisValue<=gsmSoilMoisture){
				sendMessage();
				lcd_print("Sending...");
				_delay_ms(2000);
				lcd_clear();
			}
			if(humidityDig1<=gsmMinHumidity){
				sendMessage();
				lcd_print("Sending...");
				_delay_ms(2000);
				lcd_clear();
			}
			if(humidityDig1>=gsmMaxHumidity){
				sendMessage();
				lcd_print("Sending...");
				_delay_ms(2000);
				lcd_clear();
			}
			if(TemparatureDig1>=gsmAirTemperature){
				sendMessage();
				lcd_print("Sending...");
				_delay_ms(2000);
				lcd_clear();
			}
			else{
				//
			}
			///////////////////////////////////////////////////////////////////////////////////////
		}//else if initializedParams
		_delay_ms(150);
	}
    return ADC;
}//main

void init(void){
	
	lcdinit();
	lcd_clear();
	
	ADCSRA =1<<ADEN|1<<ADPS1|1<<ADPS0;
}
void readKeypad(int paramType, char msg[10]){
	ADCSRA = ADCSRA|(1<<ADSC); //start convertion
	while((ADCSRA & (1<<ADSC))==0);//wait for finish
	val = ADCL|ADCH<<8;
	lcd_gotoxy(0,0);
	lcd_print(msg);
	if (val!=0){
		if((val>=230) & (val<=275)){
			 ADCSRA = ADCSRA|(0<<ADSC); //end convertion
			num=1;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=300) & (val<=350)){
			num=2;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=480) & (val<=530)){
			num=3;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=900) & (val<=1023)){
			num=4;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=105) & (val<=125)){
			num=5;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=129) & (val<=140)){
			num=6;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=145) & (val<=165)){
			num=7;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=170) & (val<=200)){
			num=8;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=71) & (val<=85)){
			num=9;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=89) & (val<=100)){
			num=0;
			itoa(num,valueSetArray,10);
			strcat(keypadValue,valueSetArray);
		}else if((val>=40) & (val<=65)){ // enter key
			if(strcmp(keypadValue," ") !=0){//compare enter value with empty keypad value
				if(paramType==1){
					soilMoisture = atoi(keypadValue);
					lcd_clear();
					lcd_print("done Moisture");
					_delay_ms(1000);
					lcd_clear();
					strcpy(keypadValue," ");
				}else if(paramType==2){
					airTemperature = atoi(keypadValue);
					lcd_clear();
					lcd_print("done Air Temp");
					_delay_ms(1000);
					lcd_clear();
					strcpy(keypadValue," ");
				}else if(paramType==3){
					maxHumidity = atoi(keypadValue);
					lcd_clear();
					lcd_print("done max Humidity");
					_delay_ms(1000);
					lcd_clear();
					strcpy(keypadValue," ");
				}else if(paramType==4){
					minHumidity = atoi(keypadValue);
					lcd_clear();
					lcd_print("done min Humidity");
					_delay_ms(1000);
					lcd_clear();
					strcpy(keypadValue," ");
					//set sms values
				}else if(paramType==5){
					gsmSoilMoisture = atoi(keypadValue);
					lcd_clear();
					lcd_gotoxy(0,0);
					lcd_print("done Moisture");
					lcd_gotoxy(2,1);
					lcd_print("For SMS");
					_delay_ms(1000);
					lcd_clear();
					strcpy(keypadValue," ");
				}else if(paramType==6){
					gsmAirTemperature = atoi(keypadValue);
					lcd_clear();
					lcd_gotoxy(0,0);
					lcd_print("done Air Temp");
					lcd_gotoxy(2,1);
					lcd_print("For SMS");
					_delay_ms(1000);
					lcd_clear();
					strcpy(keypadValue," ");
				}else if(paramType==7){
					gsmMaxHumidity = atoi(keypadValue);
					lcd_clear();
					lcd_gotoxy(0,0);
					lcd_print("done max Humidity");
					lcd_gotoxy(2,1);
					lcd_print("For SMS");
					_delay_ms(1000);
					lcd_clear();
					strcpy(keypadValue," ");
				}else if(paramType==8){
					gsmMinHumidity = atoi(keypadValue);
					lcd_clear();
					lcd_gotoxy(0,0);
					lcd_print("done min Humidity");
					lcd_gotoxy(2,1);
					lcd_print("For SMS");
					_delay_ms(1000);
					lcd_clear();
					strcpy(keypadValue," ");
				}
			}			
			_delay_ms(500);
		}
		
		
		val=0;
	}
	lcd_gotoxy(5,1);
	lcd_print(keypadValue);
}
void displaySoil(void){
	ADCSRA = ADCSRA|(1<<ADSC); //start convertion
	while((ADCSRA & (1<<ADSC))==0);//wait for finish
	curMoisDigValue = ADCL|ADCH<<8;
	curMoisValue=100-(curMoisDigValue*100.00)/1023.00;
	lcd_gotoxy(2,0);
	lcd_print("Soil Moisture");
	dtostrf(curMoisValue,3,2,soilMois);
	strcat(soilMois,"%   ");
	lcd_gotoxy(5,1);
	lcd_print(soilMois);
}
void RequestDHT11(void){//Microcontroller send start pulse

	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);
	_delay_ms(20);	
	PORTD |= (1<<DHT11_PIN);
}

void ResponseDHT11(void){//receive ResponseDHT11 from DHT11

	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}

char Receive_DHT11(void){//receive DHT11
	
	for (int q=0; q<8; q++){
	
		while((PIND & (1<<DHT11_PIN)) == 0);
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))				
		c = (c<<1)|(0x01);	
		else									
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}
void displayDHT11(void){
	RequestDHT11();	//send start pulse
	ResponseDHT11();//receive ResponseDHT11
	humidityDig1=Receive_DHT11();//store first eight bit in humidityDig1
	humidityDig2=Receive_DHT11();//store next eight bit in humidityDig2
	TemparatureDig1=Receive_DHT11();//store next eight bit in TemparatureDig1
	TemparatureDig2=Receive_DHT11();//store next eight bit in TemparatureDig2
	CheckSum=Receive_DHT11();//store next eight bit in CheckSum 
	
	if ((humidityDig1 + humidityDig2 + TemparatureDig1 + TemparatureDig2) != CheckSum){
	
		lcd_gotoxy(0,0);
		lcd_print("Error");
	}else{
		lcd_gotoxy(0,0);
		lcd_print("Humidity =");
		lcd_gotoxy(0,1);
		lcd_print("Temp = ");
		
		itoa(humidityDig1,DHT11,10);
		lcd_gotoxy(11,0);
		lcd_print(DHT11);
		lcd_print(".");
		
		itoa(humidityDig2,DHT11,10);
		lcd_print(DHT11);
		lcd_print("%");

		itoa(TemparatureDig1,DHT11,10);
		lcd_gotoxy(6,1);
		lcd_print(DHT11);
		lcd_print(".");
		
		itoa(TemparatureDig2,DHT11,10);
		lcd_print(DHT11);
		lcddata(0xDF);
		lcd_print("C ");
	}		
	_delay_ms(500);
}
void uart_init(void) {
	
	UCSRB |= (1 << RXEN) | (1 << TXEN);  // Transmission & Reception Enable (TXEN=1, RXEN=1)

	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); 
	UBRRH = (BAUD_PRESCALE >> 8); 
	UBRRL = BAUD_PRESCALE;
}

void send_uart(unsigned char gsmMsg){
	while ((UCSRA & (1 << UDRE)) == 0) ; 	//wait until previous transmition is finished
	UDR = gsmMsg;								//Transmit the charater
}
void send_string(char *s){
	//	send string
	while(*s){
		send_uart(*s);
		*s++;
	}
}
void sendMessage(void){
	send_string("AT\r");
	_delay_ms(1000);
	send_string("AT+CMGF=1\r");
	_delay_ms(1000);
	send_string("AT+CMGS=\"+94769604222\"\r");
	_delay_ms(1000);
	send_string("Warning");
	_delay_ms(1000);
	send_uart((char)26);
	_delay_ms(10000);
}