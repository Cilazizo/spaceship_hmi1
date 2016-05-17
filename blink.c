//***************************************************************************************
//  MSP430 Police Car LED Demo
//
//
//
//  Z. Zilahi
//  Texas Instruments, Inc
//  November 2015
//  Built with Code Composer Studio v6
//***************************************************************************************

#include <msp430.h>

#define FALSE 0
#define TRUE 1

//define button for editing
//port 1
#define BUTTON_1 BIT6  //P1.6

volatile unsigned char  delay_in_progress = FALSE;

typedef volatile enum  {
			  BUTTON_0, BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D } BState;
BState bState = BUTTON_0;

typedef enum  {SLEEPING,
			  POLICE_LIGHTS, LIGHTS, ALL_BLINK, ROUND, FRONT_BACK } TState;
TState state = SLEEPING;


void usdelay(unsigned int interval){
	delay_in_progress = TRUE;   // Indicate to other ISRs that they should not wake the CPU when triggered as the
                                // Setup TimerA
    TACCTL0 = CCIE;            // interrupt enabled
    TACCR0 = TAR + (interval);  // micro secs @ 8Mhz Clock - set to 8, 16MHz clock - set to 16
    TACTL = TASSEL_1 + MC_2;   // SMCLK, continuous mode MC_2 volt elotte.

                                // CPU is currently sleeping waiting for a delay to expire
    LPM3;                       // suspend CPU
}

static void config_button1_input()
{
	//config port 1 as input mode
	P1SEL &= ~BUTTON_1; // disable timera0 out function
	P1DIR &= ~BUTTON_1;// input
	P1OUT &= ~BUTTON_1;// select pull-up mode
	//P2REN |= BUTTON_1;  //todo not worked
	P1IES &= ~BUTTON_1;// select the positive edge (low -> high transition) to cause an interrupt
	P1IFG &= ~BUTTON_1;// clear any pending interrupt flags
	P1IE |= BUTTON_1;// enable button interrupt
}


