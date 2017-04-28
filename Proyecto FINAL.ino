
//* Es para validar Password
//# Es para Resetear el Password ingresado

#define LED_PIN A0 //Motor
#define LED_PIN A1 //Motor
#define LED_PIN A3 //LED AMARILLO
#define LED_PIN A4 //LED VERDE
#define LED_PIN A5 //LED ROJO

#include <SPI.h>
#include <MFRC522.h>
#include <Password.h> //Incluimos la libreria Password
#include <Keypad.h> //Incluimos la libreria Keypad
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

Password password = Password( "1234" ); //Definimos nuestro Password

const byte ROWS = 4; // Cuatro Filas
const byte COLS = 4; // Cuatro Columnas

// Definimos el Keymap
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = { 9,8,7,6 };// Conectar los keypads ROW1, ROW2, ROW3 y ROW4 a esos Pines de Arduino.
byte colPins[COLS] = { 5,4,3,2, };// Conectar los keypads COL1, COL2, COL3 y COL4 a esos Pines de Arduino.


// Creamos el Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal_I2C lcd (0x3F, 16, 2);

void setup(){

  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  keypad.addEventListener(keypadEvent); //Adicionamos un Evento listener para este keypad

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("RFID Listo...");
  Serial.println();

  pinMode(A5, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A3, OUTPUT);

  digitalWrite(A5, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A3, LOW);

}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop(){
  keypad.getKey();
  ///////////////////////////// Bluetooth /////////////////////////////
  {
while (Serial.available())
{
char dato= Serial.read();
digitalWrite(A0,LOW);
switch(dato)
{
case 'A':
  {
    Serial.println("Seguro abierto");
    digitalWrite(A0,HIGH);
    delay (300);
    digitalWrite(A0,LOW);
    break;
  }
case 'B':
  {
    Serial.println("Seguro cerrado");
    digitalWrite(A1,HIGH);
    delay (300);
    digitalWrite(A1,LOW);
    break;
        }
      }
    }
  }

  ///////////////////////////// R F I D /////////////////////////////

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    //Show ID on serial monitor
    Serial.print("ID tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message: ");
    content.toUpperCase();
    if (content.substring(1) == "E5 C0 D0 B5") //change here the UID of the card/cards that you want to give access
    {
      Serial.println("Acceso");
      Serial.println();
      pinMode(11, OUTPUT);
      pinMode(A0, OUTPUT);
      pinMode(A1, OUTPUT);

      lcd.setCursor(0,0);
      lcd.print("RFID status");
      lcd.blink();
      delay(2000);
      lcd.noBlink();
      lcd.setCursor(0,1);
      lcd.print("Correcto");
      Serial.println("Seguro abierto");
      digitalWrite(A0,HIGH);
      delay (170);
      digitalWrite(A0,LOW);
      delay (4000);

      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print("RFID status");
      lcd.blink();
      delay(2000);
      lcd.noBlink();
      lcd.setCursor(0,1);
      lcd.print("Cerrado");
      Serial.println("Seguro cerrado");
      digitalWrite(A1,HIGH);
      delay (170);
      digitalWrite(A1,LOW);
      delay(2000);

      lcd.clear();

    }

   else   {
      Serial.println("Denegado");
      Serial.println();
      pinMode(12, OUTPUT);

      lcd.setCursor(0,0);
      lcd.print("RFID status");
      lcd.blink();
      delay(2000);
      lcd.noBlink();
      lcd.setCursor(0,1);
      lcd.print("Incorrecto");
      delay(2500);
      lcd.clear();


    }

}
  ///////////////////////////// KEYPAD /////////////////////////////

//Tenemos cuidado de algunos eventos especiales
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
    case PRESSED:
  Serial.print("Tecla presionada: ");
  Serial.println(eKey);

  lcd.setCursor(0,0);
  lcd.print("Pass: ");
  lcd.setCursor(0,1);
  lcd.print(eKey);
  switch (eKey){
    case '*':
    checkPassword();
    break;

    case '#':
    password.reset();
    break;

    default: password.append(eKey);
     }
  }
}

void checkPassword(){
  if (password.evaluate()){
    Serial.println("Codigo Correcto"); //Adicionamos nuestro Codigo a ejecutar si esto SI trabaja
    lcd.setCursor(0,0);
    lcd.print("Pass Correcto");
    lcd.blink();
    delay(2000);
    lcd.noBlink();
    lcd.setCursor(0,1);
    lcd.print("Abriendo...");

    delay(100);
    digitalWrite(A0, HIGH);
    delay(170);
    digitalWrite(A0,LOW);
    delay(4000);
    lcd.clear();
    lcd.print("Cerrando...");
    Serial.println("Cerrando...");

    delay(2000);
    digitalWrite(A1, HIGH);
    delay(170);
    digitalWrite(A1,LOW);

    delay(2000);
    digitalWrite(A4, LOW);
    lcd.clear();

        digitalWrite(A3, HIGH);
        delay(1000);
        digitalWrite(A3, LOW);
        lcd.print("Resseting...");
        lcd.blink();
        Serial.println("Resseting...");
        delay(500);
        digitalWrite(A3, HIGH);
        Serial.println("Resseting...");
        delay(1000);
        lcd.noBlink();
        digitalWrite(A3, LOW);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("RESETED");
        Serial.println("RESETED");
        delay(500);
        digitalWrite(A3, HIGH);
        Serial.println("Never happens");
        lcd.clear();
        delay(1000);
        digitalWrite(A3, LOW);
        delay(500);
        resetFunc();  //call reset

  }else{
    Serial.println("Codigo Incorrecto"); //Adicionamos nuestro Codigo a ejecutar si esto NO trabaja

        digitalWrite(A5, HIGH);
        delay(1000);
        digitalWrite(A5, LOW);
        delay(500);
        digitalWrite(A5, HIGH);
        delay(1000);
        digitalWrite(A5, LOW);
        delay(500);
        digitalWrite(A5, HIGH);
        delay(1000);
        digitalWrite(A5, LOW);
        delay(500);

        Serial.println("Resseting...");
        delay(500);
        digitalWrite(A3, HIGH);
        Serial.println("Resseting...");
        delay(1000);
        digitalWrite(A3, LOW);
        Serial.println("RESETED");
        delay(500);
        digitalWrite(A3, HIGH);
        Serial.println("Never happens");
        delay(1000);
        digitalWrite(A3, LOW);
        delay(500);
        resetFunc();  //call reset
  }
}
