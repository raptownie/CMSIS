#ifndef _USB_setup
#define _USB_setup
#include "stm32f3xx.h" 
#include <stdio.h>
#include <string.h>
#include "UserSystemInit.h"
int USBDconfigure(int clk);
int USBDcoreConfigure(void);
static int USBDcentralConfigure(void);
static int USBDperipheralReset(void);
static int USBDperipheralConfigure(int clk);


#define STM32F2XX   // potrzebne w dalszej czesci - najbardziej zblizony do F3
#endif


/*** Zewnetrzna biblioteka ***/
#ifndef _USB_DEF_H
#define _USB_DEF_H 1

#include <stdint.h>

#if defined __CC_ARM
  #define __packed  __packed
#elif defined __GNUC__
  #define __packed  __attribute__ ((__packed__))
#elif defined __ICCARM__
  #define __packed  __packed
#elif defined __TASKING__
  #define __packed
#endif

/** Definitions from the USB standards **/

/* Visible device states */

typedef uint32_t usb_visible_state_t;
#define DISCONNECTED  0x00
#define ATTACHED      0x01
#define POWERED       0x03
#define DEFAULT       0x07
#define ADDRESS       0x0f
#define CONFIGURED    0x1f
#define SUSPENDED     0x80

/* PID tokens */

typedef enum {
  PID_OUT      = 0x1,
  PID_IN       = 0x9,
  PID_SOF      = 0x5,
  PID_SETUP    = 0xD,
  PID_DATA0    = 0x3,
  PID_DATA1    = 0xB,
  PID_DATA2    = 0x7,
  PID_MDATA    = 0xF,
  PID_ACK      = 0x2,
  PID_NAK      = 0xA,
  PID_STALL    = 0xE,
  PID_NYET     = 0x6,
  PID_PRE      = 0xC,
  PID_ERR      = 0xC,
  PID_SPLIT    = 0x8,
  PID_PING     = 0x4,
  PID_RESERVED = 0x0
} usb_pid_t;

static inline usb_pid_t USBtoggleDataPid(usb_pid_t pid) {
  if (pid == PID_DATA0)
    return PID_DATA1;
  else
    return PID_DATA0;
}

/* The setup packet format */

typedef struct {
  uint8_t  bmRequestType;
  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} __packed usb_setup_packet_t;

/* The notification packet format */

typedef struct {
  uint8_t  bmRequestType;
  uint8_t  bNotification;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} __packed usb_notification_packet_t;

/* The bmRequestType field in the setup packet */

#define REQUEST_DIRECTION    0x80 /* mask to get transfer direction */
#define DEVICE_TO_HOST       0x80
#define HOST_TO_DEVICE       0x00
#define REQUEST_TYPE         0x60 /* mask to get request type */
#define STANDARD_REQUEST     0x00
#define CLASS_REQUEST        0x20
#define VENDOR_REQUEST       0x40
#define REQUEST_RECIPIENT    0x1f /* mask to get recipient */
#define DEVICE_RECIPIENT     0x00
#define INTERFACE_RECIPIENT  0x01
#define ENDPOINT_RECIPIENT   0x02

/* The bRequest field in the standard request setup packet */

typedef enum {
  GET_STATUS        = 0,
  CLEAR_FEATURE     = 1,
  /* Reserved for future use = 2, */
  SET_FEATURE       = 3,
  /* Reserved for future use = 4, */
  SET_ADDRESS       = 5,
  GET_DESCRIPTOR    = 6,
  SET_DESCRIPTOR    = 7,
  GET_CONFIGURATION = 8,
  SET_CONFIGURATION = 9,
  GET_INTERFACE     = 10,
  SET_INTERFACE     = 11,
  SYNCH_FRAME       = 12
} usb_standard_request_t;

/* The bRequest field in the audio class specific request setup */

typedef enum {
  SET_CUR  = 0x01,
  GET_CUR  = 0x81,
  SET_MIN  = 0x02,
  GET_MIN  = 0x82,
  SET_MAX  = 0x03,
  GET_MAX  = 0x83,
  SET_RES  = 0x04,
  GET_RES  = 0x84,
  SET_MEM  = 0x05,
  GET_MEM  = 0x85,
  GET_STAT = 0xff
} usb_ac_request_t;

/* CS value in the audio class specific request setup */

typedef enum {
  MUTE_CONTROL              = 0x01,
  VOLUME_CONTROL            = 0x02,
  BASS_CONTROL              = 0x03,
  MID_CONTROL               = 0x04,
  TREBLE_CONTROL            = 0x05,
  GRAPHIC_EQUALIZER_CONTROL = 0x06,
  AUTOMATIC_GAIN_CONTROL    = 0x07,
  DELAY_CONTROL             = 0x08,
  BASS_BOOST_CONTROL        = 0x09,
  LOUDNESS_CONTROL          = 0x0a
} usb_ac_feature_unit_control_selector_t;

/* The bRequest field in the CDC request setup packet */

typedef enum {
  SEND_ENCAPSULATED_COMMAND = 0x00,
  GET_ENCAPSULATED_COMMAND  = 0x01,
  SET_COMM_FEATURE          = 0x02,
  GET_COMM_FEATURE          = 0x03,
  CLEAR_COMM_FEATURE        = 0x04,
  SET_AUX_LINE_STATE        = 0x10,
  SET_HOOK_STATE            = 0x11,
  PULSE_SETUP               = 0x12,
  SEND_PULSE                = 0x13,
  SET_PULSE_TIME            = 0x14,
  RING_AUX_JACK             = 0x15,
  SET_LINE_CODING           = 0x20,
  GET_LINE_CODING           = 0x21,
  SET_CONTROL_LINE_STATE    = 0x22,
  SEND_BREAK                = 0x23,
  SET_RINGER_PARAMS         = 0x30,
  GET_RINGER_PARAMS         = 0x31,
  SET_OPERATION_PARAMS      = 0x32,
  GET_OPERATION_PARAMS      = 0x33,
  SET_LINE_PARAMS           = 0x34,
  GET_LINE_PARAMS           = 0x35,
  DIAL_DIGITS               = 0x36
  /* ... and many more ... */
} usb_cdc_request_t;

/* The bNotification field in the CDC notification packet */

typedef enum {
  NETWORK_CONNECTION  = 0x00,
  RESPONSE_AVAILABLE  = 0x01,
  AUX_JACK_HOOK_STATE = 0x08,
  RING_DETECT         = 0x09,
  SERIAL_STATE        = 0x20,
  CALL_STATE_CHANGE   = 0x28,
  LINE_STATE_CHANGE   = 0x29
} usb_cdc_pstn_subclass_notification_t;

/* The bRequest field in the HID class request setup packet */

typedef enum {
  GET_REPORT   = 1,
  GET_IDLE     = 2,
  GET_PROTOCOL = 3,
  SET_REPORT   = 9,
  SET_IDLE     = 10,
  SET_PROTOCOL = 11
} usb_hid_request_t;

typedef enum {
  HID_BOOT_PROTOCOL   = 0,
  HID_REPORT_PROTOCOL = 1
} usb_hid_protocol_t;

/* When bRequest is CLEAR_FEATURE or SET_FEATURE, the wValue field
   contains the feature selector. */

typedef enum {
  ENDPOINT_HALT        = 0,
  DEVICE_REMOTE_WAKEUP = 1,
  TEST_MODE            = 2
} usb_feature_selector_t;

/* Device, interface class, subclass, etc. */

#define AUDIO_CLASS              0x01
#define AUDIOCONTROL_SUBCLASS    0x01
#define AUDIOSTREAMING_SUBCLASS  0x02
#define MIDISTREAMING_SUBCLASS   0x03

