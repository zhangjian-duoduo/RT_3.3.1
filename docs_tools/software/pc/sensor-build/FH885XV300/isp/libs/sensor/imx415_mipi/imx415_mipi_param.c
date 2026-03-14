
#define _NAME(n, s) n##_##s
#if MULTI_SENSOR
#define NAME(n) _NAME(n, imx415_mipi)
#else
#define NAME(n) n
#endif
unsigned int NAME(ImageParam)[0];
unsigned int NAME(contrast)[0];
unsigned int NAME(saturation)[0];
unsigned int NAME(sharpness)[0];
unsigned int NAME(MirrorFlipBayerFormat)[4];
unsigned int NAME(AwbGain)[4][3];
unsigned int NAME(LtmMapping00)[128];
unsigned int NAME(LtmMapping01)[128];
unsigned int NAME(LtmMapping02)[128];
unsigned int NAME(LtmMapping03)[128];
unsigned int NAME(LtmMapping04)[128];
unsigned int NAME(LtmMapping05)[128];
unsigned int NAME(LtmMapping06)[128];
unsigned int NAME(LtmMapping07)[128];
unsigned int NAME(LtmMapping08)[128];
unsigned int NAME(LtmMapping09)[128];
unsigned int NAME(LtmMapping10)[128];
