#define DMX_NUM_CHANNELS 3

enum
{
  DMX_IDLE,
  DMX_BREAK,
  DMX_START,
  DMX_RUN
};

volatile unsigned char dmx_state;

// this is the start address for the dmx frame
unsigned int dmx_start_addr = 1;

// this is the current address of the dmx frame
unsigned int dmx_addr;

// pins that will toggle based on dmx data
unsigned char dmx_pin[DMX_NUM_CHANNELS] = {A3, A4, A5};

// this is used to keep track of the channels
unsigned int chan_cnt;

// this holds the dmx data
unsigned char dmx_data[DMX_NUM_CHANNELS];

// tell us when to update the pins
volatile unsigned char update;

/**************************************************************************/
/*!
  This is the setup code
*/
/**************************************************************************/
void setup()
{
  // set update flag idle
  update = 0;
  
  // set default DMX state
  dmx_state = DMX_IDLE;
  
  // set DMX pins to ouput and idle value
  for (int i=0; i<DMX_NUM_CHANNELS; i++)
  {
    pinMode(dmx_pin[i], OUTPUT);
    digitalWrite(dmx_pin[i], LOW);
  }
  
  // initialize UART for DMX
  // this will be 250 kbps, 8 bits, no parity, 2 stop bits
  UCSR0C |= (1<<USBS0);
  Serial.begin(250000);
}

/**************************************************************************/
/*!
  This is where we translate the dmx data to the pin outputs
*/
/**************************************************************************/
void loop()
{
  if (update)
  {
    update = 0;
    
    for (int i=0; i<DMX_NUM_CHANNELS; i++)
    {
      if (dmx_data[i] != 0)
      {
        digitalWrite(dmx_pin[i], HIGH);
      }
      else
      {
        digitalWrite(dmx_pin[i], LOW); 
      }
    }
  }
}

/**************************************************************************/
/*!
  This is the interrupt service handler for the DMX
*/
/**************************************************************************/
ISR(USARTn_RX_vect) {
  unsigned char status = UCSR0A;
  unsigned char data = UDR0;
  
  switch (dmx_state)
  {
    case DMX_IDLE:
      if (status & (1<<FE0))
      {
        dmx_addr = 0;
        dmx_state = DMX_BREAK;
        update = 1;
      }
    break;
    
    case DMX_BREAK:
      if (data == 0)
      {
        dmx_state = DMX_START;
      }
      else
      {
        dmx_state = DMX_IDLE;
      }
    break;
    
    case DMX_START:
      dmx_addr++;
      if (dmx_addr == dmx_start_addr)
      {
        chan_cnt = 0;
        dmx_data[chan_cnt++] = data;
        dmx_state = DMX_RUN;
      }
    break;
    
    case DMX_RUN:
      dmx_data[chan_cnt++] = data;
      if (chan_cnt >= DMX_NUM_CHANNELS)
      {
        dmx_state = DMX_IDLE;
      }
    break;
    
    default:
      dmx_state = DMX_IDLE;
    break;
  }
}
