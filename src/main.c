//-----------------------------------------------------------------------------
// main.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
//
// Program Description:
//
// Use scope to observe DAC outputs on P3.0 and P3.1.
// Move joystick left/right to change functions.
// Move joystick up/down to change frequency.
//
// Resources:
//   SYSCLK - 24.5 MHz HFOSC0 / 1
//   DAC0
//   DAC1
//   DAC2
//   DAC3
//   ADC0   - 10-bit, VREF = VDD (3.3 V)
//   SPI0   - 1 MHz
//   Timer2 - 2 MHz (SPI CS delay)
//   Timer3 - 1 kHz (1 ms tick)
//   Timer4 - 150 kHz interrupt (DAC update trigger)
//   P0.2   - Push button (kill splash screen)
//   P0.3   - Push button (kill splash screen)
//   P0.6   - SPI SCK
//   P1.0   - SPI MOSI
//   P1.7   - ADC input / Joystick (analog voltage divider)
//   P2.6   - SPI CS (Active High)
//   P3.0   - DAC0 output
//   P3.1   - DAC1 output
//   P3.4   - Display enable
//
//-----------------------------------------------------------------------------
// How To Test: EFM8BB3 STK
//-----------------------------------------------------------------------------
// 1) Place the switch in "AEM" mode.
// 2) Connect the EFM8BB3 STK board to a PC using a mini USB cable.
// 3) Compile and download code to the EFM8BB3 STK board.
//    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
//    click the Debug button in the quick menu, or press F11.
// 4) Run the code.
//    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
//    click the Resume button in the quick menu, or press F8.
// 5) The Function Generator demo should start.
//    Press any button, move joystick, or wait 4 sec to kill splash screen
//    Use scope to observe DAC output on P3.0 and P3.1.
//    Move the joystick left/right to change functions.
//    Move the joystick up/down to increase/decrease the frequency.
//
// Target:         EFM8BB3
// Tool chain:     Generic
//
// Release 0.1 (ST)
//    - Initial Revision
//    - 15 JUL 2015
//

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "bsp.h"
#include "InitDevice.h"
#include "disp.h"
#include "function_generator.h"
#include "retargetserial.h"


SI_SBIT (DISP_EN, SFR_P3, 4);          // Display Enable
#define DISP_BC_DRIVEN   0             // 0 = Board Controller drives display
#define DISP_EFM8_DRIVEN 1             // 1 = EFM8 drives display

SI_SBIT (BC_EN, SFR_P2, 2);            // Board Controller Enable
#define BC_DISCONNECTED 0              // 0 = Board Controller disconnected
                                       //     to EFM8 UART pins
#define BC_CONNECTED    1              // 1 = Board Controller connected
                                       //     to EFM8 UART pins
//---------------------------------------------------------------------------
// main() Routine
// --------------------------------------------------------------------------
int main(void)
{


  enter_DefaultMode_from_RESET();

  // Enable all interrupts
  IE_EA = 1;

  DISP_Init();

  BC_EN = BC_CONNECTED;               // Board controller connected to EFM8
                                         // UART pins

  SCON0_TI = 1;                       // This STDIO library requires TI to
                                         // be set for prints to occur
  FunctionGenerator_main();
     while (1)
     {

     }

}
