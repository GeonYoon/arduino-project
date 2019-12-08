#include <Adafruit_NeoPixel.h>
#define BUTTON0 34
#define PIN 25
#define PIN13 13    // Grap Signal from the charger
#define NUM_LEDS 16 // Number of LEDs on the neo pixel ring
#define ringshow_noglitch() {delay(1);ring.show();delay(1);ring.show();}
Adafruit_NeoPixel ring = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);



void setup() {
  // put your setup code here, to run once:
  pinMode(PIN13, INPUT);
  Serial.begin(9600);

  // ring
  ring.begin();
  ring.setBrightness(32);
  ring.clear(); // clear all pixels
  ring.setPixelColor(0,0);
  ringshow_noglitch(); 
}

byte prev0 = 1;
int r= 255;
int g = 0;
int b = 0;


void loop() {
  int num_leds = 16;
  byte curr0 = digitalRead(PIN13); 
  if(curr0 != prev0){ 
    if(curr0 == 1){
      // previous state was 0 and when it becomes 1
      // when the phone starts to charge
      int j = 1;
      for(int i=0;i<num_leds;i++) {
        if(i< 12){
          ring.setPixelColor(i,r,g+(i*21),b); 
        }
        if(i >= 12){
          ring.setPixelColor(i,r-(j*40),255,b); 
          j++;    
        }
      }
      
       if(num_leds == 0){
        ring.setPixelColor(0,0,0,0); 
        }
      
       //Turn off any remaining LED's
       for(int i= num_leds+1;i<16;i++) {
           ring.setPixelColor(i,0,0,0); 
      }
      ringshow_noglitch(); 
    }
    else{
      Serial.println("Finish charging");
      //Turn off entire LED's
       for(int i= 0;i<16;i++) {
           ring.setPixelColor(i,0,0,0); 
      }
      ringshow_noglitch();    
    }
   }
  prev0 = curr0;
}