#define COMMUNICATION_DEVICE_CLASS       0x02
#define COMMUNICATION_INTERFACE_CLASS    0x02
#define DATA_INTERFACE_CLASS             0x0A
#define ABSTRACT_CONTROL_MODEL_SUBCLASS  0x02

#define HUMAN_INTERFACE_DEVICE_CLASS  0x03
#define BOOT_INTERFACE_SUBCLASS       0x01
#define KEYBOARD_PROTOCOL             0x01
#define MOUSE_PROTOCOL                0x02

#define MASS_STORAGE_CLASS            0x08
#define SCSI_TRANSPARENT_SUBCLASS     0x06
#define BULK_ONLY_TRANSPORT_PROTOCOL  0x50

#define VENDOR_SPECIFIC  0xFF

/* When bRequest is GET_DESCRIPTOR or SET_DESCRIPTOR, the wValue field
   contains the descriptor type. The same values are used in the
   bDescriptorType field in all descriptors. */

typedef enum {
  DEVICE_DESCRIPTOR                    = 1,
  CONFIGURATION_DESCRIPTOR             = 2,
  STRING_DESCRIPTOR                    = 3,
  INTERFACE_DESCRIPTOR                 = 4,
  ENDPOINT_DESCRIPTOR                  = 5,
  DEVICE_QUALIFIER_DESCRIPTOR          = 6,
  OTHER_SPEED_CONFIGURATION_DESCRIPTOR = 7,
  INTERFACE_POWER_DESCRIPTOR           = 8,
  OTG_DESCRIPTOR                       = 9,
  DEBUG_DESCRIPTOR                     = 10,
  INTERFACE_ASSOCIATION_DESCRIPTOR     = 11,
  CS_UNDEFINED_DESCRIPTOR              = 0x20,
  CS_DEVICE_DESCRIPTOR                 = 0x21,
  CS_CONFIGURATION_DESCRIPTOR          = 0x22,
  CS_STRING_DESCRIPTOR                 = 0x23,
  CS_INTERFACE_DESCRIPTOR              = 0x24,
  CS_ENDPOINT_DESCRIPTOR               = 0x25
} usb_descriptor_type_t;

typedef enum {
  AC_HEADER_DESCRIPTOR          = 0x01,
  AC_INPUT_TERMINAL_DESCRIPTOR  = 0x02,
  AC_OUTPUT_TERMINAL_DESCRIPTOR = 0x03,
  AC_MIXER_UNIT_DESCRIPTOR      = 0x04,
  AC_SELECTOR_UNIT_DESCRIPTOR   = 0x05,
  AC_FEATURE_UNIT_DESCRIPTOR    = 0x06,
  AC_PROCESSING_UNIT_DESCRIPTOR = 0x07,
  AC_EXTENSION_UNIT_DESCRIPTOR  = 0x08
} usb_audio_control_interface_descriptor_subtype_t;

typedef enum {
  AS_GENERAL_DESCRIPTOR         = 0x01,
  AS_FORMAT_TYPE_DESCRIPTOR     = 0x02,
  AS_FORMAT_SPECIFIC_DESCRIPTOR = 0x03
} usb_audio_streaming_interface_descriptor_subtype_t;


typedef enum {
  EP_GENERAL_DESCRIPTOR = 0x01
} usb_audio_data_endpoint_descriptor_subtype_t;

typedef enum {
  CDC_HEADER_DESCRIPTOR          = 0x00,
  CDC_CALL_MANAGEMENT_DESCRIPTOR = 0x01,
  CDC_ACM_DESCRIPTOR             = 0x02,
  CDC_UNION_DESCRIPTOR           = 0x06
} usb_communication_data_class_functional_descriptor_subtype_t;

typedef enum {
  HID_MAIN_DESCRIPTOR     = 0x21,
  HID_REPORT_DESCRIPTOR   = 0x22,
  HID_PHYSICAL_DESCRIPTOR = 0x23,
} usb_hid_descriptor_type_t;

/* Descriptor formats */

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bcdUSB;
  uint8_t  bDeviceClass;
  uint8_t  bDeviceSubClass;
  uint8_t  bDeviceProtocol;
  uint8_t  bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t  iManufacturer;
  uint8_t  iProduct;
  uint8_t  iSerialNumber;
  uint8_t  bNumConfigurations;
} __packed usb_device_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bcdUSB;
  uint8_t  bDeviceClass;
  uint8_t  bDeviceSubClass;
  uint8_t  bDeviceProtocol;
  uint8_t  bMaxPacketSize0;
  uint8_t  bNumConfigurations;
  uint8_t  bReserved;
} __packed usb_device_qualifier_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t wTotalLength;
  uint8_t  bNumInterfaces;
  uint8_t  bConfigurationValue;
  uint8_t  iConfiguration;
  uint8_t  bmAttributes;
  uint8_t  bMaxPower;
} __packed usb_configuration_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bInterfaceNumber;
  uint8_t  bAlternateSetting;
  uint8_t  bNumEndpoints;
  uint8_t  bInterfaceClass;
  uint8_t  bInterfaceSubClass;
  uint8_t  bInterfaceProtocol;
  uint8_t  iInterface;
} __packed usb_interface_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bEndpointAddress;
  uint8_t  bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t  bInterval;
} __packed usb_endpoint_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bcdHID;
  uint8_t  bCountryCode;
  uint8_t  bNumDescriptors;
  uint8_t  bDescriptorType1;
  uint16_t wDescriptorLength1;
} __packed usb_hid_main_descriptor_t;

typedef struct {
  uint8_t  bFunctionLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint16_t bcdCDC;
} __packed usb_cdc_header_descriptor_t;

typedef struct {
  uint8_t bFunctionLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmCapabilities;
  uint8_t bDataInterface;
} __packed usb_cdc_call_management_descriptor_t;

typedef struct {
  uint8_t bFunctionLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmCapabilities;
} __packed usb_cdc_acm_descriptor_t;

typedef struct {
  uint8_t bFunctionLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bControlInterface;
  uint8_t bSubordinateInterface0;
} __packed usb_cdc_union_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint16_t bcdADC;
  uint16_t wTotalLength;
  uint8_t  bInCollection;
  uint8_t  baInterfaceNr;
} __packed usb_ac_header_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bTerminalID;
  uint16_t wTerminalType;
  uint8_t  bAssocTerminal;
  uint8_t  bNrChannels;
  uint16_t wChannelConfig;
  uint8_t  iChannelNames;
  uint8_t  iTerminal;
} __packed usb_ac_input_terminal_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bUnitID;
  uint8_t  bSourceID;
  uint8_t  bControlSize;
  uint16_t bmaControls0;
  uint8_t  iFeature;
} __packed usb_ac_feature_unit_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bTerminalID;
  uint16_t wTerminalType;
  uint8_t  bAssocTerminal;
  uint8_t  bSourceID;
  uint8_t  iTerminal;
} __packed usb_ac_output_terminal_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bTerminalLink;
  uint8_t  bDelay;
  uint16_t wFormatTag;
} __packed usb_as_general_descriptor_t;

typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFormatType;
  uint8_t bNrChannels;
  uint8_t bSubFrameSize;
  uint8_t bBitResolution;
  uint8_t bSamFreqType;
  uint8_t tSamFreq[3];
} __packed usb_as_format_type_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bEndpointAddress;
  uint8_t  bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t  bInterval;
  uint8_t  bRefresh;
  uint8_t  bSynchAddress;
} __packed usb_std_audio_data_endpoint_descriptor_t;

typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bmAttributes;
  uint8_t  bLockDelayUnits;
  uint16_t wLockDelay;
} __packed usb_cs_audio_data_endpoint_descriptor_t;

#define usb_string_descriptor_t(n) \
  struct {                         \
    uint8_t  bLength;              \
    uint8_t  bDescriptorType;      \
    uint16_t bString[(n)];         \
  } __packed

/* Maximal length USB string descriptor */

typedef usb_string_descriptor_t(126) usb_max_string_descriptor_t;

/* The bmAttributes field in the configuration descriptor */

#define D7_RESERVED    0x80
#define SELF_POWERED   0x40
#define REMOTE_WAKEUP  0x20

/* Device status returned in GET_STATUS request */

#define STATUS_SELF_POWERED   0x0001
#define STATUS_REMOTE_WAKEUP  0x0002

/* The endpoint address (number and direction) */

#define ENDP0   0
#define ENDP1   1
#define ENDP2   2
#define ENDP3   3
#define ENDP4   4
#define ENDP5   5
#define ENDP6   6
#define ENDP7   7
#define ENDP8   8
#define ENDP9   9
#define ENDP10  10
#define ENDP11  11
#define ENDP12  12
#define ENDP13  13
#define ENDP14  14
#define ENDP15  15
#define ENDP_DIRECTION_MASK  0x80
#define ENDP_NUMBER_MASK     0x7f
#define ENDP_IN              0x80
#define ENDP_OUT             0x00

#define USB_EP_MAX_COUNT  16

/* The bmAttributes field in the endpoint descriptor */

typedef enum {
  CONTROL_TRANSFER     = 0,
  ISOCHRONOUS_TRANSFER = 1,
  BULK_TRANSFER        = 2,
  INTERRUPT_TRANSFER   = 3
} usb_transfer_t;

#define CONTROL_TRANSFER_BM              0x00
#define ISOCHRONOUS_TRANSFER_BM          0x01
#define BULK_TRANSFER_BM                 0x02
#define INTERRUPT_TRANSFER_BM            0x03
#define ASYNCHRONOUS_ENDPOINT            0x04
#define ADAPTIVE_ENDPOINT                0x08
#define SYNCHRONOUS_ENDPOINT             0x0c
#define FEEDBACK_ENDPOINT                0x10
#define IMPLICIT_FEEDBACK_DATA_ENDPOINT  0x20

/* Endpoint wMaxPacketSize */

#define MAX_LS_CONTROL_PACKET_SIZE         8
#define MAX_FS_CONTROL_PACKET_SIZE        64
#define MAX_HS_CONTROL_PACKET_SIZE        64
#define MAX_FS_ISOCHRONOUS_PACKET_SIZE  1023
#define MAX_HS_ISOCHRONOUS_PACKET_SIZE  1024
#define MAX_FS_BULK_PACKET_SIZE           64
#define MAX_HS_BULK_PACKET_SIZE          512
#define MAX_LS_INTERRUPT_PACKET_SIZE       8
#define MAX_FS_INTERRUPT_PACKET_SIZE      64
#define MAX_HS_INTERRUPT_PACKET_SIZE    1024

/* The language selector in the string descriptor */

#define LANG_US_ENGLISH  0x0409
#define LANG_POLISH      0x0415

/* Mouse boot protocol report */

typedef struct {
  uint8_t buttons;
  int8_t  x;
  int8_t  y;
} __attribute__ ((__packed__)) hid_mouse_boot_report_t;

#define MOUSE_LEFT_BUTTON     0x01
#define MOUSE_RIGHT_BUTTON    0x02
#define MOUSE_MIDDLE_BUTTON   0x04

/* Keyboard boot protocol report */

#define KEYBOARD_LEFT_CTRL    0x01
#define KEYBOARD_LEFT_SHIFT   0x02
#define KEYBOARD_LEFT_ALT     0x04
#define KEYBOARD_LEFT_GUI     0x08
#define KEYBOARD_RIGHT_CTRL   0x10
#define KEYBOARD_RIGHT_SHIFT  0x20
#define KEYBOARD_RIGHT_ALT    0x40
#define KEYBOARD_RIGHT_GUI    0x80

#define KEYBOARD_NUM_LOCK_LED     0x01
#define KEYBOARD_CAPS_LOCK_LED    0x02
#define KEYBOARD_SCROLL_LOCK_LED  0x04
#define KEYBOARD_COMPOSE_LED      0x08
#define KEYBOARD_KANA_LED         0x10

#define KEYBOARD_MAX_PRESSED_KEYS  6
#define CAPS_LOCK_SCAN_CODE        57
#define NUM_LOCK_SCAN_CODE         83

/* RS-232 emulation data structures */

typedef struct {
  uint32_t dwDTERate;   /* bits per second */
  uint8_t  bCharFormat; /* stop bit(s), see below */
  uint8_t  bParityType; /* parity, see below */
  uint8_t  bDataBits;
} __packed usb_cdc_line_coding_t;

#define ONE_STOP_BIT            0
#define ONE_AND_HALF_STOP_BITS  1
#define TWO_STOP_BITS           2

#define NO_PARITY     0
#define ODD_PARITY    1
#define EVEN_PARITY   2
#define MARK_PARITY   3
#define SPACE_PARITY  4

typedef struct {
  usb_notification_packet_t notification;
  uint16_t                  wData;
} __packed usb_cdc_serial_state_t;

/* Mass Storage Class, Bulk-Only Transport structures and constants */

/* Mass Storage Class requests */
#define MSC_GET_MAX_LUN      0xfe
#define MSC_BULK_ONLY_RESET  0xff

#define MSC_CBWCB_LENGTH  16

/* Mass Storage Class, Bulk-Only Transport, Command Block Wrapper */
typedef struct {
  uint32_t dCBWSignature;
  uint32_t dCBWTag;
  uint32_t dCBWDataTransferLength;
  uint8_t  bmCBWFlags;   /* Only 7-th bit, bits 0-6 are reserved. */
  uint8_t  bCBWLUN;      /* Only 4 LS bits, 4 MS bits are reserved. */
  uint8_t  bCBWCBLength; /* Only 5 LS bits, 3 MS bits are reserved. */
  uint8_t  CBWCB[MSC_CBWCB_LENGTH];
} __packed msc_bot_cbw_t;

#define MSC_BOT_CBW_SIGNATURE                0x43425355
#define MSC_BOT_CBW_FLAGS_RESERVED_BITS      0x7f
#define MSC_BOT_CBW_LUN_RESERVED_BITS        0xf0
#define MSC_BOT_CBW_CB_LENGTH_RESERVED_BITS  0xe0
#define MSC_BOT_CBW_DATA_IN                  0x80

/* Mass Storage Class, Bulk-Only Transport, Command Status Wrapper */
typedef struct {
  uint32_t dCSWSignature;
  uint32_t dCSWTag;
  uint32_t dCSWDataResidue;
  uint8_t  bCSWStatus;
} __packed msc_bot_csw_t;

#define MSC_BOT_CSW_SIGNATURE       0x53425355
#define MSC_BOT_CSW_COMMAND_PASSED  0x00
#define MSC_BOT_CSW_COMMAND_FAILED  0x01
#define MSC_BOT_CSW_PHASE_ERROR     0x02
#define MSC_BOT_UNDEF_ERROR         0xFF

/** Host limits **/

/* Standard timers (milliseconds) */

#define STARTUP_TIME_MS          100
#define RESET_TIME_MS            15
#define RECOVERY_TIME_MS         10
#define DATA_STAGE_TIMEOUT_MS    5000
#define NODATA_STAGE_TIMEOUT_MS  50

/* Application specific timers (microseconds) */

