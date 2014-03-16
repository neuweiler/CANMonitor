/*
 * CAN Monitor
 *
 * Author: Michael Neuweiler
 * Created: 02.06.2013
 *
 * A small Arduino Due program which enables both CAN buses
 * and outputs all incoming traffic to the serial bus and
 * if defined also to an LCD shield.
 *
 * A library for the Due's CAN bus is available here:
 * https://github.com/collin80/due_can
 *
 * For a design of a dual CAN shield, check my blog
 * http://s80ev.blogspot.ch/2013/05/springtime.html
 * or http://arduino.cc/forum/index.php/topic,131096.30.html
 *
 */

#include "CANMonitor.h"

#ifdef LiquidCrystal_h
#define CFG_LCD_MONITOR_PINS 8, 9, 4, 5, 6, 7 // specify the pin sequence used for LiquidCrystal initialisation
#define CFG_LCD_MONITOR_COLUMNS 16 // specify the number of columns of the display
#define CFG_LCD_MONITOR_ROWS 2 // specify the number of rows of the display
LiquidCrystal lcd(CFG_LCD_MONITOR_PINS);
#endif

/*
 * Output a data package of a bus to the serial port and the LCD shield
 */
void processData(int busNumber, CAN_FRAME &frame) {

	Serial.print("CAN");
	Serial.print(busNumber);
	Serial.print(": dlc=0x");
	Serial.print(frame.length, HEX);
	Serial.print(" fid=0x");
	Serial.print(frame.fid, HEX);
	Serial.print(" id=0x");
	Serial.print(frame.id, HEX);
	Serial.print(" ide=0x");
	Serial.print(frame.extended, HEX);
	Serial.print(" rtr=0x");
	Serial.print(frame.rtr, HEX);
	Serial.print(" data=0x");

#ifdef LiquidCrystal_h
	lcd.setCursor(0, busNumber);
#endif

	for (int i = 0; i < 8; i++) {
#ifdef LiquidCrystal_h
		lcd.print(frame.data.bytes[i], HEX);
#endif
		Serial.print(frame.data.bytes[i], HEX);
		Serial.print(",0x");
	}
	Serial.println();
}

void setup()
{
	Serial.begin(115200);
	Serial.println("CAN Monitor");

#ifdef LiquidCrystal_h
	lcd.begin(CFG_LCD_MONITOR_COLUMNS, CFG_LCD_MONITOR_ROWS);
	lcd.print("CAN Monitor");
#endif

	// Initialize the CAN0 bus
	CAN.init(CAN_BPS_500K); // set the correct speed here
	CAN.setRXFilter(0, 0, 0, false);

	// Initialize the CAN1 bus
	CAN2.init(CAN_BPS_500K); // set the correct speed here
	CAN2.setRXFilter(0, 0, 0, false);

	Serial.println("setup complete");
}

void loop()
{
	CAN_FRAME incoming;

	// check for available data on CAN0
	if (CAN.rx_avail()) {
		CAN.get_rx_buff(incoming);
		processData(0, incoming);
	}
	// check for available data on CAN1
	if (CAN2.rx_avail()) {
		CAN2.get_rx_buff(incoming);
		processData(1, incoming);
	}
}
