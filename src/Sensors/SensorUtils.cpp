#include "SensorUtils.h"

String getZambrettiExplanation(char c)
{
    String explanation = "";
    switch (c)
    {
    case 'A':
        explanation = "Settled Fine Weather";
        break;
    case 'B':
        explanation = "Fine Weather";
        break;
    case 'C':
        explanation = "Becoming Fine";
        break;
    case 'D':
        explanation = "Fine, Becoming Less Settled";
        break;
    case 'E':
        explanation = "Fine, Possibly showers";
        break;
    case 'F':
        explanation = "Fairly Fine, Improving";
        break;
    case 'G':
        explanation = "Fairly Fine, Possibly showers early";
        break;
    case 'H':
        explanation = "Fairly Fine, Showers Later";
        break;
    case 'I':
        explanation = "Showery Early, Improving";
        break;
    case 'J':
        explanation = "Changeable Improving";
        break;
    case 'K':
        explanation = "Fairly Fine, Showers likely";
        break;
    case 'L':
        explanation = "Rather Unsettled Clearing Later";
        break;
    case 'M':
        explanation = "Unsettled, Probably Improving";
        break;
    case 'N':
        explanation = "Showery Bright Intervals";
        break;
    case 'O':
        explanation = "Showery Becoming Unsettled";
        break;
    case 'P':
        explanation = "Changeable some rain";
        break;
    case 'Q':
        explanation = "Unsettled, short fine Intervals";
        break;
    case 'R':
        explanation = "Unsettled, Rain later";
        break;
    case 'S':
        explanation = "Unsettled, rain at times";
        break;
    case 'T':
        explanation = "Very Unsettled, Finer at times";
        break;
    case 'U':
        explanation = "Rain at times, Worse later";
        break;
    case 'V':
        explanation = "Rain at times, becoming very unsettled";
        break;
    case 'W':
        explanation = "Rain at Frequent Intervals";
        break;
    case 'X':
        explanation = "Very Unsettled, Rain";
        break;
    case 'Y':
        explanation = "Stormy, possibly improving";
        break;
    case 'Z':
        explanation = "Stormy, much rain";
        break;
    default:
        explanation = "Unknown";
        break;
    }
    return explanation;
}

char getZambrettiChar(float P, Trend trend)
{
    byte M = month();
    char result = '0';

    // FALLING
    if (trend == T_FALLING)
    {
        double Z = 130 - (P / 8.1);
        // A Winter falling generally results in a Z value higher by 1 unit.
        if (M < 4 || M > 9)
        {
            Z += 1;
        }
        else
        {
            Z += 2;
        }

        Z = round(Z);
        switch (int(Z))
        {
        case 1:
            result = 'A';
            break; //Settled Fine
        case 2:
            result = 'B';
            break; //Fine Weather
        case 3:
            result = 'D';
            break; //Fine Becoming Less Settled
        case 4:
            result = 'H';
            break; //Fairly Fine Showers Later
        case 5:
            result = 'O';
            break; //Showery Becoming unsettled
        case 6:
            result = 'R';
            break; //Unsettled, Rain later
        case 7:
            result = 'U';
            break; //Rain at times, worse later
        case 8:
            result = 'V';
            break; //Rain at times, becoming very unsettled
        case 9:
            result = 'X';
            break; //Very Unsettled, Rain
        }
    } else if(trend == T_STEADY) {
        double Z = round(147 - (5 * (P / 37.6)));
        switch (int(Z))
        {
        case 10:
            result = 'A';
            break; //Settled Fine
        case 11:
            result = 'B';
            break; //Fine Weather
        case 12:
            result = 'E';
            break; //Fine, Possibly showers
        case 13:
            result = 'K';
            break; //Fairly Fine, Showers likely
        case 14:
            result = 'N';
            break; //Showery Bright Intervals
        case 15:
            result = 'P';
            break; //Changeable some rain
        case 16:
            result = 'S';
            break; //Unsettled, rain at times
        case 17:
            result = 'W';
            break; //Rain at Frequent Intervals
        case 18:
            result = 'X';
            break; //Very Unsettled, Rain
        case 19:
            result = 'Z';
            break; //Stormy, much rain
        }
    } else if (trend == T_RISING)
    {
        double Z = 179 - (2 * (P / 12.9));
        //A Summer rising, improves the prospects
        if (M < 4 || M > 9)
        {
            Z -= 1;
        }
        else
        {
            Z -= 2;
        }

        Z = round(Z);

        switch (int(Z))
        {
        case 20:
            result = 'A';
            break; //Settled Fine
        case 21:
            result = 'B';
            break; //Fine Weather
        case 22:
            result = 'C';
            break; //Becoming Fine
        case 23:
            result = 'F';
            break; //Fairly Fine, Improving
        case 24:
            result = 'G';
            break; //Fairly Fine, Possibly showers, early
        case 25:
            result = 'I';
            break; //Showery Early, Improving
        case 26:
            result = 'J';
            break; //Changeable, Improving
        case 27:
            result = 'L';
            break; //Rather Unsettled Clearing Later
        case 28:
            result = 'M';
            break; //Unsettled, Probably Improving
        case 29:
            result = 'Q';
            break; //Unsettled, short fine Intervals
        case 30:
            result = 'T';
            break; //Very Unsettled, Finer at times
        case 31:
            result = 'Y';
            break; //Stormy, possibly improving
        case 32:
            result = 'Z';
            break; //Stormy, much rain
        }
    }

    return result;
}

byte getForecastImageNumberFromZambrettiChar(char zambrettiChar)
{
    int M = month();

    switch (zambrettiChar)
    {
    case 'A':
    case 'B':
        return 1;
        break;
    case 'D':
    case 'H':
    case 'O':
    case 'E':
    case 'K':
    case 'N':
    case 'G':
    case 'I':
        //Winter
        if (M < 4 || M > 9)
        {
            return 3;
            break;
        }
        else
        {
            return 2;
            break;
        }
        break;
    case 'R':
    case 'U':
    case 'V':
    case 'X':
    case 'S':
    case 'W':
        //Winter
        if (M < 4 || M > 9)
        {
            return 6;
            break;
        }
        else
        {
            return 5;
            break;
        }
    case 'Z':
    case 'Y':
        return 7;
        break;
    case 'P':
    case 'J':
    case 'L':
    case 'M':
    case 'T':
        return 8;
        break;
    case 'C':
    case 'F':
    case 'Q':
        return 4;
        break;
    default:
        return 9;
        break;
        break;
    }
}

double dewPointC(double celsius, double humidity)
{
    // (1) Saturation Vapor Pressure = ESGG(T)
    double RATIO = 373.15 / (273.15 + celsius);
    double RHS = -7.90298 * (RATIO - 1);
    RHS += 5.02808 * log10(RATIO);
    RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO))) - 1);
    RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1);
    RHS += log10(1013.246);
    // factor -3 is to adjust units - Vapor Pressure SVP * humidity
    double VP = pow(10, RHS - 3) * humidity;
    // (2) DEWPOINT = F(Vapor Pressure)
    double T = log(VP / 0.61078); // temp var
    return (241.88 * T) / (17.558 - T);
}

long humindex(float temp, float dewPoint)
{
    return round(temp + 0.5555 * (6.11 * exp(5417.753 * (1 / 273.16 - 1 / (273.15 + dewPoint))) - 10));
}