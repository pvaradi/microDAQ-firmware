/* PICCOLO project
 * Copyright (c) 2009 Istvan Cserny (cserny@atomki.hu)
 * Ez az állomány a Microchip FSUSB keretrendszerébõl átvett
 * elemeket is tartalmaz!
 */

#include "piccolo_config.h"
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "usb_config.h"
#include "./USB/usb_device.h"
#include "./USB/usb.h"
#include "./USB/usb_function_cdc.h"
#include "HardwareProfile.h"
/** \file
 *  Ez az állomány tartalmazza azokat a függvényeket, amelyekkel az
 *  alkalmazói programból az USB kapcsolat kezelhetjük. 
 *  \see piccolo_usb.h a további részletekért.
 */
/* GLOBÁLIS VÁLTOZÓK az USB kommunikációhoz **************************/
#pragma udata

char USB_In_Buffer[64];       /**< Az USB bemeneti buffere (ebbe írunk...) **/ 
char USB_Out_Buffer[64];      /**< Az USB kimeneti buffere (ebbõl olvasunk...) **/    
BYTE numBytesRead=0;          /**< A vett karakterek száma   **/
BYTE numBytesToSend=0;        /**< Az elküldendõ karakterek száma  **/   
BYTE Buffercp=0;              /**< Mutató a buffer kiolvasásához  **/      

/** USB státusz kijelzése: 1 = engedélyezett, 0 = tiltott **/
BYTE BlinkUSBStatus_enabled=Blinking_at_start; 

/* FÜGGVÉNY PROTOTÍPUSOK *********************************************/

void InitializeSystem(void);  /**< Rendszer inicializálása (USB csatlakoztatása) **/
void ProcessIO(void);         /**< USB karakterbufferek kezelése **/
void USBDeviceTasks(void);    /**< USB kezelése (interrupt vagy polling) */
void BlinkUSBStatus(void);    /**< USB státusz kijelzése (LED1 & LED2) */
unsigned int ReadPOT(void);   /**< Analóg jel mérése (potméter) */
int _user_putc(char c);       /**< Egy karakter kiírása */
void outString(const rom char* psz_s); /**< Szöveg kiíratása */
char usb_cdc_getc(void);      /**< Egy karakter beolvasása */
void outdec(long data, unsigned int ndigits); /**< Decimális kiíratás adott számú tizedesjegyre */
void out4hex(unsigned int t); /**< Egy 16 bites szám kiírása hexadecimálisan */
unsigned int get4hex(void);   /**< Négyjegyû hexadecimális szám beolvasása */ 
unsigned char get2hex(void);  /**< Kétjegyû hexadecimális szám beolvasása */
void delay_ms(unsigned int d); /**< Késleltetõ eljárás, ami ProcessIO()-t is rendszeresen meghívja */

#pragma code

/** Egy karakter kiírása a kimeneti bufferbe.
 * Blokkoló típusú függvény, tehát hívogatja a ProcessIO függvényt, amíg 
 * várakozik.
 * \param char c a kiírandó karakter
 * \return a kiírt karakter kódja int típussá konvertálva
 */
int _user_putc(char c)
{
  while (numBytesToSend>60)
    {
      ProcessIO();
    }
  USB_In_Buffer[numBytesToSend++]=c;
  return (int)c;
}

/** Kiír egy nullával lezárt szöveget a ROM memóriából a kimeneti bufferbe.
* Ez a függvény a blokkoló típusú _user_putc() függvényt hívja!  
* \param const rom char* psz_s mutató, a ROM-ban tárolt szöveghez 
*/
void outString(const rom char* psz_s)
{
  char c;
  while ((c=*psz_s))
    {
      if (c == '\n')
        {
          _user_putc(0x0D);
        }
      _user_putc(c);
      psz_s++;
    }
}

/** Beolvas egy karaktert az input bufferbõl, vagy várakozik, ha az üres.
 *  Blokkoló típusú függvény, tehát hívogatja a ProcessIO függvényt, amíg 
 *  várakozik.
 * \return char típus, a beolvasott karakter kódja
 */

char usb_cdc_getc(void)
{
  while (Buffercp==numBytesRead)
    {
      ProcessIO();
    }
  return(USB_Out_Buffer[Buffercp++]);
}

/** Decimális kiíratás adott számú tizedesjegyre. 
 * \param data a kiírandó szám (elõjelesen)
 * \param ndigits a kiírandó tizedesek száma
 */
