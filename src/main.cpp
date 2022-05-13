#include <Arduino.h>

#define LATCH_PIN 4
#define CLOCK_PIN 7
#define DATA_PIN 8

//минимальная задержка, заметная человеческому глазу - чаще делать бессмысленно
#define DISPLAY_DELAY 16

//коды для цифр от 0 до 9 под соответствующими индексами
int number_codes[] = {~B11111100, ~B01100000, ~B11011010, ~B11110010, ~B01100110,
                      ~B10110110, ~B10111110, ~B11100000, ~B11111110, ~B11110110,
                      ~B0000000};

// коды для разрядов числа, начиная с разряда единиц и заканчивая разрядом тысяч
int digit_codes[] = {0xF8, 0xF4, 0xF2, 0xF1};

// массив выводимых цифр (а точнее индексов для массива кодов)
int digits[4];

void setup() {
    Serial.begin(9600);

    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);

    Serial.println("");
}

void separateNumber(int num) {
    if (num < 0 || num > 9999) {
        Serial.println("Inputted number must to be from 0 to 9999!");
        return;
    }

    for (int &digit: digits) {
        digit = num % 10;
        num /= 10;
    }

    bool isFirstDigit = true;
    for (int i = 3; i > 0; i--) {
        if (digits[i] == 0 && isFirstDigit) {
            digits[i] = 10;
        } else {
            isFirstDigit = false;
        }
    }
}

void getNumber() {
    String input;
    if (Serial.available()) {
        input = Serial.readString();
        int number = input.toInt();
        Serial.print("A number was inputted: ");
        Serial.println(number);
        separateNumber(number);
    }
}

long displayingMillis;
void displayNumbers() {
    if (millis() - displayingMillis > DISPLAY_DELAY) {
        for (int i = 0; i < 4; i++) {
            digitalWrite(LATCH_PIN, LOW); //put the shift register to read
            shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, number_codes[digits[i]]); //send the data
            shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digit_codes[i]); //send the data
            digitalWrite(LATCH_PIN, HIGH);
        }
    }
}

void loop() {
    getNumber();
    displayNumbers();
}