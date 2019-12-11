/*
 * File:   main.c
 * Author: tpeea
 *
 * Created on 16 octobre 2019, 16:59
 */

// DSPIC30F6014 Configuration Bit Settings

// 'C' source line config statements

// FOSC
#pragma config FPR = XT_PLL16           // Primary Oscillator Mode (XT w/PLL 16x)
#pragma config FOS = PRI                // Oscillator Source (Primary Oscillator)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF             // Watchdog Timer (Enabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = NONE            // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <xc.h>

#define HOMECLEAR   0x82
#define WRITECHAR   0xA8
#define CURSBLINK   0xAD
#define GOTO        0xC5




void __attribute__((interrupt,auto_psv)) _INT1Interrupt(void);
void __attribute__((interrupt,auto_psv)) _INT2Interrupt(void);
void __attribute__((interrupt,auto_psv)) _INT3Interrupt(void);
void __attribute__((interrupt,auto_psv)) _INT4Interrupt(void);

void InitBoutonLed(void);
void LCDinit(void);
void _LCDwritecmd(char);
void _LCDwritechar(char c);
void _LCDbitmap(int);
void LCDreset(void);
void LCDhome(void);
void LCDhomeclear(void);
void LCDgoto(int, int);
void _LCDwritestr(char []);

void DCIinit(void);

void wait(unsigned int);

int volume  = 23;
int chang_volume = 0;
int pseudo_sin[]={0x0000,0x278D,0x4B3B,0x678D,0x79BB,0x7FFF,0x79BB,0x678D,0x4B3B,0x278D,0x0000,0xD873,0xB4C5,0x9873,0x8645,0x8001,0x8645,0x9873,0xB4C5,0xD873};

void main(void) 
{
    InitBoutonLed();
    LCDinit();
    DCIinit();
    LCDhomeclear();
    LCDgoto(0,0);
    char pass[3] = {0x30,0x30,0x00};
    pass[0]+=volume/10;
    pass[1]+=volume%10;
    _LCDwritestr("VOLUME: ");
    _LCDwritestr(pass);
    LCDgoto(0,2);
    _LCDwritestr("SON: ");
    _LCDwritestr("OFF");
    while(1)
    {
        
    }
}

void InitBoutonLed(void)
{
    TRISDbits.TRISD0=0; // configuration LED1 en sortie
    TRISDbits.TRISD1=0;
    TRISDbits.TRISD2=0;
    TRISDbits.TRISD3=0;

    
    // initialisation INT1 sur SW1
    INTCON2bits.INT1EP=1; // interruption sur front descendant.
    IPC4bits.INT1IP=1; // priorit�e 1
    IFS1bits.INT1IF=0; // raz FLAG
    IEC1bits.INT1IE=1; // autorisation interruption INT1
    
    // initialisation INT2 sur SW2
    INTCON2bits.INT2EP=1; // interruption sur front descendant.
    _INT2IP=1; // priorit�e 1
    _INT2IF=0; // raz FLAG
    _INT2IE=1; // autorisation interruption INT2
    
    // initialisation INT3 sur SW3
    INTCON2bits.INT3EP=1; // interruption sur front descendant.
    _INT3IP=1; // priorit�e 1
    _INT3IF=0; // raz FLAG
    _INT3IE=1; // autorisation interruption INT3
    
    // initialisation INT1 sur SW1
    INTCON2bits.INT4EP=1; // interruption sur front descendant.
    _INT4IP=1; // priorit�e 1
    _INT4IF=0; // raz FLAG
    _INT4IE=1; // autorisation interruption INT4
}
void __attribute__((interrupt,auto_psv)) _INT1Interrupt(void)
{
    chang_volume = 1;
    if(volume>0)volume--;
    char pass[3] = {0x30,0x30,0x00};
    pass[0]+=volume/10;
    pass[1]+=volume%10;
    
    PORTDbits.RD0=~PORTDbits.RD0;
    
    LCDgoto(0,0);
    _LCDwritestr("VOLUME: ");
    _LCDwritestr(pass);
    
    IFS1bits.INT1IF=0; // acquittement FLAG
}
void __attribute__((interrupt,auto_psv)) _INT2Interrupt(void)
{
    chang_volume = 1;
    if(volume<31)volume++;
    char pass[3] = {0x30,0x30,0x00};
    pass[0]+=volume/10;
    pass[1]+=volume%10;
    
    PORTDbits.RD1=~PORTDbits.RD1;
    
    LCDgoto(0,0);
    _LCDwritestr("VOLUME: ");
    _LCDwritestr(pass);
    
    
    IFS1bits.INT2IF=0; // acquittement FLAG
}
void __attribute__((interrupt,auto_psv)) _INT3Interrupt(void)
{
    LCDgoto(0,2);
    _LCDwritestr("SON: ");
    _LCDwritestr("OFF");
    PORTDbits.RD2=~PORTDbits.RD2;
    _INT3IF=0; // acquittement FLAG
}
void __attribute__((interrupt,auto_psv)) _INT4Interrupt(void)
{
    LCDgoto(0,2);
    _LCDwritestr("SON: ");
    _LCDwritestr("ON  ");
    PORTDbits.RD3=~PORTDbits.RD3;
    _INT4IF=0; // acquittement FLAG
}

