/*
             LUFA Library
     Copyright (C) Dean Camera, 2021.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2021  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the KeyboardHost demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "KeyboardHost.h"
#include "inout.h"
/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */

int upper = 0;
int shiftKey = 0;

int main(void)
{
	SetupHardware();
	GlobalInterruptEnable();

	for (;;)
	{
		KeyboardHost_Task();
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	//Initialization leds
	initLeds();
	
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#endif

	/* Hardware Initialization */
	Serial_Init(9600, false);
	LEDs_Init();
	USB_Init();

	/* Create a stdio stream for the serial port for stdin and stdout */
	Serial_CreateStream(NULL);
}

/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(void)
{
	puts_P(PSTR(ESC_FG_GREEN "Device Attached.\r\n" ESC_FG_WHITE));
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(void)
{
	puts_P(PSTR(ESC_FG_GREEN "Device Unattached.\r\n" ESC_FG_WHITE));
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(void)
{
	puts_P(PSTR("Getting Config Data.\r\n"));

	uint8_t ErrorCode;

	/* Get and process the configuration descriptor data */
	if ((ErrorCode = ProcessConfigurationDescriptor()) != SuccessfulConfigRead)
	{
		if (ErrorCode == ControlError)
		  puts_P(PSTR(ESC_FG_RED "Control Error (Get Configuration).\r\n"));
		else
		  puts_P(PSTR(ESC_FG_RED "Invalid Device.\r\n"));

		printf_P(PSTR(" -- Error Code: %d\r\n" ESC_FG_WHITE), ErrorCode);

		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		return;
	}

	/* Set the device configuration to the first configuration (rarely do devices use multiple configurations) */
	if ((ErrorCode = USB_Host_SetDeviceConfiguration(1)) != HOST_SENDCONTROL_Successful)
	{
		printf_P(PSTR(ESC_FG_RED "Control Error (Set Configuration).\r\n"
		                         " -- Error Code: %d\r\n" ESC_FG_WHITE), ErrorCode);

		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		return;
	}

	/* HID class request to set the keyboard protocol to the Boot Protocol */
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = HID_REQ_SetProtocol,
			.wValue        = 0,
			.wIndex        = 0,
			.wLength       = 0,
		};

	/* Select the control pipe for the request transfer */
	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	/* Send the request, display error and wait for device detach if request fails */
	if ((ErrorCode = USB_Host_SendControlRequest(NULL)) != HOST_SENDCONTROL_Successful)
	{
		printf_P(PSTR(ESC_FG_RED "Control Error (Set Protocol).\r\n"
		                         " -- Error Code: %d\r\n" ESC_FG_WHITE), ErrorCode);

		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		USB_Host_SetDeviceConfiguration(0);
		return;
	}

	puts_P(PSTR("Keyboard Enumerated.\r\n"));
	LEDs_SetAllLEDs(LEDMASK_USB_READY);
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t ErrorCode)
{
	USB_Disable();

	printf_P(PSTR(ESC_FG_RED "Host Mode Error\r\n"
	                         " -- Error Code %d\r\n" ESC_FG_WHITE), ErrorCode);

	LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	for(;;);
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t ErrorCode,
                                            const uint8_t SubErrorCode)
{
	printf_P(PSTR(ESC_FG_RED "Dev Enum Error\r\n"
	                         " -- Error Code %d\r\n"
	                         " -- Sub Error Code %d\r\n"
	                         " -- In State %d\r\n" ESC_FG_WHITE), ErrorCode, SubErrorCode, USB_HostState);

	LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
}

/** Task to read in and processes the next report from the attached device, displaying the report
 *  contents on the board LEDs and via the serial port.
 */

char lowerKey(char key){
	if((key >= 'A') && (key <= 'Z')){
		return key + 0x20;
	}
	
	switch (key){
		case '1':
			return '&';
		case '2':
			return 'e'; // char é
		case '3':
			return '"';
		case '4':
			return 0x27; // char '
		case '5':
			return '(';
		case '6':
			return '-';
		case '7':
			return 'e'; // char è
		case '8':
			return '_';
		case '9':
			return 'c'; // char ç
		case '0':
			return 'a'; // char à
		case '?':
			return ',';
		case '.':
			return ';';
		case '/':
			return ':';
		case 0x80: // char §
			return '!';
		case '%':
			return 'u'; // char ù
		case 0x81: // char µ
			return '*';
		case 0x82: // char ¨
			return '^';
		case 0x83: // char £
			return '$';
		case 0x84: // char °
			return ')';
		case '+':
			return '=';
		default: 
			return  key;
	}
}

void processKeyUp(uint8_t KeyCode){
	clearLeds();
}

void processKeyDown(uint8_t KeyCode, uint8_t Modifier){
	
	char PressedKey = 0;
	
	if ((KeyCode >= HID_KEYBOARD_SC_A) && (KeyCode <= HID_KEYBOARD_SC_Z))
	{
		PressedKey = (KeyCode - HID_KEYBOARD_SC_A) + 'A';
	}
		else if ((KeyCode >= HID_KEYBOARD_SC_1_AND_EXCLAMATION) &
			(KeyCode  < HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS))
	{
		PressedKey = (KeyCode - HID_KEYBOARD_SC_1_AND_EXCLAMATION) + '1';
	}
	else if (KeyCode == HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS)
	{
		PressedKey = '0';
	}
	else if (KeyCode == HID_KEYBOARD_SC_SPACE)
	{
		PressedKey = ' ';
	}
	else if (KeyCode == HID_KEYBOARD_SC_ENTER)
	{
		PressedKey = '\n';
	}
	else if (KeyCode == HID_KEYBOARD_SC_TAB)
	{
		PressedKey = '\t';
	}
	else if (KeyCode == HID_KEYBOARD_SC_DELETE)
	{
		PressedKey = 0x7f;
	}
	else if (KeyCode == HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN)
	{
		PressedKey = '<';
	}
	else if (KeyCode == HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN)
	{
		PressedKey = '>';
	}
	else if (KeyCode == HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK)
	{
		PressedKey = '?';
	}
	else if (KeyCode == HID_KEYBOARD_SC_SEMICOLON_AND_COLON)
	{
		PressedKey = ':';
	}
	else if (KeyCode == HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE)
	{
		PressedKey = '"';
	}
	else if (KeyCode == HID_KEYBOARD_SC_BACKSLASH_AND_PIPE)
	{
		PressedKey = '|';
	}
	else if (KeyCode == HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE)
	{
		PressedKey = '{';
	}
	else if (KeyCode == HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE)
	{
		PressedKey = '}';
	}
	else if (KeyCode == HID_KEYBOARD_SC_EQUAL_AND_PLUS)
	{
		PressedKey = '+';
	}
	else if (KeyCode == HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE)
	{
		PressedKey = '_';
	}
	else if (KeyCode == HID_KEYBOARD_SC_BACKSPACE){
		PressedKey = 0x08;
	}
	else if (KeyCode == HID_KEYBOARD_SC_ESCAPE){
		PressedKey = 0x1b;
	}
	else if (KeyCode == HID_KEYBOARD_SC_CAPS_LOCK)
	{
		if(upper){
			upper = 0;
		}else {
			upper = 1;
		}
	}
		
	// Qwerty to Azerty
	switch (PressedKey) {
		case 'A':
			PressedKey = 'Q';
			break;
		case 'Z':
			PressedKey = 'W';
			break;
		case 'Q':
			PressedKey = 'A';
			break;
		case 'W':
			PressedKey = 'Z';
			break;
		case 'M':
			PressedKey = '?';
			break;
		case '<':
			PressedKey = '.';
			break;
		case '>':
			PressedKey = '/';
			break;
		case '?':
			PressedKey = 0x80; // char §
			break;
		case ':':
			PressedKey = 'M';
			break;
		case '"':
			PressedKey = '%';
			break;
		case '|':
			PressedKey = 0x81; // char µ
			break;
		case '{':
			PressedKey = 0x82; // char ¨
			break;
		case '}': 
			PressedKey = 0x83; // char £
			break;
		case '_':
			PressedKey = 0x84; // char °
		default:
			break;
	}
		
	// Upper Cast
	
	if((Modifier & HID_KEYBOARD_MODIFIER_LEFTSHIFT) || (Modifier & HID_KEYBOARD_MODIFIER_RIGHTSHIFT)){
		shiftKey = 1;
	}
	else{
		shiftKey = 0;
	}
	
	if (upper == 0 && !shiftKey){
		PressedKey = lowerKey(PressedKey);
	}
	else if (upper == 1 && shiftKey){
		PressedKey = lowerKey(PressedKey);
	}
	
	printLeds(PressedKey);
}

void KeyboardHost_Task(void)
{
	if (USB_HostState != HOST_STATE_Configured)
	  return;

	/* Select keyboard data pipe */
	Pipe_SelectPipe(KEYBOARD_DATA_IN_PIPE);

	/* Unfreeze keyboard data pipe */
	Pipe_Unfreeze();

	/* Check to see if a packet has been received */
	if (!(Pipe_IsINReceived()))
	{
		/* Refreeze HID data IN pipe */
		Pipe_Freeze();

		return;
	}

	/* Ensure pipe contains data before trying to read from it */
	if (Pipe_IsReadWriteAllowed())
	{
		USB_KeyboardReport_Data_t keyboardReport;
		
		/* Read in keyboard report data */
		Pipe_Read_Stream_LE(&keyboardReport, sizeof(keyboardReport), NULL);

		uint8_t KeyCode = keyboardReport.KeyCode[0];
		uint8_t Modifier = keyboardReport.Modifier;
		
		// Vérifier si la touche est pressée
		if (KeyCode){
			processKeyDown(KeyCode, Modifier);
		}else{
			processKeyUp(KeyCode);
		}
	}

	/* Clear the IN endpoint, ready for next data packet */
	Pipe_ClearIN();

	/* Refreeze keyboard data pipe */
	Pipe_Freeze();
}

