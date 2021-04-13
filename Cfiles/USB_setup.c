#include "../Headers/USB_setup.h"

static  usb_speed_t speed = FULL_SPEED;
static  usb_phy_t   phy = USB_PHY_A;

int USBDconfigure(int clk) {
  if (USBDcoreConfigure() < 0)
    return -1;
  if (USBDcentralConfigure() < 0)
    return -1;
  if (USBDperipheralReset() < 0)
    return -1;
  if (USBDperipheralConfigure(clk) < 0)
    return -1;
  return 0;
}

/** USB device core structure **/

/* States of the control pipe */
typedef enum {
  IDLE, DATA_IN, DATA_OUT, LAST_DATA_IN,
  WAIT_STATUS_IN, WAIT_STATUS_OUT
} usb_control_state_t;

typedef struct {
  usb_visible_state_t        visibleState;
  usb_control_state_t        controlState;
  usb_setup_packet_t         setup;
  usbd_callback_list_t const *callback;
  uint8_t              const *txdata;
  uint8_t                    *rxdata;
  uint16_t                   length;
  uint8_t                    maxPacketSize0;
} usb_device_state_t;

static usb_device_state_t DeviceState;

/** Device initialization **/

int USBDcoreConfigure() {
  memset(&DeviceState, 0, sizeof(usb_device_state_t));
  DeviceState.callback = USBDgetApplicationCallbacks();
  if (DeviceState.callback == 0)
    return -1;
  if (DeviceState.callback->Configure)
    if (DeviceState.callback->Configure() < 0)
      return -1;
  DeviceState.visibleState = POWERED;
  DeviceState.controlState = IDLE;
  return 0;
}

/* Configure USB central components. */
static int USBDcentralConfigure() {
   
     /*** GPIO config ***/
   /* PA11 - USB_DM; PA12 - USB_DP */
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
   
   // Set Alternate function
   GPIOA->MODER |= GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1;
   GPIOA->MODER &= ~(GPIO_MODER_MODER11_0) | ~(GPIO_MODER_MODER12_0);
   
   // set low speed 
   GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR11_0 | ~GPIO_OSPEEDER_OSPEEDR12_0;
   
   // set AF 14 for PA11 & PA12
   GPIOA->AFR[1] |= 0xEE000;
   GPIOA->AFR[1] &= ~(uint32_t)0x11000;
   
   // set OTYPE push pull
   GPIOA->OTYPER &= ~GPIO_OTYPER_OT_11 | ~GPIO_OTYPER_OT_12;
   
   // no pullup no pulldown (00)
   GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR11_1 | ~GPIO_PUPDR_PUPDR12_1; 
   GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR11_0 | ~GPIO_PUPDR_PUPDR12_0;
   
   RCC->APB1ENR |= RCC_APB1ENR_USBEN;

  return 0;
}

/* Reset USB peripheral and simulate cable disconnection. */
static int USBDperipheralReset(void) {
  USB_OTG_GRSTCTL_TypeDef grstctl;
  USB_OTG_DCTL_TypeDef    dctl;
  int timeout;

  /* Reset the USB core. */
  grstctl.d32 = 0;
  grstctl.b.csrst = 1;
  P_USB_OTG_GREGS->GRSTCTL = grstctl.d32;
  timeout = 20;
  do {
    if (--timeout < 0)
      return -1;
    grstctl.d32 = P_USB_OTG_GREGS->GRSTCTL;
  } while (grstctl.b.csrst == 1);

  /* Wait for the AHB master idle state. */
  timeout = 20;
  do {
    if (--timeout < 0)
      return -1;
    grstctl.d32 = P_USB_OTG_GREGS->GRSTCTL;
  } while (grstctl.b.ahbidl == 0);

  /* Disconnect as early as possible. */
  dctl.d32 = 0;
  dctl.b.sdis = 1;
  P_USB_OTG_DREGS->DCTL = dctl.d32;

  return 0;
}

