4 ���� 2010
   ���� bootloader � ������-�� ���� ������ - http://www.schatenseite.de/dulcimer.html?L=2, ���� ��� �� �� ����� bootloader, ��� � USBaspLoader �� Objective Development:
   [4]http://www.obdev.at/products/avrusb/usbasploader.html

   usbasploader ��������� ��������������� ISP-������������, ������� �������������� avrdude. ����� �������, �� �� ����� ��������� ����� �������������� ��� ���������������� ���� ����� bootloader, ������� ������������ � ��� bootloader.

   ����� ����������� ATmega32, �� ������ ���������� ��� � ������ ���������� � ISP-��������������, ������� �� �������, �������� makefile �������������� ����� ���������� (������� ������, ��� ����������), ����������������� � AVRStudio ������. ����� ����� �������� usbasploader.hex ����� ��������������, ��� ���� ���������� ��������� ����� � ���� ���������� (������� � makefile):
0xCF - ������� ���� fuses
0xD8 - ������� ���� fuses
0x2f - ���� ��� ����������

   �� ���������� ��������� ��� ���������� (lock) ��� bootloader������ �� ������: �� ������ ������ �������� ������ ������� ��������������. ����������, ��� ������������� � ������ ������������� ���� �������� ������������� ��� ���������� ����, ��� ������������� ����� ����������� �����. ���������� ������ ������ �� ��������� bootloader.

   ����� ����� �� ������ ������ ��� ������������ � ������� �� ������������� � ������������ bootloader. �� �������� ��������� �������: ��� ��������� ��-�� ��� ���������� ������������� ���������� ���������� �� ��� bootloader (����� 7000h flash). ���� ����������� ��������� ����� ���������� 6 � 4 ���������� ISP, �� ����� MOSI (PB5) �������� �� �����, � ��� ������������ bootloader. ����� ���������� ���������� ���������������� ��������� �� ������ 0000h. ��� ���� � ������.
   
   ����� � ���������������� bootloader ��� ��� �������� ������ D+ � D- (��. � bootloaderconfig.h ������� USB_CFG_DPLUS_BIT � USB_CFG_DMINUS_BIT ��������������), �� � ���� ��� ����� bootloader �� ���������. ����� ��� ����������� bootloaderconfig.h -> bootLoaderInit, bootLoaderExit, bootLoaderCondition, ����� ��� ��������������� ���� ���������.
   
[���� � bootLoaderInit]
/**
 * Prepare IO-ports for detection of bootloader-condition, which happens in
 * bootLoaderCondition().
 */
static inline void bootLoaderInit(void)
{
    // switch on leds
    DDRD  |= (1 << PIND4) | (1 << PIND5) | (1 << PIND6);
    PORTD &= ~((1 << PIND4) | (1 << PIND5) | (1 << PIND6));
    // choose matrix position for hotkey. we use KEY_KPminus, so we set row 13
    // and later look for pin 7
    DDRA  = 0x00;
    PORTA = 0xff;
    DDRC  = (1 << DDC2);
    PORTC = ~(1 << PINC2);
} 

[����� � bootLoaderInit]
static inline void bootLoaderInit(void) 
{
    DDRB  |= (1 << PB0);   //auoia aey e?aniiai naaoiaeiaa
    PORTB |= (1 << PB5)|(1 << PB0);   //ia PB5 (MOSI, eiioaeo 4 eiiiaeoi?a U1 ISP) iiaee??aai pull-up
    // boot loader active, blink led
    _delay_ms(1);
    PORTB &= ~(1 << PB0);
}

[���� � bootLoaderExit]
/**
 * Clean up after boot loader action. In this case: switch off all LEDs.
 */
static inline void bootLoaderExit(void) 
{
    DDRB  |= (1 << PB0);   //��������� ����� ��� ����� ��� �������� ���������� ������� AVR-USB-MEGA16
    //�� PB5 (MOSI, ������� 4 ���������� U1 ISP) ���������� pull-up, � ����� �������� ������� ���������
    PORTB |= (1 << PB5)|(1 << PB0);
}

[����� � bootLoaderExit]
static inline void bootLoaderExit(void) 
{
    OFF_RED();
}

[���� � bootLoaderCondition]
/**
 * Check if conditions for boot loader are met. This function is called in an
 * endless loop, so we use our spare time to display a nice running light on
 * the LEDs.
 * \return 1 if bootloader should be active, 0 otherwise
 */
