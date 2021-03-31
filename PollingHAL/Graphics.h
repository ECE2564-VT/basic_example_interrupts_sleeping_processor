/*
 * Graphics.h
 *
 *  Created on: Dec 30, 2019
 *      Author: Matthew Zhong
 */

#ifndef HAL_GRAPHICS_H_
#define HAL_GRAPHICS_H_

#include <PollingHAL/LcdDriver/Crystalfontz128x128_ST7735.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

#define FG_COLOR GRAPHICS_COLOR_BLACK
#define BG_COLOR GRAPHICS_COLOR_WHITE

struct _GFX
{
    Graphics_Context context;
    uint32_t foreground;
    uint32_t background;
    uint32_t defaultForeground;
    uint32_t defaultBackground;
};
typedef struct _GFX GFX;

GFX GFX_construct();

void GFX_resetColors(GFX *gfx_p);
void GFX_clear(GFX *gfx_p);

void GFX_setForeground(GFX *gfx_p, uint32_t foreground);
void GFX_setBackground(GFX *gfx_p, uint32_t background);

void GFX_drawBasicElements(GFX *gfx_p, char *title);
void GFX_drawNumTrials(GFX *gfx_p, int numTrials);
void GFX_drawTrialResults(GFX *gfx_p, int position, double trialResult);

#endif /* HAL_GRAPHICS_H_ */
