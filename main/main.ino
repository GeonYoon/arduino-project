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

// flags 
byte prev0 = 1; // detect to start charging
byte siri_flag = 1; 
byte battery_flag = 1;

// global variables 
int r= 255;
int g = 0;
int b = 0;
int led = 0;
int off = 0;

// helper functions 
void update_led() {
  if(server.hasArg("value")) {
    // get the value from the param 
    int led_value = server.arg("value").toInt();

    // convert the battery to the proper number of LED
    float temp = NUM_LEDS * (led_value/100.0);
    int temp_led = int(temp);
    Serial.print(temp_led);
    // change flag if the new battery info is different from the cached one
    if(temp_led != led){
      battery_flag = 0;
      led = temp_led;
    } 
    
    // send response back to the pohone
    server.send(200, "text/html", "{\"result\":1}");
  } else { server.send(200, "text/html", "{\"result\":0}"); }
}


// This function is trigger by siri call. 
// ex. "Hey Siri Trun off the LED" or "Hey Siri Trun on the LED"
void update_led_status() {
  if(server.hasArg("value")) {
    
    // get the value from the param 
    int val = server.arg("value").toInt();

    // change the flag to 0
    siri_flag = 0;
    off = val;
    
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

  // Open up the wifi and recieve the API calls 
  WiFi.mode(WIFI_AP); // start ESP in AP mode
  WiFi.softAP(ssid, pass); 
  server.on("/led", update_led); 
  server.on("/status", update_led_status);
  server.begin();  // starts server
}

void loop(void){
  int num_leds = led;              // based on the phone's battery
  byte curr0 = digitalRead(PIN13); // get the signal from the charger. 1 == ON, 0 == OFF
  
  // Server is running
  server.handleClient();  // handle client requests, must call frequently

  // Update when the charging starts or phone leavs
  if(curr0 != prev0){    // when we turn off
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

  // update when we get siri order either turn_on or turn_off
  if(siri_flag == 0){
    if(off == 0 && curr0 == 1){
      turn_on(num_leds);
      trun_off(num_leds);
      ringshow_noglitch(); 
    }
    else{
      trun_off(-1);
      ringshow_noglitch();
    }
    siri_flag = 1;
  }
  
  // Update when the battery changes 
  if(battery_flag == 0 && curr0 == 1){
      turn_on(num_leds);
      trun_off(num_leds);
      ringshow_noglitch(); 
      battery_flag = 1;
   }
}
