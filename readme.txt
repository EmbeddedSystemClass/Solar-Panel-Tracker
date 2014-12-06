Updates:

04-13-2013
    1) Added rit_isr.h to provide API to setup repetitive callback which also provides
        a free running timer for free.  RIT uses higher priority than FreeRTOS and if
        it sees that FreeRTOS is not running, it will set BASEPRI register to zero to
        ensure that accidental use of FreeRTOS API hasn't set the BASEPRI to non-zero
        therefore blocking our interrupts higher than IP_SYSCALL
    2) Fixed bug at uart_base.cpp for timeout without FreeRTOS.
    3) Changed uart_base.cpp such that last activity time is when a char was received
        such that when a char is sent, it is not considered as any activity.
    4) Fixed bug at uart_iface.cpp for gets()

03-29-2013
    Removed memory configuration options.  Now heap can use almost all of 64Kb memory
    
02-01-2013:
    Added telemetry system.
    Newlib hooks added to use C FILE API, such as fopen() and fclose()
    
10-16-2012:
    Added option to optimize SPI Flash write operation
    Changed UART0 PCLK to CPU Clock instead of CPU/4 to support higher baud rates
    
10-07-2012:
    Fixed critical FreeRTOS Priority bug by adding interruptPriorities.h
    Included RTC Semaphore for FreeRTOS
    DMA for SSP1
    Added C++ Interface inheritance for UART driver
    Added Boot information by L0/bootType.h
    Added bit manipulation by L0/bit_manip.h
    Added watchdog timer L0/watchdog.h
    Added different memory schemes by configuring it at sys_config.h
    Added more functionality to storage.h
    Enabled sleep-mode configuration for temperature sensor.
    
09-16-2012:
    str needed str.printf() fixed when string couldn't be printed due to insufficient capacity.
    FATFS needed fix to get_fattime() due to change to RTC API()