int main(void) {

	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
	P1DIR = 0xFF;                         // Minden P1.x legyen kimenet
	P1OUT = 0;                            // Minden P1.x alacsony állapotba
	P2SEL = 0;                            // P2.x legyen digitális I/O
	P2DIR = 0xFF;                         // Minden P2.x legyen kimenet
	P2OUT = 0;                            // Minden P2.x alacsony állapotba

    BCSCTL3 |= LFXT1S_2;                  // ACLK forrása: LFXT1 helyett VLO
	IFG1 &= ~OFIFG;                       // "Oszcillátor hiba" bit törlése
	__bis_SR_register(SCG0);              // DCO kikapcsolása
	BCSCTL1 |= DIVA_3;                    // ACLK osztója: 8
	BCSCTL2 = SELM_3 +                    // MCLK forrása: LFXT1/VLO
			DIVM_3 +                    // MLCK osztója: 8
			SELS   +                    // SCLK forrása: LFXT1/VLO
			DIVS_3;                     // SMCLK osztója: 8

	int i =0;

	//config push buttons
	config_button1_input();
	__enable_interrupt();

	LPM4;

	while(1) {

		switch(state){
		case SLEEPING:
			LPM4;
			break;
		case POLICE_LIGHTS:
			for(i=3;i>0;i--){
				P1OUT |= BIT0;                      // P1.0 (LED1) bekapcsolása
				usdelay(200);                // kb. 100 ms várakozás
				P1OUT |= BIT1;
				usdelay(200);
				P1OUT |= BIT2;
				usdelay(200);               // P1.0 (LED1) lekapcsolása
				P1OUT |= BIT3;                      // P1.1 (LED2) bekapcsolása
				usdelay(180);                // kb. 100 ms várakozás
				P1OUT |= BIT4;
				usdelay(140);               // P1.1 (LED2) lekapcsolása
				P1OUT |= BIT5;                      // P1.0 (LED1) bekapcsolása
				usdelay(100);                // kb. 100 ms várakozás
				P1OUT |= BIT7;
				usdelay(80);               // P1.0 (LED1) lekapcsolása
				P2OUT |= BIT6;                      // P1.1 (LED2) bekapcsolása
				usdelay(50);                // kb. 100 ms várakozás
				P2OUT |= BIT7;                     // P1.1 (LED2) lekapcsolása
				usdelay(50);
				P1OUT = 0;
				P2OUT = 0;
			}


			P1OUT |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT7;
			P2OUT |= BIT6;
			usdelay(5000);
			P2OUT = 0;
			usdelay(5000);
			P1OUT &= ~BIT7;
			usdelay(5000);
			P1OUT &= ~BIT5;
			usdelay(5000);
			P1OUT &= ~BIT4;
			usdelay(5000);
			P1OUT &= ~BIT3;
			usdelay(5000);
			P1OUT &= ~BIT2;
			usdelay(5000);
			P1OUT &= ~BIT1;
			usdelay(5000);
			P1OUT = 0;

			for(i=5;i>0;i--){
				P1OUT |= BIT0+BIT1+BIT2;
				usdelay(1000);
		        P1OUT = 0;
		        usdelay(1000);
			}

			state = SLEEPING;
			P1OUT = 0;
			i=0;
			break;
		case LIGHTS:
			P1OUT |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5;                      // P1.0 (LED1) bekapcsolása
			usdelay(10000);                // kb. 100 ms várakozás
			P1OUT = 0;
			state = SLEEPING;

			break;
		case ALL_BLINK:
			for(i=6;i>0;i--){
				P1OUT |= BIT2+BIT3+BIT4+BIT5;                      // P1.0 (LED1) bekapcsolása
				usdelay(1000);                // kb. 100 ms várakozás
				P1OUT = 0;                     // P1.0 (LED1) lekapcsolása
				P1OUT |=BIT0;
				usdelay(1000);                // kb. 100 ms várakozás
				P1OUT &=~BIT0;
				P1OUT|=BIT1;
				usdelay(1000);
				P1OUT &=~BIT1;
			}
			state = SLEEPING;
			P1OUT = 0;
			i=0;
			break;
		case ROUND:
			for(i=4;i>0;i--){
				P1OUT =BIT2;                      // P1.0 (LED1) bekapcsolása
				usdelay(1000);                // kb. 100 ms várakozás
				P1OUT =BIT3;
				usdelay(1000);                // kb. 100 ms várakozás
				P1OUT =BIT4;
				usdelay(1000);
				P1OUT =BIT5;
				usdelay(1000);
				P1OUT =BIT0;                      // P1.0 (LED1) bekapcsolása
				usdelay(1000);                // kb. 100 ms várakozás
				P1OUT =BIT1;
				usdelay(1000);
			}
			state = SLEEPING;
			P1OUT = 0;
			i=0;
			break;
		case FRONT_BACK:
			for(i=8;i>0;i--){
				P1OUT =BIT2+BIT3;                      // P1.0 (LED1) bekapcsolása
				usdelay(1000);                // kb. 100 ms várakozás
				P1OUT =BIT4+BIT5;
				usdelay(1000);                // kb. 100 ms várakozás
			}
			state = SLEEPING;
			P1OUT = 0;
			i=0;
			break;
		}
        /*
        P1OUT |= BIT0 + BIT2;                      // P1.0 (LED1) bekapcsolása
		usdelay(50);                // kb. 100 ms várakozás
		P1OUT &= ~BIT0 + BIT2;                     // P1.0 (LED1) lekapcsolása
		P1OUT |= BIT1 + BIT3;                      // P1.1 (LED2) bekapcsolása
		usdelay(50);                // kb. 100 ms várakozás
		P1OUT &= ~BIT1 + BIT3;                     // P1.1 (LED2) lekapcsolása
		*/

        /*P1OUT |= BIT0 + BIT2 + BIT3 + BIT4 + BIT5;                      // P1.0 (LED1) bekapcsolása
		usdelay(500);                // kb. 100 ms várakozás
		P1OUT &= ~BIT0;                     // P1.0 (LED1) lekapcsolása
		P1OUT |= BIT1;                      // P1.1 (LED2) bekapcsolása
		usdelay(500);                // kb. 100 ms várakozás
		P1OUT &= ~BIT1;*/                     // P1.1 (LED2) lekapcsolása
	}
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
	if(!delay_in_progress){
		if(bState == BUTTON_0 && state == SLEEPING) {
			state = POLICE_LIGHTS;
			bState = BUTTON_A;
			LPM4_EXIT;

		}
		else if(bState == BUTTON_A && state == SLEEPING) {
			state = LIGHTS;
			bState = BUTTON_B;
			LPM4_EXIT;
		}
		else if(bState == BUTTON_B && state == SLEEPING) {
			state = ALL_BLINK;
			bState = BUTTON_C;
			LPM4_EXIT;
		}
		else if(bState == BUTTON_C && state == SLEEPING) {
			state = ROUND;
			bState = BUTTON_D;
			LPM4_EXIT;
		}
		else if(bState == BUTTON_D && state == SLEEPING) {
			state = FRONT_BACK;
			bState = BUTTON_0;
			LPM4_EXIT;
		}
		else if(state == POLICE_LIGHTS || state == LIGHTS)
		{
			//do nothing
		}
	}

	P1IFG &= ~BUTTON_1; // clear the interrupt flag
	P1IE |= BUTTON_1;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
{
	TACTL = 0;                 // Stop Timer_A1
    delay_in_progress = FALSE;  // Indicate to other ISRs that they can wake the CPU now if needed
    LPM3_EXIT;                  // Return active //__bic_SR_register_on_exit(LPM0_bits|GIE);
}