static int USBDperipheralConfigure(int clk) {
  USB_OTG_DCFG_TypeDef    dcfg;
  USB_OTG_GUSBCFG_TypeDef gusbcfg;
  USB_OTG_GCCFG_TypeDef   gccfg;
  USB_OTG_GI2CCTL_TypeDef gi2cctl;
  USB_OTG_GINTMSK_TypeDef gintmsk;
  USB_OTG_GAHBCFG_TypeDef gahbcfg;

  /* Configure USB in the device mode.
     DCFG OTG HS register is badly documented in RM0033.
      Reset value is 0x00200200.
      If the nzlsohsk field is 1 then the STALL handshake is sent on
      a non-zero-length status OUT transaction and the received OUT
      packet is not sent to the application.
      The dspd field has the same coding as usb_speed_t and
      the XcvrSelect field in USB3300 PHY:
       0 == HS transceiver,
       1 == FS transceiver,
       2 == LS transceiver,
       3 == FS transceiver for LS packets (FS preamble automatically
            pre-ended).
     GUSBCFG OTG HS register is badly documented in RM0033.
      Reset value is 0x00001410.
      Undocumented or badly documented fields:
       phyif (bit 3) - 0 == 8 bit interface,
       ulpisel (bit 4) - read-only, always 1,
       fsintf (bit 5) - function unknown,
       physel (bit 6) - perhaps select internal PHY,
       ddrsel (bit 7) - 0 == single data rate, 1 == double data rate,
       trdt (bits 10:13) - reset value 5, for 120 MHz should be 3.
    For OTG FS registers see comment is usbd_configure_107.c. */
  dcfg.d32 = 0;
  dcfg.b.nzlsohsk = 1;
  dcfg.b.dspd = speed;
  P_USB_OTG_DREGS->DCFG = dcfg.d32;
  gusbcfg.d32 = 0;
  gusbcfg.b.fdmod = 1; /* Force the device mode. */
  if (clk >= 96)
    gusbcfg.b.trdt = 3;
  else if (clk >= 64)
    gusbcfg.b.trdt = 4;
  else
    gusbcfg.b.trdt = 5;
  if (phy == USB_PHY_B) /* TODO: not tested */
    gusbcfg.b.physel = 1;
  else if (phy == USB_PHY_ULPI)
    gusbcfg.b.ulpievbusd = 1; /* Use an external VBUS supply. */
  else if (phy == USB_PHY_I2C) /* TODO: not tested */
    gusbcfg.b.otgutmifssel = 1;
  P_USB_OTG_GREGS->GUSBCFG = gusbcfg.d32;
  gccfg.d32 = 0;
  gccfg.b.vbusbsen = 1; /* Set VBUS sensing on B device. */
  if (phy == USB_PHY_A || phy == USB_PHY_B)
    gccfg.b.pwrdwn = 1; /* Deactivate PHY power down. */
  else if (phy == USB_PHY_I2C) { /* TODO: not tested */
    gccfg.b.pwrdwn = 1;
    gccfg.b.i2cpaden = 1;
  }
  P_USB_OTG_GREGS->GCCFG = gccfg.d32;
  if (phy == USB_PHY_I2C) { /* TODO: not tested */
    gi2cctl.d32 = 0;
    gi2cctl.b.addr = 0x2d;
    gi2cctl.b.i2cdevaddr = 1;
    gi2cctl.b.i2cdatse0 = 1;
    P_USB_OTG_GREGS->GI2CCTL = gi2cctl.d32;
    delay_ms(200);
    gi2cctl.b.i2cen = 1;
    P_USB_OTG_GREGS->GI2CCTL = gi2cctl.d32;
  }
  P_USB_OTG_PREGS->PCGCCTL = 0;  /* Not reset by grstctl.b.csrst */

  /* Configure the global interrupt. */
  gintmsk.d32 = 0;
  gintmsk.b.usbrstm = 1;
  gintmsk.b.enumdnem = 1;
  gintmsk.b.rxflvlm = 1;
  gintmsk.b.oepim = 1;
  gintmsk.b.iepim = 1;
  gintmsk.b.sofm = 1; /* Required for isochronous transfers */
  P_USB_OTG_GREGS->GINTMSK = gintmsk.d32;
  gahbcfg.d32 = 0;
  gahbcfg.b.txfelvl = 1; /* Interrupt when TX FIFO completely empty */
  gahbcfg.b.gintmsk = 1; /* Enable the global interrupt. */
  P_USB_OTG_GREGS->GAHBCFG = gahbcfg.d32;

  /* Simulate cable reconnection. Disconnet time for high speed must
     be longer than 125 us - add some delay. */
  delay_ms(400);
  P_USB_OTG_DREGS->DCTL = 0;

  return 0;
}

