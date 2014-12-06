/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

#include <stdio.h>              // printf()
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"               // vTaskList()

#include "command_handler.hpp"  // CMD_HANDLER_FUNC()
#include "rtc.h"                // Set and Get System Time
#include "boot_type.h"          // getBootTime();

#include "utilities.h"          // printMemoryInfo()
#include "storage.hpp"          // Get Storage Device instances
#include "file_logger.hpp"      // Logger class
#include "watchdog.h"           // rebootHandler()

#include "uart0.hpp"
#include "uart3.hpp"
#include "io.hpp"
#include "handles.h"
#include "c_tlm_stream.h"
#include "c_tlm_var.h"



static UartIface& getOutputIface()
{
    Uart0& uart0 = Uart0::getInstance();
#if ENABLE_UART3_TERMINAL
    Uart3& uart3 = Uart3::getInstance();

    if (uart3.recentlyActive()) {
        return uart3;
    } else {
        return uart0;
    }
#else
    return uart0;
#endif
}

CMD_HANDLER_FUNC(taskListHandler)
{
    const int delayInMs = (int)cmdParams;  // cast parameter str to integer

    if(delayInMs > 0) {
        // Need to printf because output str won't get printed until this function exits
        vTaskResetRunTimeStats();
        vTaskDelayMs(delayInMs);
    }

    // Warning: We need roughly 50 chars of space per task
    output.reserve(1024);
    output.printf("%10s  S Pr  %5s  # CPU  Ticks\n", "Name", "Stack");

    // Append to end of output str
    signed char *endOfOutputStr = (signed char*) (output.c_str() + output.getLen());
    vTaskList(endOfOutputStr);
}

CMD_HANDLER_FUNC(memInfoHandler)
{
    output.reserve(512);
    printMemoryInfo((char*)output.c_str());
}

CMD_HANDLER_FUNC(healthHandler)
{
    Uart0 &u0 = Uart0::getInstance();

    unsigned int total = 0;
    unsigned int available = 0;
    Storage::getFlashDrive().getDriveInfo(&total, &available);

    float floatTemp = TS.getFarenheit();
    int floatSig1 = (int) floatTemp;
    int floatDec1 = ((floatTemp - floatSig1) * 10);
    RTC bt = getBootTime();

    char dateTime[24] = { 0 };
    rtc_getDateTimeString(dateTime);

    output.printf("Flash: %u/%u\n"
                    "Temp : %u.%u\n"
                    "Light: %u\n"
                    "Time : %s\n"
                    "Boot Time: %u/%u/%u,%02u:%02u:%02u\n"
                    "Rx/Tx Queue Watermarks: Uart0: %u/%u",
                    available, total,
                    floatSig1, floatDec1,
                    LS.getRawValue(),
                    dateTime,
                    bt.month, bt.day, bt.year, bt.hour, bt.min, bt.sec,
                    u0.getRxQueueWatermark(), u0.getTxQueueWatermark()
    );
}

CMD_HANDLER_FUNC(timeHandler)
{
    RTC time;

    /**
     * If cmdParam contains "set" with six spaces, we can parse the time
     * Example: set 11 30 2014 8 25 0 1
     */
    if(cmdParams.beginsWith("set"))
    {
        if(cmdParams.countOf(" ") != 7) {
            output = "Need time in terms of MM DD YYYY HH MM SS Wday";
            return;
        }

        unsigned m, d, y, hr, mn, sc, w;
        if( 7 != cmdParams.scanf("%*s %u %u %u %u %u %u %u", &m, &d, &y, &hr, &mn, &sc, &w))
        {
            output = "Parse failure";
            return;
        }

        time.month = m; time.day = d; time.year = y;
        time.hour = hr, time.min = mn; time.sec = sc;
        time.dow = w;
        rtc_settime(&time);
    }

    time = rtc_gettime();
    output.printf("%02u/%02u/%u  --  %02u:%02u:%02u (%u)",
                    time.month, time.day, time.year,
                    time.hour, time.min, time.sec, time.dow);
}

CMD_HANDLER_FUNC(logHandler)
{
    if (cmdParams == "flush") {
        FileLogger::getInstance().flush();
        output = "Log(s) have been flushed";
    }
    else {
        LOG_INFO(cmdParams());
        output.printf("Logged: |%s|", cmdParams());
    }
}

