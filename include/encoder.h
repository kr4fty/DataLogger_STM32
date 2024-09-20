#ifndef _ENCODER_H
#define _ENCODER_H
#include <AiEsp32RotaryEncoder.h>
#include "config.h"
#include "constantes.h"

AiEsp32RotaryEncoder encoder = AiEsp32RotaryEncoder(ENCODER_PIN_A,
                                                    ENCODER_PIN_B,
                                                    BUTTON_PIN,
                                                    ENCODER_VCC_PIN,
                                                    ENCODER_STEPS,
                                                    ENCODER_CENTRAL_PIN_TO_VCC);

void readEncoderISR()
{
	encoder.readEncoder_ISR();
}

bool wasButtonDown;
unsigned long lastTimeButtonDown;
bool shortClick, longClick, licked;
//paramaters for button
unsigned long shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned long longPressAfterMiliseconds = 1000;  //how long čong press shoud be.

void encoder_init()
{
    encoder.begin();
    encoder.setup(readEncoderISR);
    bool circleValues = false;
    encoder.setBoundaries(ADC_VGS_THRESHOLE, 4095, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    //encoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
    encoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
    encoder.setEncoderValue(0);
}

void encoder_miniInit()
{
    encoder.begin();
    encoder.setup(readEncoderISR);
    bool circleValues = true;
    encoder.setBoundaries(1, 2, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    //encoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
    encoder.setAcceleration(0); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
    encoder.setEncoderValue(1);
}

bool isButtonDown()
{
    return digitalRead(BUTTON_PIN) ? false : true;
}

bool isButtonClicked()
{
    bool clicked=false;
    // Deteccion de pulsacion de boton
    if (isButtonDown()&&!wasButtonDown) {
        if (!wasButtonDown) {
            //start measuring
            lastTimeButtonDown = millis();
        }
        //else we wait since button is still down
        wasButtonDown = true;
    }
    // Deteccion de pulsacion corta o larga
    if (!isButtonDown()&&wasButtonDown) {
        if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
            clicked = true;
        }
        wasButtonDown = false;
    }

    return clicked;
}

#endif