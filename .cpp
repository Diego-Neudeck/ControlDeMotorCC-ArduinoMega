/*
 * ejercicio1_integrador3_V1.c
 *
 * Created: 11/10/2019 10:05:37
 * Author : Diego Neudeck
 */ 
//definiciones--------------------------------
#define is_high(p,b)	(p & _BV(b)) == _BV(b)	//	is_high(p,b) p/testear si el bit b de p es 1.
#define is_low(p,b)		(p & _BV(b)) == 0		//	is_low(p,b) p/testear si el bit b de p es 0.
#define	sbi(p,b)		p |= _BV(b)				//	sbi(p,b) setea el bit b de p.
#define	cbi(p,b)		p &= ~(_BV(b))			//	cbi(p,b) borra el bit b de p.
#define TOP1 19999
#define VPC0 143								//para tomar las muestras
#define F_CPU	16000000UL	//	Define Fcpu = 16 MHz p/cálculo de retardos
#define var 1000
#define muestras 0
#define boton 60
//#define D4 eS_PORTA0		// Pin de dato D4 = PA0.
//#define D5 eS_PORTA1		// Pin de dato D5 = PA1.
//#define D6 eS_PORTA2		// Pin de dato D6 = PA2.
//#define D7 eS_PORTA3		// Pin de dato D7 = PA3.
//#define RS eS_PORTA4		// Pin RS = PA4 (Reset).
//#define EN eS_PORTA6		// Pin EN = PA5 (Enable).

//librerias-----------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
//#include "lcd.h"			// Contiene funciones para manejo del LCD.

//variables globales--------------------------
int horario=0;
int prendido=0;					//se pone en uno cuando se prende 
int ban2=0;
int Mu1=0;
int Mu2=0;
int Mu3=0;
int Mu4=0;
int valor=0;
int valor1=0;
int valor2=0;
int bandera=0;
int cont=0;

//funciones-----------------------------------
//--------------------------------------------

void confi_TIMER1();			//PWM
void conf_ADC();
void conf_TIMER0();				//para tomar las muestras.
void def_puertos();
void testeo();
void guiro();
//rutinas de la interrupcion------------------
//--------------------------------------------
ISR (TIMER0_OVF_vect)
{
	Mu1=ADCH;
	if(bandera==0)
	{
		bandera=1;
		Mu2=Mu1;
		Mu3=Mu1;
		Mu4=Mu1;
	}
	valor=Mu1+Mu2+Mu3+Mu4;
	valor1=valor>>2;
	valor2=valor1*63+2000;			//normalizacion

	
	if(valor2>=18000)
	{
		valor2=18000;
	}
	if(valor2<=2000)
	{
		valor2=2000;
	}
	OCR1B = valor2;
	Mu4 = Mu3;
	Mu3 = Mu2;
	Mu2 = Mu1;
		
}


int main(void)
{
	int a=0;
	def_puertos();
	confi_TIMER1();
	conf_ADC();
	conf_TIMER0();
	sei();
    for(;;)
    {	a=1;
		//_delay_ms(1000);
		PORTD = 0b00000001;
		//_delay_ms(10);
		a=2;
		PORTD = 0b00000000;
		a=3;
		//testeo();
		//guiro();
    }		
}

void def_puertos()
{
	DDRB = 0xFF;						 //configuro el puerto B como salida (PB5= pin OCA1)
	DDRA = 0xFF;
	DDRC = 0x00;						//configuro como entrada b0 y b1
	PORTC = 0x03;						//activo las resistencias internas del puerto C b0 y b1
	DDRD = 0x00;						//configuro el puerto D como salida in1 e in2
}

void confi_TIMER1()		//	Configura Timer1 como Fast PWM -> Fpwm = 16MHz/[N*(TOP1 + 1)].
{
	TCCR1A = 0x23;		//	Modo fast PWM 
	TCCR1B = 0x1A;		//	N=8 
	TCCR1C = 0x00;		//	
	OCR1A = TOP1;		//	Carga el valor TOP para del Timer1.
	TIMSK1 = 0x00;		// no habilito las interrupciones.
}

void conf_ADC()
{
	ADMUX = 0b00100000;		//modo comun ADC0, Vref=5 bits[7,6]"00"//Vref[7,6]=2,67 bits[7,6]"11"
	ADCSRB = 0b01000000;	//modo normal y habilito bits ACME.
	ADCSRA = 0b11111110;	//prescaler de 64
	DIDR0 = 0b11111111;		//desabilito las entradas digitales .
}

void conf_TIMER0()
{
	TCCR0A = 0x00;				//modo normal
	TCCR0B = 0x02;				//prescaler en 8
	TIMSK0 = 0x01;				//habilito interrupcion por desbordamiento del timer
	//TCNT0 = VPC0;				//cargo el valor de precarga
	//----------------------------------------------------------------
	
}

void testeo()
{
	_delay_ms(boton);
	if (is_low(PINC,PC0)){
		
		if(ban2==0){
			prendido=1;
			ban2=1;
		}
		else{
			prendido=0;
			ban2=0;
		}
	//_delay_ms(boton);
	if(is_low(PINC,PC1)){
		if(prendido==1){
			horario=1;
		}
	}
	else{
		if(prendido==1){
			horario=0;
		}
	}
	}
}

void guiro()
{
	if(prendido==1){
		if(horario==1){
			PORTD = 0b00000010;			//prendo in1 y apago in2
		}
		else{
			PORTD = 0b00000001;         //prendo in2 y apago in2
		}
	}
	else{
			PORTD = 0x00;				//apago el motor
	}
}