static inline uint8_t bootLoaderCondition() 
{
    // look for pin 7
    if (!(PINB & (1 << PINB7))) {
        // boot loader active, blink leds
        _delay_ms(1);
        ledcounter++;
        if (ledcounter == 127) {
            switch (ledstate) {
                case 0:
                    PORTD &= ~(1 << PIND6);
                    PORTD |= (1 << PIND4) | (1 << PIND5);
                    ledstate = 1;
                    break;
                case 1:
                    PORTD &= ~(1 << PIND5);
                    PORTD |= (1 << PIND4) | (1 << PIND6);
                    ledstate = 2;
                    break;
                case 2:
                    PORTD &= ~(1 << PIND4);
                    PORTD |= (1 << PIND5) | (1 << PIND6);
                    ledstate = 0;
                    break;
                default:
                    ledstate = 0;
            }
            ledcounter = 0;
        }
        return 1;
    } else {
        // no boot loader
        return 0;
    }
}

[����� � bootLoaderCondition]
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

����� ����� ��� ����������.

------------------------------------------------------------------------
5 ���� 2010
   ������� ��������� �������� ������ - �������� �������� ����������.
   
[������ ��� �����]
[USB bootloader ��� ����������������� AVR]

� ������ �����������, ��� ������ ��������� <b>USB</b> bootloader ��� ����������������� ATmega16 � ATmega32 �� ������� ���������� <b>usbasploader</b> �������� Objective Development.<habracut/>

���������� USB bootloader ���� ����-������������ ����������� - ���������� ������������� � ����������� ������������� ��� ������ ������������ ����������� (<b>firmware</b>) � ������� - ���������� ����������� � ���������� �� USB. ������ ��� ������������� ������� � ����� ������������, ��� ��� ��������� � USB ���� �����. ���� ���� �������� ����� �������� bootloader, �� ����� ���������� ������ �� ������� ������������� ��� ����� �� ��� ������������ - ��� ������ ����������� ��� ����������.

��� AVR ���������� ����� ����� bootloader-�� - ��. [1]. ��� ������ � ������ ������ usbasploader [2] ������, ��� �� ��������� � ����� ���������� �������������� <b>USBasp</b>. ��� ��������� ������������� firmware ��� ��� Linux, ��� � ��� Windows � �������������� ���������� �������� (��. [3]) avrdude, eXtreme Burner - AVR, Khazama AVR Programmer � ���� �� ����� ���������������� BASCOM-AVR (������� ���������������� �� ������� BASCOM-AVR - ��������� ����, ��������� ��������� ������). ����� ����, usbasploader ������������ �� ����� �����������, ������� ���������������� � ����� ������������ ��� ����� ������������ (�� ���� �����).

��������� usbasploader �������� ����� ������ - ��� ����������� � USB �� ������������� �������������� USBasp. ������� ��� ���������, ��� ��������������, ����� �������������� firmware � ����� ����������, ��� ����� ��� ���������� ������������ USBasp. Bootloader ���������� � ������� ������� flash-������ �������� ����������������, � ���������� ��������� ������������ � ������� ������ (������ ������� � ������ 0), �. �. ��� ������������ ���������������� bootloader �� ����������. ����� ��������� ������ bootloader �������� ���������� ��������� ������������. ������ ���������, ��� ��� �������� � ������������������ ATmega, �� ������� <b>ATmega32</b>.

��� ATmega32 usbasploader ������������� ���, ����� �� ���������� �� flash � ������ 7000h (�������, ��� �������� ������������ ������ �������� ATmega32 ���������� 0000h..7FFFh, � ����� ������� ������ ���� ������, �. �. ������ ������ ����� � ��������� 0000h..37FFh). ��� ��������� ������������ �������� ��������� ������������ 0000h..6FFFh (28672 ����). ��������� usbasploader ���������� ���������� � ��������������� ATmega32 ����������� ������� � ������, �������������� � ������� ������� ������ �������� (�������� ��������� ������������� �������, ���������� ����������� - �������, ��. [4]). ����� ��� ��������� ������� � ������ ���������� ������ ������������ �� ����� 0x7000, ���������� ����������������� ��������� (�����, fuse-bits) <b>BOOTSZ0</b> � <b>BOOTSZ1</b> � ��������������� ��������� (��� bootloader ������ ���������� 4000h ����, ��� 2048 ���� ���� ���������), � ����� ����������������� ��������� <b>BOOTRST</b>. ����� ����� ��� ��� ������ ��� ��������� ����� ���������� �� � ������ 0, � � ������ 3800h � ������ ������ AVR, ��� � ������ 7000h � ������ flash (�������, ��� ����������� ������ ������� AVR - ��� �����).

