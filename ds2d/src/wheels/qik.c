/*
 * pololu2.c
 *
 *  Created on: Aug 28, 2014
 *      Author: DiamondS
 */

#include <stdio.h>

#include "qik.h"
#include "comport.h"
#include "types.h"

const unsigned char QIK_CRC_Table[256] =
{
        0x00, 0x41, 0x13, 0x52, 0x26, 0x67, 0x35, 0x74, 0x4C, 0x0D, 0x5F, 0x1E, 0x6A, 0x2B, 0x79, 0x38,
        0x09, 0x48, 0x1A, 0x5B, 0x2F, 0x6E, 0x3C, 0x7D, 0x45, 0x04, 0x56, 0x17, 0x63, 0x22, 0x70, 0x31,
        0x12, 0x53, 0x01, 0x40, 0x34, 0x75, 0x27, 0x66, 0x5E, 0x1F, 0x4D, 0x0C, 0x78, 0x39, 0x6B, 0x2A,
        0x1B, 0x5A, 0x08, 0x49, 0x3D, 0x7C, 0x2E, 0x6F, 0x57, 0x16, 0x44, 0x05, 0x71, 0x30, 0x62, 0x23,
        0x24, 0x65, 0x37, 0x76, 0x02, 0x43, 0x11, 0x50, 0x68, 0x29, 0x7B, 0x3A, 0x4E, 0x0F, 0x5D, 0x1C,
        0x2D, 0x6C, 0x3E, 0x7F, 0x0B, 0x4A, 0x18, 0x59, 0x61, 0x20, 0x72, 0x33, 0x47, 0x06, 0x54, 0x15,
        0x36, 0x77, 0x25, 0x64, 0x10, 0x51, 0x03, 0x42, 0x7A, 0x3B, 0x69, 0x28, 0x5C, 0x1D, 0x4F, 0x0E,
        0x3F, 0x7E, 0x2C, 0x6D, 0x19, 0x58, 0x0A, 0x4B, 0x73, 0x32, 0x60, 0x21, 0x55, 0x14, 0x46, 0x07,
        0x48, 0x09, 0x5B, 0x1A, 0x6E, 0x2F, 0x7D, 0x3C, 0x04, 0x45, 0x17, 0x56, 0x22, 0x63, 0x31, 0x70,
        0x41, 0x00, 0x52, 0x13, 0x67, 0x26, 0x74, 0x35, 0x0D, 0x4C, 0x1E, 0x5F, 0x2B, 0x6A, 0x38, 0x79,
        0x5A, 0x1B, 0x49, 0x08, 0x7C, 0x3D, 0x6F, 0x2E, 0x16, 0x57, 0x05, 0x44, 0x30, 0x71, 0x23, 0x62,
        0x53, 0x12, 0x40, 0x01, 0x75, 0x34, 0x66, 0x27, 0x1F, 0x5E, 0x0C, 0x4D, 0x39, 0x78, 0x2A, 0x6B,
        0x6C, 0x2D, 0x7F, 0x3E, 0x4A, 0x0B, 0x59, 0x18, 0x20, 0x61, 0x33, 0x72, 0x06, 0x47, 0x15, 0x54,
        0x65, 0x24, 0x76, 0x37, 0x43, 0x02, 0x50, 0x11, 0x29, 0x68, 0x3A, 0x7B, 0x0F, 0x4E, 0x1C, 0x5D,
        0x7E, 0x3F, 0x6D, 0x2C, 0x58, 0x19, 0x4B, 0x0A, 0x32, 0x73, 0x21, 0x60, 0x14, 0x55, 0x07, 0x46,
        0x77, 0x36, 0x64, 0x25, 0x51, 0x10, 0x42, 0x03, 0x3B, 0x7A, 0x28, 0x69, 0x1D, 0x5C, 0x0E, 0x4F,
};

comport_t QIK_ComData;
int QIK_ComState = 0;
unsigned char QIK_DeviceID = QIK_DEFAULT_DEVICE_ID;

static unsigned char QIK_GetCRC(unsigned char data[], int length);
static void QIK_SendCommand(unsigned char command);
static void QIK_SendCommandParam(unsigned char command, unsigned char param);
static unsigned char QIK_GetCommand(unsigned char command);
static unsigned char QIK_GetCommandParam(unsigned char command, unsigned char param);

int QIK_Init(const char *device, int baudrate, unsigned char deviceID)
{
    COMPORT_Init(&QIK_ComData, FALSE, device, baudrate);
    if (COMPORT_Connect(&QIK_ComData) != 1)
    {
        QIK_ComState = 0;
        return -1;
    }
    QIK_ComState = 1;
    QIK_DeviceID = deviceID;

    return 0;
}