#include <board_usb_def.h>
#include <error.h>
#include <lcd_util.h>
#include <pwr_periph.h>
#include <stdio.h>
#include <usb_endianness.h>
#include <usb_vid_pid.h>
#include <usbd_api.h>
#include <usbd_callbacks.h>

/** Descriptors **/

#define BLK_BUFF_SIZE  MAX_FS_BULK_PACKET_SIZE
#define INT_BUFF_SIZE  16

static usb_device_descriptor_t const device_descriptor = {
  sizeof(usb_device_descriptor_t), /* bLength */
  DEVICE_DESCRIPTOR,               /* bDescriptorType */
  HTOUSBS(0x0200),                 /* bcdUSB */
  COMMUNICATION_DEVICE_CLASS,      /* bDeviceClass */
  0,                               /* bDeviceSubClass */
  0,                               /* bDeviceProtocol */
  64,                              /* bMaxPacketSize0 */
  HTOUSBS(VID),                    /* idVendor */
  HTOUSBS(PID + 2),                /* idProduct */
  HTOUSBS(0x0100),                 /* bcdDevice */
  1,                               /* iManufacturer */
  2,                               /* iProduct */
  3,                               /* iSerialNumber */
  1                                /* bNumConfigurations */
};

typedef struct {
  usb_configuration_descriptor_t       cnf_descr;
  usb_interface_descriptor_t           if0_descr;
  usb_cdc_header_descriptor_t          cdc_h_descr;
  usb_cdc_call_management_descriptor_t cdc_cm_descr;
  usb_cdc_acm_descriptor_t             cdc_acm_descr;
  usb_cdc_union_descriptor_t           cdc_u_descr;
  usb_endpoint_descriptor_t            ep2in_descr;
  usb_interface_descriptor_t           if1_descr;
  usb_endpoint_descriptor_t            ep1out_descr;
  usb_endpoint_descriptor_t            ep1in_descr;
} __packed usb_com_configuration_t;

#ifndef USB_BM_ATTRIBUTES
  #define USB_BM_ATTRIBUTES  (SELF_POWERED | D7_RESERVED)
#endif
#ifndef USB_B_MAX_POWER
  #define USB_B_MAX_POWER  1
#endif

