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

/**
 * @file
 * @brief Contains FreeRTOS Tasks
 */
#ifndef TASKS_HPP_
#define TASKS_HPP_
#include "cpp_task.hpp"
#include "command_handler.hpp"
#include "str.hpp"
#include "averager.hpp"
#include "memory.h"
#include "file_logger.hpp"



class terminalTask : public scheduler_task
{
public:
        terminalTask();
        bool init(void);      ///< Registers commands
        bool regTlm(void);    ///< Registers telemetry
        bool taskEntry(void); ///< Inits interrupt-driven UART and restores disk telemetry
        bool run(void *p);

private:
        void restoreTlm(void);
        bool saveDiskTlm(void);

        uint16_t mCommandCount;     ///< terminal command count
        uint16_t mDiskTlmSize;      ///< Size of disk variables in bytes
        char *mpBinaryDiskTlm;      ///< Binary disk telemetry
        CommandProcessor mCmdProc;  ///< Command processor
};

class remoteTask : public scheduler_task
{
public:
        remoteTask();
        bool init(void);
        bool regTlm(void);
        bool taskEntry(void);
        bool run(void *p);

private:
        typedef enum { learn, waitForFirstNum, waitForSecondNum } StateType;

        bool getNumberFromCode(uint32_t code, uint32_t& num);
        uint32_t mNumCodes[10];     ///< IR Number codes
        uint32_t mIrNumber;         ///< Current IR number we're decoding
        StateType mState;           ///< We toggle between "Learn" and between 1st and 2nd digit
};

void LightSensorsTask(void *p);
void ServoDirectionTask(void *p);
void MatrixTask(void *p);



#endif /* TASKS_HPP_ */
