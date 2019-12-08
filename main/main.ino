#define BUTTON0 34
#define PIN13 13

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN13, INPUT);
  Serial.begin(9600);
}

byte prev0 = 1;
void loop() {
  byte curr0 = digitalRead(PIN13); 
  if(curr0 != prev0){ 
    if(curr0 == 1){
      // previous state was 0 and when it becomes 1
      // when the phone starts to charge
      Serial.println("Starts Charging");   
    }
    else{
      Serial.println("Finish charging");   
    }
   }
  prev0 = curr0;
  
}
