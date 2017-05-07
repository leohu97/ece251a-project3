///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////
SI_SBIT(LED1, SFR_P1, 4);
SI_SBIT(LED2, SFR_P1, 5);
bool JN, JS;
int itrp=0;
uint8_t i;
uint8_t x=0;
uint8_t y=0;
uint8_t j;
uint8_t k=0;
uint8_t flag;
uint8_t ori;
char xdata text1[17];
char xdata input1[200][17];
unsigned char inputchar;      // Used to store character from UART

// Generic line buffer
SI_SEGMENT_VARIABLE(Line[DISP_BUF_SIZE], uint8_t, RENDER_LINE_SEG);

// Demo state variables
static DemoState currentDemoState = DEMO_SINE;
static SI_VARIABLE_SEGMENT_POINTER(currentTable, uint16_t, const SI_SEG_CODE) = sineTable; // current waveform table for DAC output
static SI_VARIABLE_SEGMENT_POINTER(currentWaveform, uint8_t, const SI_SEG_CODE) = sine_bits; // current waveform picture

// Frequency selection
#define SUPPORTED_NUM_FREQ 8
static SI_SEGMENT_VARIABLE(frequency[SUPPORTED_NUM_FREQ], uint16_t, SI_SEG_XDATA) = {
		10L,
		20L,
		50L,
		100L,
		200L,
		500L,
		1000L,
		2000L
};

// Current Frequency Selection
static uint8_t currentFreqIndex = 3;

// Phase offset (updated when frequency is changed)
static uint16_t phaseOffset = 100 * PHASE_PRECISION / SAMPLE_RATE_DAC;

// Kill splash
KillSpash killSplashFlag = SHOW_SPLASH;

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
static void transitionDemoWaveform(uint8_t dir)
{
  if (dir == JOYSTICK_E)
  {
	  switch (currentDemoState)
	  {
		case DEMO_SINE:
		  currentDemoState = DEMO_SQUARE;
		  currentWaveform = square_bits;
		  currentTable = squareTable;
		  break;

		case DEMO_SQUARE:
		  currentDemoState = DEMO_TRIANGLE;
		  currentWaveform = triangle_bits;
		  currentTable = triangleTable;
		  break;

		case DEMO_TRIANGLE:
          currentDemoState = DEMO_SAWTOOTH;
		  currentWaveform = sawtooth_bits;
		  currentTable = sawtoothTable;
		  break;

		case DEMO_SAWTOOTH:
		  currentDemoState = DEMO_WINDOWED_SINE;
		  currentWaveform = windowed_sine_bits;
		  currentTable = windowedSineTable;
		  break;

		case DEMO_WINDOWED_SINE:
		  currentDemoState = DEMO_SINE;
		  currentWaveform = sine_bits;
		  currentTable = sineTable;
		  break;
	  }
  }
  else if (dir == JOYSTICK_W)
  {
	  switch (currentDemoState)
	  {
		case DEMO_SINE:
		  currentDemoState = DEMO_WINDOWED_SINE;
		  currentWaveform = windowed_sine_bits;
		  currentTable = windowedSineTable;
		  break;

		case DEMO_SQUARE:
		  currentDemoState = DEMO_SINE;
		  currentWaveform = sine_bits;
		  currentTable = sineTable;
		  break;

		case DEMO_TRIANGLE:
		  currentDemoState = DEMO_SQUARE;
		  currentWaveform = square_bits;
		  currentTable = squareTable;
		  break;

		case DEMO_SAWTOOTH:
		  currentDemoState = DEMO_TRIANGLE;
		  currentWaveform = triangle_bits;
		  currentTable = triangleTable;
		  break;

		case DEMO_WINDOWED_SINE:
		  currentDemoState = DEMO_SAWTOOTH;
		  currentWaveform = sawtooth_bits;
		  currentTable = sawtoothTable;
		  break;
	  }
  }
}

//-----------------------------------------------------------------------------
// transitionDemoFrequency
//-----------------------------------------------------------------------------
//
// Change frequency of the function.
// Up   - increase frequency
// Down - decrease frequency
//
// dir - valid arguments are: JOYSTICK_N, JOYSTICK_S
//
static void transitionDemoFrequency(uint8_t dir)
{
  if (dir == JOYSTICK_N)
  {
	  // increase freq
	  if(currentFreqIndex == SUPPORTED_NUM_FREQ-1)
	  {
		  currentFreqIndex = 0;
	  }
	  else
	  {
		  currentFreqIndex++;
	  }
  }
  else if (dir == JOYSTICK_S)
  {
	  // decrease freq
	  if (currentFreqIndex == 0)
	  {
		  currentFreqIndex = SUPPORTED_NUM_FREQ-1;
	  }
	  else
	  {
		  currentFreqIndex--;
	  }
  }

  phaseOffset = frequency[currentFreqIndex] * PHASE_PRECISION / SAMPLE_RATE_DAC;
}


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
static uint8_t getWaitJoystick(void)
{
  uint8_t dir, dirSave;

  dir = getJoystick();
  dirSave = dir;

  // wait for release then transition
  while (dir != JOYSTICK_NONE)
  {
    dir = getJoystick();
  }

  return dirSave;
}

