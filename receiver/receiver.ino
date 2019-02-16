#include <SPI.h>
#include <RF24.h>

RF24 radio(9, A10);

const uint32_t pipe = 123456789;

byte data[5];

void setup()
{
  radio.begin();
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.setPALevel(RF24_PA_MAX);

  radio.setChannel(100);
  radio.setAutoAck(false);
  radio.powerUp();
  radio.openWritingPipe(pipe);
}

void loop()   
{
  for(byte i = 0; i < 5 ; i++) data[i] = i;
  radio.write(&data, sizeof(data));
  delay(500);
}
