///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
#include "adc.h"
#include "bsp.h"
#include "tick.h"
#include "disp.h"
#include "render.h"
#include "joystick.h"
#include "thinfont.h"
#include "function_generator.h"
#include "sine.h"
#include "square.h"
#include "triangle.h"
#include "sawtooth.h"
#include "windowed_sine.h"
#include "nav_up.h"
#include "nav_down.h"
#include "nav_left.h"
#include "nav_right.h"
#include "waveform_tables.h"
#include "retargetserial.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define txtlen 20
///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////
SI_SBIT(LED1, SFR_P1, 4);
SI_SBIT(LED2, SFR_P1, 5);
SI_SBIT(LED3, SFR_P1, 6);
SI_SBIT(BTR, SFR_P0, 2);
SI_SBIT(BTL, SFR_P0, 3);
bool JN, JS;
uint8_t i;
uint8_t z;
uint8_t j;
uint8_t flag;
uint8_t ori;
uint8_t dir;
uint8_t len;
uint8_t xdata ii, ij;
char xdata text1[txtlen+1];
char xdata input1[150][txtlen+1];
char outword[33];
unsigned char inputchar;      // Used to store character from UART
uint8_t x=0;
uint8_t y=0;
uint8_t page=0;
int itrp=0;
// Generic line buffer
SI_SEGMENT_VARIABLE(Line[DISP_BUF_SIZE], uint8_t, RENDER_LINE_SEG);

// Demo state variables
//static DemoState currentDemoState = DEMO_SINE;
//static SI_VARIABLE_SEGMENT_POINTER(currentTable, uint16_t, const SI_SEG_CODE) = sineTable; // current waveform table for DAC output
//static SI_VARIABLE_SEGMENT_POINTER(currentWaveform, uint8_t, const SI_SEG_CODE) = sine_bits; // current waveform picture

// Frequency selection
//#define SUPPORTED_NUM_FREQ 8
//static SI_SEGMENT_VARIABLE(frequency[SUPPORTED_NUM_FREQ], uint16_t, SI_SEG_XDATA) = {
//		10L,
//		20L,
//		50L,
//		100L,
//		200L,
//		500L,
//		1000L,
//		2000L
//};

//// Current Frequency Selection
//static uint8_t currentFreqIndex = 3;
//
//// Phase offset (updated when frequency is changed)
//static uint16_t phaseOffset = 100 * PHASE_PRECISION / SAMPLE_RATE_DAC;
//
//// Kill splash
//KillSpash killSplashFlag = SHOW_SPLASH;

///////////////////////////////////////////////////////////////////////////////
// Supporting Functions
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// transitionDemoWaveform
//-----------------------------------------------------------------------------
//
// Change function/waveform.
// Left  - change function order: sine < square < triangle < sawtooth < windowed sine
// Right - change function order: sine > square > triangle > sawtooth > windowed sine
//
// dir - valid arguments are: JOYSTICK_E, JOYSTICK_W
//


// Must see two or more consecutive joystick presses in the
// same direction to qualify as a press.
//
// This helps reduce problems with secondary joystick
// position detection as the joystick bounces to a cardinal
// direction before returning to center.
uint8_t GetJoystickDirection()
{
  static uint8_t prevDirection = JOYSTICK_NONE;
  uint8_t curDirection = joystickDirection;
  uint8_t direction = JOYSTICK_NONE;

  if (curDirection == prevDirection)
  {
    direction = curDirection;
  }

  prevDirection = curDirection;

  return direction;
}

// Determines when a Joystick toggle has occurred
uint8_t DetectJoystickChange()
{
  static uint8_t prevDirection = JOYSTICK_NONE;
  uint8_t curDirection = GetJoystickDirection();
  uint8_t direction = JOYSTICK_NONE;

  if (curDirection != prevDirection)
  {
    direction = curDirection;
  }

  prevDirection = curDirection;

  return direction;
}

static void drawScreenText1(SI_VARIABLE_SEGMENT_POINTER(str, char, RENDER_STR_SEG), uint8_t rowNum, uint8_t colNum)
{
  uint8_t i;

  for (i = 0; i < FONT_HEIGHT; i++)
  {
    RENDER_ClrLine(Line);
    RENDER_StrLine(Line, colNum, i, str);
    DISP_WriteLine(rowNum + i, Line);
  }
}

