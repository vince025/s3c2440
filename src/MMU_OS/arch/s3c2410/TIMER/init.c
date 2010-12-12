#include "s3c2410.h"

/* SDRAM 13个寄存器的值 */ 
unsigned long  const	mem_cfg_val[]={	0x22111110,		//BWSCON
					0x00000700,		//BANKCON0
					0x00000700,		//BANKCON1
					0x00000700,		//BANKCON2
					0x00000700,		//BANKCON3	
					0x00000700,		//BANKCON4
					0x00000700,		//BANKCON5
					0x00018005,		//BANKCON6
					0x00018005,		//BANKCON7
					0x008e07a3,		//REFRESH
					0x000000b2,		//BANKSIZE
					0x00000030,		//MRSRB6
					0x00000030,		//MRSRB7
					 };


 
void disable_watch_dog();
void memsetup();
void reset_nand();
void init_nand();
inline void wait_idle(void);
void nand_read_ll(unsigned char *buf, unsigned long start_addr, int size);
void init_uart();
void Timer0_init();


/*上电后，WATCH DOG默认是开着的，要把它关掉 */
void disable_watch_dog()
{
	WTCON	= 0;
}

/* 设置控制SDRAM的13个寄存器 */
void memsetup()
{
	int 	i = 0;
	unsigned long *p = (unsigned long *)MEM_CTL_BASE;
	for(; i < 13; i++)
		p[i] = mem_cfg_val[i];
}

/* 在第一次实用NAND Flash前，复位一下NAND Flash */
void reset_nand()
{
	int i=0;
	NFCONF &= ~0x800;
    	for(; i<10; i++);
	NFCMD = 0xff;	//reset command
	wait_idle();
}

/* 初始化NAND Flash */
void init_nand()
{
	NFCONF = 0xf830;
	reset_nand();
}


/*************************************************************************
*	以下读NAND Flash的代码来自mizi公司的bootloader vivi
*************************************************************************/
#define BUSY 1
inline void wait_idle(void) {
    int i;

    while(!(NFSTAT & BUSY))
      for(i=0; i<10; i++);
}

#define NAND_SECTOR_SIZE	512
#define NAND_BLOCK_MASK		(NAND_SECTOR_SIZE - 1)

/* low level nand read function */
void nand_read_ll(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;

    if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) {
        return ;	/* invalid alignment */
    }

    /* chip Enable */
    NFCONF &= ~0x800;
    for(i=0; i<10; i++);

    for(i=start_addr; i < (start_addr + size);) {
      /* READ0 */
      NFCMD = 0;

      /* Write Address */
      NFADDR = i & 0xff;
      NFADDR = (i >> 9) & 0xff;
      NFADDR = (i >> 17) & 0xff;
      NFADDR = (i >> 25) & 0xff;

      wait_idle();

      for(j=0; j < NAND_SECTOR_SIZE; j++, i++) {
	*buf = (NFDATA & 0xff);
	buf++;
      }
    }

    /* chip Disable */
    NFCONF |= 0x800;	/* chip disable */

    return ;
}


#define EINT1		(2<<(1*2))
#define EINT2		(2<<(2*2))
#define EINT3		(2<<(3*2))
#define EINT7		(2<<(7*2))

void init_irq( )
{
	INTMSK &= (~(1<<10));	//INT_TIMER0使能
}


/*************************************************************************
* Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
* {prescaler value} = 0~255
* {divider value} = 2, 4, 8, 16
* 本实验的Timer0的时钟频率=12MHz/(119+1)/(16)=6250Hz
* 设置Timer0 0.5秒钟触发一次中断：
*************************************************************************/
void Timer0_init()
{
	TCFG0 = 119;	//Prescaler0 = 119	  	 
	TCFG1 = 0x03;	//Select MUX input for PWM Timer0:divider=16
	TCNTB0 = 3125;	//0.5秒钟触发一次中断
	TCON |=  (1<<1);	//Timer 0 manual update
	TCON = 0x09;	/*Timer 0 auto reload on
			  Timer 0 output inverter off
			  清"Timer 0 manual update"
			  Timer 0 start */
}


