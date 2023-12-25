//-------------------------------------------
// WS2812B_LED_Strip.h - Header File
// Library for Programming WS2812B LED Strip
// by: Anas Kuzechie (June, 2022)
//-------------------------------------------
//header guards
#ifndef WS2812B_LED_Strip_h
#define WS2812B_LED_Strip_h
//-----------------------------------------------------------------------
//class definition
class WS2812B_LED_Strip
{
    public:
        WS2812B_LED_Strip(byte DIN);
        void clearLEDs(byte numLEDs, boolean resetPulse);
        void color(byte Gbyte, byte Rbyte, byte Bbyte);
    private:
    byte _DIN;
};
//----------------------------------------------------------------------
#endif