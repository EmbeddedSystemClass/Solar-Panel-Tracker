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
 * @brief UART0 Interrupt driven IO driver
 * @ingroup Drivers
 */
#ifndef UART0_HPP_
#define UART0_HPP_

#include "uart_base.hpp"           // Base class
#include "singleton_template.hpp"  // Singleton Template


/**
 * UART0 Interrupt Driven Driver
 * This simply uses UART_Base, and forwards the UART0 IRQ to UART_Base::handleInterrupt()
 * This enforces Singleton class, otherwise it's a thin wrapper around UART_Base class.
 *
 * @ingroup Drivers
 */
class Uart0 : public UART_Base, public SingletonTemplate<Uart0>
{
    public:
        /**
         * Initializes UART0 at the given @param baudRate
         * @param rxQSize   The size of the receive queue  (optional, defaults to 32)
         * @param txQSize   The size of the transmit queue (optional, defaults to 64)
         */
        bool init(unsigned int baudRate, int rxQSize=32, int txQSize=64);

        /**
         * @{ \name Static functions to use for printf/scanf redirection.  @see
         *          stdio_SetInputCharFunction() and stdio_SetOutptCharFunction()
         */
        static char getcharIntrDriven(char unused);
        static char putcharIntrDriven(char thechar);
        /** @} */

    private:
        Uart0();  ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Uart0>;  ///< Friend class used for Singleton Template
};


#endif /* UART0_HPP_ */
