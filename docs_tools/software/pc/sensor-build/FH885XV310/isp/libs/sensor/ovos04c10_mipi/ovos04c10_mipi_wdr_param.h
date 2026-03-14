#ifndef __OVOS04C10_MIPI_WDR_PARAM_H__
#define __OVOS04C10_MIPI_WDR_PARAM_H__

static unsigned int AwbGainWdr[4][3] = {
    {0x2f6, 0x222, 0x63d}, //a-2600k
    {0x446, 0x222, 0x52d}, //4100k
    {0x492, 0x222, 0x3b5}, //6600k
    {0x452, 0x222, 0x40a}, //7500K
};

static unsigned int MirrorFlipBayerFormatWdr[4] =
{
        2, 2, 2, 2
};

#endif