#define HOST_LS_SCHEDULE_US  275
#define HOST_FS_SCHEDULE_US  30
#define HOST_HS_SCHEDULE_US  15

/* Application specific timers (milliseconds) */

#define DEVICE_RESET_TIME_MS  300

/* Others */

#define TRANS_MAX_REP_COUNT  3

/** USB library common definitions **/

/* Device speed or maximal host speed */

typedef enum {
  HIGH_SPEED     = 0,
  FULL_SPEED     = 1,
  LOW_SPEED      = 2,
  FULL_LOW_SPEED = 3,
  NO_SPEED       = 15 /* indicates an error */
} usb_speed_t;

/* Used transceiver */

typedef enum {
  USB_PHY_A,
  USB_PHY_B,
  USB_PHY_ULPI,
  USB_PHY_I2C
} usb_phy_t;

/* Return value of device request routines */

typedef enum {
  REQUEST_SUCCESS,
  REQUEST_ERROR
} usb_result_t;

/* Values reported after transaction completion */

typedef enum {
  TR_UNDEF = 0,
  TR_DONE,
  TR_NAK,
  TR_STALL,
  TR_ERROR
} usbh_transaction_result_t;

#endif


/*** USBD callback list ***/
#ifndef _USBD_CALLBACKS_H
#define _USBD_CALLBACKS_H 1

/** USB device application callbacks **/

typedef struct {
  /* Application configuration */
  int          (*Configure)(void);
  /* Interrupt callbacks */
  uint8_t      (*Reset)(usb_speed_t speed);
  void         (*SoF)(uint16_t frameNumber);
  /* Standard request callbacks */
  usb_result_t (*GetDescriptor)(uint16_t wValue, uint16_t wIndex,
                                uint8_t const **data,
                                uint16_t *length);
  usb_result_t (*SetDescriptor)(uint16_t wValue, uint16_t wIndex,
                                uint16_t wLength, uint8_t **data);
  uint8_t      (*GetConfiguration)(void);
  usb_result_t (*SetConfiguration)(uint16_t configurationValue);
  uint16_t     (*GetStatus)(void);
  usb_result_t (*GetInterface)(uint16_t interface, uint8_t *setting);
  usb_result_t (*SetInterface)(uint16_t interface, uint16_t setting);
  usb_result_t (*ClearDeviceFeature)(uint16_t featureSelector);
  usb_result_t (*SetDeviceFeature)(uint16_t featureSelector);
  /* Class request callbacks */
  usb_result_t (*ClassNoDataSetup)(usb_setup_packet_t const *setup);
  usb_result_t (*ClassInDataSetup)(usb_setup_packet_t const *setup,
                                   uint8_t const **data,
                                   uint16_t *length);
  usb_result_t (*ClassOutDataSetup)(usb_setup_packet_t const *setup,
                                    uint8_t **data);
  void         (*ClassStatusIn)(usb_setup_packet_t const *setup);
  /* Endpoint callbacks, the callback index is the endpoint address
     decreased by 1. */
  void         (*EPin[15])(void);
  void         (*EPout[15])(void);
  /* Peripheral power management callbacks */
  void         (*Suspend)(void);
  void         (*Wakeup)(void);
} usbd_callback_list_t;

/** Application initialization callback **/

usbd_callback_list_t const * USBDgetApplicationCallbacks(void);

#endif



#ifndef _USBD_CORE_H
#define _USBD_CORE_H 1

/** USB device core system API **/

int USBDcoreConfigure(void);
void USBDreset(usb_speed_t);
void USBDsuspend(void);
void USBDwakeup(void);
void USBDsof(uint16_t);
void USBDtransfer(uint8_t, usb_pid_t);

#endif

#ifndef _USB_OTG_REGS_H
#define _USB_OTG_REGS_H 1

#include <stdint.h>

#define STM32F2XX

#if defined STM32F10X_CL

  #define USB_OTG_BASE_ADDR  0x50000000

  /* Core supports 4 device endpoints and 8 host channels. */
  #define EP_MAX_COUNT     4
  #define CHNNL_MAX_COUNT  8

  #define CONST_EP_MAX_COUNT     EP_MAX_COUNT
  #define CONST_CHNNL_MAX_COUNT  CHNNL_MAX_COUNT

  #define FIFO_DWORDS_SIZE  320

  #define OTG_FS_REGS_USED  1
  #define OTG_HS_REGS_USED  0

#elif defined STM32F2XX || defined STM32F4XX

  #define USB_OTG_FS_BASE_ADDR  0x50000000
  #define USB_OTG_HS_BASE_ADDR  0x40040000

  /* FS core supports 4 device endpoints and 8 host channels. */
  #define FS_EP_MAX_COUNT     4
  #define FS_CHNNL_MAX_COUNT  8

  /* HS core supports 6 device endpoints and 12 host channels. */
  #define HS_EP_MAX_COUNT     6
  #define HS_CHNNL_MAX_COUNT  12

  #if HS_EP_MAX_COUNT > FS_EP_MAX_COUNT
    #define CONST_EP_MAX_COUNT  HS_EP_MAX_COUNT
  #else
    #define CONST_EP_MAX_COUNT  FS_EP_MAX_COUNT
  #endif

  #if HS_CHNNL_MAX_COUNT > FS_CHNNL_MAX_COUNT
    #define CONST_CHNNL_MAX_COUNT  HS_CHNNL_MAX_COUNT
  #else
    #define CONST_CHNNL_MAX_COUNT  FS_CHNNL_MAX_COUNT
  #endif

  #define FIFO_DWORDS_SIZE  1024

  extern uint32_t USB_OTG_BASE_ADDR;
  extern unsigned EP_MAX_COUNT;
  extern unsigned CHNNL_MAX_COUNT;

  #define USE_OTG_FS_REGS()                   \
    USB_OTG_BASE_ADDR = USB_OTG_FS_BASE_ADDR, \
    EP_MAX_COUNT      = FS_EP_MAX_COUNT,      \
    CHNNL_MAX_COUNT   = FS_CHNNL_MAX_COUNT

  #define USE_OTG_HS_REGS()                   \
    USB_OTG_BASE_ADDR = USB_OTG_HS_BASE_ADDR, \
    EP_MAX_COUNT      = HS_EP_MAX_COUNT,      \
    CHNNL_MAX_COUNT   = HS_CHNNL_MAX_COUNT

  #define OTG_FS_REGS_USED (USB_OTG_BASE_ADDR == USB_OTG_FS_BASE_ADDR)
  #define OTG_HS_REGS_USED (USB_OTG_BASE_ADDR == USB_OTG_HS_BASE_ADDR)

#endif

#if defined STM32F10X_CL || defined STM32F2XX || defined STM32F4XX

#define USB_MAX_HOST_PACKET_COUNT  1023

#define USB_OTG_CORE_GLOBAL_REG_OFFSET   0x0000
#define USB_OTG_HOST_GLOBAL_REG_OFFSET   0x0400
#define USB_OTG_HOST_CHANNEL_REG_OFFSET  0x0500
#define USB_OTG_DEV_GLOBAL_REG_OFFSET    0x0800
#define USB_OTG_DEV_IN_EP_REG_OFFSET     0x0900
#define USB_OTG_DEV_OUT_EP_REG_OFFSET    0x0B00
#define USB_OTG_PCGCCTL_OFFSET           0x0E00
#define USB_OTG_DATA_FIFO_OFFSET         0x1000
#define USB_OTG_DATA_FIFO_SIZE           0x1000

