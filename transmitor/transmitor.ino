#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Keypad.h>

RF24 radio(10, 9);
const uint32_t pipe = 123456789;

#define TYPE_INPUT 1

#define VIBRO_PIN A1
#define GROUND A0
#define STANDART 100
#define DEL 50
#define CLEAR 100
#define SEND 200
#define ERR 150

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 's'},
    {'4', '5', '6', 's'},
    {'7', '8', '9', 's'},
    {'d', '0', 'c', 'r'}};
byte rowPins[ROWS] = {3, 4, 5, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A2, A3, 8, 7}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char real_number[6] = {0};
char write_number[5] = {0};
int pos = 0;

void vibro(int ml, int repit = 0)
{
    digitalWrite(VIBRO_PIN, HIGH);
    delay(ml);
    digitalWrite(GROUND, HIGH);
    delay(10);
    digitalWrite(VIBRO_PIN, LOW);
    digitalWrite(GROUND, LOW);
    for (int i = 0; i < repit; i++)
    {
        delay(70);
        digitalWrite(VIBRO_PIN, HIGH);
        delay(ml + STANDART);
        digitalWrite(GROUND, HIGH);
        delay(10);
        digitalWrite(VIBRO_PIN, LOW);
        digitalWrite(GROUND, LOW);
    }
}

void update_number()
{
    char key = keypad.getKey();
    if (key != NO_KEY)
    {
        vibro(STANDART);
        switch (key)
        {
        case 'r':
            if (pos)
            {
                pos = 0;
                for (int i = 0; i < 5; i++)
                {
                    real_number[i] = write_number[i];
                    write_number[i] = 0;
                }
                real_number[5] = 1;
                vibro(SEND, 1);
            }
            else
                vibro(ERR, 2);
            
            break;
        case 's':
            if (pos)
            {
                pos = 0;
                for (int i = 0; i < 5; i++)
                {
                    real_number[i] = write_number[i];
                    write_number[i] = 0;
                }
                real_number[5] = 0;
                vibro(SEND, 1);
            }
            else
                vibro(ERR, 2);
            break;
        case 'd':
            write_number[max(0, pos - 1)] = 0;
            pos = max(pos - 1, 0);
            vibro(DEL);
            break;
        case 'c':
            pos = 0;
            for (int i = 0; i < 5; i++)
                write_number[i] = 0;
            vibro(CLEAR);
            break;
        default:
            if (pos == 0 && key == '0')
                ;
            else if (pos != 5)
            {
                write_number[pos] = key;
                pos++;
            }
            else
                vibro(ERR, 2);
        }
    }
}

void send_number()
{
    radio.write(&real_number, sizeof(real_number));
}

void setup()
{
    pinMode(VIBRO_PIN, OUTPUT);
    pinMode(GROUND, OUTPUT);
    digitalWrite(GROUND, 0);

    Serial.begin(9600);
    radio.begin();
    delay(2000);
    radio.setDataRate(RF24_1MBPS);
    radio.setCRCLength(RF24_CRC_8);
    radio.setPALevel(RF24_PA_MAX);

    radio.setChannel(5);
    radio.setAutoAck(false);
    radio.powerUp();
    radio.openWritingPipe(pipe);
    vibro(1000);
}

void loop()
{
    update_number();
    for (int i = 0; i < 5 && write_number[i] != 0; i++){
        Serial.print(write_number[i]);
        Serial.print("\n");
    }
    send_number();
    delay(1);
}
