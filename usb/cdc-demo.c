
#include "piccolo_all.h"
#include <delays.h>																			
#include <p18cxxx.h>																		
#include <i2c.h>																				
#include <adc.h>
#include <timers.h>
#include <pwm.h>
#include <capture.h>
																							
/** \file

Az értelmezett parancsok:
 - #A - DAC bekapcsolása
 - #B - MICROCHIP I/O expander bekapcsolása - output
 - #C - ADC mûködtetése
 - #D - MICROCHIP I/O expander bekapcsolása - input
 - #E - PWM 

Ha a program lefordítása elõtt az USE_USB és az USE_INTERRUPT szimbólumokat
is definiáljuk, akkor az USB kapcsolat kiszolgálása interrupt szinten
történik. Ha csak az USE_USB szimbólumot definiáljuk, akkor pedig lekérdezéses
üzemmódban történik a kezelés.


/**************************** VÁLTOZÓK ********************************************************/

unsigned char sync_mode=0,slew=0,data,status,I2CADAT,inputvalue,adcchannel,periodus;
unsigned int meres,dutycycle,INCAPresult1,INCAPresult2,frekvencia,Rise_edge1,Fall_edge1,magas_szint,alacsony_szint;																	
char uc,uv,flag;
unsigned char configccp=0;			//--- CCP konfiguráláshoz használt változó
unsigned char timer_value = 0;

/**************************** FÜGGVÉNYEK *****************************************************/

void I2Crutin (void);

/****************************INTERRUPT KISZOLGÁLÓ ELJÁRÁSOK **********************************/

#if defined(USE_INTERRUPT)

	#pragma interrupt hi_isr						///--- Magas prioritású programmegszakítás kiszolgálása
	void hi_isr() 
				{
				#if defined(USB_INTERRUPT)
	    		USBDeviceTasks(); 					//--- USB programmegszakítás kiszolgálása
				#endif  							//--- USB_INTERRUPT
				}
													//--- Alacsony prioritású programmegszakítás kiszolgálása
	#pragma interruptlow lo_isr
	void lo_isr() 
				{
				}
#endif 												//--- USE_INTERRUPT

/*********************************************************************
 *   							 FÕPROGRAM                           *
 *********************************************************************/

void I2Crutin (void);


//////////////////////////////////////////INICIALIZÁLÁS///////////////////////////////////////////////////

