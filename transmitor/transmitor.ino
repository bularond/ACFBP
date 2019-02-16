/*#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Keypad.h>


RF24 radio(A2,A3);
const uint32_t pipe = 123456789;


#define TUPE_INPUT 1

#define VIBRO_PIN A0
#define GROUND A1


const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', '#'},
  {'4', '5', '6', '#'},
  {'7', '8', '9', '#'},
  {'#', '#', '#', '#'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


char real_number[5]  = {0};
char write_number[5] = {0};
int now_pos = 0;
int tranzit[5] = {'1', '4', '6', '3', '9'};


void update_number(){
  #ifdef TUPE_INPUT == 1
    char key = keypad.getKey();

    if (key != NO_KEY) {
      if(key == '#'){
        now_pos = 0;
        for(int i = 0; i < 5; i++){
          real_number[i] = write_number[i];
          write_number[i] = 0;
        }
      }
      else{
        write_number[now_pos] = key;
        now_pos++;
        now_pos %= 5;
      }
    }
  #endif
}


void send_number(){
  //radio.write(&real_number, sizeof(real_number));
  radio.write(&tranzit, sizeof(tranzit));
}


void setup() {
  pinMode(VIBRO_PIN, OUTPUT);
  pinMode(GROUND, OUTPUT);
  digitalWrite(GROUND, 0);

  Serial.begin(9600);
  radio.begin();
  //delay(2000);
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_8); // длинна контрольной суммы 8-bit or 16-bit
  radio.setPALevel(RF24_PA_HIGH); // уровень питания усилителя RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
                                 // соответствует уровням:    -18dBm,      -12dBm,      -6dBM,           0dBm
  radio.setChannel(100);         // установка канала
  radio.setAutoAck(false);       // - автоответ.
  radio.powerUp();               // включение или пониженное потребление powerDown - powerUp
  radio.openWritingPipe(pipe);   // открыть канал на отправку
}

void loop() {
  //update_number();
  //Serial.println(real_number);
  send_number();
}//*/

#include <SPI.h>
#include <RF24.h>

RF24 radio(A2, A3); // можно использовать любые (порты 15-19 CSN CE MOSI MISO SCK)

const uint32_t pipe = 123456789; // адрес в канале

byte data[5] ;

void setup()
{
  radio.begin();                // старт 
  radio.setDataRate(RF24_1MBPS); // скорость обмена данными RF24_1MBPS или RF24_2MBPS
  radio.setCRCLength(RF24_CRC_16); // длинна контрольной суммы 8-bit or 16-bit
  radio.setPALevel(RF24_PA_MAX); // уровень питания усилителя RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
                                 // соответствует уровням:    -18dBm,      -12dBm,      -6dBM,           0dBm
  radio.setChannel(100);         // установка канала
  radio.setAutoAck(false);       // - автоответ.
  radio.powerUp();               // включение или пониженное потребление powerDown - powerUp
  radio.openWritingPipe(pipe);   // открыть канал на отправку
}


void loop()   
{
  for(byte i = 0; i < 5 ; i++) data[i] = i;
  radio.write(&data, sizeof(data));
  delay(500);
}


