#include "LowPower.h"
#include <Keypad.h>
#include "U8glib.h"
#include <SoftwareSerial.h>
SoftwareSerial sim(12, 13);
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NO_ACK);
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad myKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
int _timeout;
String _buffer;
char PhoneNumber[11]="";
const int speaker = 10; //buzzer to arduino pin 10
const int GasDigital = 11, GasPower=A0;
int prev_time=0, current_time=0;
void setup(){
  pinMode(GasDigital, INPUT);
  pinMode(GasPower, OUTPUT);
 // pinMode(SimRst, OUTPUT);
  digitalWrite(GasPower, HIGH);
 // digitalWrite(SimRst, LOW);
  pinMode(speaker, OUTPUT); // Set speaker - pin 10 as an output
  _buffer.reserve(50);
  sim.begin(9600);
  delay(1000);
  Serial.begin(9600);
  u8g.setColorIndex(1);             // paint pixel
  u8g.setFont(u8g_font_unifont);    // choose font
  writeEM(PhoneNumber);
  GetNumber (PhoneNumber, sizeof (PhoneNumber)- 1);
  Serial.println(PhoneNumber);
  NormalScreen();
}
  
void loop(){
  NormalScreen();
  digitalWrite(GasPower, LOW);
  delay(60000);
  BeforeReading();
  delay(2000);
  int value = digitalRead(GasDigital);
  Serial.println(value);
  digitalWrite(GasPower, HIGH);
if(value==0){
  GasAlarmScreen();
  SendMessage();
 /* digitalWrite(SimRst, HIGH);
  delay(1000);
  //Serial.println("on");
  SendMessage();
  delay(1000);
  digitalWrite(SimRst, LOW);
*/   current_time=millis(); prev_time=current_time;
  while(current_time-prev_time<90000){
  tone(speaker, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(speaker);      // Stop sound
  delay(1000);
  current_time=millis();
  }
  Serial.println("Out of while");
 }
 else {
 //Serial.println("off"); 
  SleepMode();
  for(int i=0;i<45;i++)
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
 }
}
void SendMessage()
{
  //Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(200);
  //Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + String(PhoneNumber) + "\"\r"); //Mobile phone number to send message
  delay(200);
  String SMS = "Alarmi eshte aktiv, Ka rrjedhje gazi!!!";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(200);
  _buffer = _readSerial();
}
String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}
void GetNumber(char* result, const int maxLength)
{
  char key = myKeypad.getKey();
  int i = 0;   // how far through buffer we are
  while (key != '#')
  {
    switch (key)
    {
      case NO_KEY:
        break;

      case '0' ... '9': 
        if (i < maxLength)
          {
          result [i++] = key;
          result [i] = 0;  // terminating null
          writeEM(result);
          }
        break;

      case '*':  // backspace
        if (i > 0)
          {
          i--;
          result [i] = 0;  // terminating null
          writeEM(result);
          }
        break;
     
       }
   key = myKeypad.getKey();
  }
}
void writeEM(char* value){
  Serial.println(value);
  u8g.firstPage();
  do {
    u8g.drawStr(0, 20, "Vendosni numrin");   // draw character
    u8g.drawStr(0, 32, "e emergjences!"); 
    u8g.setFont(u8g_font_fixed_v0);
    u8g.drawStr(0, 44, "* fshi dhe # konfirmo"); 
      u8g.setFont(u8g_font_unifont); 
    u8g.drawStr(45, 60, value);       // draw value
  } while (u8g.nextPage());
}
void GasAlarmScreen(){
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 20, "Gas Alarm");   // draw character
    u8g.setFont(u8g_font_fixed_v0);
    u8g.drawStr(0, 35, "Po dergohet mesazh tek: ");  
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(45, 60, PhoneNumber);   
  } while (u8g.nextPage());
}
void NormalScreen(){
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 20, "Gas Alarm");   // draw character
    u8g.setFont(u8g_font_fixed_v0);
    u8g.drawStr(0, 35, "Nuk u detektua");  
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 60, "E.N: ");   
    u8g.drawStr(35, 60, PhoneNumber);
  } while (u8g.nextPage());
}
void BeforeReading(){
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 20, "Gas Alarm");   // draw character
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 60, "Po kryhet matje!");  
    u8g.setFont(u8g_font_unifont);
  } while (u8g.nextPage());
}
void SleepMode(){
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 20, "Gas Alarm");   // draw character
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 40, "Sleep mode!");  
    u8g.setFont(u8g_font_unifont);
  } while (u8g.nextPage());
}