/*********************************************************************
* The USB_OTG_GREGS structure defines the relative field offsets for
* the core global control and status registers.
*********************************************************************/
typedef struct {
  volatile uint32_t GOTGCTL;       /* 000 control and status */
  volatile uint32_t GOTGINT;       /* 004 interrupt */
  volatile uint32_t GAHBCFG;       /* 008 AHB configuration */
  volatile uint32_t GUSBCFG;       /* 00C USB configuration */
  volatile uint32_t GRSTCTL;       /* 010 reset */
  volatile uint32_t GINTSTS;       /* 014 core interrupt */
  volatile uint32_t GINTMSK;       /* 018 interrupt mask */
  volatile uint32_t GRXSTSR;       /* 01C receive status debug read */
  volatile uint32_t GRXSTSP;       /* 020 receive status pop */
  volatile uint32_t GRXFSIZ;       /* 024 receive FIFO size */
  volatile uint32_t HNPTXFSIZ;     /* 028 host non-periodic transmit FIFO size */
  volatile uint32_t HNPTXSTS;      /* 02C host non-periodic transmit FIFO status */
  volatile uint32_t GI2CCTL;       /* 030 I2C access */
  volatile uint32_t reserved34;    /* 034 reserved */
  volatile uint32_t GCCFG;         /* 038 general core configuration */
  volatile uint32_t CID;           /* 03C core ID */
  volatile uint32_t reserved4[48]; /* 040 reserved */
  volatile uint32_t HPTXFSIZ;      /* 100 host periodic transmit FIFO size */
  volatile uint32_t DIEPTXFx[11];  /* 104 IN EP transmit FIFO size */
} USB_OTG_GREGS;

/* 028 device endpoint 0 transmit FIFO size */
#define DIEPTXF0  HNPTXFSIZ

#define P_USB_OTG_GREGS \
  ((USB_OTG_GREGS *)(USB_OTG_BASE_ADDR + USB_OTG_CORE_GLOBAL_REG_OFFSET))

typedef union { /* offset 0x0008 */
  uint32_t d32;
  struct {
    uint32_t gintmsk      :  1; /* gint - do not change */
    uint32_t hbstlen      :  4;
    uint32_t dmaen        :  1;
    uint32_t reserved6    :  1;
    uint32_t txfelvl      :  1;
    uint32_t ptxfelvl     :  1;
    uint32_t reserved9_31 : 23;
  } b;
} USB_OTG_GAHBCFG_TypeDef;

typedef union { /* offset 0x000C */
  uint32_t d32;
  struct {
    uint32_t tocal         :  3;
    uint32_t phyif         :  1; /* undocumented in RM0008 and RM0033 */
    uint32_t ulpisel       :  1; /* undocumented in RM0008 and RM0033 */
    uint32_t fsintf        :  1; /* undocumented in RM0008 and RM0033 */
    uint32_t physel        :  1; /* undocumented in RM0008 and RM0033 */
    uint32_t ddrsel        :  1; /* undocumented in RM0008 and RM0033 */
    uint32_t srpcap        :  1;
    uint32_t hnpcap        :  1;
    uint32_t trdt          :  4; /* badly documented in RM0008 and RM0033 */
    uint32_t nptxfrwnden   :  1; /* undocumented in RM0008 and RM0033 */
    uint32_t phylpcs       :  1;
    uint32_t otgutmifssel  :  1; /* undocumented in RM0008 and RM0033 */
    uint32_t ulpifsls      :  1;
    uint32_t ulpiar        :  1;
    uint32_t ulpicsm       :  1;
    uint32_t ulpievbusd    :  1;
    uint32_t ulpievbusi    :  1;
    uint32_t tsdps         :  1;
    uint32_t pcci          :  1;
    uint32_t ptci          :  1;
    uint32_t ulpiipd       :  1;
    uint32_t reserved26_28 :  3;
    uint32_t fhmod         :  1;
    uint32_t fdmod         :  1;
    uint32_t ctxpkt        :  1;
  } b;
} USB_OTG_GUSBCFG_TypeDef;

typedef union { /* offset 0x0010 */
  uint32_t d32;
  struct {
    uint32_t csrst         :  1;
    uint32_t hsrst         :  1;
    uint32_t fcrst         :  1;
    uint32_t reserved3     :  1;
    uint32_t rxfflsh       :  1;
    uint32_t txfflsh       :  1;
    uint32_t txfnum        :  5;
    uint32_t reserved11_29 : 19;
    uint32_t dmareq        :  1;
    uint32_t ahbidl        :  1;
  } b;
} USB_OTG_GRSTCTL_TypeDef;

typedef union { /* offset 0x0014 */
  uint32_t d32;
  struct {
    uint32_t cmod          :  1;
    uint32_t mmis          :  1;
    uint32_t otgint        :  1;
    uint32_t sof           :  1;
    uint32_t rxflvl        :  1;
    uint32_t nptxfe        :  1;
    uint32_t ginakeff      :  1;
    uint32_t gonakeff      :  1; /* boutnakeff - do not change */
    uint32_t reserved8_9   :  2;
    uint32_t esusp         :  1;
    uint32_t usbsusp       :  1;
    uint32_t usbrst        :  1;
    uint32_t enumdne       :  1;
    uint32_t isoodrp       :  1;
    uint32_t eopf          :  1;
    uint32_t reserved16_17 :  2;
    uint32_t iepint        :  1;
    uint32_t oepint        :  1;
    uint32_t iisoixfr      :  1;
    uint32_t iisooxfr      :  1; /* ipxfr_incompisoout - do not change */
    uint32_t datafsusp     :  1;
    uint32_t reserved23    :  1;
    uint32_t hprtint       :  1;
    uint32_t hcint         :  1;
    uint32_t ptxfe         :  1;
    uint32_t reserved27    :  1;
    uint32_t cidschg       :  1;
    uint32_t discint       :  1;
    uint32_t srqint        :  1;
    uint32_t wkuint        :  1;
  } b;
} USB_OTG_GINTSTS_TypeDef;

typedef union { /* offset 0x0018 */
  uint32_t d32;
  struct {
    uint32_t reserved0   :  1;
    uint32_t mmism       :  1;
    uint32_t otgm        :  1; /* otgint - do not change */
    uint32_t sofm        :  1;
    uint32_t rxflvlm     :  1;
    uint32_t nptxfem     :  1;
    uint32_t ginakeffm   :  1;
    uint32_t gonakeffm   :  1;
    uint32_t reserved8_9 :  2;
    uint32_t esuspm      :  1;
    uint32_t usbsuspm    :  1;
    uint32_t usbrstm     :  1; /* usbrst - do not change */
    uint32_t enumdnem    :  1;
    uint32_t isoodrpm    :  1;
    uint32_t eopfm       :  1;
    uint32_t reserved16  :  1;
    uint32_t epmism      :  1;
    uint32_t iepim       :  1; /* iepint - do not change */
    uint32_t oepim       :  1; /* oepint - do not change */
    uint32_t iisoixfrm   :  1;
    uint32_t iisooxfrm   :  1; /* ipxfrm_iisooxfrm - do not change */
    uint32_t fsuspm      :  1;
    uint32_t reserved23  :  1;
    uint32_t hprtim      :  1; /* prtim - do not change */
    uint32_t hcim        :  1;
    uint32_t ptxfem      :  1;
    uint32_t reserved27  :  1;
    uint32_t cidschgm    :  1;
    uint32_t discim      :  1; /* discint - do not change */
    uint32_t srqim       :  1;
    uint32_t wkuim       :  1; /* wuim - do not change */
  } b;
} USB_OTG_GINTMSK_TypeDef;