//static void transitionDemoWaveform(uint8_t dir)
//{
//  if (dir == JOYSTICK_E)
//  {
//	  switch (currentDemoState)
//	  {
//		case DEMO_SINE:
//		  currentDemoState = DEMO_SQUARE;
//		  currentWaveform = square_bits;
//		  currentTable = squareTable;
//		  break;
//
//		case DEMO_SQUARE:
//		  currentDemoState = DEMO_TRIANGLE;
//		  currentWaveform = triangle_bits;
//		  currentTable = triangleTable;
//		  break;
//
//		case DEMO_TRIANGLE:
//          currentDemoState = DEMO_SAWTOOTH;
//		  currentWaveform = sawtooth_bits;
//		  currentTable = sawtoothTable;
//		  break;
//
//		case DEMO_SAWTOOTH:
//		  currentDemoState = DEMO_WINDOWED_SINE;
//		  currentWaveform = windowed_sine_bits;
//		  currentTable = windowedSineTable;
//		  break;
//
//		case DEMO_WINDOWED_SINE:
//		  currentDemoState = DEMO_SINE;
//		  currentWaveform = sine_bits;
//		  currentTable = sineTable;
//		  break;
//	  }
//  }
//  else if (dir == JOYSTICK_W)
//  {
//	  switch (currentDemoState)
//	  {
//		case DEMO_SINE:
//		  currentDemoState = DEMO_WINDOWED_SINE;
//		  currentWaveform = windowed_sine_bits;
//		  currentTable = windowedSineTable;
//		  break;
//
//		case DEMO_SQUARE:
//		  currentDemoState = DEMO_SINE;
//		  currentWaveform = sine_bits;
//		  currentTable = sineTable;
//		  break;
//
//		case DEMO_TRIANGLE:
//		  currentDemoState = DEMO_SQUARE;
//		  currentWaveform = square_bits;
//		  currentTable = squareTable;
//		  break;
//
//		case DEMO_SAWTOOTH:
//		  currentDemoState = DEMO_TRIANGLE;
//		  currentWaveform = triangle_bits;
//		  currentTable = triangleTable;
//		  break;
//
//		case DEMO_WINDOWED_SINE:
//		  currentDemoState = DEMO_SAWTOOTH;
//		  currentWaveform = sawtooth_bits;
//		  currentTable = sawtoothTable;
//		  break;
//	  }
//  }
//}
//
////-----------------------------------------------------------------------------
//// transitionDemoFrequency
////-----------------------------------------------------------------------------
////
//// Change frequency of the function.
//// Up   - increase frequency
//// Down - decrease frequency
////
//// dir - valid arguments are: JOYSTICK_N, JOYSTICK_S
////
//static void transitionDemoFrequency(uint8_t dir)
//{
//  if (dir == JOYSTICK_N)
//  {
//	  // increase freq
//	  if(currentFreqIndex == SUPPORTED_NUM_FREQ-1)
//	  {
//		  currentFreqIndex = 0;
//	  }
//	  else
//	  {
//		  currentFreqIndex++;
//	  }
//  }
//  else if (dir == JOYSTICK_S)
//  {
//	  // decrease freq
//	  if (currentFreqIndex == 0)
//	  {
//		  currentFreqIndex = SUPPORTED_NUM_FREQ-1;
//	  }
//	  else
//	  {
//		  currentFreqIndex--;
//	  }
//  }
//
//  phaseOffset = frequency[currentFreqIndex] * PHASE_PRECISION / SAMPLE_RATE_DAC;
//}


//-----------------------------------------------------------------------------
// getJoystick
//-----------------------------------------------------------------------------
//
// Get new ADC sample and return joystick direction. Valid return values:
//  JOYSTICK_NONE   JOYSTICK_N   JOYSTICK_S
//  JOYSTICK_C      JOYSTICK_E   JOYSTICK_W
//
static uint8_t getJoystick(void)
{
  uint32_t mv;
  uint8_t dir;

  ADC0CN0_ADBUSY = 1;
  while (!ADC0CN0_ADINT);
  ADC0CN0_ADINT = 0;

  mv = ((uint32_t)ADC0) * 3300 / 1024;

  dir = JOYSTICK_convert_mv_to_direction(mv);

  return dir;
}

