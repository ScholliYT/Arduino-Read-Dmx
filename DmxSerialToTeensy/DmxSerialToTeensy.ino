#include <DMXSerial.h>
#include <SoftwareSerial.h>
const int RedPin = 9; // PWM output pin for Red Light.
const int GreenPin = 10; // PWM output pin for Green Light.
const int BluePin = 11; // PWM output pin for Blue Light.
#define RedDefaultLevel 255
#define GreenDefaultLevel 0
#define BlueDefaultLevel 0
SoftwareSerial serialToTeensy(4, 5); // RX, TX
void setup()
{
  DMXSerial.init(DMXReceiver);
  // enable pwm outputs
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  serialToTeensy.begin(9600);
  // Give time(sendDataDelayMs) to recieve DMX Values before sending them the first time
}

void loop()
{
  if (serialToTeensy.available() > 0)
  {
    sendData();
  }
  // Calculate how long no data backet was received
  unsigned long lastPacket = DMXSerial.noDataSince();
  if (lastPacket < 5000)
  {
    // read recent DMX values and set pwm levels
    analogWrite(RedPin, DMXSerial.read(1));
    analogWrite(GreenPin, DMXSerial.read(256));
    analogWrite(BluePin, DMXSerial.read(512));
  }
  else
  {
    // Show pure red color, when no data was received since 5 seconds or more.
    analogWrite(RedPin, RedDefaultLevel);
    analogWrite(GreenPin, GreenDefaultLevel);
    analogWrite(BluePin, BlueDefaultLevel);
  }
}

void sendData()
{
  // Need to send Data in chuncks because Serial buffer on recieving end is to small
  // Split 512 bytes of data in 16 32byte chunks
  uint8_t requestedChunk = serialToTeensy.read();
  if (requestedChunk < 0 || requestedChunk > 15)
  {
    // invalid request
    serialToTeensy.write(requestedChunk);
    uint8_t errorCode = 111;
    for (int i = 0; i < 32; ++i)
    {
      serialToTeensy.write(errorCode);
    }
    for (int a = 0; a < requestedChunk; a++)
    {
      analogWrite(BluePin, 255);
      delay(200);
      analogWrite(BluePin, 0);
      delay(200);
    }
    return;
  }
  serialToTeensy.write(requestedChunk);
  int channelOffset = requestedChunk * 32;
  for (int channel = 1 + channelOffset; channel < 1 + 32 + channelOffset; channel++)
  {
    serialToTeensy.write(DMXSerial.read(channel));
  }
}