CMD_HANDLER_FUNC(cpHandler)
{
    char srcFile[32] = {0};
    char dstFile[32] = {0};
    if(2 != cmdParams.scanf("%30s %30s", srcFile, dstFile)) {
        output = "Error, Try: 'cp <src file name> <dst file name>'";
        return;
    }

    unsigned int readTimeMs = 0;
    unsigned int writeTimeMs = 0;
    unsigned int bytesTransferred = 0;
    FRESULT copyStatus = Storage::copy(srcFile, dstFile,
                                       &readTimeMs, &writeTimeMs, &bytesTransferred);

    if(FR_OK != copyStatus) {
        output.printf("Error %u copying |%s| -> |%s|\n", copyStatus, srcFile, dstFile);
    }
    else {
        output.printf("Finished!  Read: %u Kb/sec, Write: %u Kb/sec\n",
                      bytesTransferred/(0 == readTimeMs  ? 1 : readTimeMs),
                      bytesTransferred/(0 == writeTimeMs ? 1 : writeTimeMs));
    }
}

CMD_HANDLER_FUNC(catHandler)
{
    // If -print was present, we will print to console
    bool printToScreen = !cmdParams.erase("-noprint");
    cmdParams.trimStart(" ");
    cmdParams.trimEnd(" ");

    FIL file;
    if(FR_OK != f_open(&file, cmdParams(), FA_OPEN_EXISTING | FA_READ))
    {
        output.printf("Failed to open: %s\n", cmdParams());
    }
    else
    {
        // Extra char for null terminator
        char buffer[1024 + 1] = { 0 };
        unsigned int bytesRead = 0;
        unsigned int totalBytesRead = 0;

        unsigned int startTime = xTaskGetMsCount();
        UartIface& out = getOutputIface();

        while(FR_OK == f_read(&file, buffer, sizeof(buffer)-1, &bytesRead) && bytesRead > 0)
        {
            totalBytesRead += bytesRead;
            buffer[bytesRead] = '\0';

            if(printToScreen) {
                out.put(buffer);
            }
        }
        f_close(&file);

        if(!printToScreen) {
            unsigned int timeTaken = xTaskGetMsCount() - startTime;
            // Divide by zero check
            if(0 == timeTaken) {
                timeTaken = 1;
            }
            output.printf("Read %u bytes @ %u Kb/sec\n", totalBytesRead, totalBytesRead/timeTaken);
        }
    }
}

