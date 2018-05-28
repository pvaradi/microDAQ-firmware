/* PICCOLO project
 * Copyright (c) 2009-2010 Istvan Cserny (cserny@atomki.hu)
 *
 */

#ifndef PICCOLO_14K50_H
#define PICCOLO_14K50_H
/** \file
 *  Ebben a header �llom�nyban a PIC18F14K50 alap� PICCOLO k�s�rleti �ramk�r
 *  hardver saj�toss�gait defini�ljuk. A konfigur�ci�s bitek be�ll�t�saira
 *  csak abban az esetben van t�nyleges sz�ks�g�nk, ha nem haszn�lunk bootloadert.
 *  Bootloaderes bet�lt�sn�l ugyanis a konfigur�ci�s biteket nem �rjuk fel�l, a
 *  bootloaderrel egy�tt be�getett konfigur�ci�s be�ll�t�s marad �rv�nyben.
 */  

    /*****************************************************************/
    /*** HARDVER be�ll�t�si opci�k PIC18F14K50 minta�ramk�r�h�z ******/
    /*****************************************************************/

    /*** KONFIGUR�CI�S BITEK *********************************/
	#if defined(PICCOLO_ALL)	            // Csak a f�programhoz csatolhatjuk!
		#pragma config CPUDIV   = NOCLKDIV  // Nem osztjuk le az �rajelet (48 MHz)  
		#pragma config USBDIV   = OFF       // USB-hez nem osztjuk le az �rajelet
		#pragma config FOSC     = HS        // HS oszcill�tor (12 MHz)
		#pragma config PLLEN    = ON        // 4x PLL enged�lyez�s (4x12 = 48 MHz)
		#pragma config PCLKEN   = ON        // Els�dleges �rajelgener�tor enged�lyezve 
		#pragma config FCMEN    = OFF       // Fail Safe Clock Monitor tilt�sa
		#pragma config IESO     = OFF       // Bels�/k�ls� oszcill�tor �tkapcsol�s tilt�sa
		#pragma config PWRTEN   = ON        // Bekapcsol�skor 62 ms v�rakoz�s
		#pragma config BOREN    = SBORDIS   // Brown-out Reset hardveresen enged�lyezve
		#pragma config BORV     = 27        // Reset szint = 2,7 V
		#pragma config WDTEN    = OFF       // Watchdog timer letiltva
		#pragma config WDTPS    = 256       // Watchdog ut�sz�ml�l� be�ll�t�sa
		#pragma config MCLRE    = OFF       // MCLR (reset) bemenet letiltva, RA3 bemenet
		#pragma config HFOFST   = ON        // Rendszer �ra nem v�r HFINTOSC-ra
		#pragma config STVREN   = ON        // A verem t�lcsordul�sa RESET-et okoz
		#pragma config LVP      = OFF       // Alacsonyfesz�lts�g� programoz�s letiltva
		#pragma config XINST    = OFF       // Extended Instruction Set
		#if defined(HID_BOOTLOADER)
		   #pragma config BBSIZ    = ON     // 4 kB boot blokk m�ret (HID bootloaderhez)
		#else
		   #pragma config BBSIZ    = OFF    // 2 kB boot blokk m�ret (MCHPUSB bootloaderhez)
		#endif
		#pragma config CP0      = OFF       // K�dv�delem kikapcsolva
		#pragma config CP1      = OFF
		#pragma config CPB      = ON        // Boot blokk k�dv�delem bekapcsolva 
		#pragma config WRT0     = OFF       // �r�sv�delem kikapcsolva
		#pragma config WRT1     = OFF
		#pragma config WRTB     = ON        // Boot Blokk �r�sv�delem bekapcsolva
		#pragma config WRTC     = OFF       // Konfigur�ci�s blokk �rhat�
		#pragma config EBTR0    = OFF       // T�bl�zatolvas�s ne legyen letiltva
		#pragma config EBTR1    = OFF
		#pragma config EBTRB    = OFF
	#endif

    /*** USB �rz�kel�k konfigur�l�sa *********************************/
    //  A PICCOLO projekt minta�ramk�rei nem t�mogatj�k az 
    //  USE_SELF_POWER_SENSE_IO tulajdons�got, ez�rt az al�bbi
    //  sort hagyja meg kommentnek!
    //--- #define USE_SELF_POWER_SENSE_IO

    #define tris_self_power     TRISAbits.TRISA2    // Input
    #if defined(USE_SELF_POWER_SENSE_IO)
       #define self_power          PORTAbits.RA2
    #else
       #define self_power          1
    #endif

    //  A PICCOLO projekt minta�ramk�rei nem t�mogatj�k az 
    //  USE_USB_BUS_SENSE_IO tulajdons�got, ez�rt az al�bbi
    //  sort hagyja meg kommentnek!
    //--- #define USE_USB_BUS_SENSE_IO
    #define tris_usb_bus_sense  TRISAbits.TRISA1    // Input
    #if defined(USE_USB_BUS_SENSE_IO)
       #define USB_BUS_SENSE       PORTAbits.RA1
    #else
       #define USB_BUS_SENSE       1
    #endif

	
    /*******************************************************************/
    /******** �ramk�ri hardver elemek defini�l�sa **********************/
    /*******************************************************************/
    #define CLOCK_FREQ 48000000
    #define LEDport LATC
    #define LEDtris TRISC   

    /** LED-ek  ********************************************************/
    #define mInitAllLEDs()      LATC &= 0xF0; TRISC &= 0xF0;
    #define mLED_1              LATCbits.LATC0
    #define mLED_2              LATCbits.LATC1
    #define mLED_3              LATCbits.LATC2
    #define mLED_4              LATCbits.LATC3
    
    #define mGetLED_1()         mLED_1
    #define mGetLED_2()         mLED_2
    #define mGetLED_3()         mLED_3
    #define mGetLED_4()         mLED_4

    #define mLED_1_On()         mLED_1 = 1;
    #define mLED_2_On()         mLED_2 = 1;
    #define mLED_3_On()         mLED_3 = 1;
    #define mLED_4_On()         mLED_4 = 1;
    
    #define mLED_1_Off()        mLED_1 = 0;
    #define mLED_2_Off()        mLED_2 = 0;
    #define mLED_3_Off()        mLED_3 = 0;
    #define mLED_4_Off()        mLED_4 = 0;
    
    #define mLED_1_Toggle()     mLED_1 = !mLED_1;
    #define mLED_2_Toggle()     mLED_2 = !mLED_2;
    #define mLED_3_Toggle()     mLED_3 = !mLED_3;
    #define mLED_4_Toggle()     mLED_4 = !mLED_4;
    
    /** SWITCH *********************************************************/
    #define mInitSwitch1()      // TRISAbits.TRISA3=1;
    #define SW1                 PORTAbits.RA3
    #define sw2                 PORTAbits.RA3  //Just for compatibility     

    /** I/O pin definitions ********************************************/
    #define INPUT_PIN 1
    #define OUTPUT_PIN 0


	/** BELS� FELH�Z�SOK ENGED�LYEZ�SE/TILT�SA *********************/	
	#define ENABLE_RA3_PULLUP()   WPUAbits.WPUA3 = 1
	#define DISABLE_RA3_PULLUP()  WPUAbits.WPUA3 = 0 
	#define ENABLE_RA4_PULLUP()   WPUAbits.WPUA4 = 1
	#define DISABLE_RA4_PULLUP()  WPUAbits.WPUA4 = 0
	#define ENABLE_RA5_PULLUP()   WPUAbits.WPUA5 = 1
	#define DISABLE_RA5_PULLUP()  WPUAbits.WPUA5 = 0
	#define ENABLE_RB4_PULLUP()   WPUBbits.WPUB4 = 1
	#define DISABLE_RB4_PULLUP()  WPUBbits.WPUB4 = 0
	#define ENABLE_RB5_PULLUP()   WPUBbits.WPUB5 = 1
	#define DISABLE_RB5_PULLUP()  WPUBbits.WPUB5 = 0
	#define ENABLE_RB6_PULLUP()   WPUBbits.WPUB6 = 1
	#define DISABLE_RB6_PULLUP()  WPUBbits.WPUB6 = 0
	#define ENABLE_RB7_PULLUP()   WPUBbits.WPUB7 = 1
	#define DISABLE_RB7_PULLUP()  WPUBbits.WPUB7 = 0

	/** V�LTOZ�SJELZ� INTERRUPTOK ENGED�LYEZ�SE/TILT�SA ************/
	#define ENABLE_RA0_CN_INTERRUPT()    IOCAbits.IOCA0 = 1
	#define DISABLE_RA0_CN_INTERRUPT()   IOCAbits.IOCA0 = 0
	#define ENABLE_RA1_CN_INTERRUPT()    IOCAbits.IOCA1 = 1
	#define DISABLE_RA1_CN_INTERRUPT()   IOCAbits.IOCA1 = 0
	#define ENABLE_RA3_CN_INTERRUPT()    IOCAbits.IOCA3 = 1
	#define DISABLE_RA3_CN_INTERRUPT()   IOCAbits.IOCA3 = 0
	#define ENABLE_RA4_CN_INTERRUPT()    IOCAbits.IOCA4 = 1
	#define DISABLE_RA4_CN_INTERRUPT()   IOCAbits.IOCA4 = 0
	#define ENABLE_RA5_CN_INTERRUPT()    IOCAbits.IOCA5 = 1
	#define DISABLE_RA5_CN_INTERRUPT()   IOCAbits.IOCA5 = 0
	#define ENABLE_RB4_CN_INTERRUPT()    IOCBbits.IOCB4 = 1
	#define DISABLE_RB4_CN_INTERRUPT()   IOCBbits.IOCB4 = 0
	#define ENABLE_RB5_CN_INTERRUPT()    IOCBbits.IOCB5 = 1
	#define DISABLE_RB5_CN_INTERRUPT()   IOCBbits.IOCB5 = 0
	#define ENABLE_RB6_CN_INTERRUPT()    IOCBbits.IOCB6 = 1
	#define DISABLE_RB6_CN_INTERRUPT()   IOCBbits.IOCB6 = 0
	#define ENABLE_RB7_CN_INTERRUPT()    IOCBbits.IOCB7 = 1
	#define DISABLE_RB7_CN_INTERRUPT()   IOCBbits.IOCB7 = 0

	/** ANAL�G BEMENETEK ENGED�LYEZ�SE/TILT�SA *********************/
        #define DISABLE_ALL_ANALOG()    {ANSEL=0; ANSELH=0x00; }
        #define ENABLE_ONE_ANALOG()     {ANSEL=0; ANSELH=0x04; } 
        #define ENABLE_TWO_ANALOG()     {ANSEL=0; ANSELH=0x0C; } 
	#define DISABLE_AN3_ANALOG() 	ANSELbits.ANS3=0
	#define ENABLE_AN3_ANALOG()     ANSELbits.ANS3=1
	#define DISABLE_AN4_ANALOG() 	ANSELbits.ANS4=0
	#define ENABLE_AN4_ANALOG()     ANSELbits.ANS4=1
	#define DISABLE_AN5_ANALOG() 	ANSELbits.ANS5=0
	#define ENABLE_AN5_ANALOG()     ANSELbits.ANS5=1
	#define DISABLE_AN6_ANALOG() 	ANSELbits.ANS6=0
	#define ENABLE_AN6_ANALOG()     ANSELbits.ANS6=1
	#define DISABLE_AN7_ANALOG() 	ANSELbits.ANS7=0
	#define ENABLE_AN7_ANALOG()     ANSELbits.ANS7=1
	#define DISABLE_AN8_ANALOG() 	ANSELHbits.ANS8=0
	#define ENABLE_AN8_ANALOG()     ANSELHbits.ANS8=1
	#define DISABLE_AN9_ANALOG() 	ANSELHbits.ANS9=0
	#define ENABLE_AN9_ANALOG()     ANSELHbits.ANS9=1
	#define DISABLE_AN10_ANALOG() 	ANSELHbits.ANS10=0
	#define ENABLE_AN10_ANALOG()    ANSELHbits.ANS10=1
	#define DISABLE_AN11_ANALOG() 	ANSELHbits.ANS11=0
	#define ENABLE_AN11_ANALOG()    ANSELHbits.ANS11=1

	/** ANAL�G BEMENETEK KONFIGUR�L�SA *****************************/
	#define CONFIG_AN3_AS_ANALOG()	{TRISAbits.TRISA4=1; ANSELbits.ANS3=1; }
	#define CONFIG_AN4_AS_ANALOG()	{TRISCbits.TRISC0=1; ANSELbits.ANS4=1; }
	#define CONFIG_AN5_AS_ANALOG()	{TRISCbits.TRISC1=1; ANSELbits.ANS5=1; }
	#define CONFIG_AN6_AS_ANALOG()	{TRISCbits.TRISC2=1; ANSELbits.ANS6=1; }
	#define CONFIG_AN7_AS_ANALOG()	{TRISCbits.TRISC3=1; ANSELbits.ANS7=1; }
	#define CONFIG_AN8_AS_ANALOG()	{TRISCbits.TRISC6=1; ANSELHbits.ANS8=1; }
	#define CONFIG_AN9_AS_ANALOG()	{TRISCbits.TRISC7=1; ANSELHbits.ANS9=1; }
	#define CONFIG_AN10_AS_ANALOG()	{TRISBbits.TRISB4=1; ANSELHbits.ANS10=1; }
	#define CONFIG_AN11_AS_ANALOG()	{TRISBbits.TRISB5=1; ANSELHbits.ANS11=1; }

	/** DIGIT�LIS KI/BEMENETEK KONFIGUR�L�SA ***********************/
	#define CONFIG_RA4_AS_DIGITAL_INPUT()  {ANSELbits.ANS3=0; TRISAbits.TRISA4=1; }
	#define CONFIG_RA4_AS_DIGITAL_OUTPUT() {ANSELbits.ANS3=0; TRISAbits.TRISA4=0; }
	#define CONFIG_RA5_AS_DIGITAL_INPUT()   TRISAbits.TRISA5=1
	#define CONFIG_RA5_AS_DIGITAL_OUTPUT()  TRISAbits.TRISA5=0
	#define CONFIG_RB4_AS_DIGITAL_INPUT()  {ANSELHbits.ANS10=0; TRISBbits.TRISB4=1; }
	#define CONFIG_RB4_AS_DIGITAL_OUTPUT() {ANSELHbits.ANS10=0; TRISBbits.TRISB4=0; }
	#define CONFIG_RB5_AS_DIGITAL_INPUT()  {ANSELHbits.ANS11=0; TRISBbits.TRISB5=1; }
	#define CONFIG_RB5_AS_DIGITAL_OUTPUT() {ANSELHbits.ANS11=0; TRISBbits.TRISB5=0; }
	#define CONFIG_RB6_AS_DIGITAL_INPUT()   TRISBbits.TRISB6=1 
	#define CONFIG_RB6_AS_DIGITAL_OUTPUT()  TRISBbits.TRISB6=0 
	#define CONFIG_RB7_AS_DIGITAL_INPUT()   TRISBbits.TRISB7=1 
	#define CONFIG_RB7_AS_DIGITAL_OUTPUT()  TRISBbits.TRISB7=0 
	#define CONFIG_RC0_AS_DIGITAL_INPUT()  {ANSELbits.ANS4=0; TRISCbits.TRISC0=1; }
	#define CONFIG_RC0_AS_DIGITAL_OUTPUT() {ANSELbits.ANS4=0; TRISCbits.TRISC0=0; }
	#define CONFIG_RC1_AS_DIGITAL_INPUT()  {ANSELbits.ANS5=0; TRISCbits.TRISC1=1; }
	#define CONFIG_RC1_AS_DIGITAL_OUTPUT() {ANSELbits.ANS5=0; TRISCbits.TRISC1=0; }
	#define CONFIG_RC2_AS_DIGITAL_INPUT()  {ANSELbits.ANS6=0; TRISCbits.TRISC2=1; }
	#define CONFIG_RC2_AS_DIGITAL_OUTPUT() {ANSELbits.ANS6=0; TRISCbits.TRISC2=0; }
	#define CONFIG_RC3_AS_DIGITAL_INPUT()  {ANSELbits.ANS7=0; TRISCbits.TRISC3=1; }
	#define CONFIG_RC3_AS_DIGITAL_OUTPUT() {ANSELbits.ANS7=0; TRISCbits.TRISC3=0; }
	#define CONFIG_RC4_AS_DIGITAL_INPUT()   TRISCbits.TRISC4=1
	#define CONFIG_RC4_AS_DIGITAL_OUTPUT()  TRISCbits.TRISC4=0
	#define CONFIG_RC5_AS_DIGITAL_INPUT()   TRISCbits.TRISC5=1
	#define CONFIG_RC5_AS_DIGITAL_OUTPUT()  TRISCbits.TRISC5=0
	#define CONFIG_RC6_AS_DIGITAL_INPUT()  {ANSELHbits.ANS8=0; TRISCbits.TRISC6=1; }
	#define CONFIG_RC6_AS_DIGITAL_OUTPUT() {ANSELHbits.ANS8=0; TRISCbits.TRISC6=0; }
	#define CONFIG_RC7_AS_DIGITAL_INPUT()  {ANSELHbits.ANS9=0; TRISCbits.TRISC7=1; }
	#define CONFIG_RC7_AS_DIGITAL_OUTPUT() {ANSELHbits.ANS9=0; TRISCbits.TRISC7=0; }

    /** Analog input ***************************************************/
    #define mInitPOT()          {TRISBbits.TRISB4=1;\
                                 ADCON0=0x29; ADCON1=0;\
                                 ADCON2=0x3E; ADCON2bits.ADFM = 1;}

	/** Fix fesz�lts�g� referencia (FVR) *******************************/ 
	//  REFCON0 regiszter vez�rli
	#define FVR_ENABLE   	0b10000000 	// FVR modul enged�lyezve
	#define FVR_DISABLE  	0b00000000	// FVR modul letiltva
	#define FVREN_MASK		0b01111111	// FVREN maszk
	#define FVR1S_1024		0b00010000	// FVR1S = 1,024 V
	#define FVR1S_2048		0b00100000	// FVR1S = 2,048 V
	#define FVR1S_4096		0b00110000	// FVR1S = 4,096 V
	#define FVR1S_MASK		0b11001111	// FVR1S maszk
	#define FVR_STATUS		REFCON0bits.FVR1ST		//0: ha FVR nem stabil

	/** V�ltoztathat� fesz�lts�g� referencia (DAC) *********************/ 
	//  REFCON1 �s REFCON2 regiszter vez�rli
	#define DAC_ENABLE		0b10000000  // A programozhat� fesz�lts�greferencia enged�lyez�se
	#define DAC_DISABLE		0b00000000  // A programozhat� fesz�lts�greferencia letilt�sa
	#define D1EN_MASK		0b01111111  // A DAC enged�lyez� bit maszkja
	#define D1LPS_HI		0b01000000	// energiatakar�kos m�dban a kimenet felh�z�sa
	#define D1LPS_LO		0b00000000	// energiatakar�kos m�dban a kimenet lelh�z�sa 
	#define D1LPS_MASK		0b10111111  // A D1LPS bit maszkja
	#define DACOUT_ENABLE	0b00100000	// A DAC kimenet kijut az RC2 l�bra is
	#define DACOUT_DISABLE  0b00000000	// A DAC kimenet csak bel�l �rhet� el
	#define DAC1OE_MASK		0b11011111	// A DAC kimenet enged�lyez� bitj�nek maszkja  
	#define D1PSS_VDD		0b00000000	// VDD a pozit�v referencia
	#define D1PSS_VREF		0b00000100	// RC0/Vref+ a pozit�v referencia	
	#define D1PSS_FVR		0b00001000	// FVR a pozit�v referencia	
	#define D1PSS_MASK		0b11110011	// A pozit�v referencia vez�rl�bitjeinek maszkja
	#define D1NSS_VSS		0b00000000	// VSS a negat�v referencia
	#define D1NSS_VREF		0b00000001	// RC1/Vref- a negat�v referencia	
	#define D1NSS_MASK		0b11111110	// A negat�v referencia vez�rl�bitj�nek maszkja

    /* Az OpenADC() f�ggv�ny param�terez�s�hez haszn�lhat� defin�ci�k
     * Az al�bbi p�ld�ban az AN10 �s AN11 csatorn�kat enged�lyezz�k:  
     *           #include "adc.h"
     *           #define USE_OR_MASKS
     *           OpenADC(ADC_FOSC_64 | ADC_RIGHT_JUST | ADC_20_TAD,
     *                   ADC_CH10 | ADC_INT_OFF,
     *                   ADC_REF_VDD_VDD | ADC_REF_VDD_VSS,
     *                   ENABLE_AN10 | ENABLE_AN11);  
     */
     #define ENABLE_AN3  0x0008
     #define ENABLE_AN4  0x0010
     #define ENABLE_AN5  0x0020
     #define ENABLE_AN6  0x0040
     #define ENABLE_AN7  0x0080
     #define ENABLE_AN8  0x0100
     #define ENABLE_AN9  0x0200
     #define ENABLE_AN10 0x0400
     #define ENABLE_AN11 0x0800

#endif  
/** EOF PICCOLO-14K50.H ************************************************/
