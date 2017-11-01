/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : usb_desc.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/31/2017
* Description        : Descriptors for HID Port Demo
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
/* USB Standard Device Descriptor */
const u8 HID_Port_DeviceDescriptor[] =
  {
    0x12,   /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,     /* bDescriptorType */
    0x00,0x02,   /* bcdUSB = 2.00 */
    0x02,   /* bDeviceClass: CDC */
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    0x40,   /* bMaxPacketSize40 */
    0x83,0x04,   /* idVendor = 0x0483  VID*/                           
    0x40,0x57,   /* idProduct = 0x5740  PID*/
    0x00,
    0x02,   /* bcdDevice = 2.00 */
    1,              /* Index of string descriptor describing manufacturer */
    2,              /* Index of string descriptor describing product */
    3,              /* Index of string descriptor describing the device's serial number */
    0x01    /* bNumConfigurations */
  };

const u8 HID_Port_ConfigDescriptor[HID_PORT_SIZ_CONFIG_DESC] =
{
    /*Configuation Descriptor*/
    0x09,   /* bLength: Configuation Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
    HID_PORT_SIZ_CONFIG_DESC,       /* wTotalLength:no of returned bytes */
    0x00,
    0x01,   /* bNumInterfaces: 2 interface */
    0x01,   /* bConfigurationValue: Configuration value */
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,   /* bmAttributes: self powered */
    0x96,   /* MaxPower 300 mA */
    /*Interface Descriptor*/
    0x09,   /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: One endpoints used */
    0x03,   /* bInterfaceClass: Communication Interface Class */
    0x00,   /* bInterfaceSubClass: Abstract Control Model */
    0x00,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */
    /*Header Functional Descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    HID_DESCRIPTOR_TYPE,   /* bDescriptorType: HID */
    0x00,   /* bDescriptorSubtype: Header Func Desc */
    0x10,   /* bcdCDC: spec release number */
    0x01,
    0x00,   /* bCountryCode: Hardware target country */
    0x01,   /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,   /* bDescriptorType */
    HID_PORT_SIZ_REPORT_DESC,/* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

    0x82,    /* bEndpointAddress: Endpoint Address (IN) */               
             // bit 3...0 : the endpoint number
             // bit 6...4 : reserved
              // bit 7     : 0(OUT), 1(IN)
    0x03,    /* bmAttributes: Interrupt endpoint */
    0x40,    /* wMaxPacketSize: 64 Bytes max */
    0x00,
    0x02,    /* bInterval: Polling Interval (2 ms) */
    /* 34 */
    	
    0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x01,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
    0x03,	/* bmAttributes: Interrupt endpoint */
    0x40,	/* wMaxPacketSize: 64 Bytes max  */
    0x00,
    0x02	///* bInterval: Polling Interval (2 ms) */
};

const u8 HID_ReportDescriptor[] = 
{ 
	0x05, 0x8c, /* USAGE_PAGE (ST Page) */ 
	0x09, 0x01, /* USAGE (Demo Kit) */ 
	0xa1, 0x01, /* COLLECTION (Application) */ 
	
	// The Input report 
	0x09,0x03, // USAGE ID - Vendor defined 
	0x15,0x00, // LOGICAL_MINIMUM (0) 
	0x26,0x00, 0xFF, // LOGICAL_MAXIMUM (255) 
	0x75,0x08, // REPORT_SIZE (8bit) 
	0x95,0x40, // REPORT_COUNT (64Byte) 
	0x81,0x02, // INPUT (Data,Var,Abs) 

	// The Output report 
	0x09,0x04, // USAGE ID - Vendor defined 
	0x15,0x00, // LOGICAL_MINIMUM (0) 
	0x26,0x00,0xFF, // LOGICAL_MAXIMUM (255) 
	0x75,0x08, // REPORT_SIZE (8bit) 
	0x95,0x40, // REPORT_COUNT (64Byte) 
	0x91,0x02, // OUTPUT (Data,Var,Abs) 

	0xc0 /* END_COLLECTION */ 
}; /* CustomHID_ReportDescriptor */ 

/* USB String Descriptors */
const u8 HID_Port_StringLangID[] =
  {
    HID_PORT_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04 /* LangID = 0x0409: U.S. English */
  };

const u8 HID_Port_StringVendor[] =
  {
    HID_PORT_SIZ_STRING_VENDOR,     /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'L', 0, 'a', 0, 'b', 0, 'c', 0, ':', 0, 'U', 0,'S', 0,'B', 0, '_', 0, 'H', 0,'I',0,'D',0
  };

const u8 HID_Port_StringProduct[] =
  {
    HID_PORT_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* Product name: "STM32 Virtual COM Port" */
    'B', 0, 'y', 0, ' ', 0, 'L', 0, 'a', 0, 'b', 0, 'c', 0
  };

 u8 HID_Port_StringSerial[] =
  {
    HID_PORT_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'D', 0, 'e', 0, 'm', 0, 'o', 0, ' ', 0, '2', 0, '0', 0, '1', 0,'7', 0
  }
  ;/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
