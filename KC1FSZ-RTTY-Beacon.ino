// KC1FSZ
// RTTY Beacon
//
#include <AD9850.h>    //http://github.com/F4GOJ/AD9850
#include <Wire.h>

// AD9850 Pins
#define W_CLK_PIN 6
#define FQ_UD_PIN 7
#define DATA_PIN 8
#define RESET_PIN 9
// Pushbutton
#define BUTTON_PIN 10
// LED (and PA Power)
#define LED0_PIN 13

const byte shiftLetters = 0x1f;
const byte shiftFigures = 0x1b;
const int baudotLen = 32;
const char baudotLetters[] = { 0, 'T', '\r', 'O', ' ', 'H', 'N', 'M', '\n', 'L', 'R', 'G', 
  'I', 'P', 'C', 'V', 'E', 'Z', 'D', 'B', 'S', 'Y', 'F', 'X', 'A', 'W', 'J', 0, 'U', 'Q', 'K', 0 };
const char baudotFigures[] = { 0, '5', '\r', '9', ' ', 0, ',', '.', '\n', ')', '4', '&',
  '8', '0', ':', ';', '3', '\"', '$', '?', 0, '6', '!', '/', '-', '2', '\'', 0, '7', '1', '(', 0 };

const int baudDelayMs = 22; // 45.45 baud
const int shiftHz = 170;

// Convention: Mark is the upper RF frequency and space is the lower RF frequency.
// The RTT "operating frequency" generally coincides with the mark frequency.
//
const int markFreq = 2125;
const int spaceFreq = markFreq - shiftHz;

int shiftState = 0;

double rfFreq0 = 7095000.0;
double rfFreq1 = 7095500.0;
double rfFreq = 0;

void setFreq(int baseFreq) {
  double symbolFreq = rfFreq + (double)baseFreq;
  // This is where the AD9850 actually gets programmed
  DDS.setfreq(symbolFreq,0);
}

void setEnabled(bool en) {
  if (en) {
    digitalWrite(LED0_PIN,1);
    DDS.up(); 
  } else {
    digitalWrite(LED0_PIN,0);
    DDS.down();
  }
}

void txMark() {
  setFreq(markFreq);
  delay(baudDelayMs);
}

void txSpace() {
  setFreq(spaceFreq);
  delay(baudDelayMs);
}

void txBits(int bits) {
  //Serial.println(bits);
  // Start bit
  txSpace();
  if (bits & 0x10) txMark(); else txSpace();
  if (bits & 0x08) txMark(); else txSpace();
  if (bits & 0x04) txMark(); else txSpace();
  if (bits & 0x02) txMark(); else txSpace();
  if (bits & 0x01) txMark(); else txSpace();  
  // Stop bits
  txMark();
  txMark();
}

void txChar(char c) {
  // Figure out what character we are dealing with
  int letterIndex = -1;
  int figureIndex = -1;
  for (int i = 0; i < baudotLen; i++) {
    if (letterIndex == -1 && baudotLetters[i] == c) {
      letterIndex = i;
    }
    if (figureIndex == -1 && baudotFigures[i] == c) {
      figureIndex = i;
    }    
  }
  if (figureIndex == -1 && letterIndex == -1) {
    // Unrecognized character!  Skip it.
  }
  // Look for case where no shifting is requried
  else if (figureIndex == letterIndex) {
    txBits(figureIndex);
  }
  else if (letterIndex != -1) {
    // Unshift if needed
    if (shiftState) {
      txBits(shiftLetters);
      shiftState = 0;
    }
    txBits(letterIndex);
  } else if (figureIndex != -1) {
    // Shift if needed
    if (!shiftState) {
      txBits(shiftFigures);
      shiftState = 1;
    }
    txBits(figureIndex);
  }
}


void txString(const char* txt) {
  setEnabled(true);
  // Transmit a few mark characters
  setFreq(markFreq);
  delay(250);
  // Start the receiver in letter mode
  txBits(shiftLetters);
  shiftState = 0;
  // Transmit the actual data
  while (*txt != 0) {
    txChar(*txt);
    txt++;
  }
  setEnabled(false);
}

void setup() {

  Wire.begin();  
  Serial.begin(9600);
  delay(500);

  pinMode(LED0_PIN,OUTPUT);

  digitalWrite(LED0_PIN,1);
  delay(250);
  digitalWrite(LED0_PIN,0);
  delay(250);
  digitalWrite(LED0_PIN,1);
  delay(250);
  digitalWrite(LED0_PIN,0);
  delay(250);

  // Get the pins all configured
  pinMode(W_CLK_PIN,OUTPUT);
  pinMode(FQ_UD_PIN,OUTPUT);
  pinMode(DATA_PIN,OUTPUT);
  pinMode(RESET_PIN,OUTPUT);
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  // LED is initially off
  digitalWrite(LED0_PIN,0);

  // Initialze the AD9850
  DDS.begin(W_CLK_PIN,FQ_UD_PIN,DATA_PIN,RESET_PIN);
  DDS.down();
}

void loop() {
  /*
  rfFreq = rfFreq0;
  txString("CQ CQ CQ DE KC1FSZ KC1FSZ KC1FSZ CQ\r\n");
  txString("CQ CQ CQ DE KC1FSZ KC1FSZ KC1FSZ CQ\r\n");
  txString("CQ CQ CQ DE KC1FSZ KC1FSZ KC1FSZ CQ\r\n");
  */
  rfFreq = rfFreq0;
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("\r\n");
  txString("DE DE DE KC1FSZ KC1FSZ KC1FSZ\r\n");
  txString("DE DE DE KC1FSZ KC1FSZ KC1FSZ\r\n");
  txString("DE DE DE KC1FSZ KC1FSZ KC1FSZ\r\n");
  txString("\r\n");
  txString("QST QST QST\r\n");
  txString("BE ADVISED THAT THE WELLESLEY AMATEUR RADIO SOCIETY (W1TKZ) WILL BE HOLDING\r\n");
  txString("ITS MONTHLY MEETING ON 20 NOV 2018 AT 1930 EST (21 NOV 0030Z)\r\n");
  txString("AT THE WELLESLEY POLICE STATION. ALL ARE WELCOME!\r\n");
  txString("ADDRESS IS 485 WASHINGTON STREET, WELLESLEY, MA.\r\n");  
  txString("GRID SQUARE FN42IH.\r\n");  
  txString("\r\n");
  txString("UPDATES WILL BE TRANSMITTED AS THEY ARE AVAILABLE.\r\n");  
  txString("\r\n");
  
  delay(5000);
  
  rfFreq = rfFreq1;
  txString("DE DE DE KC1FSZ FC1FSZ KC1FSZ\r\n");
  txString("THIS TRANSMISSION WAS GENERATED BY AN RTTY BEACON WITH 100MW PEP OUTPUT POWER\r\n");
 
  delay(25*1000);  
}

