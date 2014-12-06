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

#include <string.h> // strlen()
#include "command_handler.hpp"


// Define the strings returned for OK, ERROR, Invalid, and other cases:
static const char* const HELP_STR               = "help";
static const char* const NO_HELP_STR            = "Help not specified for this command";
static const char* const CMD_INVALID_STR        = "Command Invalid.  Try 'help' command";
static const char* const SUPPORTED_COMMANDS_STR = "Supported Commands:";
static const char* const CMD_LEN_ERR_STR        = "Command length too large";



void CommandProcessor::addHandler(CmdHandlerFuncPtr pFunc, const char* pPersistantCmdStr, const char* pPersistentCmdHelpStr,
                                  void* pDataParam, int dataParamLen)
{
    CmdProcessorType handler;
    handler.pCommandStr  = pPersistantCmdStr;
    handler.pCmdHelpText = pPersistentCmdHelpStr;
    handler.pFunc = pFunc;
    handler.pDataParam = pDataParam;
    handler.dataParamLen = dataParamLen;

    if (0 == handler.pCmdHelpText) {
        handler.pCmdHelpText = "";
    }
    if (0 != handler.pCommandStr && 0 != handler.pFunc) {
        mCmdHandlerVector += handler;
    }
}

const char* CommandProcessor::handleCommand(const char* pCmdStr)
{
    if(strlen(pCmdStr) >= MAX_CMD_LENGTH) {
        mOutputStr = CMD_LEN_ERR_STR;
    }
    else {
        STR_ON_STACK(command, MAX_CMD_LENGTH);
        command = pCmdStr;
        handleCmd(command, mOutputStr);
    }

    return mOutputStr();
}

void CommandProcessor::handleCmd(str& input, str& output)
{
    // Note: HELP command cannot simply have a handler because this static handler
    //       will not be able to access the vector of commands
    if(input.beginsWithWholeWordIgnoreCase(HELP_STR))
    {
        prepareCmdParam(input, HELP_STR, output);
        getHelpText(input, output);
    }
    else
    {
        unsigned int i=0;
        bool found = false;
        for(i=0; i < mCmdHandlerVector.size(); i++)
        {
            CmdProcessorType &cp = mCmdHandlerVector[i];

            // If a command matches, return the response from the attached function pointer
            if(input.beginsWithWholeWordIgnoreCase(cp.pCommandStr))
            {
                prepareCmdParam(input, cp.pCommandStr, output);
                cp.pFunc(input, output, cp.pDataParam, cp.dataParamLen);
                found = true;
                break;
            }
        }

        /**
         * If command not matched, try to partially match a command.
         * ie: If command is "magic", match "m" as command
         */
        if(!found)
        {
            for(i=0; i < mCmdHandlerVector.size(); i++)
            {
                CmdProcessorType &cp = mCmdHandlerVector[i];
                str& regCmd = output;     /* Reuse output str, but rename for clarity */
                regCmd = cp.pCommandStr;  /* Set to the command text */

                /**
                 * Check here if pCommandStr contains partial command :
                 *      - regCmd may be "thermostat", when input is "th" or "th on"
                 *      - So accept this command as shorthand command
                 */
                char shortCmd[8] = { 0 };
                input.scanf("%7s ", shortCmd);
                if(strlen(shortCmd) >= 2 && regCmd.beginsWithIgnoreCase(shortCmd))
                {
                    prepareCmdParam(input, shortCmd, output);
                    cp.pFunc(input, output, cp.pDataParam, cp.dataParamLen);
                    found = true;
                    break;
                }
            }
        }

        if(!found)
        {
            output = CMD_INVALID_STR;
        }
    }
}

void CommandProcessor::getRegisteredCommandList(str& output)
{
    output = SUPPORTED_COMMANDS_STR;
    STR_ON_STACK(temp, 64);

    for(unsigned int i=0; i<mCmdHandlerVector.size(); i++)
    {
        CmdProcessorType& c = mCmdHandlerVector[i];
        if (strlen(c.pCmdHelpText) > 20) {
            temp.printf("\n %10s : %.20s...", c.pCommandStr, c.pCmdHelpText);
        } else {
            temp.printf("\n %10s : %s", c.pCommandStr, c.pCmdHelpText);
        }
        output += temp;
    }

    temp.printf("\n 'help <command>' to get help of a command");
    output += temp;
}

void CommandProcessor::getHelpText(str& helpForCmd, str& output)
{
    // If there is a parameter, get help for this specific command
    // where this parameter itself is a command name
    if(helpForCmd.getLen() > 0)
    {
        output = CMD_INVALID_STR;
        for(unsigned int i=0; i < mCmdHandlerVector.size(); i++)
        {
            CmdProcessorType &cp = mCmdHandlerVector[i];
            if(helpForCmd.compareToIgnoreCase(cp.pCommandStr))
            {
                output = (0 == cp.pCmdHelpText || '\0' == cp.pCmdHelpText[0]) ? NO_HELP_STR : cp.pCmdHelpText;
                break;
            }
        }
    }
    else {
        getRegisteredCommandList(output);
    }
}

void CommandProcessor::prepareCmdParam(str& input, const char* pCmdToRemove, str& output)
{
    input -= pCmdToRemove;
    input.trimStart(" ");
    output.clear();
}