//-----------------------------------------------------------------------------
// getJoystickDemo
//-----------------------------------------------------------------------------
//
// Get and process joystick input.
// Left/Right = change function/waveform
// Up/Down    = change frequency
//
static void processInput(uint8_t dir)
{
  // process input
  if ((dir == JOYSTICK_E) || (dir == JOYSTICK_W))
  {
    transitionDemoWaveform(dir);
  }
  else if ((dir == JOYSTICK_N) || (dir == JOYSTICK_S))
  {
    transitionDemoFrequency(dir);
  }
}

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
static void drawScreenText(SI_VARIABLE_SEGMENT_POINTER(str, char, RENDER_STR_SEG), uint8_t rowNum)
{
  uint8_t i;

  for (i = 0; i < FONT_HEIGHT; i++)
  {
    RENDER_ClrLine(Line);
    RENDER_StrLine(Line, 4, i, str);
    DISP_WriteLine(rowNum + i, Line);
  }
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
//-----------------------------------------------------------------------------
// drawScreenSprite
//-----------------------------------------------------------------------------
//
// Displays a sprite on the screen
//
// sprite - pointer to sprite/picture
// width  - width of sprite
// height - height of sprite
// xPos - beginning row number on the screen (0 - 127)
// yPos - beginning col number on the screen (0 - 127)
//
static void drawScreenSprite(SI_VARIABLE_SEGMENT_POINTER(sprite, uint8_t, RENDER_SPRITE_SEG),
		                     uint8_t width, uint8_t height,
		                     uint8_t xPos, uint8_t yPos)
{
  uint8_t i;

  for (i = 0; i < height; i++)
  {
    RENDER_ClrLine(Line);
    RENDER_SpriteLine(Line, xPos, i, sprite, width);
    DISP_WriteLine(yPos + i, Line);
  }
}

//-----------------------------------------------------------------------------
// drawScreenWaveform
//-----------------------------------------------------------------------------
//
// Draw screen waveform and left/right arrows
//
static void drawScreenWaveform(void)
{
  uint8_t i; // row index for waveform sprite
  uint8_t j = 0; // row index for nav sprites

  for (i = 0; i < sine_height; i++)
  {
	RENDER_ClrLine(Line);
	RENDER_SpriteLine(Line, X_POS_WAVEFORM, i, currentWaveform, sine_width);

	if ((i >= Y_POS_NAV_ARROW_LEFT_RIGHT) && (i < Y_POS_NAV_ARROW_LEFT_RIGHT + nav_left_height))
	{
	  RENDER_SpriteLine(Line, X_POS_NAV_ARROW_LEFT, j, nav_left_bits, nav_left_width);
	  RENDER_SpriteLine(Line, X_POS_NAV_ARROW_RIGHT, j, nav_right_bits, nav_right_width);
	  j++;
	}
	DISP_WriteLine(Y_POS_WAVEFORM + i, Line);
  }

}

static void drawScreenFrequency(void)
{
  char freqStr[22];

  // display frequency on screen
  RETARGET_SPRINTF(freqStr, "     f = %d Hz", frequency[currentFreqIndex]);

  drawScreenText(freqStr, Y_POS_FREQ);
}

//-----------------------------------------------------------------------------
// drawScreenStaticSprites
//-----------------------------------------------------------------------------
//
// Draw static/non-changing sprites on screen. Static sprites are up and down
// arrows. Left/right arrows are drawn with waveform.
//
static void drawScreenStaticSprites(void)
{
  drawScreenSprite(nav_up_bits, nav_up_width, nav_up_height,
		           X_POS_NAV_ARROW_UP_DOWN, Y_POS_NAV_ARROW_UP);
  drawScreenSprite(nav_down_bits, nav_down_width, nav_down_height,
		           X_POS_NAV_ARROW_UP_DOWN, Y_POS_NAV_ARROW_DOWN);
}

//-----------------------------------------------------------------------------
// drawScreen
//-----------------------------------------------------------------------------
//
// Draw dynamic/changeable sprites and info (waveform and frequency) on screen.
//
static void drawScreen(void)
{
  drawScreenWaveform();
  drawScreenFrequency();
}

//-----------------------------------------------------------------------------
// synchFrame
//-----------------------------------------------------------------------------
//
// Delay until start of next frame
//
static void synchFrame(void)
{
  static uint16_t lastTick = 0;
  uint16_t tick;

  // Render at 50 Hz
  while (((tick = GetTickCount()) - lastTick) < HZ_TO_MS(DEMO_FRAME_RATE));
  lastTick = tick;
}

///////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routines
///////////////////////////////////////////////////////////////////////////////

SI_INTERRUPT_USING (TIMER4_ISR, TIMER4_IRQn, 1)
{
  static uint16_t phaseAcc = 0;       // Holds phase accumulator

  SI_UU16_t temp;   // The temporary value that holds
									  // value before being written
									  // to the DAC

  TMR4CN0 &= ~TMR3CN0_TF3H__BMASK;    // Clear Timer4 overflow flag

  phaseAcc += phaseOffset;            // Increment phase accumulator

  // Read the table value
  temp.u16 = currentTable[phaseAcc >> 8];

  // Set the value of <temp> to the next output of DAC at full-scale
  // amplitude. The rails are 0x000 and 0xFFF. DAC low byte must be
  // written first.

  SFRPAGE = PG4_PAGE;

  DAC1L = DAC0L = temp.u8[LSB];
  DAC1H = DAC0H = temp.u8[MSB];
}
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
	DISP_ClearAll();
	if (pos!=0)
	{
		int index;
		int temp=pos;
		if(end-pos+1>=16){
			for (index=0;pos<(temp+16);index++){
				sprintf(text1,"%s",input1[pos-1]);
				drawScreenText1(text1,8*index,0);
				pos++;
			}
		}
		else{
			for (index=0;pos<end;index++){
				sprintf(text1,"%s",input1[pos-1]);
				drawScreenText1(text1,8*index,0);
				pos++;
			}
		}
		pos=temp-1;
	}
	return pos;
}

