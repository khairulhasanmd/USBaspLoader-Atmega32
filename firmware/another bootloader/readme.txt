4 июля 2010
   Этот bootloader я почесу-то взял отсюда - http://www.schatenseite.de/dulcimer.html?L=2, хотя это то же самый bootloader, что и USBaspLoader от Objective Development:
   [4]http://www.obdev.at/products/avrusb/usbasploader.html

   usbasploader эмулирует широкоизвестный ISP-программатор, который поддерживается avrdude. Таким образом, та же самая программа может использоваться для программирования чипа через bootloader, которая используется и без bootloader.

   Чтобы подготовить ATmega32, Вы должны подключить его к своему компьютеру с ISP-программатором, который Вы выбрали, изменить makefile соответственно Вашим настройкам (частота кварца, тип процессора), перекомпилировать в AVRStudio проект. После этого прошейте usbasploader.hex Вашим программатором, при этом установите правильно фьюзы и биты блокировки (указаны в makefile):
0xCF - младший байт fuses
0xD8 - старший байт fuses
0x2f - байт бит блокировки

   Не опасайтесь установки бит блокировки (lock) для bootloaderзащиты от записи: Вы можете всегда сбросить защиту обычным программатором. Фактически, это заблокировано в момент использования Вами обычного программатора для перезаписи чипа, без использования любых специальных опций. Блокировка влияет только на поведение bootloader.

   После этого Вы можете убрать Ваш программатор в коробку за ненадобностью и использовать bootloader. Он работает следующим образом: при включении из-за бит блокировки принудительно передается управление на код bootloader (адрес 7000h flash). Если установлена перемычка между контактами 6 и 4 коннектора ISP, то ножка MOSI (PB5) замкнута на землю, и это активизирует bootloader. Иначе управление передается пользовательской программе по адресу 0000h. Все тупо и просто.
   
   Когда я перекомпилировал bootloader под мои значения портов D+ и D- (см. в bootloaderconfig.h макросы USB_CFG_DPLUS_BIT и USB_CFG_DMINUS_BIT соответственно), то у меня все равно bootloader не заработал. Нужно еще переправить bootloaderconfig.h -> bootLoaderInit, bootLoaderExit, bootLoaderCondition, чтобы они соответствовали моим ожиданиям.
   
[Было в bootLoaderInit]
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

[Стало в bootLoaderInit]
static inline void bootLoaderInit(void) 
{
    DDRB  |= (1 << PB0);   //auoia aey e?aniiai naaoiaeiaa
    PORTB |= (1 << PB5)|(1 << PB0);   //ia PB5 (MOSI, eiioaeo 4 eiiiaeoi?a U1 ISP) iiaee??aai pull-up
    // boot loader active, blink led
    _delay_ms(1);
    PORTB &= ~(1 << PB0);
}

[Было в bootLoaderExit]
/**
 * Clean up after boot loader action. In this case: switch off all LEDs.
 */
static inline void bootLoaderExit(void) 
{
    DDRB  |= (1 << PB0);   //настройка ножки как выход для красного светодиода макетки AVR-USB-MEGA16
    //на PB5 (MOSI, контакт 4 коннектора U1 ISP) подключаем pull-up, а также зажигаем красный светодиод
    PORTB |= (1 << PB5)|(1 << PB0);
}

[Стало в bootLoaderExit]
static inline void bootLoaderExit(void) 
{
    OFF_RED();
}

[Было в bootLoaderCondition]
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

[Стало в bootLoaderCondition]
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

После этого все заработало.

------------------------------------------------------------------------
5 июля 2010
   Добавил индикацию процесса записи - миганием красного светодиода.
   
[Статья для Хабра]
[USB bootloader для микроконтроллеров AVR]

В статье описывается, как быстро запустить <b>USB</b> bootloader для микроконтроллеров ATmega16 и ATmega32 на примере загрузчика <b>usbasploader</b> компании Objective Development.<habracut/>

Технология USB bootloader дает одну-единственную возможность - отсутствие необходимости в специальном программаторе для замены программного обеспечения (<b>firmware</b>) в приборе - достаточно подключения к компьютеру по USB. Больше нет необходимости таскать с собой программатор, так как компьютер и USB есть везде. Если Ваша макетная плата оснащена bootloader, то можно сэкономить деньги на покупке программатора или время на его изготовлении - это бывает немаловажно для начинающих.

