/*
 * File:   eeprom.c
 * Author: LucasGabriel
 *
 * Created on 11 de Março de 2023, 23:52
 */

#include "eeprom.h"
#include "system_config.h"

void EEPROM_Write_Byte(uint8_t address, uint8_t data)
{
    uint8_t status_GIE;
    EEADR               =   address;
    EEDATA              =   data;
    EECON1bits.EEPGD    =   0b0;
    EECON1bits.CFGS     =   0b0;
    EECON1bits.WREN     =   0b1;
    status_GIE          =   INTCONbits.GIE;
    INTCONbits.GIE      =   0b0;
    EECON2              =   0x55;
    EECON2              =   0xAA;
    EECON1bits.WR       =   0b1;
    
    while(1 == EECON1bits.WR);
    
    INTCONbits.GIE      =   status_GIE;
    EECON1bits.WREN     =   0b0;
}

uint8_t EEPROM_Read_Byte(uint8_t address)
{
    EEADR               =   address;
    EECON1bits.CFGS     =   0b0;
    EECON1bits.EEPGD    =   0b0;
    EECON1bits.RD       =   0b1;
    NOP();
    NOP();
    return EEDATA;
}

void EEPROM_Write_Int(uint8_t address, uint16_t data)
{
    EEPROM_Write_Byte(address, (uint8_t)data);
    EEPROM_Write_Byte(address + 1, (uint8_t)(data>>8));
}

uint16_t EEPROM_Read_Int(uint8_t address)
{
    return (uint16_t)(EEPROM_Read_Byte(address)) | (uint16_t)((EEPROM_Read_Byte(address+1))<<8);
}

void EEPROM_Write_Float(uint8_t address, uint8_t *data)
{
    EEPROM_Write_Array(address, 4, data);
}

float EEPROM_Read_Float(uint8_t address)
{
    union
    {
        uint8_t buffer[4];
        float   value;
    }result;
    
    EEPROM_Read_Array(address, 4, result.buffer);
    return result.value;
}

void EEPROM_Write_Array(uint8_t address, uint8_t word_width, uint8_t *data)
{
    for(uint8_t _i = 0; _i < word_width; _i++)
    {
        EEPROM_Write_Byte(address+_i, *data);
        data++;
    }
}

void EEPROM_Read_Array(uint8_t address, uint8_t word_width, uint8_t *buffer)
{
    for(uint8_t _i = 0; _i < word_width; _i++)
    {
        *buffer      =   EEPROM_Read_Byte(address+_i);
        ++buffer;
    }
}