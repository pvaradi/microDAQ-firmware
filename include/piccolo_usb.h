/* PICCOLO project
 * Copyright (c) 2009-2010 Istvan Cserny (cserny@atomki.hu)
 *
 */

/** \file
 *  Ez az �llom�ny deklar�lja azokat a f�ggv�nyeket, amelyekkel az
 *  alkalmaz�i programb�l az USB kapcsolat kezelhetj�k. Ezt az �llom�nyt
 *  a programjainkba k�zvetlen�l nem kell becsatolnunk. Ha defini�ltuk
 *  az USE_USB szimb�lumot, akkor a \ref piccolo_all.h �llom�nyban elhelyezett
 *  felt�teles ford�t�si parancsok gondoskodnak az automatikus becsatol�s�r�l.
 *
 *  \see piccolo_usb.c a tov�bbi r�szletek�rt.
 */

#ifndef PICCOLO_USB
#define PICCOLO_USB
	#include "GenericTypeDefs.h"
	#include "Compiler.h"
	#include "usb_config.h"
	#include "./USB/usb_device.h"
	#include "./USB/usb.h"
	#include "./USB/usb_function_cdc.h"
	
	/*  V�LTOZ�K *********************************************************/
	#pragma udata
	extern char USB_In_Buffer[64];         // Az USB bemeneti buffere (ebbe �runk...)
	extern char USB_Out_Buffer[64];        // Az USB kimeneti buffere (ebb�l olvasunk...)
	extern BYTE numBytesRead;              // A vett karakterek sz�ma
	extern BYTE numBytesToSend;            // Az elk�ldend� karakterek sz�ma
	extern BYTE Buffercp;                  // Mutat� a buffer kiolvas�s�hoz
	extern BYTE BlinkUSBStatus_enabled;    // USB st�tusz kijelz�s: 1=enged, 0=tilt
	
	/* F�GGV�NY PROTOT�PUSOK *********************************************/
	void InitializeSystem(void);           // Rendszer inicializ�l�sa (USB csatlakoztat�sa)
	void ProcessIO(void);                  // USB karakterbufferek kezel�se
	void USBDeviceTasks(void);             // USB kezel�se (interrupt vagy polling)
	void BlinkUSBStatus(void);             // USB st�tusz kijelz�se (LED1 & LED2)
	unsigned int ReadPOT(void);            // Anal�g jel m�r�se (potm�ter)
	
	/** USB I/O FUNCTIONS ************************************************/ 
	int _user_putc(char c);                // Egy karakter ki�r�sa
	#define usb_cdc_putc(c) _user_putc(c)  // Alternat�v elnevez�s a _user_putc() f�ggv�nyhez
	void outString(const rom char* psz_s); // Sz�veg ki�rat�sa
	char usb_cdc_getc(void);               // Egy karakter beolvas�sa  
	void outdec(long data, unsigned int ndigits); // Decim�lis ki�rat�s adott sz�m� tizedesjegyre 
	void out4hex(unsigned int t);          // Egy 16 bites sz�m ki�r�sa hexadecim�lisan
	unsigned int get4hex(void);	           // N�gyjegy� hexadecim�lis sz�m beolvas�sa
	unsigned char get2hex(void);	       // K�tjegy� hexadecim�lis sz�m beolvas�sa
	void delay_ms(unsigned int d);         // K�sleltet� elj�r�s, ami ProcessIO()-t is rendszeresen megh�vja 
	#define usb_cdc_kbhit() (Buffercp<numBytesRead) // Bemeneti buffer ellen�rz�se
#endif