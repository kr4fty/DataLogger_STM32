#ifndef _DISPLAY_LCD_H
#define _DISPLAY_LCD_H

#include <Adafruit_PCD8544.h>
#include "config.h"
                                        

Adafruit_PCD8544 lcd = Adafruit_PCD8544(LCD_SCLK_PIN, LCD_DIN_PIN, LCD_DC_PIN, LCD_CS_PIN, LCD_RST_PIN);
#define LED LED_BUILTIN
#define DISPLAY_UPDATE_WINDOW 200 // Actualizo cada 200 mili segundos

bool updateDisplay = false;
char buff[20], aux[10];

void floatTostr(float numero, uint8_t size_buff, uint8_t decimales, char *buf= buff)
{
    if((decimales+1)<=size_buff){
        int parte_entera, parte_decimal;

        parte_entera = (int)numero; // Obtenemos parte Entera
        switch (size_buff-decimales-1) // digitos de la parte entera
        {
        case 0:
            sprintf(buf,".");
            break;
        case 1:
            sprintf(buf,"%1d.",parte_entera); // Obtenemos parte Entera
            break;
        case 2:
            sprintf(buf,"%2d.",parte_entera); // Obtenemos parte Entera
            break;
        case 3:
            sprintf(buf,"%3d.",parte_entera); // Obtenemos parte Entera
            break;
        default:
            break;
        }

        parte_decimal = (int)((numero - parte_entera) * pow(10,decimales));  // Multiplicamos por 10^x para obtener los decimales deseados
        switch (decimales)
        {
        case 1:
            sprintf(buf, "%s%01d", buf, parte_decimal);
            break;
        case 2:
            sprintf(buf, "%s%02d", buf, parte_decimal);
            break;
        case 3:
            sprintf(buf, "%s%03d", buf, parte_decimal);
            break;
        case 4:
            sprintf(buf, "%s%04d", buf, parte_decimal);
            break;
        case 5:
            sprintf(buf, "%s%05d", buf, parte_decimal);
            break;
        default:
            break;
        }
    }
    else
        sprintf(buf,"NULL");
}

void lcd_init()
{
    pinMode(LCD_BKLIGHT_PIN, OUTPUT);
    lcd.begin();
    lcd.cp437(true);
    lcd.setContrast(75);
    lcd.setRotation(2);
    lcd.clearDisplay();
    lcd.setTextColor(BLACK, WHITE);
    lcd.setTextSize(1);
    lcd.setCursor(8, LCDHEIGHT/2-4);
    lcd.print("INICIANDO...");
    lcd.display();
    lcd.clearDisplay();
}

void lcd_printCalibration()
{
    lcd.clearDisplay();
    lcd.setTextSize(1);
    lcd.print("CALIBRACION");
    lcd.setCursor(0*6, 2*8);
    lcd.print("Coloque una carga de 1A y presione para finalizar");
    lcd.display();
}

void lcd_printSelection()
{
    lcd.clearDisplay();
    lcd.setTextSize(1);
    lcd.setTextColor(WHITE, BLACK);
    lcd.print(" SELECT MOSFET");
    lcd.setTextColor(BLACK, WHITE);
    lcd.setCursor(0*6, 1*8);
    lcd.print("Seleccione mosfet a muestrear");
    lcd.display();
}
void lcd_clear()
{
    lcd.clearDisplay();
}

void lcd_printMosfetSelection(uint8_t sel)
{
    lcd.setTextSize(2);
    lcd.setCursor(0*6*2, 2*8*2);
    lcd.print("MOSFET");
    lcd.print(sel);
    lcd.display();
    lcd.setTextSize(1);
}

void lcd_printMosfet(uint8_t sel, double vRaw, double iRaw)
{
    //sprintf(buff, "M%d %3.1f %3.1f",sel,vRaw, iRaw);
    sprintf(buff, "M%d ", sel);
    floatTostr(vRaw, 5, 1, aux);
    strcat(buff, aux);
    strcat(buff, " ");
    floatTostr(iRaw, 5, 1, aux);
    strcat(buff, aux);

    lcd.setTextSize(1);
    lcd.setCursor(0*6,0*8);
    lcd.print(buff);

    updateDisplay = true;
}

void lcd_printBaseFrame()
{
    lcd.clearDisplay();
    lcd.setTextSize(2);
    lcd.setCursor(2*6*6,2*0*8);
    lcd.print("V");
    lcd.setCursor(2*6*6,2*1*8);
    lcd.print("A");
    lcd.setTextSize(1);
    lcd.setCursor(1*4*6,1*4*8);
    lcd.print("mAh");
    lcd.setCursor(1*12*6,1*4*8);
    lcd.print("Wh");
    lcd.setCursor(1*12*6,1*5*8);
    //lcd.print("  :  :      ");
    lcd.print((char)248); // ascii de 'º'
    lcd.print("C");

    updateDisplay = true;
}