Для AVR понаделано очень много bootloader-ов - см. [1]. Для статьи я выбрал именно usbasploader [2] потому, что он совместим с очень популярным программатором <b>USBasp</b>. Это позволяет перепрошивать firmware как под Linux, так и под Windows с использованием популярных программ (см. [3]) avrdude, eXtreme Burner - AVR, Khazama AVR Programmer и даже из среды программирования BASCOM-AVR (система программирования на бейсике BASCOM-AVR - маленькое чудо, достойное отдельной статьи). Кроме того, usbasploader поставляется со всеми исходниками, отлично задокументирован и легко адаптируется под нужды пользователя (об этом далее).

Загрузчик usbasploader работает очень просто - при подключении к USB он прикидывается программатором USBasp. Поэтому все программы, его поддерживающие, будут перезаписывать firmware в Вашем устройстве, как будто они используют программатор USBasp. Bootloader расположен в старших адресах flash-памяти программ микроконтроллера, и записывает программу пользователя в младшие адреса (обычно начиная с адреса 0), т. е. при перепрошивке микроконтроллера bootloader не затирается. После окончания записи bootloader передает управление программе пользователя. Теперь подробнее, как это работает с микроконтроллерами ATmega, на примере <b>ATmega32</b>.

Для ATmega32 usbasploader скомпилирован так, чтобы он размещался во flash с адреса 7000h (напомню, что адресное пространство памяти программ ATmega32 составляет 0000h..7FFFh, и адрес команды кратен двум байтам, т. е. адреса команд лежат в диапазоне 0000h..37FFh). Для программы пользователя остается свободным пространство 0000h..6FFFh (28672 байт). Загрузчик usbasploader использует заложенную в микроконтроллер ATmega32 возможность запуска с адреса, расположенного в старших адресах памяти программ (доступно несколько фиксированных адресов, выбираемых перемычками - фьюзами, см. [4]). Чтобы при включении питания и сбросе управление всегда передавалось на адрес 0x7000, необходимо запрограммировать перемычки (фьюзы, fuse-bits) <b>BOOTSZ0</b> и <b>BOOTSZ1</b> в соответствующее состояние (под bootloader должно отводиться 4000h байт, или 2048 слов кода программы), а также запрограммировать перемычку <b>BOOTRST</b>. После этого код при сбросе или включении будет стартовать не с адреса 0, а с адреса 3800h в словах команд AVR, или с адреса 7000h в байтах flash (напомню, что минимальный размер команды AVR - два байта).

Получив управление после сброса, код usbasploader проверяет условие своей активизации, т. е. сигнал, по которому он должен начать работать как программатор USBasp. Обычно таким сигналом является замыкание какой-нибудь ножки на землю. Для макетной платы в качестве такой ножки я выбрал <b>PB5</b> микроконтроллера ATmega32. Эта ножка является одновременно и сигналом MOSI, выведенным на коннектор, поэтому перемычкой между контактами 4 и 6 коннектора U1 ISP очень удобно подавать на PB5 сигнал лог. 0. Итак, код usbasploader проверяет уровень на порте PB5, и если там лог. 0 (перемычка между ножками 4 и 6 коннектора U1 ISP установлена), то начинается работа как программатора USBasp. Т. е. при установленной перемычке в момент включения питания bootloader активизируется, и если его подключить к компьютеру, то в компьютере появится USB-устройство USBasp. Если в момент включения питания перемычки нет (при этом на PB5 микроконтроллером читается лог. 1), то usbasploader сразу передает управление в программу пользователя (на адрес 0). Вот такой нехитрый алгоритм запуска.