void main(void) 
				{
    			InitializeSystem();
    			while (!usb_cdc_kbhit()) 						//--- Megvárjuk az elsõ karakter érkezését
										{
        								ProcessIO();
    									}

///////////////////////////////////////////FÕPROGRAM KEZDETE///////////////////////////////////////////////    		

				while (1) {										//--- Ez a fõ ciklus, amiben fut a switch-case is
        					do 
							{
            				uv=usb_cdc_getc();					//--- Random karaterre várakozás
        					} 
							while (uv!='#');					//--- Várunk amíg # nem érkezik be
        					uc=usb_cdc_getc();

        					switch (uc) {

//////////////////////////////////////////DAC mûködtetése//////////////////////////////////////////////////

        								case 'A':   

										CloseI2C();										//--- I2C bezárása 

										sync_mode = MASTER;								//--- sync és slew beállítása
										slew = SLEW_OFF;
  
										OpenI2C(sync_mode,slew);						//--- konfiguráció

    									SSPADD=0x77;									//--- 100kHz Baud clock(9) @48MHz

										IdleI2C();
										StartI2C();
	
										data = SSPBUF;									//--- buffer ürítése
	
										I2Crutin ();
										IdleI2C();
										StopI2C();

										break;

/////////////////////////////////////IO EXPANDER mûködtetése////////////////////////////////////////////////

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

////////////////////////////////////////ADC mûködtetése/////////////////////////////////////////////////

										case 'C':
										adcchannel = get2hex();					//--- ADC csatorna kiválasztása

										#define ADC_REF_VDD_VDD_X 0b11110011 	//--- MC18 fordítóban állítólag rosszul van definiálva ADC voltage source VREF+ = AVDD

										TRISCbits.TRISC3 = 1;					//--- Bemenetre állítás analóg lábak miatt
										TRISCbits.TRISC2 = 1;
										TRISCbits.TRISC6 = 1;
										TRISCbits.TRISC7 = 1;	

										ANSEL = 0b11000000;						//--- Lábakat analógként használom használata 
										ANSELH = 0b00000011; 

										ADCON0=(0b00111101 & adcchannel);   	//--- ADC port channel X (ANX), és ADC engedélyezése
 										ADCON1=0b00000000;   					//--- Belsõ referencia használata (Vdd and Vss)
  										ADCON2=0b00101110;   					//--- Balra igazítás, 12 TAD, 48MHz használata

										//--- Ez valamiért nem mûködik egyenlõre OpenADC( ADC_FOSC_64 & ADC_LEFT_JUST & ADC_20_TAD, ADC_CH7 & ADC_INT_OFF, ADC_REF_VDD_VDD_X & ADC_REF_VDD_VSS , ADC_7ANA );

										ConvertADC();
 										while( BusyADC() );
										meres = ReadADC();						//--- Eredmény eltárolása
									
										outdec(meres,0);						//--- Változó kiküldése, tizedesjegyek száma 0
										
 		      
       									break;
		

///////////////////////////////////////INPUT mûködtetése////////////////////////////////////////////////

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
										outdec(inputvalue,0);					//--- Változó kiküldése, tizedesjegyek száma 0

										IdleI2C();
										StopI2C();

  		      
        								break;

////////////////////////////////////////PWM mûködtetése/////////////////////////////////////////////////

										case 'E':
										
										TRISCbits.TRISC5 = 0;					//--- Láb kimenetként használata
										LATCbits.LATC5 = 0;						

										OpenTimer2(T2_PS_1_4 & TIMER_INT_OFF); 	//--- Timer2 használata a pwm-hez. Ezt bekell állítani a PWM mûködtetése elõtt. 4-es elõosztó, interrupt kikapcsolva.

										periodus = get2hex();					//--- Perl-ben kiszámolt periódus érték beolvasása
										dutycycle = get4hex();					//--- Perl-ben kiszámolt kitöltési tényezõ beolvasása

										OpenPWM1(periodus);                     //--- Példa: PWM period =[(period ) + 1] x 4 x TOSC x TMR2 prescaler. The value of period is from 0x00 to 0xff. Channel 1:PWM period = 1/((199+1)*4*(1/48e6)*1) = 60 kHz
										SetDCPWM1(dutycycle);                   //--- Példa: PWM x Duty cycle = (DCx<9:0>) x TOSC x TMR2 prescaler. Setting duty cycle for channel 1: DC = 1/(300 * (1/48e6)*1) = 160 KHz
										
 		      
       									break;

////////////////////////////////////////CC mûködtetése/////////////////////////////////////////////////

										case 'F':
										

      									SetTmrCCPSrc(T1_SOURCE_CCP);                            //--- Timer1 lesz a Capture-re kötve
										PIR1bits.CCP1IF = 0;
										TMR1H = 0;
										TMR1L = 0;
										CCPR1L = 0;
										CCPR1H = 0;												//--- Capture konfigurálása
      									configccp = (CAP_EVERY_RISE_EDGE & CAPTURE_INT_OFF) ;       
     									OpenCapture1(configccp);								//--- OpenCapture1 függvény hívása
      									OpenTimer1(0);                                  		//--- Timer1 indítása

      									while(!PIR1bits.CCP1IF);  								//--- Várakozás amíg capture nem történik                             
										PIR1bits.CCP1IF = 0;
																							
      									INCAPresult1 = ReadCapture1();                          //--- Eredmény kiolvasása a CCPL és CCPH ból

										while(!PIR1bits.CCP1IF);  								//--- Várakozás amíg capture nem történik                             
										PIR1bits.CCP1IF = 0;
																							
      									INCAPresult2 = ReadCapture1();

										frekvencia = (INCAPresult2 - INCAPresult1);

      									CloseCapture1();										//--- Modul leállítása
										CloseTimer1();
										

								


		      							SetTmrCCPSrc(T1_SOURCE_CCP);                            //--- Timer1 lesz a Capture-re kötve
										PIR1bits.CCP1IF = 0;
										TMR1H = 0;
										TMR1L = 0;
										CCPR1L = 0;
										CCPR1H = 0;												//--- Capture konfigurálása
      									configccp = (CAP_EVERY_RISE_EDGE & CAPTURE_INT_OFF) ;       
     									OpenCapture1(configccp);								//--- OpenCapture1 függvény hívása
      									OpenTimer1(0);                                  		//--- Timer1 indítása

      									while(!PIR1bits.CCP1IF);  								//--- Várakozás amíg capture nem történik                             
										PIR1bits.CCP1IF = 0;
																							
      									INCAPresult1 = ReadCapture1();                          //--- Eredmény kiolvasása a CCPL és CCPH ból

										configccp = (CAP_EVERY_FALL_EDGE & CAPTURE_INT_OFF) ;       
     									OpenCapture1(configccp);

										while(!PIR1bits.CCP1IF);  								//--- Várakozás amíg capture nem történik                             
										PIR1bits.CCP1IF = 0;
																							
      									INCAPresult2 = ReadCapture1();

										alacsony_szint = (INCAPresult2 - INCAPresult1);

										outdec(frekvencia,0);
										
										delay_ms(10);

										outdec(alacsony_szint,0);

      									CloseCapture1();										//--- Modul leállítása
										CloseTimer1();
       									break;

//////////////////////////////////////////Ismeretlen parancs/////////////////////////////////////////////////
//
        								default:
            							usb_cdc_putc('?');          		//Ismeretlen parancs
            							break;
        								} 									//switch vége
    					} 													//while
				}     														//main

///******** Fõprogram vége *********************************************/


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