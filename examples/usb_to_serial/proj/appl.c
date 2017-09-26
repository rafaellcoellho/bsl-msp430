/******************************************************************************
 * file name  : appl.c
 * description: a small example application to show invoking custom BSL
 *****************************************************************************/

/******************************************************************************
 * includes
 *****************************************************************************/
#include "msp430x552x.h"
#include <string.h>

/******************************************************************************
 * constants and macros
 *****************************************************************************/

/* 
 * Define the following macro invoke the BSL without using the BSL flag 
 * string and hardware reset, but simply by jumping to the beginning of 
 * Z-AREA (address 0x1000)
 */
#define INVOKE_BSL_WITHOUT_RESET

/* 
 * Uncomment the following macro to use software reset to try invoke the BSL
 * WARNING: this is only for test purpose to show that software reset will
 * not trigger the boot code 
 */
//#define USE_SW_RESET

#ifdef USE_SW_RESET
/* 
 * SW reset implementation - choose one of the following using:
 * watchdog timer, software BOR, or software POR
 */
//#define SW_RESET()      WDTCTL = WDT_MRST_0_064; while(1);  // watchdog reset
#define SW_RESET()      PMMCTL0 = PMMPW + PMMSWBOR + (PMMCTL0 & 0x0003);  // software BOR reset
//#define SW_RESET()      PMMCTL0 = PMMPW + PMMSWPOR + (PMMCTL0 & 0x0003);  // software POR reset
#endif

/******************************************************************************
 * global variables
 *****************************************************************************/


/******************************************************************************
 * local variables
 *****************************************************************************/

#ifndef INVOKE_BSL_WITHOUT_RESET
/* 
 * The run BSL Flag string to be written into the beginning of Info B
 */
static const char RunBslFlagString[] = {"RUNBSL"};
#endif

/* 
 * UART receive buffer where the command string will be first stored
 */
static char RxBuf[128];

/* 
 * the length of UART command in RxBuf
 */
static unsigned int RxLen, SysRstIv;

/******************************************************************************
 * local function declarations
 *****************************************************************************/

void init(void);
void send_byte (char byte);
void send_string(char *string);
#ifndef INVOKE_BSL_WITHOUT_RESET
void erase_info_b(void);
void write_bsl_flag(void);
#endif

/******************************************************************************
 * main function
 *****************************************************************************/
void main(void)
{
  /*
   * do initialization
   */ 
  init();
   
  /*
   * send string indicating application is running
   */ 
  send_string("MSP430APP\n");

  while(1)
  {
    /*
     * send console command
     */
    send_string("CMD> ");

    /*
     * go sleep until whole command (ended with '\r') is received
     */
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
    
    /*
     * check for valid command length
     */
    if(RxLen > 0)
    {
      
      RxLen = 0;

      /*
       *  setting LED on command
       */
      if(!memcmp(RxBuf, "LED ON", 6))
      {
        P1OUT |= BIT0;
        send_string("Turning LED ON - OK!\n");
      }
      /*
       *  setting LED off command
       */
      else if(!memcmp(RxBuf, "LED OFF", 7))
      {
        P1OUT &= ~BIT0;
        send_string("Turning LED OFF - OK!\n");
      }            
      /*
       *  running BSL command
       */
      else if(!memcmp(RxBuf, "RUN BSL", 7))
      {
      #ifdef INVOKE_BSL_WITHOUT_RESET
        /* 
         * jump directly to BSL
         */
        ((void (*)(void))0x1000)();
      
      #else
      
        /* 
         * write BSL flag string
         */
        write_bsl_flag();
        send_string("Writing BSL Flag - OK!\n");

      #ifdef USE_SW_RESET
        /*
         * do soft reset
         * WARNING: this will not trigger the boot code and the BSL
         * will not be executed - use for test purpose only
         */
        send_string("Resetting ...\n");
        SW_RESET();
      #else        
        /*
         *  ask user to do hard reset
         */
        send_string("Please reset the hardware\n");
        while(1);
      #endif /* USE_SW_RESET */
      
      #endif /* INVOKE_BSL_WITHOUT_RESET */
      }            
      /*
       *  unknown command
       */
      else
      {
        send_string("Invalid CMD - ERROR!\n");
      }            
    }
  }
}

