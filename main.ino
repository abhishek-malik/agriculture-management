#include<SoftwareSerial.h>
#include<LiquidCrystal.h>

SoftwareSerial sim(6,7);//(RX,TX)
LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

/*
Probe is connected to Analog Input Pin 0.
ESP is connected to hardware serial, pins 0 & 1
SIM800 is connected to software sserial, pins 6 & 7
Motor driver is connected to pins 8(forward) & 9(reverse)
*/

boolean state = true;

void setup()
{
    Serial.begin(115200);
    sim.begin(9600);
    lcd.begin(16, 2);
    lcd.print("Moisture");
    lcd.setCursor(0,1);
    lcd.print("Detection");
    pinMode(A0,INPUT);
    pinMode(13,OUTPUT);
    pinMode(8,OUTPUT);
    pinMode(9,OUTPUT);
    Serial.println("AT+CWJAP=\"___\",\"___\"");//Replace the blank with the Access point name/SSID and the password
    delay(5000);
}

void loop()
{
  int moisture;
  moisture = analogRead(A0);
  lcd.clear();
  lcd.print("Moisture = ");
  lcd.print(moisture);
  if( moisture > 750 )
  {
    digitalWrite(13,HIGH);
    lcd.setCursor(0,1);
    lcd.print("Motor ON");
    if(state)
      motor_on();
  }
  else
  {
    digitalWrite(13,LOW);
    lcd.setCursor(0,1);
    lcd.print("Motor OFF");
    if(!state)
      motor_off();
  }
  Serial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80");
  delay(10000);
  String cip = "AT+CIPSEND=";
  delay(1000);
  String temp1 = String(moisture);
  String temp = "GET /update?key=__&field1=";// Replace the blank with API write key from your ThingSpeak account.
  temp += String(moisture);
  cip += String(temp1.length()+42);
  Serial.println(cip);
  delay(5000);
  Serial.println(temp);
  delay(20000);
}

void motor_on()
{
  digitalWrite(8,HIGH);
  digitalWrite(9,LOW);
  state = false;
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"__\"\r");//Replce the blank with the Mobile number to which the SMS is to be sent.
  delay(1000);
  sim.println("The pump has been switched on.");
  delay(100);
  sim.println((char)26);
  delay(1000);
}


void motor_off()
{
  digitalWrite(8,LOW);
  digitalWrite(9,LOW);
  state = true;
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"__\"\r");//Replce the blank with the Mobile number to which the SMS is to be sent.
  delay(1000);
  sim.println("The pump has been switched off.");
  delay(100);
  sim.println((char)26);
  delay(1000);
}
