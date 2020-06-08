#include <Wire.h>
#include <LiquidCrystal_I2C.h>

void(* resetFunc) (void) = 0; //declare reset function @ address 0

//https://arduino-info.wikispaces.com/LCD-Blue-I2C#v3
//I2C: A4 (SDA) and A5 (SCL).

//buttons on pin 2...9
const int buttonNum = 8;
int buttonPins[buttonNum];

//speaker on pin 10
int speakerPin = 10;

const int maxNoteNum = 4;
const int noteDuration = 1000 / 4;
const int pauseBetweenNotes = noteDuration * 1.30;

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

int notePool[buttonNum] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5  
};

int melody[maxNoteNum];

const int introLen = 5;
int intro[]   = {NOTE_C4, NOTE_C5, NOTE_C5, NOTE_C4, NOTE_C5};

const int winLen = 8;
int win[]     = {NOTE_E4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_C5};
unsigned long usbegin;

//LCD
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
#define EMPTY_LINE "                "

void setup() {
  //debug
  
  Serial.begin(9600);
  Serial.println("Memory Game starts...Press any key to continue");

  // buttons
  for (int i = 0; i < buttonNum; i++) {
    buttonPins[i] = i + 2;
    pinMode(buttonPins[i], INPUT);
  }

  //tone
  pinMode(speakerPin, OUTPUT);

  //LCD
  lcd.begin(16, 2);
  for (int i = 0; i < 2; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }

  playTones(intro, introLen);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Memory Game");
  lcd.setCursor(0, 1);
  lcd.print("Press any key...");
  usbegin = micros();  
  waitForKey();  
  //seed random with time took to press button and noise on analog pin
  randomSeed(analogRead(0) + micros() - usbegin);

  lcd.clear();
  //init melody
  for (int i = 0; i < maxNoteNum; i++) {
    melody[i] = notePool[random(buttonNum)];
  }
}

int level = 2;
int tryNum = 0;

void loop() {

  bool gameOn = true;
  while(gameOn) {
    tryNum++;
    String str="level=";
    str+=level;
    Serial.println(str);       

    str="tryNum";
    str+=tryNum;
    Serial.println(str);

    lcd.setCursor(0,0);
    lcd.print("Len:");
    lcd.print(level);
    lcd.print(" Try:");
    lcd.print(tryNum);
    delay(500);
    
    playTones(melody, level);

    lcd.setCursor(0,1);
    lcd.print("Play back melody");
    lcd.setCursor(0,1);
  
    //wait for user to play melody
    for(int i=0;i<level;i++){
      int userKey = waitForKey();
      playNote(notePool[userKey]);
      if (notePool[userKey] != melody[i]) {
        Serial.println("Wrong note");
        lcd.print(EMPTY_LINE);
        lcd.setCursor(0,1);
        lcd.print("Wrong note!");
        //failed, back to beginning.
        gameOn = false;
        delay(500);
        break;
      }
    }

    if(!gameOn) {
      gameOn = true;
      continue;
    }
      
    level++;
    if(level>maxNoteNum){
      Serial.println("You win!!!");
      lcd.print(EMPTY_LINE);
      lcd.setCursor(0,1);
      lcd.print("You win!!!");
      delay(500);
      playTones(win, winLen);            
      delay(3000);
      resetFunc();  //call reset
    }
    lcd.print(EMPTY_LINE);
    lcd.setCursor(0,1);
    lcd.print("Level increased!");
    Serial.println("Level increased!");

    delay(500);
  }
}

int waitForKey() {
  while (true) {
    for (int i = 0; i < buttonNum; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        return i;
      }
    }
  }
}

void playNote(int note) {
  tone(speakerPin, note, noteDuration);
  delay(pauseBetweenNotes);
  noTone(speakerPin);
}

void playTones(int* tones, int len) {
  for (int thisNote = 0; thisNote < len; thisNote++) {
    playNote(tones[thisNote]);
  }
}