/******************************************************************************
 * low level initialization function
 *****************************************************************************/
void _system_pre_init(void)
{
  /*
   * Stop WDT
   */
  WDTCTL = WDTPW + WDTHOLD;

  // save reset information
  SysRstIv = SYSRSTIV;
}

/******************************************************************************
 * function for initializing peripherals
 *****************************************************************************/
void init(void)
{
  /* 
   * Set the UCS to work with REFO (ACLK) and DCO (MCLK, SMCLK)
   */
  UCSCTL6 |= XT1OFF + XT2OFF;
  UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
  UCSCTL4 |= SELA_2;                        // Set ACLK = REFO

  // Loop until XT1,XT2 & DCO fault flag is cleared
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

  /*
   *  setup P1.0 as output connected to a LED
   */
  P1DIR |= BIT0;
  P1OUT &= ~BIT0;

  /*
   *  setup UART interface
   */
  P3SEL |= BIT3 + BIT4;                     // P5.6,7 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 6;                              // 1MHz 9600 (see User's Guide)
  UCA0BR1 = 0;                              // 1MHz 9600
  UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
                                            // over sampling
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

  /*
   * reset UART command receive buffer
   */
  memset(RxBuf, 0x00, sizeof(RxBuf));
  RxLen = 0;
  
#ifndef INVOKE_BSL_WITHOUT_RESET
  /*
   * erase BSL run flag from info memory if it is available
   */
  if(!memcmp((char*)0x1900, RunBslFlagString, sizeof(RunBslFlagString)))
  {
    erase_info_b();
  }
#endif  
}

/******************************************************************************
 * function for sending a byte via UART interface
 *****************************************************************************/
void send_byte (char byte)
{ 
  if((byte == '\r') || (byte == '\n'))
  {
    /*
     * send new line
     */
    while (!(UCA0IFG&UCTXIFG));
    UCA0TXBUF = '\r';
    while (!(UCA0IFG&UCTXIFG));
    UCA0TXBUF = '\n';
  }
  else
  {
    while (!(UCA0IFG&UCTXIFG));
    UCA0TXBUF = byte;
  }
}

/******************************************************************************
 * function for sending a NULL terminated string via UART interface
 *****************************************************************************/
void send_string (char *string)
{
  unsigned int i;
  for(i=0 ; i<strlen(string) ; i++)
  {
    send_byte(string[i]);
  }
}

#ifndef INVOKE_BSL_WITHOUT_RESET
/******************************************************************************
 * function for eraseing Info B memory content
 *****************************************************************************/
void erase_info_b(void)
{
  char * Flash_ptr;                         // Initialize Flash pointer
  Flash_ptr = (char *) 0x1900;
  __disable_interrupt();                    // 5xx Workaround: Disable global
                                            // interrupt while erasing. Re-Enable
                                            // GIE if needed
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
  FCTL1 = FWKEY;                            // Clear ERASE bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
}

/******************************************************************************
 * function for writing a flag string into the beginning of Info B memory
 *****************************************************************************/
void write_bsl_flag(void)
{
  unsigned int i;
  char * Flash_ptr;                         // Initialize Flash pointer
  Flash_ptr = (char *) 0x1900;
  __disable_interrupt();                    // 5xx Workaround: Disable global
                                            // interrupt while erasing. Re-Enable
                                            // GIE if needed
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

  for (i = 0; i < sizeof(RunBslFlagString) ; i++)
  {
    *Flash_ptr++ = RunBslFlagString[i];     // Write string to flash
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
}
#endif

/******************************************************************************
 * ISR function for UART communication
 *****************************************************************************/
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  char rx_byte;
  switch(__even_in_range(UCA0IV,4))
  {
    case 0:break;                             // Vector 0 - no interrupt
    case 2:                                   // Vector 2 - RXIFG

      rx_byte = UCA0RXBUF;
      if(rx_byte == '\r')
      {
        __bic_SR_register_on_exit(LPM0_bits);
      }
      
      // save into rx buffer
      RxBuf[RxLen++] = rx_byte;
      
      // echo
      send_byte(rx_byte);
      
      break;
    case 4:break;                             // Vector 4 - TXIFG
    default: break;
  }
}

