/* PICCOLO project
 * Copyright (c) 2009-2010 Istvan Cserny (cserny@atomki.hu)
 *
 */

/** \file
 *  Ez a header állomány gondoskodik a RESET és az interupt vektorok
 *  áthelyezésérõl, s a PIC18F14K50 és a PIC18F4550 kísérleti áramkörök közötti
 *  triviális eltérések feloldásáról. A \ref piccolo_config.h tartalmától, vagy
 *  a fordítónak közvetlenül átadott makrodefinícióktól függõen további állományok
 *  becsatolására is sor kerülhet, illetve feltételes fordítás segítségével 
 *  alternatív kódrészletek kerülnek bele a lefordított projektbe.
 *
 *  Figyelem! ez a header állomány csak a fõprogramba csatolható be!
 *
 *  A legfontosabb konfigurációs paraméterek:
 *  - <b>__18F14K50:</b> Azt jelzi, hogy a Configure/Select Device menüben PIC18F14K50 mikrovezérlõt 
 *    választottuk. Ezt a szimbólumot a fordító automatikusan definiálja. 
 *
 *  - <b>__18F4550:</b>  Azt jelzi, hogy a Configure/Select Device menüben PIC18F4550 mikrovezérlõt 
 *    választottuk. Ezt a szimbólumot a fordító automatikusan definiálja. 
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
 *  - <b>HID_BOOTLOADER:</b> Akkor definiáljuk, ha HID bootloadert használunk (\ref piccolo_config.h  
 *    állományban vagy az MPLAB IDE Project/Build Options/Project menüpontjában definiálhatjuk). 
 *    Ezzel szinkronban a linker programnak is át kell adni a HID_BOOTLOADER szimbolumot 
 *    (parancssori /uHID_BOOTLOADER opció megadása a Project/Build Options/Project menüben, MPLINK beállításai lapján).
 *   
 *  - <b>MCHPUSB_BOOTLOADER:</b> Akkor definiáljuk, ha az MCHPUSB bootloadert használunk (\ref piccolo_config.h  
 *    állományban vagy a Project/Build Options/Project menüben definiálhatjuk). Ezzel szinkronban a
 *    Linker programnak is át kell adni a MCHPUSB_BOOTLOADER szimbolumot (parancssori /u opció megadása
 *    a Project/Build Options/Project menüben, MPLINK beállításai lapján). 
 * 
 *    \see piccolo_config.h projekt szintû konfigurációs beállítások
 *    \see piccolo_usb.h az USB kapcsolat alkalmazásszintû kezeléséhez szükséges függvények és változók
 *    \see piccolo-14k50.h a PIC18F14K50 mintaáramkör hardverspecifikus makródefiníciói
 *    \see piccolo-4550.h a PIC18F4550 mintaáramkör hardverspecifikus makródefiníciói
 */

#ifndef PICCOLO_ALL
#define PICCOLO_ALL 

/*  INCLUDE állományok becsatolása ***********************************/
#include "piccolo_config.h" 	//Implicit módon becsatolja p18cxxx.h-t is!

    #warning *** P R O J E K T   K O N F I G U R Á C I Ó ***
#if defined(USE_USB)
    #include "piccolo_usb.h" 
    #warning * USE__USB        : DEFINIÁLT                 *
#else
    #warning * USE__USB        : NEM DEFINIÁLT             * 
#endif
#if defined(USE_INTERRUPT)
    #warning * USE__INTERRUPT  : DEFINIÁLT                 *
#else
    #warning * USE__INTERRUPT  : NEM DEFINIÁLT             * 
#endif
#if defined(HID_BOOTLOADER)
    #warning * HID__BOOTLOADER : DEFINIÁLT                 *
#else
    #warning * HID__BOOTLOADER : NEM DEFINIÁLT             * 
#endif
#if defined(MCHPUSB_BOOTLOADER)
    #warning * MCHPUSB__BOOTLOADER : DEFINIÁLT             *
#endif

    #warning *********************************************** 

/** Inline függvény, amely egy regiszter valamelyik bitcsoportját
 * atomi mûvelettel módosítja. 
 * \param reg a módosítandó regiszter neve
 * \param val a beírandó érték (helyiértéken helyesen!)
 * \param mask a módosítandó bitcsoportot kijelölõ maszk
 */
#define ChangeBits(reg,val,mask) reg ^= ((reg^val) & mask)	///<Bitcsoport átírása atomi mûvelettel


/*  Vektorok áthelyezése  ********************************************/
    void hi_isr(void);     ///< Magas prioritású programmegszakítás kiszolgálása
    void lo_isr(void);     ///< Alacsony prioritású programmegszakítás kiszolgálása

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
// Ha nem használunk bootloadert, akkor a C18 startup kódja automatikuan létrehozza!
// Ezért az alábbi sorok csak bootloader használata esetén kellenek.
#if defined(HID_BOOTLOADER)||defined(MCHPUSB_BOOTLOADER)		
	extern void _startup (void);
/** Az áthelyzett RESET vektor.  
 *  A bootloader használattól függõ abszolút címre kerül.
 */
	#pragma code REMAPPED_RESET_VECTOR = RESET_VECTOR
	void _reset (void) 
	{
	    _asm goto _startup _endasm
	}
#endif
	
/** Az áthelyzett magas prioritású interrupt vektor.  
 *  A bootloader használattól függõ abszolút címre kerül.
 */
#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = HIGH_INTERRUPT_VECTOR
void Remapped_High_ISR (void)
{
	 _asm goto hi_isr _endasm
}

/** Az áthelyzett alacsony prioritású interrupt vektor.  
 *  A bootloader használattól függõ abszolút címre kerül.
 */
#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = LOW_INTERRUPT_VECTOR
void Remapped_Low_ISR (void)
{
	 _asm goto lo_isr _endasm 
}	

#pragma code
//Üres eljárásokat gyártunk az elszabadult interruptok megfékezésére,
//ha a felhasználó nem akar programmegszakítást használni!
#ifndef USE_INTERRUPT
	void hi_isr()
	{
	}	
	
	void lo_isr()
	{	
	}
#endif //USE_INTERRUPT

#endif //PICCOLO_ALL