#include <LCD_I2C.h>
/*
Arduino Nano
LCD 2x16 with LED back-lights
JSN-SR04T Ultrasonic Distance Sensor
*/
LCD_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules, change according

#define trigPin 2 
#define echoPin 3
#define lenght 16.0
 
double percent=100.0;
unsigned char b;
unsigned int peace;
 
// custom charaters

byte p1[8] = {
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10};
 
byte p2[8] = {
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18};
 
byte p3[8] = {
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C};
 
byte p4[8] = {
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E};
 
byte p5[8] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F};
long duration;
int distance;

void setup() { 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.createChar(0, p1);
  lcd.createChar(1, p2);
  lcd.createChar(2, p3);
  lcd.createChar(3, p4);
  lcd.createChar(4, p5);  
  Serial.begin(9600);
  lcd.begin(); // If you are using more I2C devices using the Wire library use lcd.begin(false)
                 // this stop the library(LCD_I2C) from calling Wire.begin()
  lcd.backlight();
}

void loop() {

  lcd.backlight();
  lcd.setCursor(0, 0);
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
   
  duration = pulseIn(echoPin, HIGH); 
  distance = duration*0.038/2;
  
  //Serial.print("Distance = ");
  //Serial.print(distance);
  //Serial.println(" cm");
  
  lcd.print("D:");
  lcd.print(distance);
  lcd.print("  CM");

  if (distance < 21){ distance = 21;}
  if (distance > 300){ distance = 300;}

  //unsigned int value = (distance / 300)*1024.0;
  
  Serial.println(distance);
  lcd.setCursor(0,1);
  percent = distance/300.0*100.0;
  double a=lenght/100*percent; 
 // drawing black rectangles on LCD
 
  if (a>=1) { 
    for (int i=1;i<a;i++) { 
      lcd.write(4);
      b=i;
    } 
    a=a-b; 
  } 
  peace=a*5;
 
// drawing charater's colums
 
  switch (peace) { 
  case 0: 
    break; 
  case 1:
    lcd.write(0); 
    break; 
  case 2:
    lcd.write(1);
    break; 
  case 3:
    lcd.write(2);
    break; 
  case 4:
    lcd.write(3);
    break; 
  }
  for (int i =0;i<(lenght-b);i++) {
 
    lcd.print(" ");
  }
 
  delay(1000);
  lcd.print((char)0);
  
}