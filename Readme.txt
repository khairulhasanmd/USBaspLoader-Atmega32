USB bootloader for AVR microcontrollers
DIY or Do it yourself
The article describes how to quickly launch USB bootloader for the ATmega32 microcontroller as an example of the loader usbasploader Objective Development. 

Technology USB bootloader allows one single possibility - no need for special programming device to replace the software (firmware) in the device - just connect to a PC via USB. There is no need to carry a programmer, because the computer and the USB is everywhere. If your breadboard has a bootloader, you can save money on buying a programmer or the time of its manufacture - it is important for beginners. 

The AVR curve had a lot of bootloader-s - see [ 1 ]. For this article, I chose usbasploader [ 2 ] because it is compatible with the very popular programmer USBasp. This allows you to reflash firmware as on Linux, and Windows with popular applications (see [3]) avrdude, eXtreme Burner - AVR, Khazama AVR Programmer and even from the programming environment BASCOM-AVR (BASIC programming system for BASCOM-AVR - little wonder, worthy of another article). In addition, usbasploader comes with all source code, well documented and easily adaptable to the needs of the user (on this later). 

Usbasploader loader works is very simple - when you connect to a USB programmer, he pretends USBasp.Therefore, all the programs it supports, will overwrite the firmware in your device, as if they are using the programmer USBasp. Bootloader is located in the upper address of flash-memory microcontroller programs, and writes the user program in the lower address (usually starting at address 0), ie, when flashing the microcontroller bootloader is not erased. After recording bootloader passes control to the user. Now the details of how this works with microcontrollers ATmega, for example, the ATmega32. 

For the ATmega32 usbasploader compiled so that it is placed into flash at address 7000h (remember that the program memory space ATmega32 is 0000h .. 7FFFh, and the address is a multiple of two bytes of the command, ie instruction addresses are in the range 0000h .. 37FFh) . For the user program is still free space 0000h .. 6FFFh (28,672 bytes). The loader uses usbasploader inherent in the ATmega32 microcontroller can run at the address located at the highest address of program memory (available in several fixed addresses, selectable with jumpers - fuse bits, see [ 4 ]). To power-on reset and control is always passed to the address 0x7000, the jumper must be programmed (fuse bits, fuse-bits) BOOTSZ0 BOOTSZ1 and the appropriate state (under the bootloader should be allocated 4000h bytes, or 2048 words of program code), and the program jumper BOOTRST. After this code when you reset or turn will not start at address 0, and from the address 3800h in the words of command AVR, or from the address in bytes 7000h flash (recall that the minimum team size AVR - two bytes). 

Having control after reset, the code checks the condition usbasploader its activation, ie, a signal that he should start working as a programmer USBasp. Typically, such a signal is the closure of some feet on the ground. For prototyping board as such, I chose PB5 pins of the microcontroller ATmega32. This pin is also the signal MOSI, the on-connector, so the jumper between terminals 4 and 6 of U1 ISP connector is very convenient to apply for a signal PB5 log. 0. So, the code checks the level of usbasploader on port PB5, and if there is a log. 0 (the jumper between pins 4 and 6 of U1 ISP connector is installed), then begins work as a programmer USBasp. That is, when the jumper installed at power-up bootloader is activated, and if it is connected to a computer, the computer will be USB-device USBasp. If at the time of the power jumper is not (in this case on the PB5 microcontroller reads the log. 1), usbasploader immediately passes control to the user program (address 0).Here's a simple algorithm is run. 

In usbasploader have the opportunity to change the behavior of the bootloader, it is sufficient to edit the three functions (they are very simple, and are in a file bootloaderconfig.h) - bootLoaderInit, bootLoaderCondition, bootLoaderExit. Assignment of functions is almost obvious by their names. BootLoaderInit function is to configure the environment, which can monitor the condition of enhanced bootloader: 

static inline void bootLoaderInit( void ) 
{ 
DDRB |= (1 << PB0); //????? ??? ???????? ?????????? 
//?? PB5 (MOSI, ??????? 4 ?????????? U1 ISP) ?????????? pull-up ? ???????? ????????? 
PORTB |= (1 << PB5)|(1 << PB0); 
}

The code is very simple - it just connects to the foot of PB5 plus internal pull-up resistor (pull-up) - it allows you to determine if the jumper between terminals 4 and 6 of connector U1 ISP, and the red LED lights on the maketke. The function is designed to test bootLoaderCondition - yes or no jumper between terminals 4 and 6: 

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
If the jumper is, the pin PB5 is read as a log. 0 and the function returns a bootLoaderCondition (which means - bootloader works). If the jumper is not, then the function returns 0, which means inactive for bootloader (control immediately sent to the address 0 - the user program). Function bootLoaderExit I did not do anything, just red LED extinguishes maketki: 

