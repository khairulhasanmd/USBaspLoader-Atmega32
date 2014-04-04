/**
 * \file bootloader/bootloaderconfig.h
 * \brief This file (together with some settings in Makefile) configures the
 * boot loader according to the hardware.
 *
 * This file contains (besides the hardware configuration normally found in
 * usbconfig.h) two functions or macros: bootLoaderInit() and
 * bootLoaderCondition(). Whether you implement them as macros or as static
 * inline functions is up to you, decide based on code size and convenience.
 *
 * bootLoaderInit() is called as one of the first actions after reset. It should
 * be a minimum initialization of the hardware so that the boot loader condition
 * can be read. This will usually consist of activating a pull-up resistor for an
 * external jumper which selects boot loader mode.
 *
 * bootLoaderCondition() is called immediately after initialization and in each
 * main loop iteration. If it returns TRUE, the boot loader will be active. If it
 * returns FALSE, the boot loader jumps to address 0 (the loaded application)
 * immediately.
 *
 * For compatibility with Thomas Fischl's avrusbboot, we also support the macro
 * names BOOTLOADER_INIT and BOOTLOADER_CONDITION for this functionality. If
 * these macros are defined, the boot loader usees them.
 *
 * \author Ronald Schaten <ronald@schatenseite.de>
 * \version $Id: bootloaderconfig.h,v 1.1 2008-07-09 20:47:11 rschaten Exp $
 *
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef __bootloaderconfig_h_included__
#define __bootloaderconfig_h_included__

#define LED_PORT_DDR        DDRB
#define LED_PORT_OUTPUT     PORTB
#define LED_RED_BIT         PB0         //выход, включение тестового светодиода (красный)
#define ON_RED()  (LED_PORT_OUTPUT |=  (1<<LED_RED_BIT)) //включение тестового светодиода
#define OFF_RED() (LED_PORT_OUTPUT &= ~(1<<LED_RED_BIT)) //выключение тестового светодиода
#define RED() (LED_PORT_OUTPUT & (1<<LED_RED_BIT))       //получение состояния тестового светодиода
#define CHANGE_RED() (RED()?OFF_RED():ON_RED())          //смена состояния тестового светодиода

/* ---------------------------- Hardware Config ---------------------------- */

/** This is the port where the USB bus is connected. When you configure it to
 * "B", the registers PORTB, PINB and DDRB will be used.
 */
#define USB_CFG_IOPORTNAME      D
/** This is the bit number in USB_CFG_IOPORT where the USB D- line is connected.
 * This may be any bit in the port.
 */
#define USB_CFG_DMINUS_BIT      4
/** This is the bit number in USB_CFG_IOPORT where the USB D+ line is connected.
 * This may be any bit in the port. Please note that D+ must also be connected
 * to interrupt pin INT0!
 */
#define USB_CFG_DPLUS_BIT       2
/** Clock rate of the AVR in MHz. Legal values are 12000, 16000 or 16500.
 * The 16.5 MHz version of the code requires no crystal, it tolerates +/- 1%
 * deviation from the nominal frequency. All other rates require a precision
 * of 2000 ppm and thus a crystal!
 * Default if not specified: 12 MHz
 */
#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)

/* ----------------------- Optional Hardware Config ------------------------ */

/* If you connect the 1.5k pullup resistor from D- to a port pin instead of
 * V+, you can connect and disconnect the device from firmware by calling
 * the macros usbDeviceConnect() and usbDeviceDisconnect() (see usbdrv.h).
 * This constant defines the port on which the pullup resistor is connected.
 */
/* #define USB_CFG_PULLUP_IOPORTNAME   D */
/* This constant defines the bit number in USB_CFG_PULLUP_IOPORT (defined
 * above) where the 1.5k pullup resistor is connected. See description
 * above for details.
 */
/* #define USB_CFG_PULLUP_BIT          4 */

/* ------------------------------------------------------------------------- */
/* ---------------------- feature / code size options ---------------------- */
/* ------------------------------------------------------------------------- */

/** If HAVE_EEPROM_PAGED_ACCESS is defined to 1, page mode access to EEPROM is
 * compiled in. Whether page mode or byte mode access is used by AVRDUDE
 * depends on the target device. Page mode is only used if the device supports
 * it, e.g. for the ATMega88, 168 etc. You can save quite a bit of memory by
 * disabling page mode EEPROM access. Costs ~ 138 bytes.
 */
#define HAVE_EEPROM_PAGED_ACCESS    1
/** If HAVE_EEPROM_BYTE_ACCESS is defined to 1, byte mode access to EEPROM is
 * compiled in. Byte mode is only used if the device (as identified by its
 * signature) does not support page mode for EEPROM. It is required for
 * accessing the EEPROM on the ATMega8. Costs ~54 bytes.
 */
#define HAVE_EEPROM_BYTE_ACCESS     1
/** If this macro is defined to 1, the boot loader will exit shortly after the
 * programmer closes the connection to the device. Costs ~36 bytes.
 */
#define BOOTLOADER_CAN_EXIT         1
/** This macro defines the signature bytes returned by the emulated USBasp to
 * the programmer software. They should match the actual device at least in
 * memory size and features. If you don't define this, values for ATMega8,
 * ATMega88, ATMega168 and ATMega328 are guessed correctly.
 */
#define SIGNATURE_BYTES             0x1e, 0x95, 0x02, 0     /* ATMega32 */

/* The following block guesses feature options so that the resulting code
 * should fit into 2k bytes boot block with the given device and clock rate.
 * Activate by passing "-DUSE_AUTOCONFIG=1" to the compiler.
 * This requires gcc 3.4.6 for small enough code size!
 */
#if USE_AUTOCONFIG
#   undef HAVE_EEPROM_PAGED_ACCESS
#   define HAVE_EEPROM_PAGED_ACCESS     (USB_CFG_CLOCK_KHZ >= 16000)
#   undef HAVE_EEPROM_BYTE_ACCESS
#   define HAVE_EEPROM_BYTE_ACCESS      1
#   undef BOOTLOADER_CAN_EXIT
#   define BOOTLOADER_CAN_EXIT          1
#   undef SIGNATURE_BYTES
#endif /* USE_AUTOCONFIG */

/* ------------------------------------------------------------------------- */

#ifndef __ASSEMBLER__   /* assembler cannot parse function definitions */

//uint8_t ledcounter = 0; ///< counter used to set the speed of the running light
//uint8_t ledstate = 0;   ///< state of the running light

/**
 * Prepare IO-ports for detection of bootloader-condition, which happens in
 * bootLoaderCondition().
 */
static inline void bootLoaderInit(void) 
{
    DDRB  |= (1 << LED_RED_BIT);   //выход для красного светодиода
    //на PB5 (MOSI, контакт 4 коннектора U1 ISP) подключаем pull-up, а также зажигаем светодиод
    PORTB |= (1 << PB5)|(1 << LED_RED_BIT);
}

/**
 * Clean up after boot loader action. In this case: switch off all LEDs.
 */
static inline void bootLoaderExit(void) 
{
    OFF_RED();
}

/**
 * Check if conditions for boot loader are met. This function is called in an
 * endless loop, so we use our spare time to display a nice running light on
 * the LEDs.
 * \return 1 if bootloader should be active, 0 otherwise
 */
static inline uint8_t bootLoaderCondition() 
{
    if (!(PINB & (1 << PB5))) 
    {
        return 1;
    } 
    else 
    {
        // no boot loader
        return 0;
    }
}

#endif /* __ASSEMBLER__ */

/* ------------------------------------------------------------------------- */

#endif /* __bootloader_h_included__ */
