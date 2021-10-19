#include <xc.h>

#define dutyCycle1 OC1R
#define PERIOD1 OC1RS
#define dutyCycle2 OC2R
#define PERIOD2 OC2RS
#define mrDir _RB1
#define mlDir _RB9
#define mrON _RA0
#define mlON _RB8
#define CW 1
#define CCW 0

#pragma config FNOSC = LPRC

int steps1 = 0;
int steps2 = 0;
float wheelDiameter = 3.5;

void config_pwmR(float period, float dutyCycle, int direction); // Sets up PWM #1 with a specified period
void config_pwmL(float period, float dutyCycle, int direction); // Sets up PWM #2 with a specified period

//void updateMotorR(float dutyCycle, int direction);
//void updateMotorL(float dutyCylce, int direction);

void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {

    _OC1IF = 0; // Clear OC1 interrupt flag

    if (steps1 >= 0) {
        ++steps1;
            //After 4 Revolutions prepare for turn
    
            //Shut motor down
            if (steps1 == 400) {
                mlON = 1; //Shut motor off before changing directions
    
            }
    
            
            //Leaves motor off for 50 counts before executing turn
            if (steps1 == 450) {
                
                mlON = 0; //Turn motor on again
                
            }
    
            //Shut motor down
            if (steps1 == 720) {
    
                mlON = 1; // Shut motor off
            }
    
            //Leaves motor off for 50 counts before moving again
            if (steps1 == 770) {
                mlON = 0; //Turn motor on
    
            }
        
        if (steps1 == 1080) {
            steps1 = -1;
            mlON = 1;
        }
    
    }
    
}
void __attribute__((interrupt, no_auto_psv)) _OC2Interrupt(void)
{
    _OC2IF = 0; // Clear OC2 interrupt flag

    if (steps1 >= 0) {

        //After 2 Revolutions prepare for turn

        //Shut motor down
        if (steps1 == 400) {
            mrON = 1; //Shut motor off before changing directions

        }

        //Changes direction of motor to execute turn
        if (steps1 == 450) {
            mrDir = 0; //Change direction 
            mrON = 0; //Turn motor on again
        }

        //When motor executes full turn change direction again to continue moving forward;

        //Shut motor down
        if (steps1 == 720) {

            mrON = 1; // Shut motor off
        }

        //Change direction of motor to drive straight
        if (steps1 == 770) {
            mrDir = 1; //Change direction 
            mrON = 0; //Turn motor on

        }


    }
    else {
        
        mrON = 1;
    }

    //Arc length is S = r * Theta where theta is in radians
    // S = 5*pi/2 ~ 7.86 inches
    // Number of revolutions of wheel: R = S/(pi*D) ~ .7148 revolutions == 286 steps
    
}


int main()
{

//    _RCDIV = 0b101;
    
    TRISA = 0x0000; //Sets all outputs
    TRISB = 0x0000; //Sets all outputs except B1
    ANSB = 0x0000; //Sets all digital

    config_pwmR(85, .5 , CW);
    config_pwmL(85, .5, CCW);


    while (1);

    return 0;
}

void config_pwmR(float period, float dutyCycle, int direction)
{

    PERIOD2 = period;
    dutyCycle2 = (int)(PERIOD2*dutyCycle);
    mrDir = direction;
    mrON = 0;

    OC2CON1 = 0;

    // Configure OC1
    OC2CON1bits.OCTSEL = 0b111; // System (peripheral) clock as timing source
    OC2CON2bits.SYNCSEL = 0x1F; // Select OC1 as synchronization source
    OC2CON2bits.OCTRIG = 0; // Synchronizes with OC1 source instead of triggering with the OC1 source
    OC2CON1bits.OCM = 0b110; // Edge-aligned PWM mode
    
    _OC2IP = 4; // Select OCx interrupt priority
    _OC2IE = 1; // Enable OCx interrupt
    _OC2IF = 0; // Clear OCx interrupt flag

}

void config_pwmL(float period, float dutyCycle, int direction)
{

    PERIOD1 = period;
    dutyCycle1 = (int)(PERIOD1*dutyCycle);
    mlDir = direction;
    
    OC1CON1 = 0;

    // Configure OC1
    OC1CON1bits.OCTSEL = 0b111; // System (peripheral) clock as timing source
    OC1CON2bits.SYNCSEL = 0x1F; // Select OC1 as synchronization source
    OC1CON2bits.OCTRIG = 0; // Synchronizes with OC1 source instead of triggering with the OC1 source
    OC1CON1bits.OCM = 0b110; // Edge-aligned PWM mode
    
    _OC1IP = 4; // Select OCx interrupt priority
    _OC1IE = 1; // Enable OCx interrupt
    _OC1IF = 0; // Clear OCx interrupt flag

}
 /*
 PERIOD CALCULATION:
 
 * Time per pulse = 1 / Frequency
 * PERIOD = Time per pulse / [clock period * PRESCALE] - 1
 
 
 */