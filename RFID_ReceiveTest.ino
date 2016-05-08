
#include <Manchester.h>

/*

  Manchester Receiver example
  
  In this example receiver will receive array of 10 bytes per transmittion

  try different speeds using this constants, your maximum possible speed will 
  depend on various factors like transmitter type, distance, microcontroller speed, ...

  MAN_300 0
  MAN_600 1
  MAN_1200 2
  MAN_2400 3
  MAN_4800 4
  MAN_9600 5
  MAN_19200 6
  MAN_38400 7

*/

#define MAX_NR_BADGES 10
#define BADGE_LENGTH 11
#define HISTORY_SIZE 5

#define RX_PIN 4
#define LED_PIN 13
uint8_t _step = 8;
uint8_t moo = 1;
byte m_LoginBadgeHistory[HISTORY_SIZE];
byte m_LoginTimeHistory[HISTORY_SIZE];
char m_StoredBadges[MAX_NR_BADGES][BADGE_LENGTH];
byte delimiter[5];
uint8_t badgesCounter =0;
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); 
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH); 
  digitalWrite(LED_PIN, moo);
  man.setupReceive(RX_PIN, MAN_600);
  man.beginReceiveArray(5, (uint8_t*)delimiter);
}

void loop() {
  // wait for start frame
  if(_step == 8 && man.receiveComplete()){
    Serial.println("Ok");
    moo = ++moo % 2;
    digitalWrite(LED_PIN, moo);
    if(delimiter[0] == 0 && delimiter[1] == 1 && delimiter[2] == 0 && delimiter[3] == 1 && delimiter[4] == 0){
      badgesCounter = 0;
      man.beginReceiveArray(BADGE_LENGTH,(uint8_t*) m_StoredBadges[badgesCounter]);
      _step = 0;
    }else{
      man.beginReceiveArray(5, (uint8_t*)delimiter);
    }
  }

  switch(_step){
    case 0:
      if (man.receiveComplete()) {
        badgesCounter ++;
        if(badgesCounter == MAX_NR_BADGES){
          man.beginReceiveArray(HISTORY_SIZE, m_LoginBadgeHistory);
          _step = 1;
          break;        
        }
        man.beginReceiveArray(BADGE_LENGTH,(uint8_t*) m_StoredBadges[badgesCounter]);
        moo = ++moo % 2;
        digitalWrite(LED_PIN, moo);
      }
      break; 
    case 1:
      if (man.receiveComplete()) {
        man.beginReceiveArray(HISTORY_SIZE, m_LoginTimeHistory);       
        moo = ++moo % 2;
        digitalWrite(LED_PIN, moo);
        _step = 2;
      }
      break;
    case 2:
      if (man.receiveComplete()) {
        man.beginReceiveArray(5, (uint8_t*)delimiter);       
        moo = ++moo % 2;
        digitalWrite(LED_PIN, moo);
        EEPROM_PrintBadges();
        EEPROM_PrintHistory();
        _step = 8;
      }
      break;
    default:
    break;
  }
}

void EEPROM_PrintBadges(){
  Serial.println("---Badges---");
  int i,j;
  for(i = 0; i<MAX_NR_BADGES; i++){
    Serial.print("Badge ");
    Serial.print(i);
    Serial.print(" = ");
    for(j = 0; j<BADGE_LENGTH; j++){
       Serial.print(m_StoredBadges[i][j]);
       if(j<BADGE_LENGTH-2)
        Serial.print(":");
    }
    Serial.println("");
  }
}

void EEPROM_PrintHistory(){
  Serial.println("---History---");
  for(int i = 0; i<HISTORY_SIZE; i++){
    Serial.print(i+1);
    Serial.print("| ");
    Serial.print("Badge= ");
    for(int j = 0; j<BADGE_LENGTH; j++){
       Serial.print(m_StoredBadges[m_LoginBadgeHistory[i]][j]);
       if(j<BADGE_LENGTH-2)
        Serial.print(":");
    }
    Serial.print(" Time:");
    Serial.print(m_LoginTimeHistory[i]*2,DEC);
    Serial.println("min");
  }
}