void outdec(long data, unsigned int ndigits) {
static char sign, s[12];
unsigned int i;
	i=0; sign='+';
	if(data<0) { sign='-'; data = -data;}
	do {
		s[i]=data%10 + '0';
		data=data/10;
		i++;
		if(i==ndigits) {s[i]='.'; i++;}
	} while(data>0);
	_user_putc(sign);
	do{
		_user_putc(s[--i]);
	} while(i);
}


/** Négy hexadecimális számjegy beolvasása és átalakítása unsigned int típusra.
 * Ez a függvény blokkoló típusú, addig vár, amíg be nem érkezik négy kararakter,
 * amelyeket a _user_putc() eljárással vissza is tükrözünk.
 */
unsigned int get4hex(void)
{
  char c,i;
  unsigned int t;
  t=0;
  for (i=0; i<4; i++)
    {
      c=usb_cdc_getc();
     //_user_putc(c);
      if (c>0x40)
        {
          c -=7;
        }
      t= (t<<4) + (c & 0x0F);
    }
  return t;
}

/** Egy elõjel nélküli egész számot hexadecimális formában kiír
 * a kimeneti bufferbe. Ez a függvény meghívja a blokkoló típusú
 *  _user_putc() függvényt! 
 */
void out4hex(unsigned int t)
{
  char c;
  c=(char)((t>>12) & 0x0F);
  if (c>9) c+=7;
  _user_putc(c+'0');
  c=(char)((t>>8) & 0x0F);
  if (c>9) c+=7;
  _user_putc(c+'0');
  c=(char)((t>>4) & 0x0F);
  if (c>9) c+=7;
  _user_putc(c+'0');
  c=(char)(t & 0x0F);
  if (c>9) c+=7;
  _user_putc(c+'0');
}

/** Két hexadecimális számjegy beolvasása és átalakítása unsigned char típusra
 *  Ez a függvény blokkoló típusú, addig vár, amíg be nem érkezik két kararakter,
 *  amelyeket a _user_putc() eljárással vissza is tükrözünk. 
 */
unsigned char get2hex(void)
{
  char c,i;
  unsigned char t;
  t=0;
  for (i=0; i<2; i++)
    {
      c=usb_cdc_getc();
  //    _user_putc(c);
      if (c>0x40)
        {
          c -=7;
        }
      t= (t<<4) + (c & 0x0F);
    }
  return t;
}

/** Késleltetõ eljárás, amely az USB adatforgalom fenntartásáról is gondoskodik
 * ProcessIO() rendszeres hívogatása útján. A belsõ ciklus kb. 1 ms késleltetést végez.
 * \param d az elõírt várakozási idõ, 1 ms egységekben
 */
void delay_ms(unsigned int d) {
unsigned int i,j;
	for(i=0; i<d; i++) {
		for(j=0; j<85; j++) { 
			ProcessIO();
		}
	}
}

/*****************************************************************************/
/*** Az alábbi függvények a Microchip FSUSB programcsomagból vannak átvéve ***/ 
/*****************************************************************************/

/******************************************************************************
 * Function:        void InitializeSystem(void)
 * Overview:        InitializeSystem is a centralized initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.
 *****************************************************************************/
void InitializeSystem(void)
{
#if defined(__18F4550)
  ADCON1 |= 0x0F;                 // Default all pins to digital
#endif


//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	HardwareProfile.h file.
#if defined(USE_USB_BUS_SENSE_IO)
  tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
#endif

//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2"
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in HardwareProfile.h, and that an appropriate I/O pin has been mapped
//	to it in HardwareProfile.h.
#if defined(USE_SELF_POWER_SENSE_IO)
  tris_self_power = INPUT_PIN;    // See HardwareProfile.h
#endif

  mInitAllLEDs();                 //Initialize all of the LED pins

  USBDeviceInit();	              //usb_device.c.  Initializes USB module SFRs and 
                                  //firmware variables to known states.
#if defined(USB_INTERRUPT)
  USBDeviceAttach();              //USB eszköz csatlakoztatása
#endif

  stdout=_H_USER;                 //standard output átirányítása _user_putc()-re
  stderr=_H_USER;                 //standard error átirányítása _user_putc()-re
//--- Globális változók inicializálása---------------------------------
  numBytesRead=0;          //A vett karakterek száma
  numBytesToSend=0;        //Az elküldendõ karakterek száma
  Buffercp=0;              //Mutató a buffer kiolvasásához
  BlinkUSBStatus_enabled=Blinking_at_start; //USB státusz kijelzése:
}//end InitializeSystem


