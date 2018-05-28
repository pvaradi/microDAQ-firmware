
#include "piccolo_all.h"
#include <delays.h>																			
#include <p18cxxx.h>																		
#include <i2c.h>																				
#include <adc.h>
#include <timers.h>
#include <pwm.h>
#include <capture.h>
																							
/** \file

Az �rtelmezett parancsok:
 - #A - DAC bekapcsol�sa
 - #B - MICROCHIP I/O expander bekapcsol�sa - output
 - #C - ADC m�k�dtet�se
 - #D - MICROCHIP I/O expander bekapcsol�sa - input
 - #E - PWM 

Ha a program leford�t�sa el�tt az USE_USB �s az USE_INTERRUPT szimb�lumokat
is defini�ljuk, akkor az USB kapcsolat kiszolg�l�sa interrupt szinten
t�rt�nik. Ha csak az USE_USB szimb�lumot defini�ljuk, akkor pedig lek�rdez�ses
�zemm�dban t�rt�nik a kezel�s.


/**************************** V�LTOZ�K ********************************************************/

unsigned char sync_mode=0,slew=0,data,status,I2CADAT,inputvalue,adcchannel,periodus;
unsigned int meres,dutycycle,INCAPresult1,INCAPresult2,frekvencia,Rise_edge1,Fall_edge1,magas_szint,alacsony_szint;																	
char uc,uv,flag;
unsigned char configccp=0;			//--- CCP konfigur�l�shoz haszn�lt v�ltoz�
unsigned char timer_value = 0;

/**************************** F�GGV�NYEK *****************************************************/

void I2Crutin (void);

/****************************INTERRUPT KISZOLG�L� ELJ�R�SOK **********************************/

#if defined(USE_INTERRUPT)

	#pragma interrupt hi_isr						///--- Magas priorit�s� programmegszak�t�s kiszolg�l�sa
	void hi_isr() 
				{
				#if defined(USB_INTERRUPT)
	    		USBDeviceTasks(); 					//--- USB programmegszak�t�s kiszolg�l�sa
				#endif  							//--- USB_INTERRUPT
				}
													//--- Alacsony priorit�s� programmegszak�t�s kiszolg�l�sa
	#pragma interruptlow lo_isr
	void lo_isr() 
				{
				}
#endif 												//--- USE_INTERRUPT

/*********************************************************************
 *   							 F�PROGRAM                           *
 *********************************************************************/

void I2Crutin (void);


//////////////////////////////////////////INICIALIZ�L�S///////////////////////////////////////////////////

