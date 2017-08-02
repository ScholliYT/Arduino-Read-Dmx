#define RX_STATUS_PIN 13

volatile uint8_t  DmxRxField[3]; //array of DMX vals (raw)
volatile uint16_t DmxAddress;    //start address

enum {DMX_IDLE, DMX_BREAK, DMX_STARTB, DMX_STARTADR}; //DMX states

volatile uint8_t gDmxState;

void setup() {
  Serial.begin(250000); //Enable serial reception with a 250k rate
  pinMode(RX_STATUS_PIN, OUTPUT); //RX STATUS LED pin, blinks on incoming DMX data
  gDmxState = DMX_IDLE; // initial state
  DmxAddress = 1; // The desired DMX Start Address
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
}

ISR(USART0_RX_vect) {
  digitalWrite(RX_STATUS_PIN, HIGH);
  static  uint16_t DmxCount;
  uint8_t  USARTstate = UCSR0A;   //get state before data!
  uint8_t  DmxByte   = UDR0;	    //get data
  uint8_t  DmxState  = gDmxState;	//just load once from SRAM to increase speed

  if (USARTstate & (1 << FE0))		//check for break
  {
    DmxCount =  DmxAddress;		//reset channel counter (count channels before start address)
    gDmxState = DMX_BREAK;
  }

  else if (DmxState == DMX_BREAK)
  {
    if (DmxByte == 0) gDmxState = DMX_STARTB; //normal start code detected
    else			  gDmxState = DMX_IDLE;
  }

  else if (DmxState == DMX_STARTB)
  {
    if (--DmxCount == 0)	//start address reached?
    {
      DmxCount = 1;		//set up counter for required channels
      DmxRxField[0] = DmxByte;	//get 1st DMX channel of device
      gDmxState = DMX_STARTADR;
    }
  }

  else if (DmxState == DMX_STARTADR)
  {
    DmxRxField[DmxCount++] = DmxByte;	//get channel
    if (DmxCount >= sizeof(DmxRxField)) //all ch received?
    {
      gDmxState = DMX_IDLE;	//wait for next break
    }
  }
  digitalWrite(RX_STATUS_PIN, LOW);
}



void loop() {
  analogWrite(A3, DmxRxField[0]);
  analogWrite(A4, DmxRxField[1]);
  analogWrite(A5, DmxRxField[2]);
}

