#include "LSM330DLC.h"
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "driverlib\gpio.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\sysctl.h"

uint8_t LSM330DLC_RdRegGyro(uint8_t address)
{
	uint8_t ret;
	LSM330DLC_CS_G_lo;
	LSM330DLC_transferB(address | LSM330DLC_READ_EN);
	ret = LSM330DLC_transferB(0);
	LSM330DLC_CS_G_hi;
	return(ret);
}

uint8_t LSM330DLC_RdRegAccel(uint8_t address)
{
	uint8_t ret;
	LSM330DLC_CS_A_lo;
	LSM330DLC_transferB(address | LSM330DLC_READ_EN);
	ret = LSM330DLC_transferB(0);
	LSM330DLC_CS_A_hi;
	return(ret);
}

void LSM330DLC_WrRegAccel(uint8_t address, uint8_t data)
{
	LSM330DLC_CS_A_lo;
	LSM330DLC_transferB(address);
	LSM330DLC_transferB(data);
	LSM330DLC_CS_A_hi;
}

void LSM330DLC_WrRegGyro(uint8_t address, uint8_t data)
{
	LSM330DLC_CS_G_lo;
	LSM330DLC_transferB(address);
	LSM330DLC_transferB(data);
	LSM330DLC_CS_G_hi;
}

void LSM330DLC_AccelRdData(LSM330DLC_AccelData_t* data)
{
	LSM330DLC_CS_A_lo;
	LSM330DLC_transferB(LSM330DLC_OUT_X_L_A	 | LSM330DLC_READ_EN | LSM330DLC_IncrementAddr);
	data->X = LSM330DLC_transferB(0); //issiklokinam Xlow
	data->X |= LSM330DLC_transferB(0) << 8; //issiklokinam Xhigh
	data->Y = LSM330DLC_transferB(0); //issiklokinam Ylow
	data->Y |= LSM330DLC_transferB(0) << 8; //issiklokinam Yhigh
	data->Z = LSM330DLC_transferB(0); //issiklokinam Zlow
	data->Z |= LSM330DLC_transferB(0) << 8; //issiklokinam Zhigh
	LSM330DLC_CS_A_hi;
	
	data->X /= 16;
	data->Y /= 16;
	data->Z /= 16;
	
}

void LSM330DLC_GyroRdData(LSM330DLC_GyroData_t* data)
{
	LSM330DLC_CS_G_lo;
	LSM330DLC_transferB(LSM330DLC_OUT_X_L_G	 | LSM330DLC_READ_EN | LSM330DLC_IncrementAddr);
	data->Roll = LSM330DLC_transferB(0); //issiklokinam Rolllow
	data->Roll |= LSM330DLC_transferB(0) << 8; //issiklokinam Rollhigh
	data->Pitch = LSM330DLC_transferB(0); //issiklokinam Pitchlow
	data->Pitch |= LSM330DLC_transferB(0) << 8; //issiklokinam Pitchhigh
	data->Yaw = LSM330DLC_transferB(0); //issiklokinam Yawlow
	data->Yaw |= LSM330DLC_transferB(0) << 8; //issiklokinam Yawhigh
	LSM330DLC_CS_G_hi;
}

void LSM330DLC_init(void)
{
	//uint8_t temp;
	
	LSM330DLC_hw_init();
	
	// registru inicializacija
	LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG1_A,0x07);
	LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG2_A,0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG3_A,0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG4_A,0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG5_A,0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG6_A,0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_REFERENCE_A,0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_FIFO_CTRL_REG_A, 0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_INT1_CFG_A, 0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_INT1_THS_A, 0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_INT1_DURATION_A, 0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_INT2_CFG_A, 0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_INT2_THS_A, 0x00);
	LSM330DLC_WrRegAccel(LSM330DLC_INT2_DURATION_A, 0x00);
	
	LSM330DLC_WrRegGyro(LSM330DLC_CTRL_REG1_G,0x07);
	LSM330DLC_WrRegGyro(LSM330DLC_CTRL_REG2_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_CTRL_REG3_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_CTRL_REG4_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_CTRL_REG5_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_REFERENCE_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_FIFO_CTRL_REG_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_INT1_CFG_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_INT1_TSH_XH_G	,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_INT1_TSH_XL_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_INT1_TSH_YH_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_INT1_TSH_YL_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_INT1_TSH_ZH_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_INT1_TSH_ZL_G,0x00);
	LSM330DLC_WrRegGyro(LSM330DLC_INT1_DURATION_G,0x00);
	
	// uzkuriu akcelerometra +-2g, 50 Hz per FIFO
	LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG4_A, 0 | (1 << LSM330DLC_CTRL_REG4_A_HR_mode_pos));
	//LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG5_A, 1 << LSM330DLC_CTRL_REG5_A_FIFO_EN_pos);
	//LSM330DLC_WrRegAccel(LSM330DLC_FIFO_CTRL_REG_A, 0x80); //stream mode
	LSM330DLC_WrRegAccel(LSM330DLC_CTRL_REG1_A,0
													|LSM330DLC_CTRL_REG1_A_ODR_200Hz << LSM330DLC_CTRL_REG1_A_ODR_Pos
													|0 << LSM330DLC_CTRL_REG1_A_LPen_Pos
													|1 << LSM330DLC_CTRL_REG1_A_Zen_Pos
													|1 << LSM330DLC_CTRL_REG1_A_Yen_Pos
													|1 << LSM330DLC_CTRL_REG1_A_Xen_Pos);
													
	//LSM330DLC_WrRegGyro(LSM330DLC_CTRL_REG5_G, 1 << LSM330DLC_CTRL_REG5_G_FIFO_EN_pos);
	//LSM330DLC_WrRegGyro(LSM330DLC_FIFO_CTRL_REG_G,	0
	//																								|LSM330DLC_FIFO_CTRL_REG_G_FM_stream << LSM330DLC_FIFO_CTRL_REG_G_FM_pos); //stream mode
	LSM330DLC_WrRegGyro(LSM330DLC_CTRL_REG1_G,	0
																							|1 << LSM330DLC_CTRL_REG1_G_PD_pos
																							|1 << LSM330DLC_CTRL_REG1_G_Zen_pos
																							|1 << LSM330DLC_CTRL_REG1_G_Yen_pos
																							|1 << LSM330DLC_CTRL_REG1_G_Xen_pos
																							|LSM330DLC_CTRL_REG1_G_ODR95_BW25 << LSM330DLC_CTRL_REG1_G_BW_pos);
	
	//LSM330DLC_write_register(LSM330DLC_CTRL_REG1_A, 0x97);
	//LSM330DLC_write_register(LSM330DLC_CTRL_REG2_A, 0x00);
	//LSM330DLC_write_register(LSM330DLC_CTRL_REG3_A, 0x08);
	///LSM330DLC_write_register(LSM330DLC_CTRL_REG4_A, 0x08);
	//LSM330DLC_write_register(LSM330DLC_CTRL_REG5_A, 0x40);
}
