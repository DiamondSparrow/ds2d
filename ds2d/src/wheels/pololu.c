/*
 * pololu.c
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#include <stdio.h>

#include "pololu.h"
#include "comport.h"
#include "types.h"

const unsigned char POLOLU_CRC_Table[256] =
{ 0x00, 0x41, 0x13, 0x52, 0x26, 0x67, 0x35, 0x74, 0x4C, 0x0D, 0x5F, 0x1E, 0x6A,
		0x2B, 0x79, 0x38, 0x09, 0x48, 0x1A, 0x5B, 0x2F, 0x6E, 0x3C, 0x7D, 0x45,
		0x04, 0x56, 0x17, 0x63, 0x22, 0x70, 0x31, 0x12, 0x53, 0x01, 0x40, 0x34,
		0x75, 0x27, 0x66, 0x5E, 0x1F, 0x4D, 0x0C, 0x78, 0x39, 0x6B, 0x2A, 0x1B,
		0x5A, 0x08, 0x49, 0x3D, 0x7C, 0x2E, 0x6F, 0x57, 0x16, 0x44, 0x05, 0x71,
		0x30, 0x62, 0x23, 0x24, 0x65, 0x37, 0x76, 0x02, 0x43, 0x11, 0x50, 0x68,
		0x29, 0x7B, 0x3A, 0x4E, 0x0F, 0x5D, 0x1C, 0x2D, 0x6C, 0x3E, 0x7F, 0x0B,
		0x4A, 0x18, 0x59, 0x61, 0x20, 0x72, 0x33, 0x47, 0x06, 0x54, 0x15, 0x36,
		0x77, 0x25, 0x64, 0x10, 0x51, 0x03, 0x42, 0x7A, 0x3B, 0x69, 0x28, 0x5C,
		0x1D, 0x4F, 0x0E, 0x3F, 0x7E, 0x2C, 0x6D, 0x19, 0x58, 0x0A, 0x4B, 0x73,
		0x32, 0x60, 0x21, 0x55, 0x14, 0x46, 0x07, 0x48, 0x09, 0x5B, 0x1A, 0x6E,
		0x2F, 0x7D, 0x3C, 0x04, 0x45, 0x17, 0x56, 0x22, 0x63, 0x31, 0x70, 0x41,
		0x00, 0x52, 0x13, 0x67, 0x26, 0x74, 0x35, 0x0D, 0x4C, 0x1E, 0x5F, 0x2B,
		0x6A, 0x38, 0x79, 0x5A, 0x1B, 0x49, 0x08, 0x7C, 0x3D, 0x6F, 0x2E, 0x16,
		0x57, 0x05, 0x44, 0x30, 0x71, 0x23, 0x62, 0x53, 0x12, 0x40, 0x01, 0x75,
		0x34, 0x66, 0x27, 0x1F, 0x5E, 0x0C, 0x4D, 0x39, 0x78, 0x2A, 0x6B, 0x6C,
		0x2D, 0x7F, 0x3E, 0x4A, 0x0B, 0x59, 0x18, 0x20, 0x61, 0x33, 0x72, 0x06,
		0x47, 0x15, 0x54, 0x65, 0x24, 0x76, 0x37, 0x43, 0x02, 0x50, 0x11, 0x29,
		0x68, 0x3A, 0x7B, 0x0F, 0x4E, 0x1C, 0x5D, 0x7E, 0x3F, 0x6D, 0x2C, 0x58,
		0x19, 0x4B, 0x0A, 0x32, 0x73, 0x21, 0x60, 0x14, 0x55, 0x07, 0x46, 0x77,
		0x36, 0x64, 0x25, 0x51, 0x10, 0x42, 0x03, 0x3B, 0x7A, 0x28, 0x69, 0x1D,
		0x5C, 0x0E, 0x4F
};

comport_t com;
int comState = 0;

const char POLOLU_Errors[10][32] = {
		"Safe start violation",
		"Required channel invalid",
		"Serial Error",
		"Command timeout",
		"Limit/Kill switch",
		"Low VIN",
		"High VIN",
		"Over Temperature",
		"Motor driver error"
		"ERR line high"
};

unsigned char crcGet(unsigned char data[], int length)
{
	unsigned char i, crc = 0;
	for (i = 0; i < length; i++)
	{
		crc = POLOLU_CRC_Table[crc ^ data[i]];
	}
	return crc;
}

int crcCheck(unsigned char data[], int length)
{
	if (crcGet(data, length - 1) == data[length - 1])
	{
		return 0;
	}

	return -1;
}

int POLOLU_Init(char *device, int baudrate)
{
	COMPORT_Init(&com, FALSE, device, baudrate);
	if (COMPORT_Connect(&com) != 1)
	{
		comState = 0;
		return -1;
	}
	comState = 1;

	return 0;
}

void POLOLU_ExitSafeStart(int deviceNumber)
{
	unsigned char packet[4] = { POLOLU_COMMAND_BYTE, 0x00, POLOLU_COMMAND_EXIT_SAFE_START, 0x00 };
	packet[1] = deviceNumber;
	packet[3] = crcGet(packet, 3);

	if (comState)
	{
		COMPORT_Send(&com, packet, 4);
	}

	return;
}

void POLOLU_MotorForward(int deviceNumber, int speed)
{
	unsigned char packet[] = { POLOLU_COMMAND_BYTE, 0x00, POLOLU_COMMAND_EXIT_MOTOR_FORWARD, 0x00, 0x00, 0x00 };

	packet[1] = deviceNumber;
	packet[3] = speed % 32;
	packet[4] = speed / 32;
	packet[5] = crcGet(packet, 5);

	if (comState)
	{
		COMPORT_Send(&com, packet, 6);
	}

	return;
}

void POLOLU_MotorReverse(int deviceNumber, int speed)
{
	unsigned char packet[] = { POLOLU_COMMAND_BYTE, 0x00, POLOLU_COMMAND_EXIT_MOTOR_REVERSE, 0x00, 0x00, 0x00 };

	packet[1] = deviceNumber;
	packet[3] = speed % 32;
	packet[4] = speed / 32;
	packet[5] = crcGet(packet, 5);

	if (comState)
	{
		COMPORT_Send(&com, packet, 6);
	}

	return;
}

void POLOLU_MotorBreake(int deviceNumber, int brakeAmount)
{
	unsigned char packet[] = { POLOLU_COMMAND_BYTE, 0x00, POLOLU_COMMAND_EXIT_MOTOR_BRAKE, 0x00, 0x00 };

	packet[1] = deviceNumber;
	packet[3] = brakeAmount;
	packet[4] = crcGet(packet, 4);

	if (comState)
	{
		COMPORT_Send(&com, packet, 5);
	}

	return;
}

int POLOLU_GetVariable(int deviceNumber, int variableID)
{

	unsigned char packet[] = { POLOLU_COMMAND_BYTE, 0x00, POLOLU_COMMAND_EXIT_GET_VARIABLE, 0x00, 0x00 };
	unsigned char response[2] =
	{ 0 };

	packet[1] = deviceNumber;
	packet[3] = variableID;
	packet[4] = crcGet(packet, 4);

	if (comState)
	{
		COMPORT_Send(&com, packet, 5);
		COMPORT_Receive(&com, response, 3);
	}

	if (crcCheck(response, 3) < 0)
	{
		return POLOLU_ERROR_CRC;
	}

	return ((response[1] << 8) | response[0]);
}

int POLOLU_SetMotorLimit(int deviceNumber, int limitID, int limitValue)
{
	return -1;
}

int POLOLU_GetFirmwareVersion(int deviceNumber)
{
	return -1;
}

void POLOLU_StopMotor(int deviceNumber)
{
	unsigned char packet[4] = { POLOLU_COMMAND_BYTE, 0x00, POLOLU_COMMAND_EXIT_STOP_MOTOR, 0x00 };
	packet[1] = deviceNumber;
	packet[3] = crcGet(packet, 3);

	if (comState)
	{
		COMPORT_Send(&com, packet, 4);
	}

	return;
}