static usb_com_configuration_t const com_configuration = {
  {
    sizeof(usb_configuration_descriptor_t),   /* bLength */
    CONFIGURATION_DESCRIPTOR,                 /* bDescriptorType */
    HTOUSBS(sizeof(usb_com_configuration_t)), /* wTotalLength */
    2,                                        /* bNumInterfaces */
    1,                                        /* bConfigurationValue */
    0,                                        /* iConfiguration */
    USB_BM_ATTRIBUTES,                        /* bmAttributes */
    USB_B_MAX_POWER                           /* bMaxPower */
  },
  {
    sizeof(usb_interface_descriptor_t), /* bLength */
    INTERFACE_DESCRIPTOR,               /* bDescriptorType */
    0,                                  /* bInterfaceNumber */
    0,                                  /* bAlternateSetting */
    1,                                  /* bNumEndpoints */
    COMMUNICATION_INTERFACE_CLASS,      /* bInterfaceClass */
    ABSTRACT_CONTROL_MODEL_SUBCLASS,    /* bInterfaceSubClass */
    0,                                  /* bInterfaceProtocol */
    0                                   /* iInterface */
  },
  {
    sizeof(usb_cdc_header_descriptor_t), /* bFunctionLength */
    CS_INTERFACE_DESCRIPTOR,             /* bDescriptorType */
    CDC_HEADER_DESCRIPTOR,               /* bDescriptorSubtype */
    HTOUSBS(0x120)                       /* bcdCDC */
  },
  {
    sizeof(usb_cdc_call_management_descriptor_t),/* bFunctionLength */
    CS_INTERFACE_DESCRIPTOR,                     /* bDescriptorType */
    CDC_CALL_MANAGEMENT_DESCRIPTOR,           /* bDescriptorSubtype */
    3,                                        /* bmCapabilities */
    1                                         /* bDataInterface */
  },
  {
    sizeof(usb_cdc_acm_descriptor_t), /* bFunctionLength */
    CS_INTERFACE_DESCRIPTOR,          /* bDescriptorType */
    CDC_ACM_DESCRIPTOR,               /* bDescriptorSubtype */
    2                                 /* bmCapabilities */
  },
  {
    sizeof(usb_cdc_union_descriptor_t), /* bFunctionLength */
    CS_INTERFACE_DESCRIPTOR,            /* bDescriptorType */
    CDC_UNION_DESCRIPTOR,               /* bDescriptorSubtype */
    0,                                  /* bControlInterface */
    1                                   /* bSubordinateInterface0 */
  },
  {
    sizeof(usb_endpoint_descriptor_t), /* bLength */
    ENDPOINT_DESCRIPTOR,               /* bDescriptorType */
    ENDP2 | ENDP_IN,                   /* bEndpointAddress */
    INTERRUPT_TRANSFER,                /* bmAttributes */
    HTOUSBS(INT_BUFF_SIZE),            /* wMaxPacketSize */
    3                                  /* bInterval */
  },
  {
    sizeof(usb_interface_descriptor_t), /* bLength */
    INTERFACE_DESCRIPTOR,               /* bDescriptorType */
    1,                                  /* bInterfaceNumber */
    0,                                  /* bAlternateSetting */
    2,                                  /* bNumEndpoints */
    DATA_INTERFACE_CLASS,               /* bInterfaceClass */
    0,                                  /* bInterfaceSubClass */
    0,                                  /* bInterfaceProtocol */
    0                                   /* iInterface */
  },
  {
    sizeof(usb_endpoint_descriptor_t), /* bLength */
    ENDPOINT_DESCRIPTOR,               /* bDescriptorType */
    ENDP1 | ENDP_OUT,                  /* bEndpointAddress */
    BULK_TRANSFER,                     /* bmAttributes */
    HTOUSBS(BLK_BUFF_SIZE),            /* wMaxPacketSize */
    0                                  /* bInterval */
  },
  {
    sizeof(usb_endpoint_descriptor_t), /* bLength */
    ENDPOINT_DESCRIPTOR,               /* bDescriptorType */
    ENDP1 | ENDP_IN,                   /* bEndpointAddress */
    BULK_TRANSFER,                     /* bmAttributes */
    HTOUSBS(BLK_BUFF_SIZE),            /* wMaxPacketSize */
    0                                  /* bInterval */
  }
};

static usb_string_descriptor_t(1) const string_lang = {
  sizeof(usb_string_descriptor_t(1)),
  STRING_DESCRIPTOR,
  {HTOUSBS(LANG_US_ENGLISH)}
};

static usb_string_descriptor_t(16) const string_manufacturer = {
  sizeof(usb_string_descriptor_t(16)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('M'), HTOUSBS('a'), HTOUSBS('r'), HTOUSBS('c'),
    HTOUSBS('i'), HTOUSBS('n'), HTOUSBS(' '), HTOUSBS('P'),
    HTOUSBS('e'), HTOUSBS('c'), HTOUSBS('z'), HTOUSBS('a'),
    HTOUSBS('r'), HTOUSBS('s'), HTOUSBS('k'), HTOUSBS('i')
  }
};

