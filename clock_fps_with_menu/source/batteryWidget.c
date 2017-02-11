#include "global.h"
#include "ov.h"


#define SECONDS_IN_DAY 86400
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60

#define TICKS_PER_SEC 0xFFB3D58
#define TICKS_PER_MIN 0x3BEE260A0UL
#define TICKS_IN_5_MIN 0x12BA6BE320UL

#define ADJ(posX, isBottom) (isBottom ? posX - 80 : posX)

u64     osGetTime(void);
Result  PTMU_GetBatteryLevel(u8 *out);
Result  MCU_GetBatteryLevel(u8* out);

#define LIMEGREEN   50,205,50
#define ORANGE      255,140,0
#define RED         255,0,0
#define BLANK       255,255,255

void    GetTimeString(char *output, u32 clockType)
{
    u64 timeInSeconds = osGetTime() / 1000;
    u64 dayTime = timeInSeconds % SECONDS_IN_DAY;
    
    u8 hour = dayTime / SECONDS_IN_HOUR;
    u8 min = (dayTime % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
    u8 seconds = dayTime % SECONDS_IN_MINUTE;
    
    if (clockType == 1) {
        
        char period[3] = {'A', 'M', '\0'};
        
        // Handle AM/PM
        if (hour == 0 || hour >= 12)
        {
            period[0] = 'P';
        }
    
        // 12 Hour Clock because 'merica
        if (hour==0)
        {
            hour = 12;
        }
        else if (hour > 12)
        {
            hour = hour - 12;
        }
        
        xsprintf(output, "%02d:%02d %s", hour, min, period);
        
    }
    else
    {
        xsprintf(output, "%02d:%02d", hour, min);
    }
    
}

int     DrawClockAndBattery(int isBottom, u32 percent, u32 clockType) 
{
    static u32      batval = 0;
    static u8      batPercent = 0;
    static u64      tick = 0;

    if (svc_getSystemTick() >= tick)
    {
        u8 batteryLevel = 0;
        PTMU_GetBatteryLevel(&batteryLevel);
        MCU_GetBatteryLevel(&batPercent);
        tick = svc_getSystemTick() + TICKS_IN_5_MIN;

        if (batteryLevel == 1) 
        {
            batval = 2;
        } 
        else if (batteryLevel == 2) 
        {
            batval = 5;
        } 
        else if (batteryLevel == 3) 
        {
            batval = 8;
        } 
        else if (batteryLevel == 4) 
        {
            batval = 11;
        } 
        else if (batteryLevel == 5) 
        {
            batval = 16;
        }
    }

    char buf[30] = {0}; 
    
    u32 xPos = 338;
    u32 batteryX = xPos; // This value is separate as it is also the same Position, 12 hour or not
    u32 width = 57;
    
    // If it is a 12 hour clock, adjust things slightly
    if ( clockType == 1 ) {
        xPos = 320;
        width = 75;
    }

    //DrawBackground
    // From Left, From Top, Width, Height, Transparency?
    OvDrawTranspartBlackRect(ADJ(xPos, isBottom), 9, width, 12, 1);

    // Draw battery
    OvDrawRect(ADJ(batteryX + 36, isBottom), 11, 18, 1, BLANK);
    OvDrawRect(ADJ(batteryX + 36, isBottom), 18, 18, 1, BLANK);
    OvDrawRect(ADJ(batteryX + 36, isBottom), 11, 1, 8, BLANK);
    OvDrawRect(ADJ(batteryX + 53, isBottom), 11, 1, 8, BLANK);

    if (batval >= 4)
        OvDrawRect(ADJ(batteryX + 37, isBottom), 12, batval, 6, LIMEGREEN);
    else if (batval >= 2 && batval <= 3)
        OvDrawRect(ADJ(batteryX + 37, isBottom), 12, batval, 6, ORANGE);
    else 
        OvDrawRect(ADJ(batteryX + 37, isBottom), 12, batval, 6, RED);

    OvDrawRect(ADJ(batteryX + 54, isBottom), 13, 1, 4, BLANK);

    if (percent)
    {
        xsprintf(buf, "%d%%", batPercent);
        int percentageX = ADJ(batteryX + 43, isBottom);
        int len = strlen(buf) - 1;
        percentageX -= (len) * 2;
        OvDrawStringTiny(buf, percentageX, 12, BLANK);        
    }



    // Draw clock
    GetTimeString(buf, clockType);
    OvDrawString(buf, ADJ(xPos + 3, isBottom), 11, BLANK);

    return (1);
}

int     DrawClockOnly(int isBottom, u32 clockType) 
{

    char buf[30] = {0};

    GetTimeString(buf, clockType);
    
    u32 xPos = 352;
    u32 width = 38;
    
    // If it is a 12 hour clock, adjust things slightly
    if ( clockType == 1 ) {
        xPos = 339;
        width = 51;
    }

    //DrawBackground
    // From Left, From Top, Width, Height, Transparency?
    OvDrawTranspartBlackRect(ADJ(xPos, isBottom), 9, width, 12, 1);
    // Draw clock
    OvDrawString(buf, ADJ(xPos, isBottom) + 2, 11, 255, 255, 255);
    return (1);
}
