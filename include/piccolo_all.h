/* PICCOLO project
 * Copyright (c) 2009-2010 Istvan Cserny (cserny@atomki.hu)
 *
 */

/** \file
 *  Ez a header �llom�ny gondoskodik a RESET �s az interupt vektorok
 *  �thelyez�s�r�l, s a PIC18F14K50 �s a PIC18F4550 k�s�rleti �ramk�r�k k�z�tti
 *  trivi�lis elt�r�sek felold�s�r�l. A \ref piccolo_config.h tartalm�t�l, vagy
 *  a ford�t�nak k�zvetlen�l �tadott makrodefin�ci�kt�l f�gg�en tov�bbi �llom�nyok
 *  becsatol�s�ra is sor ker�lhet, illetve felt�teles ford�t�s seg�ts�g�vel 
 *  alternat�v k�dr�szletek ker�lnek bele a leford�tott projektbe.
 *
 *  Figyelem! ez a header �llom�ny csak a f�programba csatolhat� be!
 *
 *  A legfontosabb konfigur�ci�s param�terek:
 *  - <b>__18F14K50:</b> Azt jelzi, hogy a Configure/Select Device men�ben PIC18F14K50 mikrovez�rl�t 
 *    v�lasztottuk. Ezt a szimb�lumot a ford�t� automatikusan defini�lja. 
 *
 *  - <b>__18F4550:</b>  Azt jelzi, hogy a Configure/Select Device men�ben PIC18F4550 mikrovez�rl�t 
 *    v�lasztottuk. Ezt a szimb�lumot a ford�t� automatikusan defini�lja. 
 *
 *  - <b>USE_USB:</b> Akkor defini�ljuk, ha haszn�lni k�v�njuk az USB kapcsolatot (\ref piccolo_config.h 
 *    �llom�nyban vagy az MPLAB IDE Project/Build Options/Project men�j�ben defini�lhatjuk)
 *
 *  - <b>USE_INTERRUPT:</b> Akkor defini�ljuk, ha haszn�lni k�v�njuk a programmegszak�t�st 
 *    (vagy a \ref piccolo_config.h �llom�nyban, vagy az MPLAB IDE Project/Build Options/Project 
 *    men�ben defini�lhatjuk). <b>Figyelem! Programmegszak�t�s eset�n gondoskodnunk kell a magas �s az 
 *    alacsony priorit�s� interruptok kiszolg�l�s�t ell�t� hi_isr() �s lo_isr() elj�r�sokr�l!</b>  
 *    Ha az USE_USB �s az USE_INTERRUPT szimb�lumok defini�lva vannak, akkor automatikusan az
 *    USB_INTERRUPT szimb�lum is defini�l�sra ker�l, ellenkez� esetben pedig az USB_POLLING 
 *    ker�l defini�l�sra. A \ref hello-int.c mintaprogramban l�that�, hogy az USB interruptos 
 *    kiszolg�l�sa hogyan v�gezhet�.
 *
 *  - <b>Blinking_at_start:</b> azt enged�lyezhetj�k vele, hogy LED1 �s LED2 kijelezze az USB kapcsolat 
 *    �llapot�t. A lehets�ges �rt�kek: 0 = tilt�s, 1 = enged�lyez�s. 
 * 
 *  - <b>HID_BOOTLOADER:</b> Akkor defini�ljuk, ha HID bootloadert haszn�lunk (\ref piccolo_config.h  
 *    �llom�nyban vagy az MPLAB IDE Project/Build Options/Project men�pontj�ban defini�lhatjuk). 
 *    Ezzel szinkronban a linker programnak is �t kell adni a HID_BOOTLOADER szimbolumot 
 *    (parancssori /uHID_BOOTLOADER opci� megad�sa a Project/Build Options/Project men�ben, MPLINK be�ll�t�sai lapj�n).
 *   
 *  - <b>MCHPUSB_BOOTLOADER:</b> Akkor defini�ljuk, ha az MCHPUSB bootloadert haszn�lunk (\ref piccolo_config.h  
 *    �llom�nyban vagy a Project/Build Options/Project men�ben defini�lhatjuk). Ezzel szinkronban a
 *    Linker programnak is �t kell adni a MCHPUSB_BOOTLOADER szimbolumot (parancssori /u opci� megad�sa
 *    a Project/Build Options/Project men�ben, MPLINK be�ll�t�sai lapj�n). 
 * 
 *    \see piccolo_config.h projekt szint� konfigur�ci�s be�ll�t�sok
 *    \see piccolo_usb.h az USB kapcsolat alkalmaz�sszint� kezel�s�hez sz�ks�ges f�ggv�nyek �s v�ltoz�k
 *    \see piccolo-14k50.h a PIC18F14K50 minta�ramk�r hardverspecifikus makr�defin�ci�i
 *    \see piccolo-4550.h a PIC18F4550 minta�ramk�r hardverspecifikus makr�defin�ci�i
 */