typedef union { /* offset 0x0020 */
  uint32_t d32;
  struct {
    uint32_t ch_ep_num     :  4;
    uint32_t bcnt          : 11;
    uint32_t dpid          :  2;
    uint32_t pktsts        :  4;
    uint32_t frmnum        :  4;
    uint32_t reserved25_31 :  7;
  } b;
} USB_OTG_GRXSTS_TypeDef;

#define GRXSTS_DPID_DATA0  0
#define GRXSTS_DPID_DATA2  1
#define GRXSTS_DPID_DATA1  2
#define GRXSTS_DPID_MDATA  3

#define GRXSTS_PKTSTS_IN_RECEIVED        2
#define GRXSTS_PKTSTS_IN_COMPLETED       3
#define GRXSTS_PKTSTS_DATA_TOGGLE_ERROR  5
#define GRXSTS_PKTSTS_CHANNEL_HALTED     7

#define GRXSTS_PKTSTS_GLOBAL_OUT_NAK   1
#define GRXSTS_PKTSTS_OUT_RECEIVED     2
#define GRXSTS_PKTSTS_OUT_COMPLETED    3
#define GRXSTS_PKTSTS_SETUP_COMPLETED  4
#define GRXSTS_PKTSTS_SETUP_RECEIVED   6

typedef union { /* offset 0x002C */
  uint32_t d32;
  struct {
    uint32_t nptxfsav           : 16;
    uint32_t nptxqsav           :  8; /* nptqxsav - do not change */
    uint32_t nptxqtop_terminate :  1;
    uint32_t nptxqtop_timer     :  2;
    uint32_t nptxqtop_chnum     :  4;
    uint32_t reserved31         :  1;
  } b;
} USB_OTG_HNPTXSTS_TypeDef;

/* DIEPTXF0 offset 0x0028
   DIEPTXFx offset 0x0104 + 4 * (fifo_num - 1) */
typedef union {
  uint32_t d32;
  struct {
    uint32_t startaddr : 16;
    uint32_t depth     : 16;
  } b;
} USB_OTG_FIFOSIZE_TypeDef;

typedef union { /* offset 0x0030h */
  uint32_t d32;
  struct {
    uint32_t rwdata     :  8;
    uint32_t regaddr    :  8;
    uint32_t addr       :  7;
    uint32_t i2cen      :  1;
    uint32_t ack        :  1;
    uint32_t reserved25 :  1;
    uint32_t i2cdevaddr :  2; /* i2cdevadr - do not change */
    uint32_t i2cdatse0  :  1;
    uint32_t reserved29 :  1;
    uint32_t rw         :  1;
    uint32_t bsyndne    :  1;
  } b;
} USB_OTG_GI2CCTL_TypeDef;

typedef union { /* offset 0x0038h */
  uint32_t d32;
  struct {
    uint32_t reserved0_15  : 16;
    uint32_t pwrdwn        :  1;
    uint32_t i2cpaden      :  1;
    uint32_t vbusasen      :  1;
    uint32_t vbusbsen      :  1;
    uint32_t sofouten      :  1;
    uint32_t novbussens    :  1;
    uint32_t reserved22_31 : 10;
  } b;
} USB_OTG_GCCFG_TypeDef;

/*********************************************************************
* The USB_OTG_HOST structure defines the relative field offsets for
* the host-mode control and status global registers.
* These registers are visible only in the host mode and must not be
* accessed in the device mode, as the results are unknown.
*********************************************************************/
typedef struct {
  volatile uint32_t HCFG;          /* 400 configuration */
  volatile uint32_t HFIR;          /* 404 frame interval */
  volatile uint32_t HFNUM;         /* 408 frame number / frame time remaining */
  volatile uint32_t reserved0C;    /* 40C reserved */
  volatile uint32_t HPTXSTS;       /* 410 periodic transmit FIFO status */
  volatile uint32_t HAINT;         /* 414 all channels interrupt */
  volatile uint32_t HAINTMSK;      /* 418 all channels interrupt mask */
  volatile uint32_t reserved1C[9]; /* 41C reserved */
  volatile uint32_t HPRT;          /* 440 port control and status */
} USB_OTG_HREGS;

#define P_USB_OTG_HREGS \
  ((USB_OTG_HREGS *)(USB_OTG_BASE_ADDR + USB_OTG_HOST_GLOBAL_REG_OFFSET))

typedef union { /* offset 0x0400 */
  uint32_t d32;
  struct {
    uint32_t fslspcs      :  2; /* the same coding as usb_speed_t */
    uint32_t fslss        :  1;
    uint32_t reserved3_31 : 29;
  } b;
} USB_OTG_HCFG_TypeDef;

/*
#define HCFG_FSLSPCS_30_60_MHZ  0
#define HCFG_FSLSPCS_48_MHZ     1
#define HCFG_FSLSPCS_6_MHZ      2
*/

typedef union { /* offset 0x0404 */
  uint32_t d32;
  struct {
    uint32_t frivl         : 16;
    uint32_t reserved16_31 : 16;
  } b;
} USB_OTG_HFIR_TypeDef;

typedef union { /* offset 0x0408 */
  uint32_t d32;
  struct {
    uint32_t frnum : 16;
    uint32_t ftrem : 16;
  } b;
} USB_OTG_HFNUM_TypeDef;

typedef union { /* offset 0x0410 */
  uint32_t d32;
  struct {
    uint32_t ptxfsav           : 16;
    uint32_t ptxqsav           :  8;
    uint32_t ptxqtop_terminate :  1;
    uint32_t ptxqtop_type      :  2;
    uint32_t ptxqtop_chnum     :  4;
    uint32_t ptxqtop_odd       :  1;
  } b;
} USB_OTG_HPTXSTS_TypeDef;

typedef union { /* offset 0x0414 */
  uint32_t d32;
  struct {
    uint32_t haint         : 16;
    uint32_t reserved16_31 : 16;
  } b;
} USB_OTG_HAINT_TypeDef;

typedef union { /* offset 0x0418 */
  uint32_t d32;
  struct {
    uint32_t haintm        : 16;
    uint32_t reserved16_31 : 16;
  } b;
} USB_OTG_HAINTMSK_TypeDef;

typedef union { /* offset 0x0440 */
  uint32_t d32;
  struct {
    uint32_t pcsts         :  1;
    uint32_t pcdet         :  1;
    uint32_t pena          :  1;
    uint32_t penchng       :  1;
    uint32_t poca          :  1;
    uint32_t pocchng       :  1;
    uint32_t pres          :  1;
    uint32_t psusp         :  1;
    uint32_t prst          :  1;
    uint32_t reserved9     :  1;
    uint32_t plsts         :  2;
    uint32_t ppwr          :  1;
    uint32_t ptctl         :  4;
    uint32_t pspd          :  2; /* the same coding as usb_speed_t */
    uint32_t reserved19_31 : 13;
  } b;
} USB_OTG_HPRT_TypeDef;

/*********************************************************************
* The USB_OTG_HC_REGS structure defines the relative field offsets for
* the host-mode channel control and status global registers.
* These registers are visible only in the host mode and must not be
* accessed in the device mode, as the results are unknown.
*********************************************************************/
typedef struct {
  volatile uint32_t HCCHARx;    /* 500 + 20 * ch_num, channel characteristics */
  volatile uint32_t HCSPLTx;    /* 504 + 20 * ch_num, channel split control */
  volatile uint32_t HCINTx;     /* 508 + 20 * ch_num, channel interrupt */
  volatile uint32_t HCINTMSKx;  /* 50C + 20 * ch_num, channel interrupt mask */
  volatile uint32_t HCTSIZx;    /* 510 + 20 * ch_num, transfer size */
  volatile uint32_t HCDMAx;     /* 514 + 20 * ch_num, channel DMA address */
  volatile uint32_t reserved18; /* 518 + 20 * ch_num, reserved */
  volatile uint32_t reserved1C; /* 51C + 20 * ch_num, reserved */
} USB_OTG_HCHNNLS;

