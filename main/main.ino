#include <Adafruit_NeoPixel.h>
#include <WiFi.h> // header we need to include for WiFi functions
#include <WebServer.h>

#define BUTTON0 34
#define PIN 25
#define PIN13 13    // Grap Signal from the charger
#define NUM_LEDS 16 // Number of LEDs on the neo pixel ring
#define ringshow_noglitch() {delay(1);ring.show();delay(1);ring.show();}
Adafruit_NeoPixel ring = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

// initial variables
byte prev0 = 1;
int r= 255;
int g = 0;
int b = 0;
int led = 0;

// local wifi
const char* ssid = "geonyoon"; 
const char* pass = "pass";
WebServer server(80); 

// functions go here 
void on_set() {
  if(server.hasArg("value")) {
    // get the value from the params 
    int led_value = server.arg("value").toInt();

    // convert the battery to the proper number of LED
    float temp = 16 * (led_value/100.0);
    led = int(temp);

    // send response back to the pohone
    server.send(200, "text/html", "{\"result\":1}");
  } else { server.send(200, "text/html", "{\"result\":0}"); }
}

// Turn ON LED
void turn_on(int num_leds){
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
}

// Turn OFF LED
void trun_off(int num_leds){
  for(int i= num_leds+1;i<16;i++) {
     ring.setPixelColor(i,0,0,0); 
  }
}


void setup() {
  
  // charger info setup
  pinMode(PIN13, INPUT);

  // ring setup 
  ring.begin();
  ring.setBrightness(32);
  ring.clear(); // clear all pixels
  ring.setPixelColor(0,0);
  ringshow_noglitch(); 

  // For debug
  Serial.begin(9600);

  // wifi setup
  WiFi.mode(WIFI_AP); // set ESP in AP mode
  WiFi.softAP(ssid, pass); // sets ssid and password
  server.on("/set", on_set); // set callback function
  server.begin();
}

void loop() {
  
  int num_leds = led;              // based on the phone's battery
  byte curr0 = digitalRead(PIN13); // get the signal from the charger. 1 == ON, 0 == OFF

  // run server
  server.handleClient();
  
  if(curr0 != prev0){ 
    if(curr0 == 1){
      // when the phone starts to charge
      turn_on(num_leds);
      trun_off(num_leds);
      ringshow_noglitch(); 
    }
    else{
      // when the phone leaves the charger
      trun_off(-1);
      ringshow_noglitch();    
    }
   }
   prev0 = curr0;
}
