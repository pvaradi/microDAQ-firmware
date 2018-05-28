/* PICCOLO project
 * Copyright (c) 2009-2010 Istvan Cserny (cserny@atomki.hu)
 *
 */

/** \file
 *  Ez az állomány deklarálja azokat a függvényeket, amelyekkel az
 *  alkalmazói programból az USB kapcsolat kezelhetjük. Ezt az állományt
 *  a programjainkba közvetlenül nem kell becsatolnunk. Ha definiáltuk
 *  az USE_USB szimbólumot, akkor a \ref piccolo_all.h állományban elhelyezett
 *  feltételes fordítási parancsok gondoskodnak az automatikus becsatolásáról.
 *
 *  \see piccolo_usb.c a további részletekért.
 */

#ifndef PICCOLO_USB
#define PICCOLO_USB
	#include "GenericTypeDefs.h"
	#include "Compiler.h"
	#include "usb_config.h"
	#include "./USB/usb_device.h"
	#include "./USB/usb.h"
	#include "./USB/usb_function_cdc.h"
	
	/*  VÁLTOZÓK *********************************************************/
	#pragma udata
	extern char USB_In_Buffer[64];         // Az USB bemeneti buffere (ebbe írunk...)
	extern char USB_Out_Buffer[64];        // Az USB kimeneti buffere (ebbõl olvasunk...)
	extern BYTE numBytesRead;              // A vett karakterek száma
	extern BYTE numBytesToSend;            // Az elküldendõ karakterek száma
	extern BYTE Buffercp;                  // Mutató a buffer kiolvasásához
	extern BYTE BlinkUSBStatus_enabled;    // USB státusz kijelzés: 1=enged, 0=tilt
	
	/* FÜGGVÉNY PROTOTÍPUSOK *********************************************/
	void InitializeSystem(void);           // Rendszer inicializálása (USB csatlakoztatása)
	void ProcessIO(void);                  // USB karakterbufferek kezelése
	void USBDeviceTasks(void);             // USB kezelése (interrupt vagy polling)
	void BlinkUSBStatus(void);             // USB státusz kijelzése (LED1 & LED2)
	unsigned int ReadPOT(void);            // Analóg jel mérése (potméter)
	
	/** USB I/O FUNCTIONS ************************************************/ 
	int _user_putc(char c);                // Egy karakter kiírása
	#define usb_cdc_putc(c) _user_putc(c)  // Alternatív elnevezés a _user_putc() függvényhez
	void outString(const rom char* psz_s); // Szöveg kiíratása
	char usb_cdc_getc(void);               // Egy karakter beolvasása  
	void outdec(long data, unsigned int ndigits); // Decimális kiíratás adott számú tizedesjegyre 
	void out4hex(unsigned int t);          // Egy 16 bites szám kiírása hexadecimálisan
	unsigned int get4hex(void);	           // Négyjegyû hexadecimális szám beolvasása
	unsigned char get2hex(void);	       // Kétjegyû hexadecimális szám beolvasása
	void delay_ms(unsigned int d);         // Késleltetõ eljárás, ami ProcessIO()-t is rendszeresen meghívja 
	#define usb_cdc_kbhit() (Buffercp<numBytesRead) // Bemeneti buffer ellenõrzése
#endif