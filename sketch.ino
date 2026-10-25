#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize LCD at I2C address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

const uint8_t ROWS = 4;
const uint8_t COLS = 4;

// Physical keys mapped to calculation symbols
// A = '+', B = '-', C = '*', D = '/'
// * = 'C' (Clear), # = '=' (Equals)
char keys[ROWS][COLS] = {
  { '1', '2', '3', '+' },
  { '4', '5', '6', '-' },
  { '7', '8', '9', '*' },
  { 'C', '0', '=', '/' }
};

// Original pin configuration
uint8_t rowPins[ROWS] = { 26, 22, 21, 20 };
uint8_t colPins[COLS] = { 19, 18, 17, 16 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String num1 = "";
String num2 = "";
char op = 0;
bool isResult = false;

void setup() {
  // Set explicit I2C pins for Raspberry Pi Pico
  Wire.setSDA(4); 
  Wire.setSCL(5);
  Wire.begin();
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready...");
  delay(1000);
  lcd.clear();
}

void loop() {
  char key = keypad.getKey();

  if (key != NO_KEY) {
    if (key == 'C') { // Clear everything
      num1 = ""; 
      num2 = ""; 
      op = 0; 
      isResult = false;
      lcd.clear();
    }
    else if (key == '=') { // Calculate
      if (num1 != "" && num2 != "" && op != 0) {
        float n1 = num1.toFloat();
        float n2 = num2.toFloat();
        float result = 0;

        if (op == '+') result = n1 + n2;
        else if (op == '-') result = n1 - n2;
        else if (op == '*') result = n1 * n2;
        else if (op == '/') {
          if (n2 == 0) {
            lcd.clear();
            lcd.print("Div by Zero");
            num1 = ""; num2 = ""; op = 0;
            return;
          }
          result = n1 / n2;
        }

        lcd.clear();
        lcd.print(result);
        
        // Store result for chained calculations
        num1 = String(result);
        num2 = "";
        op = 0;
        isResult = true;
      }
    }
    else if (key == '+' || key == '-' || key == '*' || key == '/') {
      if (num1 != "") {
        op = key;
        isResult = false;
        lcd.clear();
        lcd.print(num1);
        lcd.print(op);
      }
    }
    else { // Number keys
      if (isResult) {
        // Start fresh if typing a new number after equals
        num1 = String (key);
        isResult = false;
        lcd.clear();
        lcd.print(num1);
      } else {
        if (op == 0) {
          num1 += key;
          lcd.clear();
          lcd.print(num1);
        } else {
          num2 += key;
          lcd.clear();
          lcd.print(num1);
          lcd.print(op);
          lcd.setCursor(0, 1);
          lcd.print(num2);
        }
      }
    }
  }
}