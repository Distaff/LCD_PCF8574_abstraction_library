/**
 * \file LCD_PCF8574_abstraction.h
 * \brief Abstraction layer for LCD display based on HD44780 with PCF8574 controller. 
 * \author Piotr Kądziela, https://github.com/Distaff
 * \version 0.1-pre
 * 
 * \section Description
 * May be used with any I2C library.
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
    LCD_PCF8574(uint8_t colCount, uint8_t rowCount, std::function<size_t(uint8_t)> sendByte, std::function<void(int)> delayUs);

    /**
     * Initializes the display. Must be called before any other method.
    */
	void begin();

    /**
     * Clears whole display and sets cursor to 0,0
    */
    void clear();

    /**
     * Sets cursor to 0,0
    */
    void home();

    /**
     * Sets cursor to specified position (indexed from 0)
     * \param col column
     * \param row row
     * \return true if success, false otherwise (eg. position out of range)
    */
    bool setCursor(uint8_t col, uint8_t row);

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
     * \return true if success, false otherwise
    */
    bool setBacklightEnabled(bool isTrue = true);

    /**
     * Enables/disables display
     * \param enabled true to enable, false to disable
     * \return true if success, false otherwise
    */
    bool setDisplayEnabled(bool isTrue = true);

    /**
     * Enables/disables displaying an underscore at the cursor position 
     * \param enabled true to enable, false to disable
     * \return true if success, false otherwise
    */
    bool setCursorVisible(bool isTrue = true);

    /**
     * Enables/disables blinking of the cursor
     * \param enabled true to enable, false to disable
     * \return true if success, false otherwise
    */
    bool setCursorBlink(bool isTrue = true);

    /* TODO: entry modes */

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
    enum SendMode
    {
        DATA    = 0,
        COMMAND = 1
    };
    
    // default pin mapping below
    uint8_t rsPinMask_ = 0b00000001;
    uint8_t rwPinMask_ = 0b00000010;
    uint8_t enPinMask_ = 0b00000100;
    uint8_t blPinMask_ = 0b00001000;
    uint8_t d4PinMask_ = 0b00010000;
    uint8_t d5PinMask_ = 0b00100000;
    uint8_t d6PinMask_ = 0b01000000;
    uint8_t d7PinMask_ = 0b10000000;

    uint8_t colCount_;  /// number of columns
    uint8_t rowCount_;  /// number of rows
    std::array<uint8_t, 4> rowOffsets_;
    std::function<size_t(uint8_t)> sendByte_;
    std::function<void(int)> delayUs_;

    struct 
    {
        bool displayEnabled     : 1;
        bool backlightEnabled   : 1;
        bool cursorVisible      : 1;
        bool cursorBlink        : 1;
        bool incrementMode      : 1;
        bool shiftDisplay       : 1;
    } state_;




    bool updateEntryMode();
    bool updateDisplayControl();
    bool send(SendMode mode, uint8_t data);
    bool sendNibble(SendMode mode, uint8_t data);

};

#endif
