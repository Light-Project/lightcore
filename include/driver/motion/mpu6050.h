#ifndef _DRIVER_MOTION_H_
#define _DRIVER_MOTION_H_

enum MPU6050_Register{
    MPU6050_ACCEL_OFFS_REG          = 0X06,	//accel_offs寄存器,可读取版本号,寄存器手册未提到
    MPU6050_PROD_ID_REG             = 0X0C,	//prod id寄存器,在寄存器手册未提到
    MPU6050_SELF_TESTX_REG          = 0X0D,	//自检寄存器X
    MPU6050_SELF_TESTY_REG          = 0X0E,	//自检寄存器Y
    MPU6050_SELF_TESTZ_REG          = 0X0F,	//自检寄存器Z
    MPU6050_SELF_TESTA_REG          = 0X10,	//自检寄存器A
    MPU6050_SAMPLE_RATE_REG         = 0X19,	//采样频率分频器 
    MPU6050_CFG_REG                 = 0X1A,	//配置寄存器
    MPU6050_GYRO_CFG_REG            = 0X1B,
    MPU6050_ACCEL_CFG_REG           = 0X1C,
    MPU6050_RA_FF_THR               = 0x1D, 
    MPU6050_RA_FF_DUR               = 0x1E,
    MPU6050_MOTION_DET_REG          = 0X1F,
    MPU6050_RA_ZRMOT_THR            = 0x21,
    MPU6050_RA_ZRMOT_DUR            = 0x22,
    MPU6050_FIFO_EN_REG             = 0X23,
    MPU6050_I2CMST_CTRL_REG         = 0X24,

    MPU6050_I2CSLV0_ADDR_REG        = 0X25,	//IIC从机0器件地址寄存器
    MPU6050_I2CSLV0_REG             = 0X26,	//IIC从机0数据地址寄存器
    MPU6050_I2CSLV0_CTRL_REG        = 0X27,	//IIC从机0控制寄存器
    MPU6050_I2CSLV1_ADDR_REG        = 0X28,	//IIC从机1器件地址寄存器
    MPU6050_I2CSLV1_REG			    = 0X29,	//IIC从机1数据地址寄存器
    MPU6050_I2CSLV1_CTRL_REG        = 0X2A,	//IIC从机1控制寄存器
    MPU6050_I2CSLV2_ADDR_REG        = 0X2B,	//IIC从机2器件地址寄存器
    MPU6050_I2CSLV2_REG			    = 0X2C,	//IIC从机2数据地址寄存器
    MPU6050_I2CSLV2_CTRL_REG        = 0X2D,	//IIC从机2控制寄存器
    MPU6050_I2CSLV3_ADDR_REG        = 0X2E,	//IIC从机3器件地址寄存器
    MPU6050_I2CSLV3_REG			    = 0X2F,	//IIC从机3数据地址寄存器
    MPU6050_I2CSLV3_CTRL_REG        = 0X30,	//IIC从机3控制寄存器
    MPU6050_I2CSLV4_ADDR_REG        = 0X31,	//IIC从机4器件地址寄存器
    MPU6050_I2CSLV4_REG			    = 0X32,	//IIC从机4数据地址寄存器
    MPU6050_I2CSLV4_DO_REG          = 0X33,	//IIC从机4写数据寄存器
    MPU6050_I2CSLV4_CTRL_REG        = 0X34,	//IIC从机4控制寄存器
    MPU6050_I2CSLV4_DI_REG          = 0X35,	//IIC从机4读数据寄存器

    MPU6050_I2CMST_STA_REG		    = 0X36,	//IIC主机状态寄存器
    MPU6050_INTBP_CFG_REG           = 0X37,	//中断/旁路设置寄存器
    MPU6050_INT_EN_REG			    = 0X38,	//中断使能寄存器

    MPU6050_INT_STA_REG				= 0X3A,	//中断状态寄存器

    MPU6050_ACCEL_XOUTH_REG         = 0X3B, //加速度值,X轴高8位寄存器
    MPU6050_ACCEL_XOUTL_REG         = 0X3C, //加速度值,X轴低8位寄存器
    MPU6050_ACCEL_YOUTH_REG         = 0X3D, //加速度值,Y轴高8位寄存器
    MPU6050_ACCEL_YOUTL_REG         = 0X3E, //加速度值,Y轴低8位寄存器
    MPU6050_ACCEL_ZOUTH_REG         = 0X3F, //加速度值,Z轴高8位寄存器
    MPU6050_ACCEL_ZOUTL_REG         = 0X40, //加速度值,Z轴低8位寄存器   
    MPU6050_TEMP_OUTH_REG           = 0X41, //温度值高八位寄存器
    MPU6050_TEMP_OUTL_REG           = 0X42, //温度值低8位寄存器 
    MPU6050_GYRO_XOUTH_REG          = 0X43, //陀螺仪值,X轴高8位寄存器
    MPU6050_GYRO_XOUTL_REG          = 0X44, //陀螺仪值,X轴低8位寄存器
    MPU6050_GYRO_YOUTH_REG          = 0X45, //陀螺仪值,Y轴高8位寄存器
    MPU6050_GYRO_YOUTL_REG          = 0X46, //陀螺仪值,Y轴低8位寄存器
    MPU6050_GYRO_ZOUTH_REG          = 0X47, //陀螺仪值,Z轴高8位寄存器
    MPU6050_GYRO_ZOUTL_REG          = 0X48, //陀螺仪值,Z轴低8位寄存器

    MPU6050_RA_MOT_DETECT_STATUS    = 0x61,

    MPU6050_I2CSLV0_DO_REG          = 0X63, //IIC从机0数据寄存器
    MPU6050_I2CSLV1_DO_REG          = 0X64, //IIC从机1数据寄存器
    MPU6050_I2CSLV2_DO_REG          = 0X65, //IIC从机2数据寄存器
    MPU6050_I2CSLV3_DO_REG          = 0X66, //IIC从机3数据寄存器

    MPU6050_I2CMST_DELAY_REG        = 0X67,	//IIC主机延时管理寄存器
    MPU6050_SIGPATH_RST_REG         = 0X68,	//信号通道复位寄存器
    MPU6050_MDETECT_CTRL_REG        = 0X69,	//运动检测控制寄存器
    MPU6050_USER_CTRL_REG           = 0X6A,	//用户控制寄存器
    MPU6050_PWR_MGMT1_REG           = 0X6B,	//电源管理寄存器1
    MPU6050_PWR_MGMT2_REG           = 0X6C,
    MPU6050_RA_BANK_SEL             = 0x6D,  

    MPU6050_RA_MEM_START_ADDR       = 0x6E,
    MPU6050_RA_MEM_R_W              = 0x6F,
    MPU6050_RA_DMP_CFG_1            = 0x70,
    MPU6050_RA_DMP_CFG_2            = 0x71,

    MPU6050_FIFO_CNTH_REG           = 0X72, //FIFO计数寄存器高八位
    MPU6050_FIFO_CNTL_REG           = 0X73, //FIFO计数寄存器低八位
    MPU6050_FIFO_RW_REG             = 0X74, //FIFO读写寄存器
    MPU6050_DEVICE_ID_REG           = 0X75, //器件ID寄存器
};

/****************************************************************************************/
/*      Mnemonic                    value        meaning/usage                          */




#endif /* _DRIVER_MOTION_H_ */