CMD_HANDLER_FUNC(lsHandler)
{
    DIR Dir;
    FILINFO Finfo;
    FATFS *fs;
    FRESULT returnCode = FR_OK;

    unsigned int fileBytesTotal, numFiles, numDirs;
    fileBytesTotal = numFiles = numDirs = 0;
    #if _USE_LFN
        char Lfname[_MAX_LFN];
    #endif

    char dirPath[64] = { 0 };
    int lsOffset = 0;
    cmdParams.scanf("%60s %i", dirPath, &lsOffset);
    if (FR_OK != (returnCode = f_opendir(&Dir, dirPath))) {
        output.printf("Invalid directory: |%s|\n", dirPath);
        return;
    }

    // Offset the listing
    while(lsOffset-- > 0 && FR_OK == f_readdir(&Dir, &Finfo)) {
        ;
    }

    UartIface& out = getOutputIface();
    out.printf("Directory listing of: %s\n\n", dirPath);

    for (;;)
    {
        #if _USE_LFN
            Finfo.lfname = Lfname;
            Finfo.lfsize = sizeof(Lfname);
        #endif

        char returnCode = f_readdir(&Dir, &Finfo);
        if ( (FR_OK != returnCode) || !Finfo.fname[0]) {
            break;
        }

        if (Finfo.fattrib & AM_DIR){
            numDirs++;
        }
        else{
            numFiles++;
            fileBytesTotal += Finfo.fsize;
        }
        out.printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %10lu %13s",
                (Finfo.fattrib & AM_DIR) ? 'D' : '-',
                (Finfo.fattrib & AM_RDO) ? 'R' : '-',
                (Finfo.fattrib & AM_HID) ? 'H' : '-',
                (Finfo.fattrib & AM_SYS) ? 'S' : '-',
                (Finfo.fattrib & AM_ARC) ? 'A' : '-',
                (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
                (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
                Finfo.fsize, &(Finfo.fname[0]));

        // LFN names tend to increase memory requirements for output str, enable with caution
        #if (_USE_LFN)
            out.put(" - ");
            out.put(Lfname);
        #endif
        out.puts("");
    }
    out.printf("\n%4u File(s), %10u bytes total\n%4u Dir(s)", numFiles, fileBytesTotal, numDirs);

    if (f_getfree(dirPath, (DWORD*) &fileBytesTotal, &fs) == FR_OK)
    {
        out.printf(", %10uK bytes free\n", fileBytesTotal * fs->csize / 2);
    }
}

CMD_HANDLER_FUNC(mkdirHandler)
{
    output = (FR_OK == f_mkdir(cmdParams())) ? "OK" : "ERROR";
}

CMD_HANDLER_FUNC(rmHandler)
{
    output.printf("Delete '%s' : %s", cmdParams(), FR_OK == f_unlink(cmdParams()) ? "OK" : "ERROR");
}

CMD_HANDLER_FUNC(mvHandler)
{
    char srcFile[99] = {0};
    char dstFile[99] = {0};
    if(2 != cmdParams.scanf("%90s %90s", srcFile, dstFile)) {
        output = "Error, Try: 'mv <src file name> <dst file name>'";
    }
    else {
        output = (FR_OK == f_rename(srcFile, dstFile))  ? "File has been moved" : "Error moving file";
    }
}

CMD_HANDLER_FUNC(dcpHandler)
{
    DIR Dir;
    FILINFO Finfo;
    FRESULT returnCode = FR_OK;
    #if _USE_LFN
        char Lfname[_MAX_LFN];
    #endif

    char srcDir[64] = { 0 };
    char dstDir[64] = { 0 };
    char srcName[128] = { 0 };
    char dstName[128] = { 0 };
    if(2 != cmdParams.scanf("%60s %60s", srcDir, dstDir)) {
        output = "ERROR: Give me source and destination directory separated by a space";
        return;
    }

    /* Check destination directory */
    if (FR_OK != (returnCode = f_opendir(&Dir, dstDir))) {
        output.printf("Invalid source directory: |%s|\n", srcDir);
        return;
    }
    /* Check source directory */
    if (FR_OK != (returnCode = f_opendir(&Dir, srcDir))) {
        output.printf("Invalid source directory: |%s|\n", srcDir);
        return;
    }

    UartIface& uart = getOutputIface();
    for (;;)
    {
        #if _USE_LFN
            Finfo.lfname = Lfname;
            Finfo.lfsize = sizeof(Lfname);
        #endif

        /* If no more files */
        if ( (FR_OK != f_readdir(&Dir, &Finfo)) || !Finfo.fname[0]) {
            break;
        }

        /* If not a directory */
        if (!(Finfo.fattrib & AM_DIR))
        {
            strcpy(srcName, srcDir); strcat(srcName, "/"); strcat(srcName, Finfo.fname);
            strcpy(dstName, dstDir); strcat(dstName, "/"); strcat(dstName, Finfo.fname);

            uart.printf("Copy %s -> %s : %u Bytes : %s\n",
                           srcName, dstName, Finfo.fsize,
                           (FR_OK == Storage::copy(srcName, dstName)) ? "OK" : "ERROR");
        }
    }
}

CMD_HANDLER_FUNC(storageHandler)
{
    if(cmdParams == "format sd") {
        output = (FR_OK == Storage::getSDDrive().format()) ? "Format OK" : "Format ERROR";
    }
    else if(cmdParams == "format flash") {
        output = (FR_OK == Storage::getFlashDrive().format()) ? "Format OK" : "Format ERROR";
    }
    else if(cmdParams == "mount sd") {
        output = Storage::getSDDrive().mount() ? "SD Card mounted" : "Error mounting SD Card";
    }
    else if(cmdParams == "mount flash") {
        output = Storage::getFlashDrive().mount() ? "Flash mounted" : "Error mounting Flash Memory";
    }
    else {
        output = "Parameters are 'format sd', 'format flash', 'mount sd' and 'mount flash'";
    }
}

CMD_HANDLER_FUNC(rebootHandler)
{
    getOutputIface().puts("Rebooting System");

    // Flush out everything we can
    FileLogger::getInstance().flush();

    vTaskDelayMs(2000);
    watchdog_RebootSystem();
}

#if (ENABLE_TELEMETRY)
static void output_stream(const char* s, void *arg) { getOutputIface().put(s); }
CMD_HANDLER_FUNC(telemetryHandler)
{
    if(cmdParams.getLen() == 0) {
        tlm_stream_all(output_stream, NULL);
    }
    else if(cmdParams == "save") {
        FILE *fd = fopen(DISK_TLM_NAME, "w");
        tlm_stream_one_file(tlm_component_get_by_name(DISK_TLM_NAME), fd);
        fclose(fd);
        output = "Telemetry was saved to disk";
    }
    else {
        char compName[64] = { 0 };
        char varName[64]  = { 0 };
        char varVal[128]  = { 0 };
        if (3 != cmdParams.scanf("%63s %63s %127s", compName, varName, varVal)) {
            output = "ERROR: See 'help telemetry'";
        }
        else {
            if (tlm_variable_set_value(compName, varName, varVal)) {
                output.printf("%s:%s set to %s", compName, varName, varVal);
            } else {
                output.printf("Failed to set %s:%s to %s", compName, varName, varVal);
            }
        }
    }
}
#endif

CMD_HANDLER_FUNC(learnIrHandler)
{
    xSemaphoreGive(getHandles()->Sem.learnIrCodes);
    output = "Learning mode enabled";
}