//-----------------------------------------------------------------------------
// getWaitJoystick
//-----------------------------------------------------------------------------
//
// Get joystick input. If joystick was moved, wait for release. Return joystick
// direction. Valid return values:
//  JOYSTICK_NONE   JOYSTICK_N   JOYSTICK_S
//  JOYSTICK_C      JOYSTICK_E   JOYSTICK_W
//
//static uint8_t getWaitJoystick(void)
//{
//  uint8_t dir, dirSave;
//
//  dir = getJoystick();
//  dirSave = dir;
//
//  // wait for release then transition
//  while (dir != JOYSTICK_NONE)
//  {
//    dir = getJoystick();
//  }
//
//  return dirSave;
//}

//-----------------------------------------------------------------------------
// getJoystickDemo
//-----------------------------------------------------------------------------
//
// Get and process joystick input.
// Left/Right = change function/waveform
// Up/Down    = change frequency
//
//static void processInput(uint8_t dir)
//{
//  // process input
//  if ((dir == JOYSTICK_E) || (dir == JOYSTICK_W))
//  {
//    transitionDemoWaveform(dir);
//  }
//  else if ((dir == JOYSTICK_N) || (dir == JOYSTICK_S))
//  {
//    transitionDemoFrequency(dir);
//  }
//}

//-----------------------------------------------------------------------------
// drawScreenText
//-----------------------------------------------------------------------------
//
// Show one line of text on the screen
//
// str - pointer to string text (0 - 21 characters) to display
// rowNum - row number of the screen (0 - 127)
// fontScale - font scaler (1 - 4)
//
//static void drawScreenText(SI_VARIABLE_SEGMENT_POINTER(str, char, RENDER_STR_SEG), uint8_t rowNum)
//{
//  uint8_t i;
//
//  for (i = 0; i < FONT_HEIGHT; i++)
//  {
//    RENDER_ClrLine(Line);
//    RENDER_StrLine(Line, 4, i, str);
//    DISP_WriteLine(rowNum + i, Line);
//  }
//}


///////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routines
///////////////////////////////////////////////////////////////////////////////

//SI_INTERRUPT_USING (TIMER4_ISR, TIMER4_IRQn, 1)
//{
//  static uint16_t phaseAcc = 0;       // Holds phase accumulator
//
//  SI_UU16_t temp;   // The temporary value that holds
//									  // value before being written
//									  // to the DAC
//
//  TMR4CN0 &= ~TMR3CN0_TF3H__BMASK;    // Clear Timer4 overflow flag
//
//  phaseAcc += phaseOffset;            // Increment phase accumulator
//
//  // Read the table value
//  temp.u16 = currentTable[phaseAcc >> 8];
//
//  // Set the value of <temp> to the next output of DAC at full-scale
//  // amplitude. The rails are 0x000 and 0xFFF. DAC low byte must be
//  // written first.
//
//  SFRPAGE = PG4_PAGE;
//
//  DAC1L = DAC0L = temp.u8[LSB];
//  DAC1H = DAC0H = temp.u8[MSB];
//}
//
////-----------------------------------------------------------------------------
//// PMATCH_ISR
////-----------------------------------------------------------------------------
////
//// The Port Match interrupt occurs whenever (Pn & PnMASK) != (PnMAT & PnMASK).
//// We must disable the interrupt and wait until the button is released in
//// main, or else the interrupt will trigger multiple times.  It is generally
//// not good practice to sit in an ISR for int32_t periods of time.
////

uint8_t scrollup(int pos,int end)
{
	int i=0;
	DISP_ClearAll();
	if (pos!=0)
	{
		for (pos=pos-2;pos<=end+2;pos++)
		{
				sprintf(text1,"%s",input1[pos]);
				drawScreenText1(text1,8*i,0);
				i++;
		}
	}
	return end+3;
}

uint8_t scrolldown(int pos,int end)
{
	int i=0;
	DISP_ClearAll();
	for (pos=pos+1;pos<=end;pos++)
	{
		sprintf(text1,"%s",input1[pos]);
		drawScreenText1(text1,8*i,0);
		i++;
	}
	return end-2;
}
void paints(void)
{
	uint8_t zi;
	for(zi=0;zi<16;zi++)
		{
		strcpy(text1,input1[zi+page*16]);
		drawScreenText1(text1,8*zi,0);
		}
}

