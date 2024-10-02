#include <Arduino.h>
#include "config.h"
#include "encoder.h"
#include "display.h"
#include "pwm.h"
#include "sound.h"

int dutyCycle = 0, selection;
bool changed=true;
double vBattRawOld, iBattRawOld;
unsigned long nextTime, startTime;
#define WINDOW_CAPTURE   5
#define WINDOW_10SEG 10000

void setup() {
  Serial.begin(57600);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  
  lcd_init();
  pwm_init();
  tone_init();

  // Lectura inicial de los sensores
  vBattRawOld = analogRead(VBATT_SENSE_PIN)-.01; // le resto un pequeño valor para que imprima la tension cuando
  iBattRawOld = analogRead(IBATT_SENSE_PIN);

  tone(BUZZER_PIN, 434, 100);
  lcd_printSelection();
  encoder_miniInit();
  do{    
    if (changed)
    {
      selection = encoder.readEncoder();

      lcd_printMosfetSelection(selection);
    }
    changed = encoder.encoderChanged();
  }while(!isButtonClicked());
  lcd_clear();

  tone(BUZZER_PIN, 2000, 150);
  encoder_init();

  dutyCycle = 2982; // Aprox 1A

  encoder.setEncoderValue(dutyCycle);
  lcd_printNewSetpoint(dutyCycle);

  pwm_setDuty1(0);
  pwm_setDuty2(0);
}

bool isPowerOn = false; // TRUE: en funcionamiento, False: no ejecutandose
double vBattRaw, iBattRaw;
bool wasVUpdated, wasIUpdated;
long timeToUpdateDisplay=millis()+DISPLAY_UPDATE_WINDOW;
unsigned long powerStateMessageTime, showMessageDuringThisTime = 2000; // 2 seg
bool printStatusMessage= false;
bool isTheSetpointUpdated; // Para tener prioridad al mostrar nuevo setpoint
unsigned long timeToPrintNewSetpoint, windowNewSetpoint=1000;
double    Time;

void loop() {
  /***************************************************************************/
  /*                            SETEO DE CORRIENTE                           */
  /***************************************************************************/
  if (encoder.encoderChanged())
  {
    dutyCycle = encoder.readEncoder();

    lcd_printNewSetpoint(dutyCycle);
    tone(BUZZER_PIN, 600, 10);
    isTheSetpointUpdated = true;    

    if(isPowerOn){ // Solo actualizo si esta en ejecucion la carga electronica
      switch(selection){
        case 1:
              pwm_setDuty1(dutyCycle); pwm_setDuty2(0); break;
        case 2:
              pwm_setDuty1(0); pwm_setDuty2(dutyCycle); break;
        default: break;
      }
    }

    timeToPrintNewSetpoint = millis() + windowNewSetpoint;
  }
  if(isTheSetpointUpdated && (millis()>timeToPrintNewSetpoint)){
      isTheSetpointUpdated = false;
      if(isPowerOn){
        tone(BUZZER_PIN, 7000, 20);
        delay(75);
        tone(BUZZER_PIN, 7000, 80);
      }
  }

  if (encoder.isEncoderButtonClicked()){
    isPowerOn = not isPowerOn;

    // ENCENDIDO
    if(isPowerOn){
      lcd_printPowerOnMessage();
      tone(BUZZER_PIN, 4000, 50);
      delay(20);
      tone(BUZZER_PIN, 4500, 50);
      delay(20);
      tone(BUZZER_PIN, 5000, 50);

      switch(selection){
        case 1:
              pwm_setDuty1(dutyCycle); pwm_setDuty2(0); break;
        case 2:
              pwm_setDuty1(0); pwm_setDuty2(dutyCycle); break;
        default: break;
      }
      
      startTime = millis();
      nextTime = startTime + WINDOW_CAPTURE;    
    }
    // APAGADO
    else{
      lcd_printPowerOffMessage();
      //pwm_setDuty(0);
      tone(BUZZER_PIN, 436, 100);
      tone(BUZZER_PIN, 200, 150);

      pwm_setDuty1(0);
      pwm_setDuty2(0);
    }
    printStatusMessage = true;
    powerStateMessageTime = millis() + showMessageDuringThisTime;

    digitalWrite(LED, digitalRead(LED)?LOW:HIGH);
  }  
  // FIN CONFIGURACION DE CORRIENTE DE CARGA


  vBattRaw = analogRead(VBATT_SENSE_PIN);
  // Filtro de Wiener, Adaptativo
  vBattRaw = vBattRawOld + MU * (vBattRaw - vBattRawOld); 

  if(vBattRaw!=vBattRawOld){// si cambio V entonces actualizo valor en el LCD
    wasVUpdated = true;
    vBattRawOld = vBattRaw;
  }

  iBattRaw = (double)analogRead(IBATT_SENSE_PIN);
  // Filtro de Wiener, Adaptativo
  iBattRaw = iBattRawOld + MU * (iBattRaw - iBattRawOld);

  if(iBattRaw!=iBattRawOld){ // si cambio I entonces actualizo valor en el LCD
    wasIUpdated = true;
    iBattRawOld = iBattRaw;
  }

  if(millis()>timeToUpdateDisplay){
    /*// Imprimo Tension y Corriente
    if(wasVUpdated){
      // Print Vin
      lcd_printVin();
      wasVUpdated = false;
    }*/
    if(wasIUpdated && isPowerOn){
      // Print Iin
      lcd_printMosfet(selection, vBattRaw, iBattRaw);
      wasIUpdated = false;
    }

    lcd_display();
    
    timeToUpdateDisplay = millis()+DISPLAY_UPDATE_WINDOW;
  }


  /***************************************************************************/
  /*                            Refresco la Pantalla                         */
  /***************************************************************************/
  if(millis()>nextTime && isPowerOn && millis()<(startTime+WINDOW_10SEG)){
   
    Time = (millis()-startTime);
    // Para graficar y obtener datos utilizando serial_port_plotter
    //sprintf(buff, "$%d %ld %d;", (int)dutyCycle, Time, (int)iBattRaw);
    Serial.printf("$%d %ld %d;", dutyCycle, (unsigned long)Time, (int)iBattRaw);
    nextTime = millis() + WINDOW_CAPTURE;
  }  
  // FIN UPDATE DISPLAY
}