void __attribute__((interrupt,auto_psv)) _DCIInterrupt(void)
{
    static int s=0; //variable pour defiler dans le sinus
    if(s>19)s=0;
    int datcom=0; //choix entre envoie de commande et envoie de donn�es;
    int val = pseudo_sin[s]&(~0x0001); //on met 0 sur le dernier bit car si on met � 1, on demande une commande
    if(chang_volume)
    {
        val = val|1; //on remet 1 comme on va envoyer une commande de changement de valeur
        datcom = 0x0700|((0x00ff&volume)*4); //*4 pour decaler de 2bit car slm et srm tjrs = 0; registre 7 pour le reglage du gain
        chang_volume=0;
    }
    TXBUF0=val;
    TXBUF1=datcom;
    s++;
    IFS2bits.DCIIF=0; //remise  � zero du flag
}

void LCDinit()
{
    _LATG9=1; //slave select 
    _TRISG9=0;
    SPI2CONbits.PPRE = 1;
    SPI2CONbits.SPRE = 7;
    SPI2CONbits.CKP = 0;
    SPI2CONbits.CKE = 0;
    SPI2CONbits.MODE16 = 0;
    SPI2CONbits.MSTEN = 1;
    _LATG9=0; //slave select 
    SPI2STATbits.SPIEN = 1;
}

void wait(unsigned int time10mus)
{
    unsigned int i;
    for(i=0;i<time10mus;i++)
    {
        asm("REPEAT #295");
        asm("NOP");
    }
}

void DCIinit(void)
{
    TRISFbits.TRISF6=1;
    
    DCICON2bits.COFSG=0xF;
    DCICON2bits.WS=0xF;
    
    DCICON3=6;
    
    DCICON1=0x0000;
    
    TSCON=0x101;
    RSCON=0x101;
    
    DCICON2bits.BLEN=0x1;
    
    TXBUF0=0x0001;
    TXBUF1=0x0300;
    
    PORTFbits.RF6=1;
    TRISFbits.TRISF6=0;
    PORTFbits.RF6=0;
    wait(1);
    PORTFbits.RF6=1;
    
    DCICON1bits.DCIEN=1;
    
    while(DCISTATbits.TMPTY==0);
    
    //----------� compl�ter
    
    
    
    //registre4
    TXBUF0=0x0001;
    TXBUF1=0x0413;
    while(DCISTATbits.TMPTY==0);
    
    //registre1
    TXBUF0=0x0001;
    TXBUF1=0x010A;
    while(DCISTATbits.TMPTY==0);
   
    //registre5
    TXBUF0=0x0001;
    TXBUF1=0x0547;
    while(DCISTATbits.TMPTY==0);
    //registre6
    TXBUF0=0x0001;
    TXBUF1=0x065E;
    while(DCISTATbits.TMPTY==0);
    //registre7
    TXBUF0=0x0001;
    TXBUF1=0x075C;
    while(DCISTATbits.TMPTY==0);
    //registre9
    TXBUF0=0x0001;
    TXBUF1=0x0900;
    while(DCISTATbits.TMPTY==0);
    
    wait(1000); //attente de 100ms
    
    IFS2bits.DCIIF=0; 
    IEC2bits.DCIIE=1;  //autorisation de l'int�ruption   
}


void _LCDwritecmd(char c)
{
   SPI2BUF = c;
   while(SPI2STATbits.SPITBF);
}

void _LCDwritechar(char c)
{
    _LCDwritecmd(WRITECHAR);
    _LCDwritecmd(c);
}

void _LCDwritestr(char ch[])
{
    int i=0;
    while(ch[i]!=0x00)
    {
        _LCDwritechar(ch[i]);
        i++;  
    }
}
void _LCDbitmap(int n)
{
    
}

void LCDreset(void)
{
    
}

void LCDhome(void)
{
    LCDgoto(0,0);
}

void LCDhomeclear(void)
{
    _LCDwritecmd(HOMECLEAR);
}

void LCDgoto(int c, int l)
{
    _LCDwritecmd(GOTO);
    _LCDwritecmd(c);
    _LCDwritecmd(l);   
}

