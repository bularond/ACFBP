#include <SPI.h>
#include <RF24.h>
#include "DFPlayer_Mini_Mp3.h"
#include <SoftwareSerial.h>

 #define DEBUG_INCOME
// #define DEBUG_SPEECH

#define BUTTON 2

#define MAX_WORD_IN_SPEECH 4

RF24 radio(10, 9);
const uint32_t pipe = 123456789;

SoftwareSerial MP3(8, 7); // RX, TX

int to_speech[MAX_WORD_IN_SPEECH] = {200, 60, 6, -1};

char my_number[5] = {'2', '6', '6', 0, 0};

char data[6];


int string_to_int(String inp, int st = 0){
    int out = 0;
    for(int i = st; i < inp.length(); i++){
        out *= 10;
        out += inp[i] - '0';
    }
    return out;
}

void set_speech_from_number(){
    set_speech(string_to_int(my_number));
}

void set_speech(int in){
    #ifdef DEBUG_SPEECH
    Serial.print("Set speech ");
    Serial.println(in);
    #endif
    for(int i = 0; i < MAX_WORD_IN_SPEECH; i++)
        to_speech[i] = -1;
    int pos_to_speech = 0;
    if(in / 100){
        to_speech[pos_to_speech] = in - in % 100;
        in -= to_speech[pos_to_speech];
        pos_to_speech++;
    }
    if(in <= 19 && in){
        to_speech[pos_to_speech] = in;
        pos_to_speech++;
    }
    else if(in){
        to_speech[pos_to_speech] = in - in % 10;
        in -= to_speech[pos_to_speech];
        pos_to_speech++;
        to_speech[pos_to_speech] = in;
        pos_to_speech++;
    }
    while(pos_to_speech < MAX_WORD_IN_SPEECH){
        to_speech[pos_to_speech] = -1;
        pos_to_speech++;
    }
    #ifdef DEBUG_SPEECH
    Serial.print("Now speech ");
    for(int i = 0; i < MAX_WORD_IN_SPEECH; i++){
        Serial.print(to_speech[i]);
        Serial.print(' ');
    }
    Serial.println();
    #endif
}

void get_speech(){
    mp3_play(0);
    delay(1900);
    mp3_stop();
    delay(50);
    for (int i = 0; to_speech[i] != -1; i++){
        #ifdef DEBUG_SPEECH        
        Serial.print("Talk ");
        Serial.println(to_speech[i]);
        #endif
        mp3_play(to_speech[i]);
        delay(1300);
        mp3_stop();
        delay(40);
    }
}

void comand_handler(String inp){
    switch (inp[0])
    {
        case 'v':
            mp3_set_volume(string_to_int(inp, 1));
            break;
        case 's':
            set_speech(string_to_int(inp, 1));
            for(int i = 0; i < 5; i++)
                my_number[i] = 0;
            for(int i = 1; i < inp.length(); i++)
                my_number[i - 1] = inp[i];
            set_speech_from_number();
            break;
        case 'g':
            Serial.println("Geting speech");
            get_speech();
            break;
    }
}

bool check(){
    for (int i = 0; i < 5; i++)
        if (my_number[i] != data[i])
            return false;
    return true;
}

void getData(){
    if (radio.available())
    {
        Serial.println("INCOME");
        radio.read(&data, sizeof(data));
        #ifdef DEBUG_INCOME
        Serial.print(data[0]);
        Serial.print(data[1]);
        Serial.print(data[2]);
        Serial.print(data[3]);
        Serial.print(data[4]);
        Serial.print('\n');
        #endif
    }
    else{
        for(int i = 0; i < 5; i++)
            data[i] = 0;
    }
}

void setup(){
    Serial.begin(9600);

    pinMode(11, INPUT_PULLUP);
    
    MP3.begin(9600);
    mp3_set_serial(MP3); 
    mp3_set_volume(30);
    delay(100);

    radio.begin();
    radio.setDataRate(RF24_1MBPS);
    radio.setCRCLength(RF24_CRC_8);
    radio.setPALevel(RF24_PA_MAX);
    radio.setChannel(5);
    radio.setAutoAck(false);
    radio.powerUp();
    radio.openReadingPipe(1, pipe);
    radio.startListening();

    pinMode(BUTTON, INPUT_PULLUP);
    set_speech_from_number();
}

unsigned long last_met = 100000000000;

void loop(){
    if(Serial.available())
        comand_handler(Serial.readString());
    getData();
    if(data[5] == 1){
        int i = 0;
        for(; i < 5 && data[i]; i++)
            my_number[i] = data[i];
        for(; i < 5; i++)
            my_number[i] = 0;
        set_speech_from_number();
    }
    if (check())
        last_met = millis();
    if (digitalRead(BUTTON) == 0 && millis() - last_met <= 3000)
    {
        Serial.print("INCOMING MESAGE \n");
        get_speech();
        if(digitalRead(BUTTON) == 0){
            mp3_play(9999);
            last_met = millis();
            while (digitalRead(BUTTON) == 0 && millis() - last_met <= 3000){
                getData();
                if (check())
                    last_met = millis();
            }
            mp3_stop();
        }
    }
}
