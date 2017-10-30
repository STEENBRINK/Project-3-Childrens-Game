/* 
 * Steenbrink
 * 23-10-2017
 * "Automated check-out project"
 */

//include libraries
#include <RFID.h>
#include <SPI.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h> // Using version 1.2.1

//register lcd panel
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

//define pins, these are hardwired so no point using variables
#define SS_PIN 10
#define RST_PIN 9

//define LED pins
#define GREEN 2
#define ORANGE 3
#define RED 4

//define button pins
#define START 5
#define DIF 6

//the set amount of different items available
#define COLOR_AMOUNT 3

//register the rfid reader
RFID rfid(SS_PIN,RST_PIN);

//register variables
unsigned char tags[COLOR_AMOUNT][5] = { {101, 182, 118, 8, 173}, {132, 241, 131, 91, 173}, {227, 179, 198, 250, 108} };    //the database with all the serialcodes of the tags
unsigned char sequence[10];   //the storage for the sequence
boolean failed = false;   //check for handlescan
int curdif = 1;   //current difficulty
int winstreak = 0;    //current winstreak

void setup() {
    //start the serial monitor
    Serial.begin(115200);

    //init libraries for reader
    SPI.begin();
    rfid.init();
    
    // register lcd, sixteen characters across 2 lines
    lcd.begin(16,2); 
    lcd.backlight();

    //register pins
    pinMode(GREEN, OUTPUT);
    pinMode(ORANGE, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(START, INPUT);
    pinMode(DIF, INPUT);

    // Clear the lcd
    lcd.clear();

    //setup
    setupLED(curdif, true);
    lcd.setCursor(0,0);
    lcd.print("Difficulty: " + String(curdif));
    lcd.setCursor(0,1);
    lcd.print("Press start!");
}

void loop() {
    if(digitalRead(START)){
        while(digitalRead(START)){int fill = 0;}
        Serial.println("Start");
        lcd.clear();
        startLED();
        delay(800);
        makeSequence();
        play();
        curdif = 1;
        setupLED(curdif, true);
        lcd.setCursor(0,0);
        lcd.print("Difficulty: " + String(curdif));
        lcd.setCursor(0,1);
        lcd.print("Winstreak: " + String(winstreak));
    }
    
    if(digitalRead(DIF)){
        while(digitalRead(DIF)){int fill = 0;}
        Serial.println("Increase difficulty");
        incDif();
        setupLED(curdif, true);
        lcd.setCursor(0,0);
        lcd.print("Difficulty: " + String(curdif));
        lcd.setCursor(0,1);
        lcd.print("Winstreak: " + String(winstreak));
    }
}

void makeSequence(){
    for(int i = 0; i < 10; i++){
        sequence[i] = (int) random(1,4);
        Serial.println(sequence[i]);
    }
}

void play(){
    failed = false;
    for(int i = 1; i < 11; i++){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Round: " + String(i));
        delay(1000);
        for(int j = 0; j < i; j++){
            setupLED(sequence[j], false);
            delay(1200/(curdif+1));
            setupLED(0, true);
            delay(1200/(curdif+1));
        }
        for(int j = 0; j < i; j++){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Tag it!");
            while(true){
                if(rfid.isCard()){
                    //checks if the card is readable
                    if(rfid.readCardSerial()){
                        for(int k = 0; k < COLOR_AMOUNT; k++){
                            if(memcmp(tags[k], rfid.serNum, 5) == 0){
                                if (k+1 == sequence[j]){
                                    lcd.setCursor(0,0);
                                    lcd.print("Correct :D");
                                    delay(500);
                                }else{
                                    lcd.setCursor(0,0);
                                    lcd.print("Incorrect :(");
                                    failed = true;
                                    delay(500);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                if (failed){
                    break;
                }
            }
        }
        if(failed){
            winstreak = 0;
            break;
        }
    }
    if(!failed){
        winstreak++;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("You won!");
        lcd.setCursor(0,1);
        lcd.print("Winstreak: " + String(winstreak));
    }
}

void incDif(){
    if(curdif == 3){
        curdif = 0;
    }else{
        curdif++;
    }
}

void setupLED(int value, boolean difcheck){
    if(difcheck){
        if(value == 0){
            digitalWrite(GREEN, LOW);
            digitalWrite(ORANGE, LOW);
            digitalWrite(RED, LOW);
        }
        if(value == 1){
            digitalWrite(GREEN, HIGH);
            digitalWrite(ORANGE, LOW);
            digitalWrite(RED, LOW);
        }
        if(value == 2){
            digitalWrite(GREEN, HIGH);
            digitalWrite(ORANGE, HIGH);
            digitalWrite(RED, LOW);
        }
        if(value == 3){
            digitalWrite(GREEN, HIGH);
            digitalWrite(ORANGE, HIGH);
            digitalWrite(RED, HIGH);
        }
    }
    if(!difcheck){
        if(value == 1){
            digitalWrite(GREEN, HIGH);
            digitalWrite(ORANGE, LOW);
            digitalWrite(RED, LOW);
        }
        if(value == 2){
            digitalWrite(GREEN, LOW);
            digitalWrite(ORANGE, HIGH);
            digitalWrite(RED, LOW);
        }
        if(value == 3){
            digitalWrite(GREEN, LOW);
            digitalWrite(ORANGE, LOW);
            digitalWrite(RED, HIGH);
        }
    }
}

void startLED(){
    digitalWrite(GREEN, HIGH);
    digitalWrite(ORANGE, HIGH);
    digitalWrite(RED, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("READY?");
    delay(400);
    digitalWrite(GREEN, LOW);
    digitalWrite(ORANGE, LOW);
    digitalWrite(RED, LOW);
    delay(400);
    digitalWrite(GREEN, HIGH);
    digitalWrite(ORANGE, HIGH);
    digitalWrite(RED, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("STEADY?");
    delay(400);
    digitalWrite(GREEN, LOW);
    digitalWrite(ORANGE, LOW);
    digitalWrite(RED, LOW);
    delay(400);
    digitalWrite(GREEN, HIGH);
    digitalWrite(ORANGE, HIGH);
    digitalWrite(RED, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("GO!");
    delay(800);
    digitalWrite(GREEN, LOW);
    digitalWrite(ORANGE, LOW);
    digitalWrite(RED, LOW);
}

