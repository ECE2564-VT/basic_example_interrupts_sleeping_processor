/*
 * Graphics.c
 *
 *  Created on: Dec 30, 2019
 *      Author: Matthew Zhong
 */

#include <PollingHAL/Graphics.h>
#include <stdio.h>

GFX GFX_construct(uint32_t defaultForeground, uint32_t defaultBackground)
{
    GFX gfx;

    gfx.defaultForeground = defaultForeground;
    gfx.defaultBackground = defaultBackground;

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    Graphics_initContext(
        &gfx.context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setFont(&gfx.context, &g_sFontFixed6x8);

    GFX_resetColors(&gfx);
    GFX_clear(&gfx);

    return gfx;
}

void GFX_resetColors(GFX *gfx_p)
{
    gfx_p->foreground = gfx_p->defaultForeground;
    gfx_p->background = gfx_p->defaultBackground;

    Graphics_setForegroundColor(&gfx_p->context, gfx_p->defaultForeground);
    Graphics_setBackgroundColor(&gfx_p->context, gfx_p->defaultBackground);
}

void GFX_clear(GFX* gfx_p)
{
    Graphics_clearDisplay(&gfx_p->context);
}

void GFX_setForeground(GFX* gfx_p, uint32_t foreground)
{
    gfx_p->foreground = foreground;
    Graphics_setForegroundColor(&gfx_p->context, foreground);
}

void GFX_setBackground(GFX *gfx_p, uint32_t background)
{
    gfx_p->background = background;
    Graphics_setBackgroundColor(&gfx_p->context, background);
}

void GFX_drawTitle(GFX *gfx_p, char *title)
{
    Graphics_setFont(&gfx_p->context, &g_sFontCm16b);
    Graphics_drawStringCentered(
        &gfx_p->context, (int8_t *)title, -1, 64, 10, true);
}
