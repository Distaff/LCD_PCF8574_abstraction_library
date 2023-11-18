/**
 * \file LCD_PCF8574_abstraction.h
 * \brief Abstraction layer for LCD display with PCF8574 controller. 
 * \author Piotr Kądziela, https://github.com/Distaff
 * \version 0.1-pre
 * 
 * \section Description
 * May be used with any I2C library.
 * Currently supports only 8-bit mode
*/



#ifndef PCF8574_ab_h
#define PCF8574_ab_h

#include <string>
#include <array>
#include <functional>


class LCD_PCF8574
{
public:
    /**
     * Constructor
     * \param colCount number of columns
     * \param rowCount number of rows
     * \param sendByte function to send a byte to the display controller
     * \param delayUs function to delay for specified number of microseconds
     * \warning colCount and rowCount must be equal to the actual number of columns and rows on the display
    */
    LCD_PCF8574(uint8_t colCount, uint8_t rowCount, std::function<int(uint8_t)> sendByte, std::function<void(int)> delayUs);


	void begin();

    /**
     * Clears whole display and sets cursor to 0,0
    */
    void clear();

    /**
     * Sets cursor to specified position (indexed from 0)
     * \param col column (0 is default)
     * \param row row (0 is default)
     * \return true if success, false otherwise (eg. position out of range)
    */
    bool setCursor(uint8_t col = 0, uint8_t row = 0);

    /**
     * Writes a single character to the display
     * \param c character to write
     * \return number of bytes written
    */
    size_t write(char c);

    /**
     * Writes a string to the display
     * \param str string to write
     * \return number of bytes written
    */
    size_t print(const char* str);

    /**
     * Writes a string to the display
     * \param str string to write
     * \return number of bytes written
    */
    size_t print(const std::string& str);

    /**
     * Enables/disables backlight
     * \param enabled true to enable, false to disable
    */
    void setBacklightEnabled(bool enabled = true);

    /**
     * Enables/disables displaying an underscore at the cursor position 
     * \param enabled true to enable, false to disable
    */
    void setCursorVisible(bool visible = true);

    /**TODO
     * Shifts display contents and cursor one character left
    */
    //void shiftLeft();

    /**TODO
     * Shifts display contents and cursor one character right
    */
    //void shiftRight();

    /**TODO
     * 
    */
    // void defineGlyph(uint8_t index, uint8_t[8] data);

protected:

    uint8_t colCount_;  /// number of columns
    uint8_t rowCount_;  /// number of rows
    std::array<uint8_t, 4> rowOffsets_;

    std::function<int(uint8_t)> sendByte_;
    std::function<void(int)> delayUs_;
};

#endif