������� ���������� ����� ������, ��� usbasploader ��������� ������� ����� �����������, �. �. ������, �� �������� �� ������ ������ �������� ��� ������������ USBasp. ������ ����� �������� �������� ��������� �����-������ ����� �� �����. ��� �������� ����� � �������� ����� ����� � ������ <b>PB5</b> ���������������� ATmega32. ��� ����� �������� ������������ � �������� MOSI, ���������� �� ���������, ������� ���������� ����� ���������� 4 � 6 ���������� U1 ISP ����� ������ �������� �� PB5 ������ ���. 0. ����, ��� usbasploader ��������� ������� �� ����� PB5, � ���� ��� ���. 0 (��������� ����� ������� 4 � 6 ���������� U1 ISP �����������), �� ���������� ������ ��� ������������� USBasp. �. �. ��� ������������� ��������� � ������ ��������� ������� bootloader ��������������, � ���� ��� ���������� � ����������, �� � ���������� �������� USB-���������� USBasp. ���� � ������ ��������� ������� ��������� ��� (��� ���� �� PB5 ����������������� �������� ���. 1), �� usbasploader ����� �������� ���������� � ��������� ������������ (�� ����� 0). ��� ����� �������� �������� �������.

� usbasploader ���� ����������� �������� ��������� bootloader, ��� ����� ���������� ��������������� ��� ������� (��� ����� �������, � ��������� � ����� bootloaderconfig.h) - <b>bootLoaderInit</b>, <b>bootLoaderCondition</b>, <b>bootLoaderExit</b>. ���������� ������� ����������� �������� �� ���������. ������� bootLoaderInit ������������� ��� ���������������� ���������, � ����� �������� ������������ ������� ����������� bootloader:
<code><blockquote><code><font size="2" face="Courier New" color="black"><font color="#0000ff">static</font> inline <font color="#0000ff">void</font> bootLoaderInit(<font color="#0000ff">void</font>) <br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;DDRB&nbsp;|= (1 &#60;&#60; PB0);&nbsp; <font color="#008000">//����� ��� �������� ����������</font><br>&nbsp;&nbsp;&nbsp;&nbsp;<font color="#008000">//�� PB5 (MOSI, ������� 4 ���������� U1 ISP) ���������� pull-up � �������� ���������</font><br>&nbsp;&nbsp;&nbsp;&nbsp;PORTB |= (1 &#60;&#60; PB5)|(1 &#60;&#60; PB0);&nbsp; <br>}</font></code></blockquote></code>
��� ����� ����� - �� ������ ���������� �� ����� PB5 ������������� � ����� ���������� ����������� �������� (pull-up) - ��� ��������� ���������� ������� ��������� ����� ���������� 4 � 6 ���������� U1 ISP, � �������� ������� ��������� �� �������. ������� bootLoaderCondition ������������� ��� �������� - ���� ��� ��� ��������� ����� ���������� 4 � 6:
<code><blockquote><code><font size="2" face="Courier New" color="black"><font color="#0000ff">static</font> inline uint8_t bootLoaderCondition() <br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;<font color="#0000ff">if</font> (!(PINB &#38; (1 &#60;&#60; PB5))) <br>&nbsp;&nbsp;&nbsp;&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#0000ff">return</font> 1;<br>&nbsp;&nbsp;&nbsp;&nbsp;} <br>&nbsp;&nbsp;&nbsp;&nbsp;<font color="#0000ff">else</font> <br>&nbsp;&nbsp;&nbsp;&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#008000">// no boot loader</font><br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#0000ff">return</font> 0;<br>&nbsp;&nbsp;&nbsp;&nbsp;}<br>}</font></code></blockquote></code>���� ��������� �����, �� ����� PB5 �������� ��� ���. 0, � ������� bootLoaderCondition ������ 1 (��� �������� - bootloader ��������). ���� ��������� ���, �� ������� ������ 0, ��� �������� ������������ ��� bootloader (���������� ���������� ���������� �� ������ 0 - � ��������� ������������). ������� bootLoaderExit � ���� �� ������ ������, ������ ����� ������� ��������� �������:
<code><blockquote><code><font size="2" face="Courier New" color="black"><font color="#0000ff">static</font> inline <font color="#0000ff">void</font> bootLoaderExit(<font color="#0000ff">void</font>) <br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;PORTB &#38;= ~(1 &#60;&#60; PB0);&nbsp; <font color="#008000">//����� ���������</font><br>}</font></code></blockquote></code>
����������� ��� ��� ������� bootLoaderInit, bootLoaderCondition, bootLoaderExit ����� ������������� � �������� ������� - �� ����� � ����� ���������� ��� ���� �����, � ����� usbasploader ����� �������� ������ ���, ��� �� ������. � ���������� ����� ������� �� ����� �� ��������� Windows (��������������, ��� � ��� ��� ����������� AVRStudio � ����� WinAVR. ���� ���, ������� ���������� �� ��������� �� ������ [6]). 

[<b>��� �������� � ��� ������ usbasploader</b>]

<b>1</b>. ����� ������� ��������� ������ usbasploader (��. [2]), �������� <a href="http://www.obdev.at/downloads/vusb/USBaspLoader.2009-03-20.zip">USBaspLoader.2009-03-20.zip</a>. ���� � ��� �������� ����� AVR-USB-MEGA16, �� ��������� ������� ������� �� ������ [5] - ��� ��� ��� ������, � ���� 2, 3, 4 ����� ����������. �������������� � ����� ������� �����.

<b>2</b>. �������������� ��������� � Makefile. ��� ����� ��������:
�) ����������� F_CPU - ������� � ��, �� ������� �������� ���������������. ��������� ������� 12, 15, 16, 16.5 � 20 ���.
�) ����������� DEVICE ��� ��� ��� ����������������.
�) ����� (������ �����������������, ������� � ������) �������� ���� usbasploader BOOTLOADER_ADDRESS.
�) (�������������, ���� �� ������ ��������� ��� �� �� Makefile) ��������� � ��� ������������� �������������� ����������� FUSEOPT � LOCKOPT, � ����� ����������� AVRDUDE.