void lcd_printNewSetpoint(float value)
{
    //floatTostr(value, 4, 2);
    //dtostrf(value, 4, 2, buff);
    lcd.setTextSize(3);
    lcd.fillRect(0, ((LCDHEIGHT-3*8)/2)-1, 84, (3*8)+1, WHITE);
    lcd.setCursor(0,(LCDHEIGHT-(3*8))/2);
    lcd.setTextColor(BLACK,WHITE);
    //lcd.print(buff);
    lcd.print((int)value);
    lcd.setCursor(1,(LCDHEIGHT-(3*8))/2+1);
    lcd.setTextColor(BLACK);
    //lcd.print(buff);
    lcd.print((int)value);
    lcd.setTextSize(2);
    lcd.setCursor(2*6*6,2*1*8);
    lcd.print("A");
    lcd.drawRect(0, ((LCDHEIGHT-3*8)/2)-2, 84, (3*8)+2, BLACK);
    lcd.setTextColor(BLACK,WHITE);
    lcd.display();
}

void lcd_printTinyNewSetpoint(float value)
{
    floatTostr(value, 6, 3);
    //dtostrf(value, 6, 3, buff);

    lcd.setTextSize(2);
    lcd.setCursor(2*0*6,2*1*8);
    lcd.setTextColor(WHITE, BLACK);
    lcd.print(buff);
    lcd.setTextColor(BLACK, WHITE);

    updateDisplay = true;
}

void lcd_printPowerOnMessage()
{
    lcd.setTextSize(1);
    lcd.setCursor(0, 1*5*8);
    lcd.setTextColor(WHITE, BLACK);
    lcd.print("   POWER ON   ");
    lcd.setTextColor(BLACK, WHITE);

    updateDisplay = true;
}

void lcd_printPowerOffMessage()
{
    lcd.setTextSize(1);
    lcd.setCursor(0, 1*5*8);
    lcd.print("   POWER OFF  ");

    updateDisplay = true;
}

void lcd_printOverTemperatureMessage()
{
    lcd.setTextSize(1);
    lcd.setCursor(0*5, 5*8);
    lcd.setTextColor(WHITE, BLACK);
    lcd.print(" TEMP. MAXIMA ");
    lcd.setTextColor(BLACK, WHITE);

    updateDisplay = true;
}

void lcd_printAmpHour(float a_h)
{
    sprintf(buff, "%4d",(int)a_h);

    lcd.setTextSize(1);
    lcd.setCursor(0*6,4*8);
    lcd.print(buff);

    updateDisplay = true;
}

void lcd_printWattHour(float w_h)
{
    lcd.setTextSize(1);

    if(w_h<1){
        lcd.setCursor(8*6,4*8);
        sprintf(buff, "%3d",(int)(w_h*1000));
        lcd.print(buff);
        lcd.print("m");
    }
    else if(w_h<10){
        lcd.setCursor(8*6,4*8);
        //dtostrf(w_h, 4, 2, buff);
        floatTostr(w_h, 4, 2);
        lcd.print(buff);
    }
    else if(w_h<100){
        lcd.setCursor(8*6,4*8);
        floatTostr(w_h, 4, 1);
        //dtostrf(w_h, 4, 1, buff);
        lcd.print(buff);
    }
    else{
        lcd.setCursor(7*6,4*8);
        //dtostrf(w_h, 4, 1, buff);
        floatTostr(w_h, 4, 1);
        lcd.print(buff);
    }

    updateDisplay = true;
}

void lcd_printTime(uint8_t hs, uint8_t min, uint8_t seg)
{
    sprintf(buff, "%02d:%02d:%02d",hs,min,seg);

    lcd.setTextSize(1);
    lcd.setCursor(0*6,5*8);
    lcd.setTextColor(WHITE, BLACK);
    lcd.print(buff);
    lcd.setTextColor(BLACK, WHITE);

    updateDisplay = true;
}

void lcd_printTemperature(float mosfet_temp)
{
    sprintf(buff, "%3d",(int)mosfet_temp);

    lcd.setTextSize(1);
    lcd.setCursor(9*6,5*8);
    lcd.print(buff);

    updateDisplay = true;
}

void lcd_printVin(float v_in)
{
    //dtostrf(v_in, 6, 2, buff);
    floatTostr(v_in, 6, 2);

    lcd.setTextSize(2);
    lcd.setCursor(2*0*6,2*0*8);
    lcd.print(buff);

    updateDisplay = true;
}

void lcd_printIin(float i_in)
{
    //dtostrf(i_in, 6, 2, buff);
    floatTostr(i_in, 6, 2);

    lcd.setTextSize(2);
    lcd.setCursor(2*0*6,2*1*8);
    lcd.print(buff);

    updateDisplay = true;
}

void lcd_printERROR(uint8_t x, uint8_t y, uint8_t sz=1)
{
    lcd.setCursor(x*sz*6, y*sz*8);
    lcd.setTextSize(sz);
    lcd.print("ERROR");
    
    lcd.display();
}

void lcd_display()
{
    lcd.display();
    updateDisplay = false;
}

#endif