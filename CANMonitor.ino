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

RX_CAN_FRAME incoming;

/*
 * Output a data package of a bus to the serial port and the LCD shield
 */
void processData(int busNumber) {

	Serial.print("CAN");
	Serial.print(busNumber);
	Serial.print(": dlc=");
	Serial.print(incoming.dlc);
	Serial.print(" fid=");
	Serial.print(incoming.fid);
	Serial.print(" id=");
	Serial.print(incoming.id);
	Serial.print(" ide=");
	Serial.print(incoming.ide);
	Serial.print(" rtr=");
	Serial.print(incoming.rtr);
	Serial.print(" data=");

#ifdef LiquidCrystal_h
	lcd.setCursor(0, busNumber);
#endif

	for (int i = 0; i < 8; i++) {
#ifdef LiquidCrystal_h
		lcd.print(incoming.data[i], HEX);
#endif
		Serial.print(incoming.data[i], HEX);
		Serial.print(",");
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
	CAN.init(SystemCoreClock, CAN_BPS_500K); // set the correct speed here
	CAN.mailbox_init(0);
	CAN.mailbox_set_mode(0, CAN_MB_RX_MODE);
	CAN.mailbox_set_accept_mask(0, 0, false);
	CAN.mailbox_set_id(0, 0, false);

	// Initialize the CAN1 bus
	CAN2.init(SystemCoreClock, CAN_BPS_125K); // set the correct speed here
	CAN2.mailbox_init(0);
	CAN2.mailbox_set_mode(0, CAN_MB_RX_MODE);
	CAN2.mailbox_set_accept_mask(0, 0, false);
	CAN2.mailbox_set_id(0, 0, false);

	Serial.println("setup complete");
}

void loop()
{
	// check for available data on CAN0
	if (CAN.mailbox_get_status(0) & CAN_MSR_MRDY) {
		CAN.mailbox_read(0, &incoming);
		processData(0);
	}
	// check for available data on CAN1
	if (CAN2.mailbox_get_status(0) & CAN_MSR_MRDY) {
		CAN2.mailbox_read(0, &incoming);
		processData(1);
	}
}