void main(void) 
				{
    			InitializeSystem();
    			while (!usb_cdc_kbhit()) 						//--- Megv�rjuk az els� karakter �rkez�s�t
										{
        								ProcessIO();
    									}

///////////////////////////////////////////F�PROGRAM KEZDETE///////////////////////////////////////////////    		

				while (1) {										//--- Ez a f� ciklus, amiben fut a switch-case is
        					do 
							{
            				uv=usb_cdc_getc();					//--- Random karaterre v�rakoz�s
        					} 
							while (uv!='#');					//--- V�runk am�g # nem �rkezik be
        					uc=usb_cdc_getc();

        					switch (uc) {

//////////////////////////////////////////DAC m�k�dtet�se//////////////////////////////////////////////////

        								case 'A':   

										CloseI2C();										//--- I2C bez�r�sa 

										sync_mode = MASTER;								//--- sync �s slew be�ll�t�sa
										slew = SLEW_OFF;
  
										OpenI2C(sync_mode,slew);						//--- konfigur�ci�

    									SSPADD=0x77;									//--- 100kHz Baud clock(9) @48MHz

										IdleI2C();
										StartI2C();
	
										data = SSPBUF;									//--- buffer �r�t�se
	
										I2Crutin ();
										IdleI2C();
										StopI2C();

										break;

/////////////////////////////////////IO EXPANDER m�k�dtet�se////////////////////////////////////////////////

										case 'B':

										CloseI2C();

										sync_mode = MASTER;
										slew = SLEW_OFF;
  
										OpenI2C(sync_mode,slew);

    									SSPADD=0x77;			 					


										IdleI2C();
										StartI2C();
	
										data = SSPBUF;

										I2Crutin ();
										IdleI2C();
										StopI2C();

										IdleI2C();
										StartI2C();
	
										data = SSPBUF;

										I2Crutin ();
										IdleI2C();
										StopI2C();
	
										delay_ms(100);
            
        								break;

////////////////////////////////////////ADC m�k�dtet�se/////////////////////////////////////////////////

										case 'C':
										adcchannel = get2hex();					//--- ADC csatorna kiv�laszt�sa

										#define ADC_REF_VDD_VDD_X 0b11110011 	//--- MC18 ford�t�ban �ll�t�lag rosszul van defini�lva ADC voltage source VREF+ = AVDD

										TRISCbits.TRISC3 = 1;					//--- Bemenetre �ll�t�s anal�g l�bak miatt
										TRISCbits.TRISC2 = 1;
										TRISCbits.TRISC6 = 1;
										TRISCbits.TRISC7 = 1;	

										ANSEL = 0b11000000;						//--- L�bakat anal�gk�nt haszn�lom haszn�lata 
										ANSELH = 0b00000011; 

										ADCON0=(0b00111101 & adcchannel);   	//--- ADC port channel X (ANX), �s ADC enged�lyez�se
 										ADCON1=0b00000000;   					//--- Bels� referencia haszn�lata (Vdd and Vss)
  										ADCON2=0b00101110;   					//--- Balra igaz�t�s, 12 TAD, 48MHz haszn�lata

										//--- Ez valami�rt nem m�k�dik egyenl�re OpenADC( ADC_FOSC_64 & ADC_LEFT_JUST & ADC_20_TAD, ADC_CH7 & ADC_INT_OFF, ADC_REF_VDD_VDD_X & ADC_REF_VDD_VSS , ADC_7ANA );

										ConvertADC();
 										while( BusyADC() );
										meres = ReadADC();						//--- Eredm�ny elt�rol�sa
									
										outdec(meres,0);						//--- V�ltoz� kik�ld�se, tizedesjegyek sz�ma 0
										
 		      
       									break;
		

///////////////////////////////////////INPUT m�k�dtet�se////////////////////////////////////////////////

										case 'D':

										CloseI2C();

										sync_mode = MASTER;
										slew = SLEW_OFF;
  
										OpenI2C(sync_mode,slew);

    									SSPADD=0x77;			 					


										IdleI2C();
										StartI2C();
	
										data = SSPBUF;

										I2Crutin ();
	
								

										IdleI2C();
										StopI2C();

										IdleI2C();
										StartI2C();
	
										data = SSPBUF;

										I2Crutin ();
										IdleI2C();
										StopI2C();


										IdleI2C();
										StartI2C();

										I2Crutin ();
										IdleI2C();
										StopI2C();


										IdleI2C();
										StartI2C();

										I2Crutin ();
										inputvalue = ReadI2C();
										NotAckI2C();
										outdec(inputvalue,0);					//--- V�ltoz� kik�ld�se, tizedesjegyek sz�ma 0

										IdleI2C();
										StopI2C();

  		      
        								break;

////////////////////////////////////////PWM m�k�dtet�se/////////////////////////////////////////////////

										case 'E':
										
										TRISCbits.TRISC5 = 0;					//--- L�b kimenetk�nt haszn�lata
										LATCbits.LATC5 = 0;						

										OpenTimer2(T2_PS_1_4 & TIMER_INT_OFF); 	//--- Timer2 haszn�lata a pwm-hez. Ezt bekell �ll�tani a PWM m�k�dtet�se el�tt. 4-es el�oszt�, interrupt kikapcsolva.

										periodus = get2hex();					//--- Perl-ben kisz�molt peri�dus �rt�k beolvas�sa
										dutycycle = get4hex();					//--- Perl-ben kisz�molt kit�lt�si t�nyez� beolvas�sa

										OpenPWM1(periodus);                     //--- P�lda: PWM period =[(period ) + 1] x 4 x TOSC x TMR2 prescaler. The value of period is from 0x00 to 0xff. Channel 1:PWM period = 1/((199+1)*4*(1/48e6)*1) = 60 kHz
										SetDCPWM1(dutycycle);                   //--- P�lda: PWM x Duty cycle = (DCx<9:0>) x TOSC x TMR2 prescaler. Setting duty cycle for channel 1: DC = 1/(300 * (1/48e6)*1) = 160 KHz
										
 		      
       									break;

////////////////////////////////////////CC m�k�dtet�se/////////////////////////////////////////////////

										case 'F':
										

      									SetTmrCCPSrc(T1_SOURCE_CCP);                            //--- Timer1 lesz a Capture-re k�tve
										PIR1bits.CCP1IF = 0;
										TMR1H = 0;
										TMR1L = 0;
										CCPR1L = 0;
										CCPR1H = 0;												//--- Capture konfigur�l�sa
      									configccp = (CAP_EVERY_RISE_EDGE & CAPTURE_INT_OFF) ;       
     									OpenCapture1(configccp);								//--- OpenCapture1 f�ggv�ny h�v�sa
      									OpenTimer1(0);                                  		//--- Timer1 ind�t�sa

      									while(!PIR1bits.CCP1IF);  								//--- V�rakoz�s am�g capture nem t�rt�nik                             
										PIR1bits.CCP1IF = 0;
																							
      									INCAPresult1 = ReadCapture1();                          //--- Eredm�ny kiolvas�sa a CCPL �s CCPH b�l

										while(!PIR1bits.CCP1IF);  								//--- V�rakoz�s am�g capture nem t�rt�nik                             
										PIR1bits.CCP1IF = 0;
																							
      									INCAPresult2 = ReadCapture1();

										frekvencia = (INCAPresult2 - INCAPresult1);

      									CloseCapture1();										//--- Modul le�ll�t�sa
										CloseTimer1();
										

								


		      							SetTmrCCPSrc(T1_SOURCE_CCP);                            //--- Timer1 lesz a Capture-re k�tve
										PIR1bits.CCP1IF = 0;
										TMR1H = 0;
										TMR1L = 0;
										CCPR1L = 0;
										CCPR1H = 0;												//--- Capture konfigur�l�sa
      									configccp = (CAP_EVERY_RISE_EDGE & CAPTURE_INT_OFF) ;       
     									OpenCapture1(configccp);								//--- OpenCapture1 f�ggv�ny h�v�sa
      									OpenTimer1(0);                                  		//--- Timer1 ind�t�sa

      									while(!PIR1bits.CCP1IF);  								//--- V�rakoz�s am�g capture nem t�rt�nik                             
										PIR1bits.CCP1IF = 0;
																							
      									INCAPresult1 = ReadCapture1();                          //--- Eredm�ny kiolvas�sa a CCPL �s CCPH b�l

										configccp = (CAP_EVERY_FALL_EDGE & CAPTURE_INT_OFF) ;       
     									OpenCapture1(configccp);

										while(!PIR1bits.CCP1IF);  								//--- V�rakoz�s am�g capture nem t�rt�nik                             
										PIR1bits.CCP1IF = 0;
																							
      									INCAPresult2 = ReadCapture1();

										alacsony_szint = (INCAPresult2 - INCAPresult1);

										outdec(frekvencia,0);
										
										delay_ms(10);

										outdec(alacsony_szint,0);

      									CloseCapture1();										//--- Modul le�ll�t�sa
										CloseTimer1();
       									break;

//////////////////////////////////////////Ismeretlen parancs/////////////////////////////////////////////////
//
        								default:
            							usb_cdc_putc('?');          		//Ismeretlen parancs
            							break;
        								} 									//switch v�ge
    					} 													//while
				}     														//main

///******** F�program v�ge *********************************************/


void I2Crutin (void)
{
while(1)
												{
												I2CADAT = get2hex();
												flag = usb_cdc_getc();
												if ( 'o' == flag)
																{
																do
																	{
																	status = WriteI2C(I2CADAT);
																	if(status == -1)
																					{
																					data = SSPBUF;
																					SSPCON1bits.WCOL=0;
																					}
																	}
																while(status!=0);
																}
												else
													{
													break;
													}
												}
}