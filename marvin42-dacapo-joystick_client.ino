#include <Arduino.h>

#define normalize01(x, min, max) (((x) - (min)) / ((max) - (min)))                          // (x - min) / (max - min)
#define normalize11(x, min, max) (((x) - (((min) + (max)) / 2)) / (((max) - (min)) / 2))    // (x - ((min + max) / 2)) / ((max - min) / 2)
#define denormalize01(x, min, max) (((x) * ((max) - (min))) + (min))                        // (x * ((max) - min)) + (min)
#define denormalize11(x, min, max) (((x) + (((min) + (max)) / 2)) * (((max) - (min)) / 2))  // (x + ((min + max) / 2)) * ((max - min) / 2)

int xVal;
int yVal;

unsigned long delayTime = 100;
const unsigned int MAX = 1024 + 1;


void setup()
{
    Serial.begin(115200);

    randomSeed(analogRead(0));

    // Initialize the output variables as outputs
    pinMode(A0, INPUT);
    pinMode(RX, INPUT);
}

void loop()
{
    xVal = random(0, MAX);
    yVal = random(0, MAX);
    //xVal = analogRead(A0);
    //yVal = analogRead(RX);

    float x = normalize11((double)xVal, 0, MAX);
    float y = normalize11((double)yVal, 0, MAX);

    Serial.print("x="); Serial.print(x);
    Serial.print(", y="); Serial.println(y);


    delay(delayTime);
}
