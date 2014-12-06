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

#include <stdio.h>      // printf
#include <string.h>     // memset() etc.
#include "FreeRTOS.h"
#include "semphr.h"

#include "io_functions.h"       // stdio set IO functions
#include "uart0.hpp"            // Interrupt driven UART0 driver
#include "uart3.hpp"

#include "utilities.h"          // PRINT_EXECUTION_SPEED()
#include "handlers.hpp"         // Command-line handlers

#include "file_logger.hpp"      // Logging
#include "io.hpp"               // For Board IO
#include "watchdog.h"           // For watchdog reboot

#include "handles.h"
#include "tasks.hpp"
#include "c_tlm_var.h"
#include "c_tlm_comp.h"
#include "c_tlm_stream.h"
#include "c_tlm_binary.h"


#include "LightSensors/lightSensor.hpp"
#include "ServoMotors/ServoMotor.hpp"

#define start_degree (double)(6) // start degree
#define middle_degree (double)(90) // middle degree
#define end_degree (double)(174) //  end degree

//#define SIZE 96
//char dataMem[SIZE];
/**
 * Gets a line of input with backspace support and stores into str s
 * @param maxLen Maximum chars to store into s before leaving this function.
 */
bool getline(str& s, const int maxLen);

static UartIface& getOutputIface()
{
    Uart0& uart0 = Uart0::getInstance();
#if 0
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

terminalTask::terminalTask() :
        scheduler_task("terminal", 1024*4, PRIORITY_HIGH),
        mCommandCount(0), mDiskTlmSize(0), mpBinaryDiskTlm(NULL),
        mCmdProc(18, 1024)
{
    /* Nothing to do */
}

bool terminalTask::init()
{
    CommandProcessor &cp = mCmdProc;

    // System information handlers
    cp.addHandler(taskListHandler, "info",    "Task/CPU Info.  Use 'info 200' to get CPU during 200ms");
    cp.addHandler(memInfoHandler,  "meminfo", "See memory info");
    cp.addHandler(healthHandler,   "health",  "Output system health");
    cp.addHandler(timeHandler,     "time",    "'time' to view time.  'time set MM DD YYYY HH MM SS Wday' to set time");

    // File I/O handlers:
    cp.addHandler(catHandler, "cat",    "Read a file.  Ex: 'cat 0:file.txt' or "
                                        "'cat 0:file.txt -noprint' to test if file can be read");
    cp.addHandler(cpHandler,   "cp",    "Copy files from/to Flash/SD Card.  Ex: 'cp 0:file.txt 1:file.txt'");
    cp.addHandler(dcpHandler,  "dcp",   "Copy all files of a directory to another directory.  Ex: 'dcp 0:src 1:dst'");
    cp.addHandler(lsHandler,   "ls",    "Use 'ls 0:' for Flash, or 'ls 1:' for SD Card");
    cp.addHandler(mkdirHandler,"mkdir", "Create a directory. Ex: 'mkdir test'");
    cp.addHandler(mvHandler,   "mv",    "Rename a file. Ex: 'rm 0:file.txt 0:new.txt'");
    cp.addHandler(rmHandler,   "rm",    "Remove a file. Ex: 'rm 0:file.txt'");

    // Misc. handlers
    cp.addHandler(storageHandler,   "storage",   "Parameters: 'format sd', 'format flash', 'mount sd', 'mount flash'");
    cp.addHandler(rebootHandler,    "reboot",    "Reboots the system");
    cp.addHandler(logHandler,       "log",       "'log <hello>' to log info.  'log flush' to flush logs");
    cp.addHandler(learnIrHandler,   "learn",     "Begin to learn IR codes for numbers 0-9");

#if (ENABLE_TELEMETRY)
    cp.addHandler(telemetryHandler, "telemetry", "Outputs registered telemetry. \n"
                                                 "'telemetry save' : Saves disk tel.\n"
                                                 "'telemetry <comp. name> <name> <value>' to set a telemetry variable");
#endif

    return true;
}

bool terminalTask::regTlm(void)
{
#if ENABLE_TELEMETRY
    return (TLM_REG_VAR(tlm_component_get_by_name("debug"), mCommandCount, tlm_uint) &&
            TLM_REG_VAR(tlm_component_get_by_name("debug"), mDiskTlmSize, tlm_uint));
#else
    return true;
#endif
}

bool terminalTask::taskEntry()
{
    // Initialize Interrupt driven version of getchar & putchar
    Uart0& uart0 = Uart0::getInstance();
    bool success = uart0.init(38400, 32, 512);
    stdio_SetInputCharFunction(uart0.getcharIntrDriven);
    stdio_SetOutputCharFunction(uart0.putcharIntrDriven);

#if ENABLE_TELEMETRY
    /* Telemetry should be registered at this point, so initialize the binary
     * telemetry space that we periodically check to save data to disk
     */
    tlm_component *disk = tlm_component_get_by_name(DISK_TLM_NAME);
    mDiskTlmSize = tlm_binary_get_size_one(disk);
    mpBinaryDiskTlm = new char[mDiskTlmSize];
    if (success) {
        success = (NULL != mpBinaryDiskTlm);
    }

    /* Restore telemetry from file to restore "disk" variables' values */
    restoreTlm();

    /* Now update our copy of disk telemetry */
    tlm_binary_get_one(disk, mpBinaryDiskTlm);
#endif

    puts(mCmdProc.handleCommand("help"));
    return success;
}

bool terminalTask::run(void* p)
{
    STR_ON_STACK(command, 128);
    printf("LPC: ");

    // If no input, then use this time to save disk telemetry
    if (!getline(command, command.getCapacity())) {
        puts("");
        return saveDiskTlm();
    }

    // If the user did not press enter key, getLen() will be greater than 0
    if (command.getLen() > 0)
    {
        PRINT_EXECUTION_SPEED()
        {
            ++mCommandCount;
            getOutputIface().puts(mCmdProc.handleCommand(command));
        }
    }
    return true;
}
void terminalTask::restoreTlm(void)
{
#if ENABLE_TELEMETRY
    // Restore telemetry registered by "disk" component
    FILE *fd = fopen(DISK_TLM_NAME, "r");
    if (fd) {
        tlm_stream_decode_file(fd);
        fclose(fd);
    }
#endif
}

bool terminalTask::saveDiskTlm(void)
{
#if ENABLE_TELEMETRY
    tlm_component *disk = tlm_component_get_by_name(DISK_TLM_NAME);

    /* Size of telemetry shouldn't change */
    if (mDiskTlmSize != tlm_binary_get_size_one(disk)) {
        return false;
    }

    if (!tlm_binary_compare_one(disk, mpBinaryDiskTlm)) {
        puts("Disk variables changed...");

        FILE *file = fopen(DISK_TLM_NAME, "w");
        if (file) {
            // Only update variables if we could open the file
            tlm_binary_get_one(disk, mpBinaryDiskTlm);

            tlm_stream_one_file(disk, file);
            fclose(file);

            puts("Changes saved to disk...");
            LOG_INFO_SIMPLE("Disk variables saved to disk");
        } else {
            return false;
        }
    }
#endif

    return true;
}

remoteTask::remoteTask() :
        scheduler_task("remote", 1024, PRIORITY_HIGH),
        mIrNumber(0),
        mState(waitForFirstNum)
{
    memset(mNumCodes, 0, sizeof(mNumCodes));
}

bool remoteTask::init(void)
{
    setFrequency(100);

    // Semaphore should be ready to be taken upon creating it once.
    return xSemaphoreTake(getHandles()->Sem.learnIrCodes, 0);
}

bool remoteTask::regTlm(void)
{
    bool success = true;
#if ENABLE_TELEMETRY
    if(success) success = TLM_REG_ARR(tlm_component_get_by_name(DISK_TLM_NAME), mNumCodes, tlm_uint);
#endif
    return success;
}
bool remoteTask::taskEntry(void)
{
    LD.clear();
    return true;
}
bool remoteTask::run(void *p)
{
    uint32_t number = 0;
    STR_ON_STACK(temp, 64);

    if(xSemaphoreTake(getHandles()->Sem.learnIrCodes, 0))
    {
        LD.setLeftDigit('-');
        LD.setRightDigit('-');
        puts("IR Codes will be learned.  Press buttons 0-9 on the remote");
        mState = learn;
    }

    if(IS.isIRCodeReceived())
    {
        switch(mState)
        {
            case learn :
            {
                for(int i=0; i < 10; i++)
                {
                    while(!IS.isIRCodeReceived()) {
                        vTaskDelayMs(100);
                    }

                    unsigned int code = IS.getLastIRCode();
                    temp.printf("Learned: #%i = %x", i, code);
                    puts(temp());

                    mNumCodes[i] = code;
                    LD.setNumber(i);
                }

                puts("Learned all numbers!");
                mState = waitForFirstNum;
                break;
            }

            case waitForFirstNum:
            {
                if(getNumberFromCode(IS.getLastIRCode(), number)) {
                    LD.setLeftDigit(number + '0');
                    LD.setRightDigit('-');

                    mIrNumber = 10 * number; // Left digit
                    mState = waitForSecondNum;
                }
                break;
            }

            case waitForSecondNum:
            {
                if(getNumberFromCode(IS.getLastIRCode(), number)) {
                    mIrNumber += number;
                    LD.setRightDigit(number + '0');
                    mState = waitForFirstNum;

                    vTaskDelayMs(3000);
                    IS.getLastIRCode(); // Discard if any code came in within the delay above
                }
                break;
            }

            default :
                LOG_ERROR("Invalid State Fault");
                return false;
        }
    }

    return true;
}

bool remoteTask::getNumberFromCode(uint32_t code, uint32_t& num)
{
    for(int i=0; i<10; i++)
    {
        if(mNumCodes[i] == code)
        {
            num = i;
            return true;
        }
    }
    return false;
}

bool getline(str& s, const int maxLen)
{
    Uart0& u0 = Uart0::getInstance();
    char c = 0;

    do
    {
#if ENABLE_UART3_TERMINAL
        Uart3& u3 = Uart3::getInstance();
        while(1)
        {
            if(u0.getChar(&c, 0)) {
                break;
            }
            else if(u3.getChar(&c, 0)) {
                break;
            }
            else if(!u0.recentlyActive(60 * 1000) && !u3.recentlyActive(60 * 1000)) {
                return false;
            }
            else {
                vTaskDelay(1);
            }
        }
#else
        if (!u0.getChar(&c, 60*1000) && s.getLen() == 0) {
            return false;
        }
#endif

        /* Echo & backspace logic over stdio */
        switch (c)
        {
            // Backspace 1 char @ terminal and erase last char of string
            case '\b':
                if (s.getLen() > 0) {
                    printf("\b ");
                    s.eraseLast(1);
                }
                else {
                    // Send "Alert" sound to terminal because we can't backspace
                    const char bellSound = '\a';
                    c =  bellSound;
                }
                break;

            // Output these chars but ignore them otherwise
            case '\r':
            case '\n':
                break;

            // Store all other characters
            default:
                s += c;
                break;
        }
        putchar(c);
    } while (c != '\n' && s.getLen() < maxLen);

    return true;
}

void LightSensorsTask(void *p){
    HandlesType *osHandles = (HandlesType*) p;
    int diffTop=0.0;
    int diffBase=0.0;
    int degreeTop = 0.0;
    int degreeBase = 0.0 ;

    while (true)
    {
        diffBase = getDirectionBase(LightSensors::Right);
        diffTop = getDirectionTop(LightSensors::Left);


        checkDegree(degreeTop);
        checkDegree(degreeBase);

        setDegree(degreeTop,diffTop);
        setDegree(degreeBase,diffBase);



        vTaskDelay(100);//Remove delay?

        if((! xQueueSend(osHandles->Queue.degree_Top, &degreeTop, portMAX_DELAY)
                ||
          ! xQueueSend(osHandles->Queue.degree_Base, &degreeBase, portMAX_DELAY) )){
            LOG_ERROR("Unable to send to degree queue, queue was likely full!");
        }
        vTaskDelay(100);//Remove delay?
    }
}

void ServoDirectionTask(void *p){ // xQueueReceive( xQueue, pvBuffer, xTicksToWait )
    HandlesType *osHandles = (HandlesType*) p;
    int degreeTop = 0.0;
    int degreeBase = 0.0;
    while (true)
    {
        if((!xQueueReceive(osHandles->Queue.degree_Top,&degreeTop , portMAX_DELAY)
           ||
           !xQueueReceive(osHandles->Queue.degree_Base,&degreeBase , portMAX_DELAY))) {
            puts("Could not receive from queue with given timeout");
        }

        change_direction_withDegree(degreeTop, ServoMotors::Top_servo);
        change_direction_withDegree(degreeBase, ServoMotors::Base_servo);
       // printf("ServoDirectionTask -- Top Degree %i  \n", degreeTop);
       // printf("ServoDirectionTask -- Base Degree %i  \n", degreeBase);
    }
}

