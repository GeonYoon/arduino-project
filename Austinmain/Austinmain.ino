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
byte demo_flag = 1;
byte timer_flag = 1;

// global variables 
int r= 255;
int g = 0;
int b = 0;
int led = 0;
int off = 0;
int time_val = 0;

// helper functions 
void update_led() {
  if(server.hasArg("value")) {
    // get the value from the param 
    int led_value = server.arg("value").toInt();

    // convert the battery to the proper number of LED
    float temp = NUM_LEDS * (led_value/100.0);
    int temp_led = int(temp);
   
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

void update_demo() {
  if(server.hasArg("value")) {
    
    // get the value from the param 
    int val = server.arg("value").toInt();

    // change the flag to 0
    demo_flag = 0;
    
    // send response back to the pohone
    server.send(200, "text/html", "{\"result\":1}");
  } else { server.send(200, "text/html", "{\"result\":0}"); }
}

void update_timer() {
  if(server.hasArg("value")) {
    
    // get the value from the param 
    int val = server.arg("value").toInt();

    // change the flag to 0
    timer_flag = 0;
    time_val = val;
    
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

void helper(int num_leds){
//      turn_on(num_leds);
   battery_pulse(num_leds, 5);
      trun_off(num_leds);
      ringshow_noglitch(); 
}

// demo function()
void demo(){
      float ten_t = NUM_LEDS * (10/100.0);
      float fifty_t = NUM_LEDS * (50/100.0);
      float seventyfive_t = NUM_LEDS * (75/100.0);
      float full_t = NUM_LEDS * (100/100.0);
      
      int ten = int(ten_t);
      int fifty = int(fifty_t);
      int seventyfive = int(seventyfive_t);
      int full = int(full_t);

       
      helper(ten);
      delay(2000);
      
      helper(fifty); 
      delay(2000);

      helper(seventyfive);
      delay(2000);

      helper(full);
      delay(2000);

      // set back to original
      helper(led);
}

void timer(int end_time){
    int until = end_time;
    int i = 0;
    float increase_f = NUM_LEDS / end_time ;
    int increase = int(increase_f);
    int curr = 0;
    while(i < until){ 
      helper(curr);
      curr = curr + increase;
      delay(1000);  // wait 1 sec   
      i = i + 1; //increment
    }
    helper(NUM_LEDS);
    
    // blink two time
    delay(500);
    trun_off(-1);
    ringshow_noglitch(); 
    delay(500);
    helper(NUM_LEDS);

    delay(500);
    trun_off(-1);
    ringshow_noglitch(); 
    delay(500);
    helper(NUM_LEDS);

    // reset 
    delay(500);
    helper(led); 
}


bool fade = false;
void battery_pulse(int curr_leds, int delay_speed) {
  int j = 1;
  //First n LEDS will be red
  int num_red = 3;    
  //Last n LEDS will be green
  int num_green = 3;
  int first_half = NUM_LEDS /2;
  int last_half = NUM_LEDS - first_half;

 //Fade brightness
 if(fade == false){
   for(int i=0;i<curr_leds;i++) {
      if(i< first_half){
        if(i< num_red){

          //Dim first 3 LEDS
           for(int k=255; k >=50;k--) {
            ring.setPixelColor(i, k, g, b); 
            delay(0.5);
            ringshow_noglitch();
           }
           
        } else{
          int gpx = g+((i-num_red)*(255/(first_half - num_red)));
          for(int k=100; k >50;k--) {
            ring.setPixelColor(i, r*(k/100.0), gpx*(k/100.0), b); 
            Serial.print(ring.getPixelColor(i));
            delay(3);
            ringshow_noglitch();
          }
        }
      }
      
      if(i >= first_half & i < NUM_LEDS - num_green){
        int rpx = r-(j*(255/last_half));
        for(int k=100; k >50;k--) {
            ring.setPixelColor(i, rpx*(k/100.0), 255*(k/100.0), b*(k/100.0)); 
            delay(3);
            ringshow_noglitch();
          }
      }
      if(i >= (NUM_LEDS - num_green)){
         for(int k=255; k >=50;k--) {
            ring.setPixelColor(i, 0, k, 0); 
            delay(0.5);
            ringshow_noglitch();
          }
      }     
    }
  fade = true;
  return;
  }


  //Fade Back to Full brightness
  for(int i=0;i<curr_leds;i++) {
    if(i< first_half){
      if(i< num_red){

        for(int k=20; k <= 100;k++) {
          ring.setPixelColor(i, 255*(k/100.0), g, b); 
          delay(delay_speed);
          ringshow_noglitch();
        }
      } else{
        int gpx = g+((i-num_red)*(255/(first_half - num_red)));
        for(int k=50; k <100;k++) {
          ring.setPixelColor(i, r*(k/100.0), gpx*(k/100.0), b); 
          delay(3);
          ringshow_noglitch();
        }
      }
    }
    if(i >= first_half & i < NUM_LEDS - num_green){
      int rpx = r-(j*(255/last_half));
      for(int k=50; k <100;k++) {
        ring.setPixelColor(i, rpx*(k/100.0), 255*(k/100.0), b); 
          delay(3);
          ringshow_noglitch();
       }
      j++;
//      for(int k=0; k <= 9;k++) {
//        ring.setPixelColor(i, rpx*k, g*k, b); 
//        delay(delay_speed);
//        ringshow_noglitch();
//      }

//      ring.setPixelColor(i,r-(j*(255/last_half)),255,b);
//      ringshow_noglitch();
//      delay(delay_speed);
//      j++;
   
    }
    if(i >= (NUM_LEDS - num_green)){
      for(int k=50; k <=255;k++) {
        ring.setPixelColor(i, 0, k, 0); 
        delay(0.5);
        ringshow_noglitch();
      }
    }
  }
  fade =false;
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
  server.on("/demo", update_demo);
  server.on("/timer", update_timer);
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
//      turn_on(num_leds);
      battery_pulse(num_leds, 5);
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
//      turn_on(num_leds);
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
//      turn_on(num_leds);
   battery_pulse(num_leds, 5);
      trun_off(num_leds);
      ringshow_noglitch(); 
      battery_flag = 1;
   }

   // update when the demo request comes
   if(demo_flag == 0 && curr0 == 1){
      demo();
      demo_flag = 1;
   }

   // update when the timer request comes
   if(timer_flag == 0 && curr0 == 1){
      timer(time_val);
      timer_flag = 1;
   }
}