static inline void bootLoaderExit( void ) 
{ 
PORTB &= ~(1 << PB0); //????? ????????? 
}

The code for the functions bootLoaderInit, bootLoaderCondition, bootLoaderExit can be considered as an example - they can and should alter to your needs, and then usbasploader will work exactly as you want. In conclusion, I will describe the process step by step on the platform of Windows (it is assumed that you have already installed and the environment AVRStudio WinAVR. If not - read the instructions on how to install them, the link [ 6 ]). 

[How to integrate into your project usbasploader] 

A. Need to download the latest version of usbasploader (see [ 2 ]), for example USBaspLoader.2009-03-20.zip .If you have a breadboard AVR-USB-MEGA16, I suggest a link to download version [ 5 ] - there is all ready, and steps 2, 3, 4 can be skipped. Unpack to any convenient location. 

Two. Edit the setting in the Makefile. There should be changed: 
a) Identification F_CPU - frequency in Hz, which operates the microcontroller. Domustimy frequencies 12, 15, 16, 16.5 and 20 MHz. 
b) determining the DEVICE under your type of microcontroller. 
c) address (hex format, the units in bytes) of code loading usbasploader BOOTLOADER_ADDRESS. 
g) (optional if you do not flash the chip from the Makefile) to verify and if necessary, determine skorrektrovat FUSEOPT and LOCKOPT, as well as the definition of AVRDUDE. 

Three. Edit bootloaderconfig.h. There should be checked and if necessary change: 
a) macro USB_CFG_IOPORTNAME - letter of the port to which the signals are connected to USB D-and D +. 
b) macros, and USB_CFG_DMINUS_BIT USB_CFG_DPLUS_BIT - the ports are connected to the signals of D-and D +. D + may need to be connected to the leg of the interrupt INT0. 
c) the code of functions bootLoaderInit, bootLoaderExit and macro bootLoaderCondition. 

4. Recompile the project, for this type make. At the command prompt to see something like: 

 

After a successful compilation will get to the root directory of the project files and main.bin main.hex - ready firmware usbasploader. By the way, in your hexfiles already have compiled a few versions of firmware for the crystals ATmega8, ATmega88, ATmega168, to different frequencies of quartz. 

Five. You must use the programmer to flash the chip code usbasploader, properly install the fuse bits. The meaning of this operation - code usbasploader need to put in the upper memory area (address BOOTLOADER_ADDRESS), and set the fuse bits so chtby resetting or power began to run code of bootloader (I already wrote about it). Details on the fuse bits, see reference [ 4 ] and in the datasheet for your microcontroller. For the microcontroller ATmega32, for example, fuse bits should be set as follows: 
LOW FUSE BYTE: 0x CF 
HIGH FUSE BYTE: 0x D8 (can and 0x98, to allow the JTAG-debug) 
LOCKOPT BYTE: 0x EF 

Breadboard AVR-USB-MEGA16 can be purchased with an already flashed usbasploader and fuse bits, so steps 1, 2, 3, 4, 5 prodelyvat not necessary. 

6. You must connect stitched breadboard to a computer via USB. If you do not mess up, then maketka defined in the system as new hardware and Windows needs the driver. The driver can be downloaded from a page [ 2 ], or take from the archives of the link [ 5 ]. For Linux driver is not needed. 

7. Need one of the programs, working with the programmer USBasp (see references [3]). For Linux users will approach avrdude, but for Windows users, the choice is very wide. I recommend Khazama AVR Programmer with a very simple and convenient interface. 

 

 

[How to use the loader as an example usbasploader maketki AVR-USB-MEGA16 and Khazama AVR Programmer] 

A. Place a jumper between pins 4 and 6 of connector U1 ISP. 
 

Two. Connect the development board to the computer via USB. At maketke red LED, and is found in the Windows system programmer USBasp. 

Three. Run Khazama AVR Programmer. In the settings option to remove the garbage of the crystal (Command -> Program Options -> uncheck Erase Chip). Select from the drop down list your chip. Download the hex-firmware (via the menu File -> Load FLASH file to Buffer). Press the big button Auto Program, launching the programming.Programmable chip very quickly, within a few seconds. 
 
After programming the LED will go out and run your program starts at address 0 (that you just recorded). 

4. Remove the jumper between pins 4 and 6 of connector U1 ISP. 
 

UPD100711: wrote an article about another USB bootloader - BootloadHID. For the source code and work style is very similar to USBASPloader. It differs in that it requires special software on your computer, but it is easier to junior embedded chips (from the ATmega8), because the reduced size requirements bootloader-section.
