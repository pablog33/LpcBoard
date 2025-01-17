/*
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */


/*
    Code reworking to fit the Edu-CIAA board.
    Santiago Germino, 2018.
    royconejo@gmail.com

    Please see board.c for comments on this implementation.
*/

#include <board.h>
#include <board_api.h>

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   includes clock setup and default pin muxing configuration. */


struct CLK_BASE_STATES
{
   CHIP_CGU_BASE_CLK_T clk;     /* Base clock */
   CHIP_CGU_CLKIN_T clkin;      /* Base clock source, see UM for allowable souorces per base clock */
   bool autoblock_enab;         /* Set to true to enable autoblocking on frequency change */
   bool powerdn;                /* Set to true if the base clock is initially powered down */
};


/* Initial base clock states are mostly on */
STATIC const struct CLK_BASE_STATES InitClkStates[] =
{
   /* Ethernet Clock base */
   {CLK_BASE_PHY_TX, CLKIN_ENET_TX, true, false},
   {CLK_BASE_PHY_RX, CLKIN_ENET_TX, true, false},

   /* Clocks derived from dividers */
   {CLK_BASE_USB0, CLKIN_IDIVD, true, true}
};

STATIC const PINMUX_GRP_T pinmuxing[] =
{
    /* Board LEDs */
    {2, 0, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC4)},
    {2, 1, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC4)},
    {2, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC4)},
    {2, 10, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0)},
    {2, 11, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0)},
    {2, 12, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0)},

    /* Board Buttons */
    {1, 0, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {1, 1, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {1, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {1, 6, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},

    /* GPIO0/8 */
    {6, 1, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {6, 4, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {6, 5, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {6, 7, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC4)},
    {6, 8, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC4)},
    {6, 9, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {6,10, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {6,11, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {6,12, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},

    /* SPI port (LPC_SSP1) PF_4: SSP1_SCK, P1_3: SSP1_MISO, P1_4: SSP1_MOSI */
    {0xF, 4, (SCU_PINIO_FAST | SCU_MODE_FUNC0)},
    {0x1, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)},
    {0x1, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)},

    /* UART2 directly wired to FT2232 IC; P7_1: U2_TXD, P7_2: U2_RXD */
    {7, 1, (SCU_MODE_INACT | SCU_MODE_FUNC6)},
    {7, 2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC6)},

    /* UART3: 232_TX/RX on P1 header */
    {2, 3, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC2)},
    {2, 4, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC2)},

    /* UART0: RS-485 with on-board transceiver */
    {9, 5, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC2)},
    {9, 6, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC7)},
    {6, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC7)},

    /* ENET Pin mux (RMII Pins) */
    {1, 15, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3)}, /* RXD0 */
    {1, 16, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC7)}, /* CRS_DV */
    {1, 17, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3)}, /* MDIO */
    {1, 18, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3)}, /* TXD0 */
    {1, 19, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0)}, /* REFCLK */
    {1, 20, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3)}, /* TXD1 */
    {7,  7, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6)}, /* MDC */
    {0,  0, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2)},  /* RXD1 */
    {0,  1, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6)}, /* TXEN */

};



void Board_SetupMuxing(void)
{
    Chip_SCU_SetPinMuxing(pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
    printf("inicio");
}


void Board_SetupClocking(void)
{

	volatile uint32_t delay;
    Chip_CREG_SetFlashAcceleration(MAX_CLOCK_FREQ);
    Chip_SetupCoreClock(CLKIN_CRYSTAL, MAX_CLOCK_FREQ, true);

    delay = 100000;

    while(delay--){} /* 201102 2300 Agregado para contemplar inestabilidad inicial cristal CIAA-NXP - PGa */


    /* Setup system base clocks and initial states. This won't enable and
       disable individual clocks, but sets up the base clock sources for
       each individual peripheral clock. */
    for (uint32_t i = 0; i < (sizeof(InitClkStates) / sizeof(InitClkStates[0])); ++i)
    {
        const struct CLK_BASE_STATES *c = &InitClkStates[i];
        Chip_Clock_SetBaseClock (c->clk, c->clkin, c->autoblock_enab,
                                c->powerdn);
    }

    /* Reset and enable 32Khz oscillator */
    LPC_CREG->CREG0 &= ~((1 << 3) | (1 << 2));
    LPC_CREG->CREG0 |= (1 << 1) | (1 << 0);
}


/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
    /* Setup system clocking and memory. This is done early to allow the
       application and tools to clear memory and use scatter loading to
       external memory. */
    Board_SetupMuxing();
    Board_SetupClocking();
}
