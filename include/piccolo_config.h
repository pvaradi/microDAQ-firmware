/* PICCOLO project
 * Copyright (c) 2009-2010 Istvan Cserny (cserny@atomki.hu)
 *
 */

/** \file
 *  Ebben a header �llom�nyban konfigur�lhatja a bootloader �s az USB kapcsolat
 *  haszn�lat�val kapcsolatosos legfontosabb be�ll�t�sokat. 
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
 *  - <b>HID_BOOTLOADER:</b> Akkor defini�ljuk, ha HID bootloadert haszn�lunk (vagy a \ref piccolo_config.h  
 *    �llom�nyban, vagy az MPLAB IDE Project/Build Options/Project men�pontj�ban defini�lhatjuk). 
 *    Ezzel szinkronban a linker programnak is �t kell adni a HID_BOOTLOADER szimbolumot 
 *    (parancssori /uHID_BOOTLOADER opci� megad�sa a Project/Build Options/Project men�ben, MPLINK be�ll�t�sai lapj�n).
 *   
 *  - <b>MCHPUSB_BOOTLOADER:</b> Akkor defini�ljuk, ha az MCHPUSB bootloadert haszn�lunk (vagy a \ref piccolo_config.h  
 *    �llom�nyban, vagy a Project/Build Options/Project men�ben defini�lhatjuk). Ezzel szinkronban a
 *    Linker programnak is �t kell adni a MCHPUSB_BOOTLOADER szimbolumot (parancssori /u opci� megad�sa
 *    a Project/Build Options/Project men�ben, MPLINK be�ll�t�sai lapj�n). 
 */

#ifndef PICCOLO_CONFIG_H
#define PICCOLO_CONFIG_H 

/*** USB haszn�lat  **************************************************/
//-- Aktiv�lja a k�vetkez� sort az USB kapcsolat haszn�lat�hoz! -------
//#define USE_USB

/*** Interrupt haszn�lat  ********************************************/
//-- Aktiv�lja az al�bbi defin�ci�t a programmegszak�t�sok haszn�lat�hoz! 
//#define USE_INTERRUPT 

/*** BlinkUSBStatus enged�lyez�se ************************************/
// A Blinking_at_start szimb�lum legyen mindig defin�lva, csup�n az 
// �rt�k�t m�dos�tsa, ha sz�ks�ges! 
// 1: Enged�lyezi, hogy LED1 �s LED2 jelezze az USB kapcsolat �llapot�t
// 0: Tiltja, hogy LED1 �s LED2 jelezze az USB kapcsolat �llapot�t
#define Blinking_at_start 1

/* BOOTLOADER *********************************************************
* Az itteni defin�ci�val szinkronban a Linker sz�m�ra is defini�lja
* a HID_BOOTLOADER vagy MCHPUSB_BOOTLOADER szimb�lumot az MPLAB
* Project/Build options/Project men�pontban az MPLINK Linker f�lre
* kattintva, s az Use Alternate Settings kiv�laszt�sa ut�n, /u kapcsol�val!  
**********************************************************************/
//--- Aktiv�lja az al�bbi sort a HID BOOTLOADER haszn�lat�hoz
#define HID_BOOTLOADER

//--- Aktiv�lja az al�bbi sort az MCHPUSB BOOTLOADER haszn�lat�hoz
//#define MCHPUSB_BOOTLOADER

//--- Egyidej�leg csak egy bootloader-t defini�lhat!
#if defined(HID_BOOTLOADER) && defined(MCHPUSB_BOOTLOADER)
    #error "Egyidej�leg nem defini�lhat k�tf�le bootloader-t!"
#endif

/*  HARDVER DEFIN�CI�K BECSATOL�SA ***********************************/
#if defined(__18F14K50)
  #include "p18f14k50.h"	
  #include "piccolo-14k50.h"
#elif defined(__18F4550)
  #include "p18f4550.h"	
  #include "piccolo-4550.h"
#else
   #error "A v�lasztott mikrovez�rl�t nem t�mogatja a PICCOLO project!"  
#endif


/*  V�ltoz�t�pusok �s uni�k defini�l�sa ******************************/
typedef unsigned char       uint8;   // 8 bites, el�jel n�lk�li sz�m
typedef unsigned int        uint16;  //16 bites, el�jel n�lk�li sz�m
typedef unsigned long       uint32;  //32 bites, el�jel n�lk�li sz�m
typedef signed char         int8;    // 8 bites, el�jeles sz�m
typedef signed int          int16;   //16 bites, el�jeles sz�m
typedef signed long         int32;   //32 bites, el�jeles sz�m

///Uni� t�pus sz� �s b�jt el�r�ssel 16 bites adatokhoz
typedef union _union16 { 
  uint16 word;
  struct {
    uint8 lo_byte;
    uint8 hi_byte;
  };
} union16;

///Uni� t�pus duplasz�, sz� �s b�jt el�r�ssel 32 bites adatokhoz
typedef union _union32 {
  uint32 dword;
  struct {
    uint16 lo_word;
    uint16 hi_word;
  };
  uint8 byte[4];
} union32;

#endif
/*** EOF PICCOLO-GONFIG.H ********************************************/