static usb_string_descriptor_t(31) const string_product = {
  sizeof(usb_string_descriptor_t(31)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('U'), HTOUSBS('S'), HTOUSBS('B'), HTOUSBS(' '),
    HTOUSBS('v'), HTOUSBS('i'), HTOUSBS('r'), HTOUSBS('t'),
    HTOUSBS('u'), HTOUSBS('a'), HTOUSBS('l'), HTOUSBS(' '),
    HTOUSBS('s'), HTOUSBS('e'), HTOUSBS('r'), HTOUSBS('i'),
    HTOUSBS('a'), HTOUSBS('l'), HTOUSBS(' '), HTOUSBS('p'),
    HTOUSBS('o'), HTOUSBS('r'), HTOUSBS('t'), HTOUSBS(' '),
    HTOUSBS('e'), HTOUSBS('x'), HTOUSBS('a'), HTOUSBS('m'),
    HTOUSBS('p'), HTOUSBS('l'), HTOUSBS('e')
  }
};

static usb_string_descriptor_t(10) const string_serial = {
  sizeof(usb_string_descriptor_t(10)),
  STRING_DESCRIPTOR,
  {
    HTOUSBS('0'), HTOUSBS('0'), HTOUSBS('0'), HTOUSBS('0'),
    HTOUSBS('0'), HTOUSBS('0'), HTOUSBS('0'), HTOUSBS('0'),
    HTOUSBS('0'), HTOUSBS('1')
  }
};

typedef struct {
  uint8_t const *data;
  uint16_t      length;
} string_table_t;

static string_table_t const strings[] = {
  {(uint8_t const*)&string_lang,         sizeof string_lang},
  {(uint8_t const*)&string_manufacturer, sizeof string_manufacturer},
  {(uint8_t const*)&string_product,      sizeof string_product},
  {(uint8_t const*)&string_serial,       sizeof string_serial}
};
static uint32_t const stringCount = sizeof(strings)/sizeof(strings[0]);

/** Callbacks **/

static int          Configure(void);
static uint8_t      Reset(usb_speed_t);
static usb_result_t GetDescriptor(uint16_t, uint16_t,
                                  uint8_t const **, uint16_t *);
static uint8_t      GetConfiguration(void);
static usb_result_t SetConfiguration(uint16_t);
static uint16_t     GetStatus(void);
static usb_result_t ClassNoDataSetup(usb_setup_packet_t const *);
static usb_result_t ClassInDataSetup(usb_setup_packet_t const *,
                                     uint8_t const **, uint16_t *);
static usb_result_t ClassOutDataSetup(usb_setup_packet_t const *,
                                      uint8_t **);
static void         ClassStatusIn(usb_setup_packet_t const *);
static void         EP1OUT(void);
static void         EP2IN(void);

