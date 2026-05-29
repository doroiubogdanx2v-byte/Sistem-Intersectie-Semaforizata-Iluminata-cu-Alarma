#include "drivers/gpio/gpio.h"
#include "drivers/timer/timer0.h"
#include "bsp/nano.h"
#include "utils/delay.h"
#include "drivers/adc/adc.h" 

#define ButonPieton0 A0
#define ButonPieton1 A1
#define RosuPieton D5
#define VerdePieton A5
#define RosuMasina A4
#define GalbenMasina D12
#define VerdeMasina D3

#define BuzzerPin D6
#define Iluminat1 D7
#define Iluminat2 D8
#define LDR_CHANNEL 2 


#define PRAG_INTUNERIC 200

void ActualizeazaIluminat() {
    
    static uint32_t last_ldr_read = 0; 
    
    // Verificăm senzorul doar o dată la 200ms pentru a nu bloca funcția Millis()
    if (Millis() - last_ldr_read >= 200) {
        last_ldr_read = Millis();
        uint16_t nivel_lumina = ADC_Read(LDR_CHANNEL);
        
        if (nivel_lumina < PRAG_INTUNERIC) {
            GPIO_Write(Iluminat1, GPIO_HIGH);
            GPIO_Write(Iluminat2, GPIO_HIGH);
        } else {
            GPIO_Write(Iluminat1, GPIO_LOW);
            GPIO_Write(Iluminat2, GPIO_LOW);
        }
    }
}

int main(void) {
    Timer0_Init();
    ADC_Init();
    
    GPIO_Init(ButonPieton0, GPIO_INPUT);
    GPIO_Init(ButonPieton1, GPIO_INPUT);
    GPIO_Init(RosuPieton, GPIO_OUTPUT);
    GPIO_Init(VerdePieton, GPIO_OUTPUT);
    GPIO_Init(RosuMasina, GPIO_OUTPUT);
    GPIO_Init(GalbenMasina, GPIO_OUTPUT);
    GPIO_Init(VerdeMasina, GPIO_OUTPUT);
    
    GPIO_Init(BuzzerPin, GPIO_OUTPUT);
    GPIO_Init(Iluminat1, GPIO_OUTPUT);
    GPIO_Init(Iluminat2, GPIO_OUTPUT);

    uint32_t waittime = 10000;
    uint32_t last_time;
    uint8_t starebuton0 = 0;
    uint8_t starebuton1 = 0;

    while (1) {
        
        // Actualizăm iluminatul la intrarea în ciclu
        ActualizeazaIluminat(); 

        last_time = Millis();
        GPIO_Write(RosuPieton, GPIO_HIGH);
        GPIO_Write(VerdeMasina, GPIO_HIGH);
        
        while (Millis() - last_time < waittime) {
            ActualizeazaIluminat(); 

            if (GPIO_Read(ButonPieton0) == GPIO_HIGH && starebuton0 == 0) {
                starebuton0 = 1;
                waittime = waittime - 1000;
            }
            if (GPIO_Read(ButonPieton1) == GPIO_HIGH && starebuton1 == 0) {
                starebuton1 = 1;
                waittime = waittime - 1000;
            }
        }
        
        waittime = 10000;
        starebuton0 = 0;
        starebuton1 = 0;
        
        GPIO_Write(VerdeMasina, GPIO_LOW);
        GPIO_Write(GalbenMasina, GPIO_HIGH);
        last_time = Millis();
        
        while (Millis() - last_time < 5000) {
            ActualizeazaIluminat(); 
        }
        
        GPIO_Write(GalbenMasina, GPIO_LOW);
        GPIO_Write(RosuPieton, GPIO_LOW);
        GPIO_Write(VerdePieton, GPIO_HIGH);
        GPIO_Write(RosuMasina, GPIO_HIGH);
        
        last_time = Millis();
        uint32_t last_beep_time = Millis();
        uint32_t last_tone_time = Millis();
        uint8_t beep_activ = 0;
        
        while (Millis() - last_time < 5000) {
            ActualizeazaIluminat(); 

            if (Millis() - last_beep_time >= 1000) { 
                last_beep_time = Millis();
                beep_activ = 1;
            }
            if (beep_activ && (Millis() - last_beep_time > 100)) {
                beep_activ = 0;
                GPIO_Write(BuzzerPin, GPIO_LOW);
            }
            if (beep_activ) {
                if (Millis() - last_tone_time >= 1) { 
                    GPIO_Toggle(BuzzerPin);
                    last_tone_time = Millis();
                }
            }
        }
        
        for (int i = 0; i < 11; i++) {
            last_time = Millis();
            last_tone_time = Millis();
            
            while (Millis() - last_time < 500) {
                ActualizeazaIluminat(); 

                if (Millis() - last_time < 80) {
                    if (Millis() - last_tone_time >= 1) {
                        GPIO_Toggle(BuzzerPin);
                        last_tone_time = Millis();
                    }
                } else {
                    GPIO_Write(BuzzerPin, GPIO_LOW); 
                }
            }
            GPIO_Toggle(VerdePieton);
        }
        
        GPIO_Write(VerdePieton, GPIO_LOW);
        GPIO_Write(BuzzerPin, GPIO_LOW); 
        GPIO_Write(RosuMasina, GPIO_LOW);
    }
}