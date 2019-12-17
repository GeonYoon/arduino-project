#include <WiFi.h> // header we need to include for WiFi functions
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

#define BUTTON0 34
#define PIN 25
#define PIN13 13    // Grap Signal from the charger
#define PIN15 15    // LED wire LED Data 
#define NUM_LEDS 71 // Number of LEDs on the neo pixel ring
#define ringshow_noglitch() {delay(1);ring.show();delay(1);ring.show();}
Adafruit_NeoPixel ring = Adafruit_NeoPixel(75, PIN15, NEO_GRB + NEO_KHZ800);

const char* ssid = "gun_final"; // !!! CHANGE THIS to your name or sth. unique
const char* pass = "cics290m";
WebServer server(80);  // define server object, at port 80

// initial variables
byte prev0 = 1;
int r= 255;
int g = 0;
int b = 0;
int led = 0;

// functions go here 
void on_set() {
  if(server.hasArg("value")) {
    // get the value from the params 
    int led_value = server.arg("value").toInt();

    // convert the battery to the proper number of LED
    float temp = NUM_LEDS * (led_value/100.0);
    led = int(temp);

    // send response back to the pohone
    server.send(200, "text/html", "{\"result\":1}");
  } else { server.send(200, "text/html", "{\"result\":0}"); }
}

// Turn ON LED
void turn_on(int num_leds){
  int j = 1;
  for(int i=0;i<num_leds;i++) {
    if(i< 35){
      ring.setPixelColor(i,r,g+(i*7),b); 
    }
    if(i >= 35){
      ring.setPixelColor(i,r-(j*7),255,b); 
      j++;    
    }
  }
}

// Turn OFF LED
void trun_off(int num_leds){
  for(int i= num_leds+1;i<NUM_LEDS;i++) {
     ring.setPixelColor(i,0,0,0); 
  }
}
void setup(void){
  // charger info setup
  pinMode(PIN13, INPUT);
  // get info
  pinMode(PIN15, OUTPUT);

  // ring setup 
  ring.begin();
  ring.setBrightness(32);
  ring.clear(); // clear all pixels
  ring.setPixelColor(0,0);
  ringshow_noglitch(); 

  // For debug
  Serial.begin(9600);
  
  WiFi.mode(WIFI_AP); // start ESP in AP mode
  WiFi.softAP(ssid, pass); // configure ssid and (optionally) password 
  server.on("/set", on_set); // set callback function
  server.begin();  // starts server
}

void loop(void){
  int num_leds = led;              // based on the phone's battery
  byte curr0 = digitalRead(PIN13); // get the signal from the charger. 1 == ON, 0 == OFF
  
  server.handleClient();  // handle client requests, must call frequently
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
      num_leds = 0;    
    }
   }
   prev0 = curr0;
}