В usbasploader есть возможность изменить поведение bootloader, для этого достаточно отредактировать три функции (они очень простые, и находятся в файле bootloaderconfig.h) - <b>bootLoaderInit</b>, <b>bootLoaderCondition</b>, <b>bootLoaderExit</b>. Назначение функций практически очевидно по названиям. Функция bootLoaderInit предназначена для конфигурирования окружения, к тором возможно отслеживание условие активизации bootloader:
<code><blockquote><code><font size="2" face="Courier New" color="black"><font color="#0000ff">static</font> inline <font color="#0000ff">void</font> bootLoaderInit(<font color="#0000ff">void</font>) <br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;DDRB&nbsp;|= (1 &#60;&#60; PB0);&nbsp; <font color="#008000">//выход для красного светодиода</font><br>&nbsp;&nbsp;&nbsp;&nbsp;<font color="#008000">//на PB5 (MOSI, контакт 4 коннектора U1 ISP) подключаем pull-up и зажигаем светодиод</font><br>&nbsp;&nbsp;&nbsp;&nbsp;PORTB |= (1 &#60;&#60; PB5)|(1 &#60;&#60; PB0);&nbsp; <br>}</font></code></blockquote></code>
Код очень прост - он только подключает на ножку PB5 подтягивающий к плюсу внутренний нагрузочный резистор (pull-up) - это позволяет определить наличие перемычки между контактами 4 и 6 коннектора U1 ISP, и зажигает красный светодиод на макетке. Функция bootLoaderCondition предназначена для проверки - есть или нет перемычка между контактами 4 и 6:
<code><blockquote><code><font size="2" face="Courier New" color="black"><font color="#0000ff">static</font> inline uint8_t bootLoaderCondition() <br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;<font color="#0000ff">if</font> (!(PINB &#38; (1 &#60;&#60; PB5))) <br>&nbsp;&nbsp;&nbsp;&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#0000ff">return</font> 1;<br>&nbsp;&nbsp;&nbsp;&nbsp;} <br>&nbsp;&nbsp;&nbsp;&nbsp;<font color="#0000ff">else</font> <br>&nbsp;&nbsp;&nbsp;&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#008000">// no boot loader</font><br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#0000ff">return</font> 0;<br>&nbsp;&nbsp;&nbsp;&nbsp;}<br>}</font></code></blockquote></code>Если перемычка стоит, то ножка PB5 читается как лог. 0, и функция bootLoaderCondition вернет 1 (что означает - bootloader работает). Если перемычки нет, то функция вернет 0, что означает неактивность для bootloader (управление немедленно передается по адресу 0 - в программу пользователя). Функция bootLoaderExit у меня не делает ничего, только гасит красный светодиод макетки:
<code><blockquote><code><font size="2" face="Courier New" color="black"><font color="#0000ff">static</font> inline <font color="#0000ff">void</font> bootLoaderExit(<font color="#0000ff">void</font>) <br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;PORTB &#38;= ~(1 &#60;&#60; PB0);&nbsp; <font color="#008000">//гасим светодиод</font><br>}</font></code></blockquote></code>
Приведенный код для функций bootLoaderInit, bootLoaderCondition, bootLoaderExit можно рассматривать в качестве примера - их можно и нужно переделать под Ваши нужды, и тогда usbasploader будет работать именно так, как Вы хотите. В заключение опишу процесс по шагам на платформе Windows (предполагается, что у Вас уже установлены AVRStudio и среда WinAVR. Если нет, читайте инструкции по установке по ссылке [6]). 

[<b>Как встроить в Ваш проект usbasploader</b>]

<b>1</b>. Нужно скачать последнюю версию usbasploader (см. [2]), например <a href="http://www.obdev.at/downloads/vusb/USBaspLoader.2009-03-20.zip">USBaspLoader.2009-03-20.zip</a>. Если у Вас макетная плата AVR-USB-MEGA16, то предлагаю скачать вариант по ссылке [5] - там уже все готово, и шаги 2, 3, 4 можно пропустить. Распаковываете в любую удобную папку.

<b>2</b>. Отредактируйте установки в Makefile. Там нужно поменять:
а) определение F_CPU - частоту в Гц, на которой работает микроконтроллер. Домустимы частоты 12, 15, 16, 16.5 и 20 МГц.
б) определение DEVICE под Ваш тип микроконтроллера.
в) адрес (формат шестнадцатеричный, единицы в байтах) загрузки кода usbasploader BOOTLOADER_ADDRESS.
г) (необязательно, если Вы будете прошивать чип не из Makefile) проверить и при необходимости скорректровать определения FUSEOPT и LOCKOPT, а также определение AVRDUDE.

<b>3</b>. Отредактируйте bootloaderconfig.h. Там нужно проверить и при необходимости поменять:
а) макрос USB_CFG_IOPORTNAME - буква имени порта, к которому подключены сигналы USB D- и D+.
б) макросы USB_CFG_DMINUS_BIT и USB_CFG_DPLUS_BIT - номера портов, которым подключены сигналы D- и D+. Сигнал D+ обязательно должен быть подключен на ножку прерывания INT0.
в) код функций bootLoaderInit, bootLoaderExit и макрос bootLoaderCondition.

<b>4</b>. Перекомпилируйте проект, для этого введите make. В командной строке увидите что-то наподобие:

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
После успешной компиляции получите в корневой папке проекта файлы main.bin и main.hex - готовая прошивка для usbasploader. Кстати, в папке hexfiles уже есть несколько скомпилированных версий прошивок для кристаллов ATmega8, ATmega88, ATmega168, на разные частоты кварца.