uint8_t scrolldown(int pos,int end)
{
	int index;
		int temp=pos;
		DISP_ClearAll();
		if(end-pos+1>16){
			for (index=0;pos<(temp+16);index++){
				pos++;
				sprintf(text1,"%s",input1[pos]);
				drawScreenText1(text1,8*index,0);
			}
		}
		else{
			for (index=0;pos<end;index++){
				pos++;
				sprintf(text1,"%s",input1[pos]);
				drawScreenText1(text1,8*index,0);
			}
		}

		pos=temp+1;
		return pos;
}

SI_INTERRUPT (PMATCH_ISR, PMATCH_IRQn) {
	uint8_t dir =getWaitJoystick();
	LED1=1;LED2=0;
	if (dir == JOYSTICK_N) {
		//get line up
		JN=true;
		itrp=1;
	}
	else if (dir == JOYSTICK_S) {
		JS=true;
		itrp=1;
	}
}
///////////////////////////////////////////////////////////////////////////////
// Driver Function
///////////////////////////////////////////////////////////////////////////////

void FunctionGenerator_main(void)
{
  //drawSplash();
	text1[16]=0;
	//inputchar[1]=0;
  //DISP_ClearAll();
 // drawScreenStaticSprites();

  while(1)
  {
    //processInput(getWaitJoystick());
    //drawScreen();
    //synchFrame();

	  while(i<200){
		  if(i<16)
		  {
			  ori=0;
			  for(i=0;i<16;i++)
			  	  {
			  		  for(j=0;j<16;j++)
			  		  {
			  			  input1[i][j]=NULL;
			  			  input1[i][16]=0;
			  		  }
			  		  sprintf(text1,"");
			  		  for(j=0;j<16;j++)
			  		  {
			  		  inputchar = getchar();
			  		  input1[i][j]=inputchar;
			  		  sprintf(text1,"%s",input1[i]);
			  		  drawScreenText1(text1,8*i,0);
			  		  flag=i;
			  		  }
			  		  RETARGET_PRINTF ("\n");
			  	  }
			  k=1;
		  }
		  if(i>=16)
		  	  {
		  		  DISP_ClearAll();
		  		  ori=i/16;
		  		  k++;
		  		  for(;i<k*16;i++){
		  		  for(j=0;j<16;j++)
		  		  {
		  		  	  input1[i][j]=NULL;
		  		  	  input1[i][16]=0;
		  		  }
		    		  sprintf(text1,"");
		    		  for(j=0;j<16;j++)
		    		  {
		    		  inputchar = getchar();
		    		  input1[i][j]=inputchar;
		    		  sprintf(text1,"%s",input1[i]);
		    		  drawScreenText1(text1,8*(i-(k-1)*16),0);
		    		  flag=i;
			  		  if (flag >16){		//enables multiple laps
			  			  EIE1 |=0x02;  //re enable
			  		  }
			  		  if(itrp)
			  		  {
			  			  SFRPAGE = 0x00;              // EIE1 interrupt on SFRPAGE 0x00
			  			  EIE1 &= ~0x02;           //disable
			  			  if(JS)
			  			  {
			  			  	ori=scrollup(ori,flag);
			  			  	JS=false;
			  			  }
			  			  else if(JN)
			  			  {
			  			  	ori=scrolldown(ori,flag);
			  			  	JN=false;
			  			  }
			  			  itrp=0;
			  			  IE_EA=1;
			  		  }
		    		  }
		    		  RETARGET_PRINTF ("\n");
		  		  }

		  	  }


	  }
  }
}

