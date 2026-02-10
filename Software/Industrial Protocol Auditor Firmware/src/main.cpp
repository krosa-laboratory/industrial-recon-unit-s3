#include <Arduino.h>

void setup()
{

    Serial.begin(115200);
    delay(1000);
    Serial.println("Booting...");
    delay(2000);
    Serial.println("Hello World!");

}

void loop() {}