static usbd_callback_list_t const ApplicationCallBacks = {
  Configure, Reset, 0, GetDescriptor, 0,
  GetConfiguration, SetConfiguration, GetStatus, 0, 0,
  0, 0,
  ClassNoDataSetup, ClassInDataSetup,
  ClassOutDataSetup, ClassStatusIn,
  {0, EP2IN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {EP1OUT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  0, 0
};

/** COM implementation **/

static uint16_t ep2queue;
static uint8_t configuration, refresh, rs232state;
static usb_cdc_line_coding_t rs232coding;

/* rs232state bits */
#define DTR  0x01
#define DCD  0x02
#define DSR  0x04
#define RTS  0x08
#define CTS  0x10

/* Print diagnostic information on LCD. */
static void LCDrefresh(void) {
  if (refresh) {
    char buffer[24], *stop, parity;

    refresh = 0;
    if (rs232coding.bCharFormat == ONE_STOP_BIT)
      stop = "1  ";
    else if (rs232coding.bCharFormat == ONE_AND_HALF_STOP_BITS)
      stop = "1.5";
    else if (rs232coding.bCharFormat == TWO_STOP_BITS)
      stop = "2  ";
    else
      stop = "?  ";
    if (rs232coding.bParityType == NO_PARITY)
      parity = 'N';
    else if (rs232coding.bParityType == ODD_PARITY)
      parity = 'O';
    else if (rs232coding.bParityType == EVEN_PARITY)
      parity = 'E';
    else if (rs232coding.bParityType == MARK_PARITY)
      parity = 'M';
    else if (rs232coding.bParityType == SPACE_PARITY)
      parity = 'S';
    else
      parity = '?';
    sprintf(buffer, "%6lu %u%c%s%2hu", rs232coding.dwDTERate,
            rs232coding.bDataBits, parity, stop, ep2queue);
    LCDgoto(0, 0);
    LCDwrite(buffer);
    LCDgoto(1, 0);
    if (rs232state & RTS)
      LCDwrite("RTS ");
    else
      LCDwrite("    ");
    if (rs232state & CTS)
      LCDwrite("CTS ");
    else
      LCDwrite("    ");
    if (rs232state & DTR)
      LCDwrite("DTR ");
    else
      LCDwrite("    ");
    if (rs232state & DSR)
      LCDwrite("DSR ");
    else
      LCDwrite("    ");
    if (rs232state & DCD)
      LCDwrite("DCD ");
    else
      LCDwrite("    ");
  }
}

static void ResetState(void) {
  ep2queue = 0;
  configuration = 0;
  rs232coding.dwDTERate = 38400;
  rs232coding.bCharFormat = ONE_STOP_BIT;
  rs232coding.bParityType = NO_PARITY;
  rs232coding.bDataBits = 8;
  rs232state = 0;
  refresh = 1;
}

usbd_callback_list_t const * USBDgetApplicationCallbacks() {
  return &ApplicationCallBacks;
}

int Configure() {
  ResetState();
  LCDsetRefresh(LCDrefresh);
  PowerLEDconfigure();
  return 0;
}

uint8_t Reset(usb_speed_t speed) {
  ErrorResetable(speed == FULL_SPEED ? 0 : -1, 6);
  ResetState();

  /* Default control endpoint must be configured here. */
  if (USBDendPointConfigure(ENDP0, CONTROL_TRANSFER,
                            device_descriptor.bMaxPacketSize0,
                            device_descriptor.bMaxPacketSize0) !=
                                                      REQUEST_SUCCESS)
    ErrorResetable(-1, 7);

  return device_descriptor.bMaxPacketSize0;
}

usb_result_t GetDescriptor(uint16_t wValue, uint16_t wIndex,
                           uint8_t const **data, uint16_t *length) {
  uint32_t index = wValue & 0xff;

  switch (wValue >> 8) {
    case DEVICE_DESCRIPTOR:
      if (index == 0 && wIndex == 0) {
        *data = (uint8_t const *)&device_descriptor;
        *length = sizeof(device_descriptor);
        return REQUEST_SUCCESS;
      }
      return REQUEST_ERROR;
    case CONFIGURATION_DESCRIPTOR:
      if (index == 0 && wIndex == 0) {
        *data = (uint8_t const *)&com_configuration;
        *length = sizeof(com_configuration);
        return REQUEST_SUCCESS;
      }
      return REQUEST_ERROR;
    case STRING_DESCRIPTOR:
      if (index < stringCount) {
        *data = strings[index].data;
        *length = strings[index].length;
        return REQUEST_SUCCESS;
      }
      return REQUEST_ERROR;
    default:
      return REQUEST_ERROR;
  }
}

uint8_t GetConfiguration() {
  return configuration;
}

usb_result_t SetConfiguration(uint16_t confValue) {
  if (confValue > device_descriptor.bNumConfigurations)
    return REQUEST_ERROR;

  configuration = confValue;
  USBDdisableAllNonControlEndPoints();
  if (confValue == com_configuration.cnf_descr.bConfigurationValue) {
    usb_result_t r1, r2;

    r1 = USBDendPointConfigure(ENDP1, BULK_TRANSFER,
                               BLK_BUFF_SIZE, BLK_BUFF_SIZE);
    r2 = USBDendPointConfigure(ENDP2, INTERRUPT_TRANSFER,
                               0, INT_BUFF_SIZE);
    if (r1 == REQUEST_SUCCESS && r2 == REQUEST_SUCCESS)
      return REQUEST_SUCCESS;
    else
      return REQUEST_ERROR;
  }

  return REQUEST_SUCCESS; /* confValue == 0 */
}

uint16_t GetStatus() {
  /* Current power setting should be reported. */
  if (com_configuration.cnf_descr.bmAttributes & SELF_POWERED)
    return STATUS_SELF_POWERED;
  else
    return 0;
}

static usb_cdc_serial_state_t state = {
  {DEVICE_TO_HOST | CLASS_REQUEST | INTERFACE_RECIPIENT,
   SERIAL_STATE, 0, 0, 2}, 0
};

usb_result_t ClassNoDataSetup(usb_setup_packet_t const *setup) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == SET_CONTROL_LINE_STATE &&
      setup->wIndex == 0 &&
      setup->wLength == 0) {
    uint8_t new_rs232state;

    /* Host do device: DTR or RTS notification */
    new_rs232state = rs232state;
    if (setup->wValue & 1) /* DTR set */
      new_rs232state |= (DTR | DSR | DCD);
    else
      new_rs232state &= ~(DTR | DSR | DCD);
    if (setup->wValue & 2) /* RTS set */
      new_rs232state |= RTS | CTS;
    else
      new_rs232state &= ~(RTS | CTS);

    /* Device to host: DCD or DSR notification */
    if ((rs232state ^ new_rs232state) & (DCD | DSR)) {
      state.wData = 0;
      if (new_rs232state & DCD)
        state.wData |= 1;
      if (new_rs232state & DSR)
        state.wData |= 2;
      if (ep2queue == 0)
        USBDwrite(ENDP2, (uint8_t const *)&state, sizeof(state));
      if (ep2queue < 2)
        ++ep2queue;
    }

    /* Set new state. */
    if (rs232state != new_rs232state) {
      rs232state = new_rs232state;
      refresh = 1;
    }

    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

usb_result_t ClassInDataSetup(usb_setup_packet_t const *setup,
                              uint8_t const **data,
                              uint16_t *length) {
  if (setup->bmRequestType == (DEVICE_TO_HOST |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == GET_LINE_CODING &&
      setup->wValue == 0 &&
      setup->wIndex == 0) {
    *data = (const uint8_t *)&rs232coding;
    *length = sizeof(rs232coding);
    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

usb_result_t ClassOutDataSetup(usb_setup_packet_t const *setup,
                               uint8_t **data) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == SET_LINE_CODING &&
      setup->wValue == 0 &&
      setup->wIndex == 0 &&
      setup->wLength == sizeof(rs232coding)) {
    *data = (uint8_t *)&rs232coding;
    return REQUEST_SUCCESS;
  }
  return REQUEST_ERROR;
}

void ClassStatusIn(usb_setup_packet_t const *setup) {
  if (setup->bmRequestType == (HOST_TO_DEVICE |
                               CLASS_REQUEST |
                               INTERFACE_RECIPIENT) &&
      setup->bRequest == SET_LINE_CODING &&
      setup->wValue == 0 &&
      setup->wIndex == 0 &&
      setup->wLength == sizeof(rs232coding)) {
    refresh = 1;
  }
}



static uint8_t const help[] =
  "Press:\r\n"
  "  G to switch green LED on,\r\n"
  "  g to switch green LED off,\r\n"
  "  R to switch red LED on,\r\n"
  "  r to switch red LED off,\r\n"
  "  W to switch white LED on,\r\n"
  "  w to switch white LED off.\r\n";