#ifndef PICCOLO_ALL
#define PICCOLO_ALL 

/*  INCLUDE �llom�nyok becsatol�sa ***********************************/
#include "piccolo_config.h" 	//Implicit m�don becsatolja p18cxxx.h-t is!

    #warning *** P R O J E K T   K O N F I G U R � C I � ***
#if defined(USE_USB)
    #include "piccolo_usb.h" 
    #warning * USE__USB        : DEFINI�LT                 *
#else
    #warning * USE__USB        : NEM DEFINI�LT             * 
#endif
#if defined(USE_INTERRUPT)
    #warning * USE__INTERRUPT  : DEFINI�LT                 *
#else
    #warning * USE__INTERRUPT  : NEM DEFINI�LT             * 
#endif
#if defined(HID_BOOTLOADER)
    #warning * HID__BOOTLOADER : DEFINI�LT                 *
#else
    #warning * HID__BOOTLOADER : NEM DEFINI�LT             * 
#endif
#if defined(MCHPUSB_BOOTLOADER)
    #warning * MCHPUSB__BOOTLOADER : DEFINI�LT             *
#endif

    #warning *********************************************** 

/** Inline f�ggv�ny, amely egy regiszter valamelyik bitcsoportj�t
 * atomi m�velettel m�dos�tja. 
 * \param reg a m�dos�tand� regiszter neve
 * \param val a be�rand� �rt�k (helyi�rt�ken helyesen!)
 * \param mask a m�dos�tand� bitcsoportot kijel�l� maszk
 */
#define ChangeBits(reg,val,mask) reg ^= ((reg^val) & mask)	///<Bitcsoport �t�r�sa atomi m�velettel


/*  Vektorok �thelyez�se  ********************************************/
    void hi_isr(void);     ///< Magas priorit�s� programmegszak�t�s kiszolg�l�sa
    void lo_isr(void);     ///< Alacsony priorit�s� programmegszak�t�s kiszolg�l�sa

#if defined(HID_BOOTLOADER)
	#define RESET_VECTOR			0x1000
	#define HIGH_INTERRUPT_VECTOR	0x1008
	#define LOW_INTERRUPT_VECTOR	0x1018
#elif defined(MCHPUSB_BOOTLOADER)	
	#define RESET_VECTOR			0x800
	#define HIGH_INTERRUPT_VECTOR	0x808
	#define LOW_INTERRUPT_VECTOR	0x818
#else	
	#define RESET_VECTOR			0x00
	#define HIGH_INTERRUPT_VECTOR	0x08
	#define LOW_INTERRUPT_VECTOR	0x18
#endif

/*  RESET VEKTOR ****************************************************/
// Ha nem haszn�lunk bootloadert, akkor a C18 startup k�dja automatikuan l�trehozza!
// Ez�rt az al�bbi sorok csak bootloader haszn�lata eset�n kellenek.
#if defined(HID_BOOTLOADER)||defined(MCHPUSB_BOOTLOADER)		
	extern void _startup (void);
/** Az �thelyzett RESET vektor.  
 *  A bootloader haszn�latt�l f�gg� abszol�t c�mre ker�l.
 */
	#pragma code REMAPPED_RESET_VECTOR = RESET_VECTOR
	void _reset (void) 
	{
	    _asm goto _startup _endasm
	}
#endif
	
/** Az �thelyzett magas priorit�s� interrupt vektor.  
 *  A bootloader haszn�latt�l f�gg� abszol�t c�mre ker�l.
 */
#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = HIGH_INTERRUPT_VECTOR
void Remapped_High_ISR (void)
{
	 _asm goto hi_isr _endasm
}

/** Az �thelyzett alacsony priorit�s� interrupt vektor.  
 *  A bootloader haszn�latt�l f�gg� abszol�t c�mre ker�l.
 */
#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = LOW_INTERRUPT_VECTOR
void Remapped_Low_ISR (void)
{
	 _asm goto lo_isr _endasm 
}	

#pragma code
//�res elj�r�sokat gy�rtunk az elszabadult interruptok megf�kez�s�re,
//ha a felhaszn�l� nem akar programmegszak�t�st haszn�lni!
#ifndef USE_INTERRUPT
	void hi_isr()
	{
	}	
	
	void lo_isr()
	{	
	}
#endif //USE_INTERRUPT

#endif //PICCOLO_ALL