<b>3</b>. �������������� bootloaderconfig.h. ��� ����� ��������� � ��� ������������� ��������:
�) ������ USB_CFG_IOPORTNAME - ����� ����� �����, � �������� ���������� ������� USB D- � D+.
�) ������� USB_CFG_DMINUS_BIT � USB_CFG_DPLUS_BIT - ������ ������, ������� ���������� ������� D- � D+. ������ D+ ����������� ������ ���� ��������� �� ����� ���������� INT0.
�) ��� ������� bootLoaderInit, bootLoaderExit � ������ bootLoaderCondition.

<b>4</b>. ���������������� ������, ��� ����� ������� make. � ��������� ������ ������� ���-�� ���������:

<img src="http://microsin.ru/images/stories/programming/USBaspLoader-compile.PNG" alt="image"/>
<code><!--
c:\asm\USBaspLoader.2009-03-20\firmware>make
avr-gcc -Wall -Os -I. -mmcu=atmega168 -DF_CPU=16000000  -x assembler-with-cpp -c
 usbdrv/usbdrvasm.s -o usbdrv/usbdrvasm.o
avr-gcc -Wall -Os -I. -mmcu=atmega168 -DF_CPU=16000000  -c usbdrv/oddebug.c -o u
sbdrv/oddebug.o
avr-gcc -Wall -Os -I. -mmcu=atmega168 -DF_CPU=16000000  -c main.c -o main.o
usbdrv/usbdrv.h:211: warning: 'usbFunctionDescriptor' used but never defined
avr-gcc -Wall -Os -I. -mmcu=atmega168 -DF_CPU=16000000  -o main.bin usbdrv/usbdr
vasm.o usbdrv/oddebug.o main.o -Wl,--section-start=.text=3800
rm -f main.hex main.eep.hex
avr-objcopy -j .text -j .data -O ihex main.bin main.hex
avr-size main.hex
   text    data     bss     dec     hex filename
      0    2120       0    2120     848 main.hex--></code>
����� �������� ���������� �������� � �������� ����� ������� ����� main.bin � main.hex - ������� �������� ��� usbasploader. ������, � ����� hexfiles ��� ���� ��������� ���������������� ������ �������� ��� ���������� ATmega8, ATmega88, ATmega168, �� ������ ������� ������.

