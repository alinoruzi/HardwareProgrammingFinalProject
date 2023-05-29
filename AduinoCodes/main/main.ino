//All rights reserved for "Ali Norouzi"

#include <avr/io.h>
#include <avr/interrupt.h>

class Packet{
 public: int sender;
 public: int reciver;
 public: int value;
};

int outLedGreen = 3;
int outLedRed = 2;
int outLedBlue = 4;
int inAnalog = A0;
int inDig0 = 12;
int inDig1 = 11;
int currentId;
bool ledState=false;
bool started = false;
Packet myPacket;

void setup() {
 Serial.begin(9600);
 pinMode(outLedGreen,OUTPUT);
 pinMode(outLedRed,OUTPUT);
 pinMode(outLedBlue,OUTPUT);
 pinMode(inAnalog,INPUT);
 pinMode(inDig0,INPUT);
 pinMode(inDig1,INPUT);
 
 currentId = digitalRead(inDig0) + (digitalRead(inDig1)*2);

TCCR1A = 0;
TCCR1B = 0; 
OCR1A = 0xF424;
TCCR1B = (1<<WGM12) | (1<<CS12); 
TIMSK1 = (1<<OCIE1A);
sei();
}

void changeLedStatus(Packet packet){
  //OCR1A = (packet.value*100/1023)*5000;
  OCR1A = (1023.0-packet.value)*(0xF000-0x1000)/1023.0+0x1000;

 if (packet.value > 512){
  //ledState = true;
 digitalWrite(outLedRed,LOW);
 digitalWrite(outLedGreen,HIGH);
 }
 else{
  //ledState = false;
 digitalWrite(outLedRed,HIGH);
 digitalWrite(outLedGreen,LOW);
 }
 started = true;
}

void loop() {
 //send data:
 myPacket.sender = currentId;
 myPacket.reciver = (currentId + 2) % 3;
 myPacket.value = analogRead(inAnalog);
if (Serial.available()>5){
  Serial.readBytes((byte*)&myPacket, sizeof(myPacket));
if (myPacket.reciver == currentId)
  changeLedStatus(myPacket);
}

 Serial.write((byte*)&myPacket, sizeof(myPacket));


 //recive data:
 if (Serial.available()>5){
    Serial.readBytes((byte*)&myPacket, sizeof(myPacket));

    if (myPacket.reciver == currentId)
        changeLedStatus(myPacket);
    else{
        Serial.write((byte*)&myPacket, sizeof(myPacket));
    }
  }
}


ISR(TIMER1_COMPA_vect) {
if (started){
  digitalWrite(outLedBlue,!digitalRead(outLedBlue));
  /*
  if (ledState){
      digitalWrite(outLedGreen, !digitalRead(outLedGreen));
      digitalWrite(outLedRed, LOW);
  }
  else{
      digitalWrite(outLedRed, !digitalRead(outLedRed));
      digitalWrite(outLedGreen, LOW);
  }
  */
}
else{
  //OCR1A = 0;
}
}