// General
unsigned char QIK_GetFirmwareVersion(void)
{
    return QIK_GetCommand(QIK_CMD_GET_FW_VERSION);
}

unsigned char QIK_GetErrorByte(void)
{
    return QIK_GetCommand(QIK_CMD_GET_ERROR_BYTE);
}

unsigned char QIK_GetConfigParam(qik_param_e param)
{
    return QIK_GetCommandParam(QIK_CMD_GET_CONFIG_PARAM, (unsigned char)param);
}

unsigned char QIK_SetConfigParam(qik_param_e param, unsigned char paramValue)
{
    unsigned char packet[8] = { 0 };
    unsigned char response[1] = { 0xFF };

    packet[0] = QIK_START_BYTE;
    packet[1] = QIK_DeviceID;
    packet[2] = QIK_CMD_SET_CONFIG_PARAM;
    packet[3] = (unsigned char)param;
    packet[4] = paramValue;
    packet[5] = 0x55;
    packet[6] = 0x2A;
    packet[7] = QIK_GetCRC(packet, 4);

    if (QIK_ComState)
    {
        COMPORT_Send(&QIK_ComData, packet, 8);
        COMPORT_Receive(&QIK_ComData, response, 1);
    }

    return response[0];
}

// Motor 0
void QIK_M0_Forward(unsigned char speed)
{
    return QIK_SendCommandParam(QIK_CMD_M0_FORWARD, speed);
}

void QIK_M0_Reverse(unsigned char speed)
{
    return QIK_SendCommandParam(QIK_CMD_M0_REVERSE, speed);
}

void QIK_M0_Brake(unsigned char brake)
{
    return QIK_SendCommandParam(QIK_CMD_M0_BRAKE, brake);
}

unsigned int QIK_M0_GetCurrent(void)
{
    return (QIK_GetCommand(QIK_CMD_M0_GET_CURRENT) * 150);
}

unsigned char QIK_M0_GetSpeed(void)
{
    return QIK_GetCommand(QIK_CMD_M0_GET_SPEED);
}

// Motor 1
void QIK_M1_Forward(unsigned char speed)
{
    return QIK_SendCommandParam(QIK_CMD_M1_FORWARD, speed);
}

void QIK_M1_Reverse(unsigned char speed)
{
    return QIK_SendCommandParam(QIK_CMD_M1_REVERSE, speed);
}

void QIK_M1_Brake(unsigned char brake)
{
    return QIK_SendCommandParam(QIK_CMD_M1_BRAKE, brake);
}

unsigned int QIK_M1_GetCurrent(void)
{
    return (QIK_GetCommand(QIK_CMD_M1_GET_CURRENT) * 150);
}

unsigned char QIK_M1_GetSpeed(void)
{
    return QIK_GetCommand(QIK_CMD_M1_GET_SPEED);
}


static unsigned char QIK_GetCRC(unsigned char data[], int length)
{
    unsigned char i = 0;
    unsigned char crc = 0;

    for (i = 0; i < length; i++)
    {
        crc = QIK_CRC_Table[crc ^ data[i]];
    }
    return crc;
}

static void QIK_SendCommand(unsigned char command)
{
    unsigned char packet[4] = {0};

    packet[0] = QIK_START_BYTE;
    packet[1] = QIK_DeviceID;
    packet[2] = command;
    packet[3] = QIK_GetCRC(packet, 3);

    if (QIK_ComState)
    {
        COMPORT_Send(&QIK_ComData, packet, 4);
    }

    return;
}

static void QIK_SendCommandParam(unsigned char command, unsigned char param)
{
    unsigned char packet[5] = {0};

    packet[0] = QIK_START_BYTE;
    packet[1] = QIK_DeviceID;
    packet[2] = command;
    packet[3] = param;
    packet[4] = QIK_GetCRC(packet, 4);

    if (QIK_ComState)
    {
        COMPORT_Send(&QIK_ComData, packet, 5);
    }

    return;
}

static unsigned char QIK_GetCommand(unsigned char command)
{
    unsigned char response[1] = { 0 };

    QIK_SendCommand(command);

    if (QIK_ComState)
    {
        COMPORT_Receive(&QIK_ComData, response, 1);
    }

    return response[0];
}


static unsigned char QIK_GetCommandParam(unsigned char command, unsigned char param)
{
    unsigned char response[1] = { 0 };

    QIK_SendCommandParam(command, param);

    if (QIK_ComState)
    {
        COMPORT_Receive(&QIK_ComData, response, 1);
    }

    return response[0];
}
