/*
 * qik.h
 *
 *  Created on: Aug 28, 2014
 *      Author: DiamondS
 * Description: High power "Pololu" motor controller "Qik 2s12v10" driver.
 *              The qik 2s12v10 adds a comprehensive yet easy-to-use, high-power option to Pololu’s line of motor controllers.
 *              The compact board—it’s almost the same size as the dual VNH2SP30 carrier board [http://www.pololu.com/catalog/
 *              product/708] by itself—allows any microcontroller or computer with a serial port to drive two brushed DC motors
 *              with full direction and speed control, providing up to 13 A (continuous) per motor channel and tolerating peaks
 *              as high as 30 A. The improvements over competing products include:
 */

#ifndef QIK_H_
#define QIK_H_

#define QIK_START_BYTE              0xAA
#define QIK_DEFAULT_DEVICE_ID       0x0A

typedef enum _qik_cmd
{
    QIK_CMD_GET_FW_VERSION      = 0x01,
    QIK_CMD_GET_ERROR_BYTE      = 0x02,
    QIK_CMD_GET_CONFIG_PARAM    = 0x03,
    QIK_CMD_SET_CONFIG_PARAM    = 0x04,
    QIK_CMD_M0_BRAKE            = 0x06,
    QIK_CMD_M1_BRAKE            = 0x07,
    QIK_CMD_M0_FORWARD          = 0x08,
    QIK_CMD_M0_FORWARD_128      = 0x09,
    QIK_CMD_M0_REVERSE          = 0x0A,
    QIK_CMD_M0_REVERSE_128      = 0x0B,
    QIK_CMD_M1_FORWARD          = 0x0C,
    QIK_CMD_M1_FORWARD_128      = 0x0D,
    QIK_CMD_M1_REVERSE          = 0x0E,
    QIK_CMD_M1_REVERSE_128      = 0x0F,
    QIK_CMD_M0_GET_CURRENT      = 0x10,
    QIK_CMD_M1_GET_CURRENT      = 0x11,
    QIK_CMD_M0_GET_SPEED        = 0x10,
    QIK_CMD_M1_GET_SPEED        = 0x11,
} qik_cmd_e;

typedef enum _qik_param
{
    QIK_PRM_DEVICE_ID                   = 0x00,
    QIK_PRM_PWM                         = 0x01,
    QIK_PRM_SHUTDOWN_ERROR              = 0x02,
    QIK_PRM_SERIAL_TIMEOUT              = 0x03,
    QIK_PRM_M0_ACCELERATION             = 0x04,
    QIK_PRM_M1_ACCELERATION             = 0x05,
    QIK_PRM_M0_BRAKE_DURATION           = 0x06,
    QIK_PRM_M1_BRAKE_DURATION           = 0x07,
    QIK_PRM_M0_CURRENT_LIMIT            = 0x08,
    QIK_PRM_M1_CURRENT_LIMIT            = 0x09,
    QIK_PRM_M0_CURRENT_LIMIT_RESPONSE   = 0x0A,
    QIK_PRM_M1_CURRENT_LIMIT_RESPONSE   = 0x0B,
} qik_param_e;

typedef enum _qik_error
{
    QIK_ERROR_M0_FAULT          = 0x01,
    QIK_ERROR_M1_FAULT          = 0x02,
    QIK_ERROR_M0_OVERCURRENT    = 0x04,
    QIK_ERROR_M1_OVERCURRENT    = 0x08,
    QIK_ERROR_SERIAL_HW_FAULT   = 0x10,
    QIK_ERROR_CRC_ERROR         = 0x20,
    QIK_ERROR_FORMAT            = 0x40,
    QIK_ERROR_TIMEOUT           = 0x80,
} qik_error_e;

typedef enum _qik_pwm
{
    QIK_PWM_19_7_KHZ    = 0x00,
    QIK_PWM_9_8_KHZ     = 0x01,
    QIK_PWM_2_5_KHZ     = 0x02,
    QIK_PWM_1_2_KHZ     = 0x03,
    QIK_PWM_310_HZ      = 0x04,
    QIK_PWM_150_HZ      = 0x05,
} qik_pwm_e;

// Init
int QIK_Init(const char *device, int baudrate, unsigned char deviceID);
// Genral
unsigned char QIK_GetFirmwareVersion(void);
unsigned char QIK_GetErrorByte(void);
unsigned char QIK_GetConfigParam(qik_param_e param);
unsigned char QIK_SetConfigParam(qik_param_e param, unsigned char paramValue);
// Motor 1
void QIK_M0_Forward(unsigned char speed);
void QIK_M0_Reverse(unsigned char speed);
void QIK_M0_Brake(unsigned char brake);
unsigned int QIK_M0_GetCurrent(void);
unsigned char QIK_M0_GetSpeed(void);
// Motor 1
void QIK_M1_Forward(unsigned char speed);
void QIK_M1_Reverse(unsigned char speed);
void QIK_M1_Brake(unsigned char brake);
unsigned int QIK_M1_GetCurrent(void);
unsigned char QIK_M1_GetSpeed(void);

#endif /* QIK_H_ */
