/* PICCOLO project
 * Copyright (c) 2009-2010 Istvan Cserny (cserny@atomki.hu)
 *
 */

/** \file
 *  Ebben a header állományban konfigurálhatja a bootloader és az USB kapcsolat
 *  használatával kapcsolatosos legfontosabb beállításokat. 
 *
 *  - <b>USE_USB:</b> Akkor definiáljuk, ha használni kívánjuk az USB kapcsolatot (\ref piccolo_config.h 
 *    állományban vagy az MPLAB IDE Project/Build Options/Project menüjében definiálhatjuk)
 *
 *  - <b>USE_INTERRUPT:</b> Akkor definiáljuk, ha használni kívánjuk a programmegszakítást 
 *    (vagy a \ref piccolo_config.h állományban, vagy az MPLAB IDE Project/Build Options/Project 
 *    menüben definiálhatjuk). <b>Figyelem! Programmegszakítás esetén gondoskodnunk kell a magas és az 
 *    alacsony prioritású interruptok kiszolgálását ellátó hi_isr() és lo_isr() eljárásokról!</b>  
 *    Ha az USE_USB és az USE_INTERRUPT szimbólumok definiálva vannak, akkor automatikusan az
 *    USB_INTERRUPT szimbólum is definiálásra kerül, ellenkezõ esetben pedig az USB_POLLING 
 *    kerül definiálásra. A \ref hello-int.c mintaprogramban látható, hogy az USB interruptos 
 *    kiszolgálása hogyan végezhetõ.
 *
 *  - <b>Blinking_at_start:</b> azt engedélyezhetjük vele, hogy LED1 és LED2 kijelezze az USB kapcsolat 
 *    állapotát. A lehetséges értékek: 0 = tiltás, 1 = engedélyezés. 
 * 
 *  - <b>HID_BOOTLOADER:</b> Akkor definiáljuk, ha HID bootloadert használunk (vagy a \ref piccolo_config.h  
 *    állományban, vagy az MPLAB IDE Project/Build Options/Project menüpontjában definiálhatjuk). 
 *    Ezzel szinkronban a linker programnak is át kell adni a HID_BOOTLOADER szimbolumot 
 *    (parancssori /uHID_BOOTLOADER opció megadása a Project/Build Options/Project menüben, MPLINK beállításai lapján).
 *   
 *  - <b>MCHPUSB_BOOTLOADER:</b> Akkor definiáljuk, ha az MCHPUSB bootloadert használunk (vagy a \ref piccolo_config.h  
 *    állományban, vagy a Project/Build Options/Project menüben definiálhatjuk). Ezzel szinkronban a
 *    Linker programnak is át kell adni a MCHPUSB_BOOTLOADER szimbolumot (parancssori /u opció megadása
 *    a Project/Build Options/Project menüben, MPLINK beállításai lapján). 
 */

#ifndef PICCOLO_CONFIG_H
#define PICCOLO_CONFIG_H 

/*** USB használat  **************************************************/
//-- Aktiválja a következõ sort az USB kapcsolat használatához! -------
//#define USE_USB

/*** Interrupt használat  ********************************************/
//-- Aktiválja az alábbi definíciót a programmegszakítások használatához! 
//#define USE_INTERRUPT 

/*** BlinkUSBStatus engedélyezése ************************************/
// A Blinking_at_start szimbólum legyen mindig definálva, csupán az 
// értékét módosítsa, ha szükséges! 
// 1: Engedélyezi, hogy LED1 és LED2 jelezze az USB kapcsolat állapotát
// 0: Tiltja, hogy LED1 és LED2 jelezze az USB kapcsolat állapotát
#define Blinking_at_start 1

/* BOOTLOADER *********************************************************
* Az itteni definícióval szinkronban a Linker számára is definiálja
* a HID_BOOTLOADER vagy MCHPUSB_BOOTLOADER szimbólumot az MPLAB
* Project/Build options/Project menüpontban az MPLINK Linker fülre
* kattintva, s az Use Alternate Settings kiválasztása után, /u kapcsolóval!  
**********************************************************************/
//--- Aktiválja az alábbi sort a HID BOOTLOADER használatához
#define HID_BOOTLOADER

//--- Aktiválja az alábbi sort az MCHPUSB BOOTLOADER használatához
//#define MCHPUSB_BOOTLOADER

//--- Egyidejûleg csak egy bootloader-t definiálhat!
#if defined(HID_BOOTLOADER) && defined(MCHPUSB_BOOTLOADER)
    #error "Egyidejûleg nem definiálhat kétféle bootloader-t!"
#endif

/*  HARDVER DEFINÍCIÓK BECSATOLÁSA ***********************************/
#if defined(__18F14K50)
  #include "p18f14k50.h"	
  #include "piccolo-14k50.h"
#elif defined(__18F4550)
  #include "p18f4550.h"	
  #include "piccolo-4550.h"
#else
   #error "A választott mikrovezérlõt nem támogatja a PICCOLO project!"  
#endif


/*  Változótípusok és uniók definiálása ******************************/
typedef unsigned char       uint8;   // 8 bites, elõjel nélküli szám
typedef unsigned int        uint16;  //16 bites, elõjel nélküli szám
typedef unsigned long       uint32;  //32 bites, elõjel nélküli szám
typedef signed char         int8;    // 8 bites, elõjeles szám
typedef signed int          int16;   //16 bites, elõjeles szám
typedef signed long         int32;   //32 bites, elõjeles szám

///Unió típus szó és bájt eléréssel 16 bites adatokhoz
typedef union _union16 { 
  uint16 word;
  struct {
    uint8 lo_byte;
    uint8 hi_byte;
  };
} union16;

///Unió típus duplaszó, szó és bájt eléréssel 32 bites adatokhoz
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