<b>5</b>. ���������� � ������� ������������� ������� � ��� ��� usbasploader, ��������� ���������� �����. ����� ���� �������� - ��� usbasploader ���������� �������� � ������� ������� ������ (�� ������ BOOTLOADER_ADDRESS), � ���������� ����� ����� �������, ���� ��� ������ ��� ��������� ������� ����� ����������� ��� bootloader (� ��� �� ���� �����). ����������� �� ������ ��. �� ������ [4] � � �������� �� ��� ���������������. ��� ���������������� ATmega32, ��������, ����� ������ ���� ����������� ��������� �������:
<b>LOW FUSE BYTE</b>: 0x<b>CF</b>
<b>HIGH FUSE BYTE</b>: 0x<b>D8</b> (����� � 0x98, ����� ��������� JTAG-�������)
<b>LOCKOPT BYTE</b>:   0x<b>EF</b>

�������� ����� AVR-USB-MEGA16 ����� ���������� � ��� �������� usbasploader � �������, ������� ���� 1, 2, 3, 4, 5 ����������� �� �����.

<b>6</b>. ���������� ���������� �������� �������� ����� � ���������� �� USB. ���� ������ �� ��������, �� ������� ����������� � ������� ��� ����� ���������� � ������� Windows �������� �������. ������� ����� ������� �� ��������� [2], ��� ����� �� ������ �� ������ [5]. ��� Linux ������� �� �����.

<b>7</b>. ����� ���� �� ��������, ���������� � �������������� USBasp (��. ������ [3]). ��� ������������� Linux �������� avrdude, � ��� ������������� Windows ����� ����� �����. � ���������� Khazama AVR Programmer � ����� ������� � ������� �����������.

<img src="http://microsin.ru/images/stories/programming/khazama01.png" alt="image"/>

<img src="http://microsin.ru/images/stories/programming/khazama02.png" alt="image"/>

[<b>��� �������� � ����������� usbasploader �� ������� ������� AVR-USB-MEGA16 � Khazama AVR Programmer</b>]

<b>1</b>. ��������� ��������� ����� ������� 4 � 6 ���������� U1 ISP.
<img src="http://microsin.ru/images/stories/programming/USBasp-bootloader-ON-IMG_0879.JPG" alt="image"/>

<b>2</b>. ���������� �������� ����� � ���������� �� USB. �� ������� ��������� ������� ���������, � � ������� Windows ����������� ������������ USBasp.

<b>3</b>. ��������� ��������� Khazama AVR Programmer. � ���������� ������� ����� ������� ������ ��������� (Command -> Program Options -> ������� ����� Erase Chip). �������� �� ����������� ������ ��� ���. ��������� hex-���� �������� (����� ���� File -> Load FLASH file to Buffer). ������� ������� ������ Auto Program, ����������� ����������������. ��������������� �������� ����� ������, �� ��������� ������.
<img src="http://microsin.ru/images/stories/programming/khazama03.png" alt="image"/>
����� ��������� ���������������� ������� ��������� ��������, � ������ ����������� ���� ��������� � ������ 0 (������� �� ������ ��� ��������).

<b>4</b>. ������� ��������� ����� ������� 4 � 6 ���������� U1 ISP.
<img src="http://microsin.ru/images/stories/programming/USBasp-bootloader-OFF-IMG_0878.JPG" alt="image"/>
  
[<b>������</b>]

<b>1</b>. <a href="http://microsin.ru/content/view/1079/44/">���������� (bootloader) ��� ����������������� AVR</a>.
<b>2</b>. <a href="http://www.obdev.at/products/vusb/usbasploader.html">�������� ��������� usbasploader</a>.
<b>3</b>. ��������� ��� ������ � �������������� USBasp - <a href="http://download.savannah.gnu.org/releases/avrdude/">AVRDUDE</a>, <a href="http://www.mcselec.com/">BASCOM-AVR</a>, <a href="http://khazama.com/project/programmer/">Khazama AVR Programmer</a>, <a href="http://extremeelectronics.co.in/avr-tutorials/gui-software-for-usbasp-based-usb-avr-programmers/">eXtreme Burner- AVR</a>. 
<b>4</b>. <a href="http://www.engbedded.com/fusecalc/">Engbedded Atmel AVR� Fuse Calculator</a> - ����������� ��������� ��� AVR.
<b>5</b>. ��� ������� usbasploader, ���������� ��� �������� ����� AVR-USB-MEGA16 (������ ��� AVRStudio � ����������� � ����������������� ���������� ��� ������� 12 ���, 16 ���, � ����������������� ATmega16 � ATmega32).
<b>6</b>. <a href="http://microsin.ru/content/view/613/44/">���������� ���������� USB - ��� ������ ������ � ������������ AVR USB (V-USB) � libusb</a>.