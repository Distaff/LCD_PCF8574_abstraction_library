#include "LCD_PCF8574_abstraction.h"

LCD_PCF8574::LCD_PCF8574(uint8_t colCount, uint8_t rowCount, std::function<int(uint8_t)> sendByte, std::function<void(int)> delayUs) 
    : colCount_(colCount), rowCount_(rowCount), sendByte_(sendByte), delayUs_(delayUs)
{

}

void LCD_PCF8574::begin()
{
    for(uint8_t i = 0; i < rowCount_; i+=2)
    {
        rowOffsets_[0] = 0x00;
        rowOffsets_[1] = 0x40;
        rowOffsets_[2] = 0x00 + colCount_;
        rowOffsets_[3] = 0x40 + colCount_;
    }
}

void LCD_PCF8574::clear()
{
    sendByte_(0x01);
    delayUs_(1600);
}


bool LCD_PCF8574::setCursor(uint8_t col, uint8_t row)
{
    if (col >= colCount_ || row >= rowCount_)
    {
        return false;
    }
    return sendByte_(0x80 | (rowOffsets_[row] + col));
}


size_t LCD_PCF8574::write(char c)
{
	return 0;
}

size_t LCD_PCF8574::print(const char* str)
{
    const char* iter;
    for(iter = str; *iter; iter++)
    {
        if(!write(*iter))
        {
            break;
        }
    }
    return iter - str;
}

size_t LCD_PCF8574::print(const std::string& str)
{
    return print(str.c_str());
}
/*
void LCD_PCF8574::setBacklightEnabled(bool enabled);
{
    
}

void LCD_PCF8574::setCursorVisible(bool visible);
{
    
}

void LCD_PCF8574::shiftLeft();
{
    
}

void LCD_PCF8574::shiftRight();
{
    
}
*/