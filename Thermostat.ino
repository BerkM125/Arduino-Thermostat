#include <Elegoo_GFX.h>  
#include <Elegoo_TFTLCD.h> 
#include <IRremote.h>
#include <Arduino.h>
#include <SimpleDHT.h>

#define LCD_CS A3 
#define LCD_CD A2
#define LCD_WR A1 
#define LCD_RD A0 

#define LCD_RESET A4 

#define BLACK   0x0000
#define BLUE    0x001F 
#define ORANGE  0xFF00 
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define DODGER  0x07BF
#define GREY 0xC618

int DHT11_PIN = 30;
SimpleDHT11 dht11;

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void createHeatReading (int heat) {
  int color[8], sz, i;

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor((28*4)+2, 242);
  
  color[0] = 0;
  color[1] = RED;
  color[2] = ORANGE;
  color[3] = YELLOW;
  color[4] = BLACK;
  color[5] = CYAN;
  color[6] = DODGER;
  color[7] = BLUE;
  sz = 28;
  
  if(heat < 19) i = 7;
  if(heat == 19) i = 6;
  if(heat == 20) i = 5;
  if(heat > 20 && heat <= 23) i = 4;
  if(heat > 23 && heat <= 25) i = 3;
  if(heat > 25 && heat <= 27) i = 2;
  if(heat > 27) i = 1;
  
  for(int index = 1; index <= 7; index++) 
    tft.fillRect(sz*index, 230, sz, sz, color[index]);
       
  tft.drawRect(sz, 230, sz*7, sz, WHITE);
  tft.fillRect((sz*i)+10, 240, 7, 7, WHITE);
  tft.print("GOOD");
}

void setup() {
  // put your setup code here, to run once:
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) { 
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
   
  }
  Serial.begin(9600);
  tft.begin(identifier);
}

void loop() {
  // put your main code here, to run repeatedly:
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  byte temperature = 0;
  byte humidity = 0;
  float fahrenheit = 0;
  
  if(dht11.read(DHT11_PIN, &temperature, &humidity, NULL)) {
    Serial.println("Reading error. Please wait...");
    return;
  }
    
  tft.setTextSize(3);
  tft.setTextColor(BLUE);
  tft.setCursor(0, 0);
  tft.print("  Thermostat\n\n");  
  fahrenheit = ((float)temperature * (1.8)) + 32;
  
  if(temperature < 19) {
    tft.setTextSize(5);
    tft.setTextColor(CYAN);
    tft.print(" * \n");
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.print("Time to turn it up\na notch! Your room has\nbecome a tad too cold\nfor comfort.\n"); 
    if(humidity < 30) {
      tft.print("\nIt also needs to be more humid. Increase heat.\n");
    }
  }
  else if(temperature >= 19 && temperature <= 26) {
    tft.setTextSize(5);
    tft.setTextColor(MAGENTA);
    tft.print(":) \n");
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.print("Your room is at a\nperfect temperature.");
    if(humidity > 70 || humidity < 30) {
      tft.setTextSize(1);
      tft.print("\nHowever, your humidity is a little off.\nTry increasing or decreasing a little.\n");
    }
  }

  else if(temperature == 27) {
    tft.setTextSize(5);
    tft.setTextColor(WHITE);
    tft.print(":| \n");   
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.print("It's starting to \nget a little toasty\nin your room."); 
  }
  else if(temperature > 27 || humidity > 70) {
     tft.setTextSize(5);
     tft.setTextColor(RED);
     tft.print(":( \n");
     tft.setTextSize(1);
     tft.setTextColor(WHITE);
     tft.print("Too hot or too humid!\nYour room is like\na sweltering desert!\nTurn it down a notch.");
  }

  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.setCursor(0, 160);
  tft.print("Temp. (C): ");
  tft.print((int)temperature);
  tft.print("*\n");
  tft.print("Temp. (F): ");
  tft.print(fahrenheit);
  tft.print("*\n");
  tft.print("Humidity: ");
  tft.print((int)humidity);
  tft.print("%\n");
  createHeatReading((int)temperature);
  delay(3000);
}