#define P_USB_OTG_HCHNNLS \
  ((USB_OTG_HCHNNLS *)(USB_OTG_BASE_ADDR + USB_OTG_HOST_CHANNEL_REG_OFFSET))

typedef union { /* offset 0x0500 + 0x0020 * ch_num */
  uint32_t d32;
  struct {
    uint32_t mpsiz      : 11;
    uint32_t epnum      :  4;
    uint32_t epdir      :  1;
    uint32_t reserved16 :  1;
    uint32_t lsdev      :  1;
    uint32_t eptyp      :  2;
    uint32_t mcnt       :  2; /* mc - do not change */
    uint32_t dad        :  7;
    uint32_t oddfrm     :  1;
    uint32_t chdis      :  1;
    uint32_t chena      :  1;
  } b;
} USB_OTG_HCCHARx_TypeDef;

typedef union { /* offset 0x0508 + 0x0020 * ch_num */
  uint32_t d32;
  struct {
    uint32_t xfrc          :  1;
    uint32_t chh           :  1;
    uint32_t ahberr        :  1;
    uint32_t stall         :  1;
    uint32_t nak           :  1;
    uint32_t ack           :  1;
    uint32_t nyet          :  1;
    uint32_t txerr         :  1;
    uint32_t bberr         :  1;
    uint32_t frmor         :  1;
    uint32_t dterr         :  1;
    uint32_t reserved11_31 : 21;
  } b;
} USB_OTG_HCINTx_TypeDef;

typedef union { /* offset 0x050C + 0x0020 * ch_num */
  uint32_t d32;
  struct {
    uint32_t xfrcm         :  1;
    uint32_t chhm          :  1;
    uint32_t ahberrm       :  1; /* ahberr - do not change */
    uint32_t stallm        :  1;
    uint32_t nakm          :  1;
    uint32_t ackm          :  1;
    uint32_t nyetm         :  1; /* nyet - do not change */
    uint32_t txerrm        :  1;
    uint32_t bberrm        :  1;
    uint32_t frmorm        :  1;
    uint32_t dterrm        :  1;
    uint32_t reserved11_31 : 21;
  } b;
} USB_OTG_HCINTMSKx_TypeDef;

typedef union { /* offset 0x0510 + 0x0020 * ch_num */
  uint32_t d32;
  struct {
    uint32_t xfrsiz      : 19;
    uint32_t pktcnt      : 10;
    uint32_t pid         :  2;
    uint32_t rerserved31 :  1;
  } b;
} USB_OTG_HCTSIZx_TypeDef;

#define HCTSIZ_PKTCNT_MAX  1023
#define HCTSIZ_DPID_DATA0  0
#define HCTSIZ_DPID_DATA2  1
#define HCTSIZ_DPID_DATA1  2
#define HCTSIZ_DPID_MDATA  3
#define HCTSIZ_DPID_SETUP  3

/*********************************************************************
* The USB_OTG_DEV structure defines the relative field offsets for the
* device mode control and status registers.
* These registers are visible only in the device mode and must not be
* accessed in the host mode, as the results are unknown.
*********************************************************************/
typedef struct {
  volatile uint32_t DCFG;         /* 800 configuration */
  volatile uint32_t DCTL;         /* 804 control */
  volatile uint32_t DSTS;         /* 808 status */
  volatile uint32_t reserved0C;   /* 80C reserved */
  volatile uint32_t DIEPMSK;      /* 810 IN endpoint interrupt mask */
  volatile uint32_t DOEPMSK;      /* 814 OUT endpoint interrupt mask */
  volatile uint32_t DAINT;        /* 818 all endpoints interrupt */
  volatile uint32_t DAINTMSK;     /* 81C all endpoints interrupt mask */
  volatile uint32_t reserved20;   /* 820 reserved */
  volatile uint32_t reserved24;   /* 824 reserved */
  volatile uint32_t DVBUSDIS;     /* 828 VBUS discharge time */
  volatile uint32_t DVBUSPULSE;   /* 82C VBUS pulsing time */
  volatile uint32_t DTHRCTL;      /* 830 threshold control */
  volatile uint32_t DIEPEMPMSK;   /* 834 IN endpoint FIFO empty interrupt mask */
  volatile uint32_t DEACHINT;     /* 838 each endpoint interrupt */
  volatile uint32_t DEACHINTMSK;  /* 83C each endpoint interrupt mask */
  volatile uint32_t DIEPEACHMSK1; /* 840 each endpoint 1 interrupt */
} USB_OTG_DREGS;

#define P_USB_OTG_DREGS \
  ((USB_OTG_DREGS *)(USB_OTG_BASE_ADDR + USB_OTG_DEV_GLOBAL_REG_OFFSET))

typedef union { /* offset 0x0800 */
  uint32_t d32;
  struct {
    uint32_t dspd          :  2; /* the same coding as usb_speed_t */
    uint32_t nzlsohsk      :  1;
    uint32_t reserved3     :  1;
    uint32_t dad           :  7;
    uint32_t pfivl         :  2;
    uint32_t reserved13_13 : 11;
    uint32_t perschivl     :  2;
    uint32_t reserved26_31 :  6;
  } b;
} USB_OTG_DCFG_TypeDef;

typedef union { /* offset 0x0804 */
  uint32_t d32;
  struct {
    uint32_t rwusig        :  1;
    uint32_t sdis          :  1;
    uint32_t ginsts        :  1;
    uint32_t gonsts        :  1;
    uint32_t tctl          :  3;
    uint32_t sginak        :  1;
    uint32_t cginak        :  1;
    uint32_t sgonak        :  1;
    uint32_t cgonak        :  1;
    uint32_t poprgdne      :  1;
    uint32_t reserved12_31 : 20;
  } b;
} USB_OTG_DCTL_TypeDef;

typedef union { /* offset 0x0808 */
  uint32_t d32;
  struct {
    uint32_t suspsts       :  1;
    uint32_t enumspd       :  2; /* the same coding as usb_speed_t */
    uint32_t eerr          :  1;
    uint32_t reserved4_7   :  4;
    uint32_t fnsof         : 14;
    uint32_t reserved22_31 : 10;
  } b;
} USB_OTG_DSTS_TypeDef;

typedef union { /* offset 0x0810 */
  uint32_t d32;
  struct {
    uint32_t xfrcm         :  1;
    uint32_t epdm          :  1;
    uint32_t reserved2     :  1;
    uint32_t tom           :  1;
    uint32_t ittxfemsk     :  1;
    uint32_t inepnmm       :  1;
    uint32_t inepnem       :  1;
    uint32_t reserved7     :  1;
    uint32_t txfurm        :  1;
    uint32_t bim           :  1;
    uint32_t reserved10_31 : 22;
  } b;
} USB_OTG_DIEPMSK_TypeDef;

typedef union { /* offset 0x0814 */
  uint32_t d32;
  struct {
    uint32_t xfrcm         :  1;
    uint32_t epdm          :  1;
    uint32_t reserved2     :  1;
    uint32_t stupm         :  1;
    uint32_t otepdm        :  1;
    uint32_t reserved5     :  1;
    uint32_t b2bstupm      :  1; /* b2bstup -do not change */
    uint32_t reserved7     :  1;
    uint32_t opem          :  1;
    uint32_t boim          :  1;
    uint32_t reserved10_31 : 22;
  } b;
} USB_OTG_DOEPMSK_TypeDef;

