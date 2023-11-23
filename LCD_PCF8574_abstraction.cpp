#include "LCD_PCF8574_abstraction.h"

namespace {
    constexpr uint8_t INIT_CODE1        = 0b00000011;
    constexpr uint8_t INIT_CODE2        = 0b00000010;

    // Commands
    constexpr uint8_t NO_COMMAND        = 0;
    constexpr uint8_t CLEAR             = 0b00000001;
    constexpr uint8_t HOME              = 0b00000010;
    constexpr uint8_t ENTRY_MODE_SET    = 0b00000100;
    constexpr uint8_t DISPLAY_CONTROL   = 0b00001000;
    constexpr uint8_t CURSOR_SHIFT      = 0b00010000;
    constexpr uint8_t FUNCTION_SET      = 0b00100000;   // Function set command. Pin config is 0 0 1 DL N F - - with DL, N and F set by flags below
    constexpr uint8_t SET_CGRAM_ADDR    = 0b01000000;   // Set CGRAM address command. Pin config is 0 1 ACG AC5 AC4 AC3 AC2 AC1 AC0
    constexpr uint8_t SET_DDRAM_ADDR    = 0b10000000;   // Set DDRAM address command. Pin config is 1 ADD ADD ADD ADD ADD ADD ADD ADD

    // Bit masks for setting flags in commands
    constexpr uint8_t ENTRY_MODE_SET_ID = 0b00000010;   // Bit high for increment mode, low for decrement mode (left-to-right or right-to-left)
    constexpr uint8_t ENTRY_MODE_SET_S  = 0b00000001;   // Bit high for shift display, low for shift cursor

    constexpr uint8_t DISPLAY_CONTROL_D = 0b00000100;   // Bit high for display on, low for display off
    constexpr uint8_t DISPLAY_CONTROL_C = 0b00000010;   // Bit high for cursor on, low for cursor off
    constexpr uint8_t DISPLAY_CONTROL_B = 0b00000001;   // Bit high for cursor blink on, low for cursor blink off

    constexpr uint8_t FUNCTION_SET_DL   = 0b00010000;   // Bit high for 8-bit mode, low for 4-bit mode
    constexpr uint8_t FUNCTION_SET_N    = 0b00001000;   // Bit high for 2-line mode, low for 1-line mode
    constexpr uint8_t FUNCTION_SET_F    = 0b00000100;   // Bit high for 5x10 font, low for 5x8 font
}


/*** public ***/

LCD_PCF8574::LCD_PCF8574(uint8_t colCount, uint8_t rowCount, std::function<size_t(uint8_t)> sendByte, std::function<void(int)> delayUs) 
    : colCount_(colCount), rowCount_(rowCount), sendByte_(sendByte), delayUs_(delayUs)
{
    state_ = {
        .displayEnabled     = true,
        .backlightEnabled   = true,
        .cursorVisible      = true,
        .cursorBlink        = true,
        .incrementMode      = true,
        .shiftDisplay       = false
    };
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

    delayUs_(50000);

    // Set all PCF8574 pins to LOW
    sendByte_(0x00);
    delayUs_(50000);

    /*** Initialization sequence - order specified by HD44780 documentation ***/
    sendNibble(COMMAND, INIT_CODE1);
    delayUs_(5000);
    sendNibble(COMMAND, INIT_CODE1);
    delayUs_(500);
    sendNibble(COMMAND, INIT_CODE1);
    delayUs_(500);
    sendNibble(COMMAND, INIT_CODE2);
    delayUs_(500);

    // Set 4-bit mode, 1 or 2-line mode, 5x8 font
    send(COMMAND, FUNCTION_SET | (rowCount_ > 1 ? FUNCTION_SET_N : 0));
    delayUs_(250);

    // Set display mode to default values
    updateDisplayControl();
    delayUs_(250);

    // Clear display
    send(COMMAND, CLEAR); 
    delayUs_(2000);

    // Set entry mode to default values
    updateEntryMode();
    delayUs_(250);
}

void LCD_PCF8574::clear()
{
    send(COMMAND, CLEAR); 
    delayUs_(2000);
}

void LCD_PCF8574::home()
{
    send(COMMAND, HOME); 
    delayUs_(37);
}

bool LCD_PCF8574::setCursor(uint8_t col, uint8_t row)
{
    switch (row) {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    return send(COMMAND, col);
}

size_t LCD_PCF8574::write(char c)
{
	return send(DATA, c);
}

size_t LCD_PCF8574::print(const char* str)
{
    const char* iter;
    for(iter = str; *iter; iter++)
    {
        bool success = write(*iter);
        if(!success)
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

bool LCD_PCF8574::setBacklightEnabled(bool isTrue){
    state_.backlightEnabled = isTrue;
    return sendNibble(COMMAND, NO_COMMAND);
}

bool LCD_PCF8574::setDisplayEnabled(bool isTrue){
    state_.displayEnabled = isTrue;
    return updateDisplayControl();
}

bool LCD_PCF8574::setCursorVisible(bool isTrue){
    state_.cursorVisible = isTrue;
    return updateDisplayControl();
}

bool LCD_PCF8574::setCursorBlink(bool isTrue){
    state_.cursorBlink = isTrue;
    return updateDisplayControl();
}


/*** private ***/

bool LCD_PCF8574::updateEntryMode()
{
    uint8_t toSend = ENTRY_MODE_SET 
        | (state_.incrementMode ? ENTRY_MODE_SET_ID : 0) 
        | (state_.shiftDisplay ? ENTRY_MODE_SET_S : 0);
    return send(COMMAND, toSend);
}

bool LCD_PCF8574::updateDisplayControl()
{
    uint8_t toSend = DISPLAY_CONTROL 
        | (state_.displayEnabled ? DISPLAY_CONTROL_D : 0) 
        | (state_.cursorVisible ? DISPLAY_CONTROL_C : 0) 
        | (state_.cursorBlink ? DISPLAY_CONTROL_B : 0);
    return send(COMMAND, toSend);
}

bool LCD_PCF8574::send(SendMode mode, uint8_t data)
{
    bool success = sendNibble(mode, (data & 0b11110000) >> 4);
    success = sendNibble(mode, (data & 0b00001111)) && success;
    return success;
}

bool LCD_PCF8574::sendNibble(SendMode mode, uint8_t data)
{
    uint8_t toSend = 0;

    toSend |= (mode == DATA ? rsPinMask_ : 0);
    toSend |= (state_.backlightEnabled ? blPinMask_ : 0);
    

    toSend |= ((data & 0b0001) ? d4PinMask_ : 0);
    toSend |= ((data & 0b0010) ? d5PinMask_ : 0);
    toSend |= ((data & 0b0100) ? d6PinMask_ : 0);
    toSend |= ((data & 0b1000) ? d7PinMask_ : 0);

    sendByte_(toSend | enPinMask_);     //send enable pulse
    delayUs_(1);                        //enable pulse must be >450ns
    sendByte_(toSend);
    return 1;
}