/********************************************************************
 * Function:        void ProcessIO(void)
 *
 * Overview:        This function is used to move data between the USB
 *                  memory and the application character buffers
 *                  It should be called periodically in order to
 *                  activate the CDCTxService() status machine.
 *******************************************************************/
void ProcessIO(void)
{
#if defined(USB_POLLING)
  USBDeviceTasks();     //Handle USB I/O
#endif
//-- Blink the LEDs according to the USB device status
  if (BlinkUSBStatus_enabled)
    {
      BlinkUSBStatus();
    }
  if ((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;
//-- Read data from host if the input buffer is empty
  if (USBUSARTIsTxTrfReady() && (numBytesRead==Buffercp))
    {
      numBytesRead = getsUSBUSART(USB_Out_Buffer,64);
      Buffercp=0;
    }
//-- Send data to host if the output buffer is not empty
  if (USBUSARTIsTxTrfReady() && (numBytesToSend!=0))
    {
      putUSBUSART(USB_In_Buffer,numBytesToSend);
      numBytesToSend=0;
    }
  CDCTxService();
}		//end ProcessIO

/********************************************************************
 * Function:        void BlinkUSBStatus(void)
 * Overview:        BlinkUSBStatus turns on and off LEDs
 *                  corresponding to the USB device state.
 *
 * Note:            mLED macros can be found in HardwareProfile.h
 *                  USBDeviceState is declared and updated in
 *                  usb_device.c.
 *******************************************************************/
void BlinkUSBStatus(void)
{
  static WORD led_count=0;

  if (led_count == 0)led_count = 10000U;
  led_count--;

#define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
#define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
#define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
#define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

  if (USBSuspendControl == 1)
    {
      if (led_count==0)
        {
          mLED_1_Toggle();
          if (mGetLED_1())
            {
              mLED_2_On();
            }
          else
            {
              mLED_2_Off();
            }
        }//end if
    }
  else
    {
      if (USBDeviceState == DETACHED_STATE)
        {
          mLED_Both_Off();
        }
      else if (USBDeviceState == ATTACHED_STATE)
        {
          mLED_Both_On();
        }
      else if (USBDeviceState == POWERED_STATE)
        {
          mLED_Only_1_On();
        }
      else if (USBDeviceState == DEFAULT_STATE)
        {
          mLED_Only_2_On();
        }
      else if (USBDeviceState == ADDRESS_STATE)
        {
          if (led_count == 0)
            {
              mLED_1_Toggle();
              mLED_2_Off();
            }//end if
        }
      else if (USBDeviceState == CONFIGURED_STATE)
        {
          if (led_count==0)
            {
              mLED_1_Toggle();
              if (mGetLED_1())
                {
                  mLED_2_Off();
                }
              else
                {
                  mLED_2_On();
                }
            }//end if
        }//end if(...)
    }//end if(UCONbits.SUSPND...)

}//end BlinkUSBStatus

/******************************************************************************
 * Function:        WORD_VAL ReadPOT(void)
 * Output:          WORD_VAL - the 10-bit right justified POT value
 *
 * Side Effects:    ADC buffer value updated
 *
 * Overview:        This function reads the POT and leaves the value in the
 *                  ADC buffer register
 *****************************************************************************/
unsigned int ReadPOT(void)
{
  unsigned int w;
  mInitPOT();
  ADCON0bits.GO = 1;              // Start AD conversion
  while (ADCON0bits.NOT_DONE);    // Wait for conversion
  w = ADRESH;
  w = (w<<8) + ADRESL;
  return w;
}//end ReadPOT

// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
  //Example power saving code.  Insert appropriate code here for the desired
  //application behavior.  If the microcontroller will be put to sleep, a
  //process similar to that shown below may be used:

  //ConfigureIOPinsForLowPower();
  //SaveStateOfAllInterruptEnableBits();
  //DisableAllInterruptEnableBits();
  //EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
  //Sleep();
  //RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
  //RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

  //IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is
  //cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause
  //things to not work as intended.


//    #if defined(__C30__)
//    #if 0
//        U1EIR = 0xFFFF;
//        U1IR = 0xFFFF;
//        U1OTGIR = 0xFFFF;
//        IFS5bits.USB1IF = 0;
//        IEC5bits.USB1IE = 1;
//        U1OTGIEbits.ACTVIE = 1;
//        U1OTGIRbits.ACTVIF = 1;
//        Sleep();
//    #endif
//    #endif
}


/******************************************************************************
 * Function:        void _USB1Interrupt(void)
 * Overview:        This function is called when the USB interrupt bit is set
 *					In this example the interrupt is only used when the device
 *					goes to sleep when it receives a USB suspend command
 *****************************************************************************/
#if 0
void __attribute__ ((interrupt)) _USB1Interrupt(void)
{
#if !defined(self_powered)
  if (U1OTGIRbits.ACTVIF)
    {
      IEC5bits.USB1IE = 0;
      U1OTGIEbits.ACTVIE = 0;
      IFS5bits.USB1IF = 0;

      //USBClearInterruptFlag(USBActivityIFReg,USBActivityIFBitNum);
      USBClearInterruptFlag(USBIdleIFReg,USBIdleIFBitNum);
      //USBSuspendControl = 0;
    }
#endif
}
#endif

/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *
 *					This call back is invoked when a wakeup from USB suspend
 *					is detected.
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
  // If clock switching or other power savings measures were taken when
  // executing the USBCBSuspend() function, now would be a good time to
  // switch back to normal full power run mode conditions.  The host allows
  // a few milliseconds of wakeup time, after which the device must be
  // fully back to normal, and capable of receiving and processing USB
  // packets.  In order to do this, the USB module must receive proper
  // clocking (IE: 48MHz clock must be available to SIE for full speed USB
  // operation).
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
  // No need to clear UIRbits.SOFIF to 0 here.
  // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *******************************************************************/
void USBCBErrorHandler(void)
{
  // No need to clear UEIR to 0 here.
  // Callback caller is already doing that.

  // Typically, user firmware does not need to do anything special
  // if a USB error occurs.  For example, if the host sends an OUT
  // packet to your device, but the packet gets corrupted (ex:
  // because of a bad connection, or the user unplugs the
  // USB cable during the transmission) this will typically set
  // one or more USB error interrupt flags.  Nothing specific
  // needs to be done however, since the SIE will automatically
  // send a "NAK" packet to the host.  In response to this, the
  // host will normally retry to send the packet again, and no
  // data loss occurs.  The system will typically recover
  // automatically, without the need for application firmware
  // intervention.

  // Nevertheless, this callback function is provided, such as
  // for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
  USBCheckCDCRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
  // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This
 *					callback function should initialize the endpoints
 *					for the device's usage according to the current
 *					configuration.
 *******************************************************************/
void USBCBInitEP(void)
{
  CDCInitEP();
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function should only be called when:
 *
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.
 *
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            Interrupt vs. Polling
 *                  -Primary clock
 *                  -Secondary clock ***** MAKE NOTES ABOUT THIS *******
 *                   > Can switch to primary first by calling USBCBWakeFromSuspend()

 *                  The modifiable section in this routine should be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of 1-13 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at lest 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
  static WORD delay_count;

  USBResumeControl = 1;                // Start RESUME signaling

  delay_count = 1800U;                // Set RESUME line for 1-13 ms
  do
    {
      delay_count--;
    }
  while (delay_count);
  USBResumeControl = 0;
}


/*******************************************************************
 * Function:        void USBCBEP0DataReceived(void)
 *
 * PreCondition:    ENABLE_EP0_DATA_RECEIVED_CALLBACK must be
 *                  defined already (in usb_config.h)
 * Overview:        This function is called whenever a EP0 data
 *                  packet is received.  This gives the user (and
 *                  thus the various class examples a way to get
 *                  data that is received via the control endpoint.
 *                  This function needs to be used in conjunction
 *                  with the USBCBCheckOtherReq() function since
 *                  the USBCBCheckOtherReq() function is the apps
 *                  method for getting the initial control transfer
 *                  before the data arrives.
 *******************************************************************/
#if defined(ENABLE_EP0_DATA_RECEIVED_CALLBACK)
void USBCBEP0DataReceived(void)
{
}
#endif

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                  USB_EVENT event, void *pdata, WORD size)
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
  switch (event)
    {
    case EVENT_CONFIGURED:
      USBCBInitEP();
      break;
    case EVENT_SET_DESCRIPTOR:
      USBCBStdSetDscHandler();
      break;
    case EVENT_EP0_REQUEST:
      USBCBCheckOtherReq();
      break;
    case EVENT_SOF:
      USBCB_SOF_Handler();
      break;
    case EVENT_SUSPEND:
      USBCBSuspend();
      break;
    case EVENT_RESUME:
      USBCBWakeFromSuspend();
      break;
    case EVENT_BUS_ERROR:
      USBCBErrorHandler();
      break;
    case EVENT_TRANSFER:
      Nop();
      break;
    default:
      break;
    }
  return TRUE;
}

/** EOF main.c *************************************************/

