#include <SoftwareSerial.h>
#define DEBUG
#undef DEBUG
SoftwareSerial serialFromAruino(3, 12); // RX, TX
void setup()
{
  Serial.begin(250000); // Enable serial To PC with a 250k rate

#ifdef DEBUG
  Serial.println("Started");
#endif

  serialFromAruino.begin(9600);
}

void loop()
{
  delay(1000);
  for (uint8_t chunk = 0; chunk < 16; chunk++)
  {
    delay(10);
    // Request chunk
    #ifdef DEBUG
    Serial.print("Requesting new data for chunk: ");
    Serial.print(chunk + 1);
    Serial.println(" / 16");
    #endif
    serialFromAruino.write(chunk);
    // Wait for the Arduino send next Chunk of data
    while (serialFromAruino.available() < 33)
    {
      delayMicroseconds(1);
    }
    uint8_t chunkFromArudino = serialFromAruino.read();
    if (chunkFromArudino != chunk)
    {
      Serial.print("INVALID CHUNK: Requested: ");
      Serial.print(chunk);
      Serial.print(" got: ");
      Serial.println(chunkFromArudino);
    }
    for (int c = 1 + chunk * 32; c < 1 + 32 + chunk * 32; c++)
    {
      Serial.print("Channel ");
      Serial.print(c);
      Serial.print(": ");
      Serial.print(serialFromAruino.read(), DEC);
      Serial.print("\n");
    }
    #ifdef DEBUG
    Serial.print("Remaining bytes in buffer: ");
    Serial.println(serialFromAruino.available());
    #endif
  }
}