typedef union { /* offset 0x0818 */
  uint32_t d32;
  struct {
    uint32_t iepint : 16;
    uint32_t oepint : 16;
  } b;
} USB_OTG_DAINT_TypeDef;

typedef union { /* offset 0x081C */
  uint32_t d32;
  struct {
    uint32_t iepm : 16;
    uint32_t oepm : 16;
  } b;
} USB_OTG_DAINTMSK_TypeDef;

/*********************************************************************
* The USB_OTG_DINEPS structure defines one set of the IN endpoint
* specific registers per implemented logical endpoint.
* These registers are visible only in the device mode and must not be
* accessed in the host mode, as the results are unknown.
*********************************************************************/
typedef struct {
  volatile uint32_t DIEPCTLx;   /* 900 + 20 * ep_num, IN EP control */
  volatile uint32_t reserved04; /* 904 + 20 * ep_num, reserved */
  volatile uint32_t DIEPINTx;   /* 908 + 20 * ep_num, IN EP interrupt */
  volatile uint32_t reserved0C; /* 90C + 20 * ep_num, reserved */
  volatile uint32_t DIEPTSIZx;  /* 910 + 20 * ep_num, IN EP FIFO size */
  volatile uint32_t DIEPDMAx;   /* 914 + 20 * ep_num, DMA address */
  volatile uint32_t DTXFSTSx;   /* 918 + 20 * ep_num, IN EP FIFO status */
  volatile uint32_t reserved1C; /* 91C + 20 * ep_num, reserved */
} USB_OTG_DINEPS;

#define P_USB_OTG_DINEPS \
  ((USB_OTG_DINEPS *)(USB_OTG_BASE_ADDR + USB_OTG_DEV_IN_EP_REG_OFFSET))

/*********************************************************************
* The USB_OTG_DOUTEPS structure defines one set of the OUT endpoint
* specific registers per implemented logical endpoint.
* These registers are visible only in the device mode and must not be
* accessed in the host mode, as the results are unknown.
*********************************************************************/
typedef struct {
  volatile uint32_t DOEPCTLx;   /* B00 + 20 * ep_num, OUT EP control */
  volatile uint32_t reserved04; /* B04 + 20 * ep_num, reserved */
  volatile uint32_t DOEPINTx;   /* B08 + 20 * ep_num, OUT EP interrupt */
  volatile uint32_t reserved0C; /* B0C + 20 * ep_num, reserved */
  volatile uint32_t DOEPTSIZx;  /* B10 + 20 * ep_num, OUT EP FIFO size */
  volatile uint32_t DOEPDMAx;   /* B14 + 20 * ep_num, DMA address */
  volatile uint32_t reserved18; /* B18 + 20 * ep_num, reserved */
  volatile uint32_t reserved1C; /* B1C + 20 * ep_num, reserved */
} USB_OTG_DOUTEPS;

#define P_USB_OTG_DOUTEPS \
  ((USB_OTG_DOUTEPS *)(USB_OTG_BASE_ADDR + USB_OTG_DEV_OUT_EP_REG_OFFSET))

/* DIEPCTL0 offset 0x0900
   DIEPCTLx offset 0x0900 + 0x20 * ep_num
   DOEPCTL0 offset 0x0B00
   DOEPCTLx offset 0x0B00 + 0x20 * ep_num */
typedef union {
  uint32_t d32;
  struct {
    uint32_t mpsiz          : 11;
    uint32_t reserved11_14  :  4;
    uint32_t usbaep         :  1;
    uint32_t dpid_eonum     :  1;
    uint32_t naksts         :  1;
    uint32_t eptyp          :  2;
    uint32_t snpm           :  1;
    uint32_t stall          :  1;
    uint32_t txfnum         :  4;
    uint32_t cnak           :  1;
    uint32_t snak           :  1;
    uint32_t sd0pid_sevnfrm :  1;
    uint32_t sd1pid_soddfrm :  1;
    uint32_t epdis          :  1;
    uint32_t epena          :  1;
  } b;
} USB_OTG_DEPCTLx_TypeDef;

#define MPSIZ_64  0
#define MPSIZ_32  1
#define MPSIZ_16  2
#define MPSIZ_8   3

typedef union { /* offset 0x0908 + 0x20 * ep_num */
  uint32_t d32;
  struct {
    uint32_t xfrc          :  1;
    uint32_t epdisd        :  1;
    uint32_t reserved2     :  1;
    uint32_t toc           :  1;
    uint32_t inixfe        :  1;
    uint32_t reserved5     :  1;
    uint32_t inepne        :  1;
    uint32_t txfe          :  1;
    uint32_t txfifoudrn    :  1;
    uint32_t bna           :  1;
    uint32_t reserved10    :  1;
    uint32_t pktdrpsts     :  1;
    uint32_t berr          :  1;
    uint32_t nak           :  1;
    uint32_t reserved14_31 : 18;
  } b;
} USB_OTG_DIEPINTx_TypeDef;

typedef union { /* offset 0x0B08 + 0x20 * ep_num */
  uint32_t d32;
  struct {
    uint32_t xfrc          :  1;
    uint32_t epdisd        :  1;
    uint32_t reserved2     :  1;
    uint32_t stup          :  1;
    uint32_t otepdis       :  1;
    uint32_t reserved5     :  1;
    uint32_t b2bstup       :  1;
    uint32_t reserved7_13  :  7;
    uint32_t nyet          :  1;
    uint32_t reserved15_31 : 17;
  } b;
} USB_OTG_DOEPINTx_TypeDef;

/* DIEPTSIZ0 offset 0x0910
   DOEPTSIZ0 offset 0x0B10
   DIEPTSIZx offset 0x0910 + 0x20 * ep_num
   DOEPTSIZx offset 0x0B10 + 0x20 * ep_num */
typedef union {
  uint32_t d32;
  struct {
    uint32_t xfrsiz             : 19;
    uint32_t pktcnt             : 10;
    uint32_t stupcnt_mcnt_rxpid :  2;
    uint32_t reserved31         :  1;
  } b;
} USB_OTG_DEPTSIZx_TypeDef;

typedef union { /* offset 0x0918 + 0x20 * ep_num */
  uint32_t d32;
  struct {
    uint32_t ineptfsav     : 16;
    uint32_t reserved16_31 : 16;
  } b;
} USB_OTG_DTXFSTSx_TypeDef;

/*********************************************************************
* The USB_OTG_PREGS structure defines the relative field offsets for
* the power and clock gate control registers.
*********************************************************************/
typedef struct {
  volatile uint32_t PCGCCTL; /* E00 power and clock gate control */
} USB_OTG_PREGS;

#define P_USB_OTG_PREGS \
  ((USB_OTG_PREGS *)(USB_OTG_BASE_ADDR + USB_OTG_PCGCCTL_OFFSET))

typedef union { /* offset 0x0E00 */
  uint32_t d32;
  struct {
    uint32_t stppclk      :  1;
    uint32_t gatehclk     :  1;
    uint32_t reserved2_3  :  2;
    uint32_t physusp      :  1;
    uint32_t reserved5_31 : 27;
  } b;
} USB_OTG_PCGCCTL_TypeDef;


/*********************************************************************
* The USB_OTG_FIFO structure defines the FIFO registers.
*********************************************************************/
typedef volatile uint32_t USB_OTG_FIFO[USB_OTG_DATA_FIFO_SIZE >> 2];

#define P_USB_OTG_FIFO \
  ((USB_OTG_FIFO *)(USB_OTG_BASE_ADDR + USB_OTG_DATA_FIFO_OFFSET))

#endif /* STM32F10X_CL || STM32F2XX || STM32F4XX */

#endif /* _USB_OTG_REGS_H */

