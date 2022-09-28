# include "LIS3DSH.H"
# include "main.h"

#define ADD_REG_WHO_AM_I				0x0F
#define ADD_REG_CTRL_4					 0x20
#define ADD_REG_OUT_X_L					0x28
#define ADD_REG_OUT_X_H					0x29
#define ADD_REG_OUT_Y_L					0x2A
#define ADD_REG_OUT_Y_H					0x2B
#define ADD_REG_OUT_Z_L					0x2C
#define ADD_REG_OUT_Z_H					0x2D

/* WHO AM I register default value */
#define UC_WHO_AM_I_DEFAULT_VALUE		0x3F

/* ADD_REG_CTRL_4 register configuration value: X,Y,Z axis enabled and 400Hz of output data rate */
#define UC_ADD_REG_CTRL_4_CFG_VALUE		0x77

/* Sensitivity for 2G range [mg/digit] */
#define SENS_2G_RANGE_MG_PER_DIGIT		((float)0.06)

/* LED threshold value in mg */
#define LED_TH_MG						(1000)	/* 1000mg (1G) */


/* ---------------- Local Macros ----------------- */

/* set read single command. Attention: command must be 0x3F at most */
#define SET_READ_SINGLE_CMD(x)			(x | 0x80)
/* set read multiple command. Attention: command must be 0x3F at most */
#define SET_READ_MULTI_CMD(x)			(x | 0xC0)
/* set write single command. Attention: command must be 0x3F at most */
#define SET_WRITE_SINGLE_CMD(x)			(x & (~(0xC0)))
/* set write multiple command. Attention: command must be 0x3F at most */
#define SET_WRITE_MULTI_CMD(x)			(x & (~(0x80))	x |= 0x40)
  

#define LIS3DSH_FLAG_TIMEOUT  ((uint32_t)0x1000)
__IO uint32_t  LIS3DSHTimeout = LIS3DSH_FLAG_TIMEOUT;

static void LIS3DSH_SetCS(unsigned char Enable)
{
 HAL_GPIO_WritePin(LIS3DSH_SPI_CS_GPIO_Port, LIS3DSH_SPI_CS_Pin,(GPIO_PinState)Enable);
}

/* Function to read a register from LIS3DSH through SPI */
static int LIS3DSH_read_reg(int reg)
{
	unsigned char InData[2]={0xFF};
 unsigned char OutData[2]={0xFF};

 OutData[0]=SET_READ_SINGLE_CMD(reg);
 OutData[1]=0xFF;
 
	/* set CS low */
	LIS3DSH_SetCS(0);
 HAL_SPI_TransmitReceive(&hspi1,OutData,InData,2,HAL_MAX_DELAY);
	/* set CS high */
	LIS3DSH_SetCS(1);
	return InData[1];
}

/* Function to write a register to LIS3DSH through SPI  */
static void LIS3DSH_write_reg(int reg, int data)
{
 unsigned char InData[2]={0xFF};
 unsigned char OutData[2]={0xFF};

 OutData[0]=SET_WRITE_SINGLE_CMD(reg);
 OutData[1]=data;
 
	/* set CS low */
	LIS3DSH_SetCS(0);
 HAL_SPI_TransmitReceive(&hspi1,OutData,InData,2,HAL_MAX_DELAY);
	/* set CS high */
	LIS3DSH_SetCS(1);
}

void LIS3DSH_Init(void)
{
	int intRegValue;

	/* init SPI1 */
 LIS3DSH_SetCS(1);
	/* get WHO AM I value */
	intRegValue = LIS3DSH_read_reg(ADD_REG_WHO_AM_I);

	/* if WHO AM I value is the expected one */
	if( intRegValue == UC_WHO_AM_I_DEFAULT_VALUE )
	{
		/* set output data rate to 400 Hz and enable X,Y,Z axis */
		LIS3DSH_write_reg(ADD_REG_CTRL_4, UC_ADD_REG_CTRL_4_CFG_VALUE);
		/* verify written value */
		intRegValue = LIS3DSH_read_reg(ADD_REG_CTRL_4);
		/* if written value is different */
		if( intRegValue != UC_ADD_REG_CTRL_4_CFG_VALUE )
		{
			/* ERROR: stay here... */
			while(1);
		}
	}
	else
	{
		/* ERROR: stay here... */
		while(1);
	}
}

static inline int twoComplToInt16(int twoComplValue)
{
	int int16Value = 0;

	/* conversion */
	if(twoComplValue > 32768)
	{
		int16Value = -(((~twoComplValue) & 0xFFFF) + 1);
	}
	else
	{
		int16Value = twoComplValue;
	}

	return int16Value;
}

int LIS3DSH_ReadX(void)
{
 int Result=0;
 Result = ((LIS3DSH_read_reg(ADD_REG_OUT_X_H) << 8) | LIS3DSH_read_reg(ADD_REG_OUT_X_L));
 
 /* transform X value from two's complement to 16-bit int */
	Result = twoComplToInt16(Result);
		/* convert X absolute value to mg value */
	Result = Result * SENS_2G_RANGE_MG_PER_DIGIT;
 return Result;
}
int LIS3DSH_ReadY(void)
{
 int Result=0;
 Result = ((LIS3DSH_read_reg(ADD_REG_OUT_Y_H) << 8) | LIS3DSH_read_reg(ADD_REG_OUT_Y_L));
 
 /* transform X value from two's complement to 16-bit int */
	Result = twoComplToInt16(Result);
		/* convert X absolute value to mg value */
	Result = Result * SENS_2G_RANGE_MG_PER_DIGIT;
 return Result;
}












