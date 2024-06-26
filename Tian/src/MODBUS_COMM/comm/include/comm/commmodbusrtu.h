/*
 * @Author: Tian_sj
 * @Date: 2023-10-13 11:47:29
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-10-16 14:25:25
 * @FilePath: /BTS/comm/include/comm/commmodbusrtu.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef COMMMODBUSRTU_H
#define COMMMODBUSRTU_H

#include <comm/commprotocol.h>

class commModbusRTU : public commProtocol
{
public:
    commModbusRTU();
    ~commModbusRTU();

protected:
    virtual void analysis_thread_function() override;

private:

    // Modbus 功能码
    enum FunctionCode
    {
        ReadCoils = 0x01,             // 读线圈状态
        ReadDiscreteInputs = 0x02,    // 读离散输入状态
        ReadHoldingRegisters = 0x03,  // 读保持寄存器
        ReadInputRegisters = 0x04,    // 读输入寄存器
        WriteSingleCoil = 0x05,       // 写单个线圈
        WriteSingleRegister = 0x06,   // 写单个保持寄存器
        WriteMultipleColis = 0x0f,    // 写多个线圈
        WriteMultipleRegisters = 0x10 // 写多个保持寄存器
    };
};

#endif // COMMMODBUSRTU_H