<b>5</b>. Необходимо с помощью программатора прошить в чип код usbasploader, правильно установить фьюзы. Смысл этой операции - код usbasploader необходимо положить в верхнюю область памяти (по адресу BOOTLOADER_ADDRESS), и установить фьюзы таким образом, чтбы при сбросе или включении питания начал выполняться код bootloader (я уже об этом писал). Подробности по фьюзам см. по ссылке [4] и в даташите на Ваш микроконтроллер. Для микроконтроллера ATmega32, например, фьюзы должны быть установлены следующим образом:
<b>LOW FUSE BYTE</b>: 0x<b>CF</b>
<b>HIGH FUSE BYTE</b>: 0x<b>D8</b> (можно и 0x98, чтобы разрешить JTAG-отладку)
<b>LOCKOPT BYTE</b>:   0x<b>EF</b>

Макетную плату AVR-USB-MEGA16 можно приобрести с уже прошитым usbasploader и фьюзами, поэтому шаги 1, 2, 3, 4, 5 проделывать не нужно.

<b>6</b>. Необходимо подключить прошитую макетную плату к компьютеру по USB. Если ничего не напутали, то макетка определится в системе как новое устройство и система Windows запросит драйвер. Драйвер можно скачать со странички [2], или взять из архива по ссылке [5]. Для Linux драйвер не нужен.

<b>7</b>. Нужна одна из программ, работающих с программатором USBasp (см. ссылки [3]). Для пользователей Linux подойдет avrdude, а для пользователей Windows выбор очень широк. Я рекомендую Khazama AVR Programmer с очень простым и удобным интерфейсом.

<img src="http://microsin.ru/images/stories/programming/khazama01.png" alt="image"/>

<img src="http://microsin.ru/images/stories/programming/khazama02.png" alt="image"/>

[<b>Как работать с загрузчиком usbasploader на примере макетки AVR-USB-MEGA16 и Khazama AVR Programmer</b>]

<b>1</b>. Поставьте перемычку между ножками 4 и 6 коннектора U1 ISP.
<img src="http://microsin.ru/images/stories/programming/USBasp-bootloader-ON-IMG_0879.JPG" alt="image"/>

<b>2</b>. Подключите макетную плату к компьютеру по USB. На макетке загорится красный светодиод, и в системе Windows обнаружится программатор USBasp.

<b>3</b>. Запустите программу Khazama AVR Programmer. В настройках уберите опцию очистки памяти кристалла (Command -> Program Options -> снимите галку Erase Chip). Выберите из выпадающего списка Ваш чип. Загрузите hex-файл прошивки (через меню File -> Load FLASH file to Buffer). Нажмите большую кнопку Auto Program, запускающую программирование. Программируется кристалл очень быстро, за несколько секунд.
<img src="http://microsin.ru/images/stories/programming/khazama03.png" alt="image"/>
После окончания программирования красный светодиод погаснет, и начнет выполняться Ваша программа с адреса 0 (которую Вы только что записали).

<b>4</b>. Снимите перемычку между ножками 4 и 6 коннектора U1 ISP.
<img src="http://microsin.ru/images/stories/programming/USBasp-bootloader-OFF-IMG_0878.JPG" alt="image"/>
  
[<b>Ссылки</b>]

<b>1</b>. <a href="http://microsin.ru/content/view/1079/44/">Загрузчики (bootloader) для микроконтроллеров AVR</a>.
<b>2</b>. <a href="http://www.obdev.at/products/vusb/usbasploader.html">Домашняя страничка usbasploader</a>.
<b>3</b>. Программы для работы с программатором USBasp - <a href="http://download.savannah.gnu.org/releases/avrdude/">AVRDUDE</a>, <a href="http://www.mcselec.com/">BASCOM-AVR</a>, <a href="http://khazama.com/project/programmer/">Khazama AVR Programmer</a>, <a href="http://extremeelectronics.co.in/avr-tutorials/gui-software-for-usbasp-based-usb-avr-programmers/">eXtreme Burner- AVR</a>. 
<b>4</b>. <a href="http://www.engbedded.com/fusecalc/">Engbedded Atmel AVR® Fuse Calculator</a> - калькулятор перемычек для AVR.
<b>5</b>. Мой вариант usbasploader, заточенный под макетную плату AVR-USB-MEGA16 (проект для AVRStudio с исходниками и скомпилированными вариантами для кварцев 12 МГц, 16 МГц, и микроконтроллеров ATmega16 и ATmega32).
<b>6</b>. <a href="http://microsin.ru/content/view/613/44/">Разработка устройства USB - как начать работу с библиотеками AVR USB (V-USB) и libusb</a>.