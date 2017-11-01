/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : Descriptor Header for Virtual COM Port Device
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

//#define VIRTUAL_COM_PORT_DATA_SIZE              64
//#define VIRTUAL_COM_PORT_INT_SIZE               8

//#define VIRTUAL_COM_PORT_SIZ_DEVICE_DESC        18
//#define VIRTUAL_COM_PORT_SIZ_CONFIG_DESC        67
//#define VIRTUAL_COM_PORT_SIZ_STRING_LANGID      4
//#define VIRTUAL_COM_PORT_SIZ_STRING_VENDOR      38
//#define VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT     50
//#define VIRTUAL_COM_PORT_SIZ_STRING_SERIAL      22

#define HID_DESCRIPTOR_TYPE                     0x21
#define HID_SIZ_HID_DESC                  0x09
#define HID_OFF_HID_DESC                  0x12

#define HID_PORT_SIZ_DEVICE_DESC        18
#define HID_PORT_SIZ_CONFIG_DESC        42
#define HID_PORT_SIZ_REPORT_DESC        33 // 162
#define HID_PORT_SIZ_STRING_LANGID      4
#define HID_PORT_SIZ_STRING_VENDOR      36
#define HID_PORT_SIZ_STRING_PRODUCT     24
#define HID_PORT_SIZ_STRING_SERIAL      26


#define STANDARD_ENDPOINT_DESC_SIZE             0x09

#define HID_PORT_DATA_SIZE              64  //endpoint length

/* Exported functions ------------------------------------------------------- */
extern const u8 HID_Port_DeviceDescriptor[HID_PORT_SIZ_DEVICE_DESC];
extern const u8 HID_Port_ConfigDescriptor[HID_PORT_SIZ_CONFIG_DESC];
extern const u8 HID_Port_ReportDescriptor[HID_PORT_SIZ_REPORT_DESC];
extern const u8 HID_Port_StringLangID[HID_PORT_SIZ_STRING_LANGID];
extern const u8 HID_Port_StringVendor[HID_PORT_SIZ_STRING_VENDOR];
extern const u8 HID_Port_StringProduct[HID_PORT_SIZ_STRING_PRODUCT];
extern u8 HID_Port_StringSerial[HID_PORT_SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