void paintsh(void)
{
	uint8_t zi;
	for(zi=0;zi<16;zi++)
		{
		strcpy(text1,input1[zi]);
		len = strlen(text1);
		if(text1[len-1]=='\n')
			text1[--len] = '\0';
			for(z = 0; z<len; z++)
			{
			sprintf(outword+z*2, "%02X", text1[z]);
			drawScreenText1(outword,8*zi,0);
			}

		}


}

SI_INTERRUPT (PMATCH_ISR, PMATCH_IRQn) {
	uint8_t SFRPAGE_save = SFRPAGE;
	if (BTL==0) {
		LED1=0;
	}
	if (BTR==0) {
		LED1=1;
	}
	SFRPAGE = PG2_PAGE;

	  //EIE1 &= ~0x02;                     // Disable Port Match interrupts to
	                                     // prevent multiple interrupts from
	                                     // occurring while the button is
	                                     // pressed
	  SFRPAGE = SFRPAGE_save;

}
///////////////////////////////////////////////////////////////////////////////
// Driver Function
///////////////////////////////////////////////////////////////////////////////



void FunctionGenerator_main(void)
{
	ii=0;
	ij=0;
	text1[txtlen]=0;
	for(i=0;i<200;i++){
		for(j=0;j<txtlen+1;j++){
			input1[i][j]=0;
		}
	}
  while(1)
  {
	  dir = DetectJoystickChange();
	  if(LED1==1)
	  paints();
	  if(LED1==0)
	  paintsh();
	  inputchar=getchar();
	  if(inputchar)
	  {
		  if((ii/16)==page)
		  {
			  if(ij<txtlen)
		  	  {
			  input1[ii][ij]=inputchar;
			  ij++;
		  	  	  if(ij==txtlen)
		  	  	  {
		  		  RETARGET_PRINTF ("\n");
		  		  ij=0;
		  		  ii++;
		  	  	  }
		  	  }
			  if((ii/16)==(page+1))
			  {
				  page++;
				  DISP_ClearAll();
			  }
		  }

	  }
	  if (dir == JOYSTICK_N)
	  {
	  LED2=0;
	  }
	  if (dir == JOYSTICK_S)
	  {
	  LED2=1;
	  }


	  //while(i<200){
		  //if(i<16)
		 // {
//			  for(i=0;i<16;i++)
//			  	  {
//			  		  sprintf(text1,"");
//			  		  for(j=0;j<16;j++)
//			  		  {
//			  		  inputchar = getchar();
//			  		  input1[i][j]=inputchar;
//			  		  paints();
//			  		  flag=i;
//			  		  ori=i-(i%16)+1;
//			  		  }
//			  		  RETARGET_PRINTF ("\n");
//			  	  }
//			  k=1;
		  //}
		  /*if(i>=16)
		  	  {
		  		  DISP_ClearAll();
		  		  k++;
		  		  for(;i<k*16;i++){
		  		  for(j=0;j<5;j++)
		  		  {
		  		  	  input1[i][j]=NULL;
		  		  	  input1[i][5]=0;
		  		  }
		    		  sprintf(text1,"");
		    		  for(j=0;j<5;j++)
		    		  {
		    			  dir = DetectJoystickChange();
		    	if (dir == JOYSTICK_N) {
		    	//get line up
		    			 LED1=0;
		    		ori=scrollup(ori,flag);
		    				LED1=1;
		    			}
		    				else if (dir == JOYSTICK_S) {
		    		LED2=0;
		    					ori=scrolldown(ori,flag);
		    							LED2=1;
		    				}
		    		  inputchar = getchar();
		    		  input1[i][j]=inputchar;
		    		  sprintf(text1,"%s",input1[i]);
		    		  drawScreenText1(text1,8*(i-(k-1)*16),0);
		    		  flag=i;
		    		  ori=i-(i%16)+1;

		    		  }
		    		  RETARGET_PRINTF ("\n");
		  		  }

		  	  }*/


	  //}
  }
}

