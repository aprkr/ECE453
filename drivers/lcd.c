#include "lcd.h"

// #define printIIC(args)	Wire.send(args)
// inline void write(uint8_t value) {
// 	send(value, Rs);
// }



// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

// LiquidCrystal_I2C(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
// {
//   _Addr = 0x27;
//   _cols = 16;
//   _rows = 4;
//   _backlightval = LCD_NOBACKLIGHT;
// }

uint8_t _Addr = 0x27;
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _numlines;
//   bool _oled = false;
  uint8_t _cols = 16;
  uint8_t _rows = 4;
  uint8_t _backlightval = LCD_NOBACKLIGHT;

// void oled_init(){
//   _oled = true;
// 	init_priv();
// }

// void init(){
// 	init_priv();
// }

// void init_priv()
// {
// 	Wire.begin();
	
// 	begin(_cols, _rows);  
// }

void lcdbegin(uint8_t cols, uint8_t lines) {
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// // for some 1 line displays you can select a 10 pixel high font
	// if ((dotsize != 0) && (lines == 1)) {
	// 	_displayfunction |= LCD_5x10DOTS;
	// }

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	cyhal_system_delay_ms(50); 
  
	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	cyhal_system_delay_ms(10);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	  // we start in 8bit mode, try to set 4 bit mode
   write4bits(0x03 << 4);
   cyhal_system_delay_us(4500); // wait min 4.1ms
   
   // second try
   write4bits(0x03 << 4);
   cyhal_system_delay_us(4500); // wait min 4.1ms
   
   // third go!
   write4bits(0x03 << 4); 
   cyhal_system_delay_us(150);
   
   // finally, set to 4-bit interface
   write4bits(0x02 << 4); 


	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
	
	home();
  
}

/********** high level commands, for the user! */
void clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	cyhal_system_delay_ms(2);  // this command takes a long time!
}

void home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	cyhal_system_delay_ms(2);  // this command takes a long time!
}

void setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// // Allows us to fill the first 8 CGRAM locations
// // with custom characters
// void createChar(uint8_t location, uint8_t charmap[]) {
// 	location &= 0x7; // we only have 8 locations 0-7
// 	command(LCD_SETCGRAMADDR | (location << 3));
// 	for (int i=0; i<8; i++) {
// 		write(charmap[i]);
// 	}
// }

// //createChar with PROGMEM input
// void createChar(uint8_t location, const char *charmap) {
// 	location &= 0x7; // we only have 8 locations 0-7
// 	command(LCD_SETCGRAMADDR | (location << 3));
// 	for (int i=0; i<8; i++) {
// 	    	write(pgm_read_byte_near(charmap++));
// 	}
// }

// Turn the (optional) backlight off/on
void noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}



/*********** mid level commands, for sending data/cmds */

inline void command(uint8_t value) {
	send(value, 0);
}


/************ low level data pushing commands **********/

// write either command or data
void send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
       write4bits((highnib)|mode);
	write4bits((lownib)|mode); 
}

void write(uint8_t value) {
    send(value, Rs);
}

void writeString(char *s) {
	clear();
	int done = 0;
	int index = 0;
	for (int rows = 0; rows < 4; rows++) {
		for (int cols = 0; cols < 20; cols++) {
			char value = s[index];
			index++;
			if (value == 0) {
				done = 1;
				break;
			} else if (value == '\n') {
				break;
			}
			write(value);
		}
		if (done == 1) {
			break;
		}
	setCursor(0, rows+1);
	}
}

void writeStringWithoutClear(int row, int col, char *s, int len) {
	setCursor(col, row);
	for (int i = 0; i < len; i++) {
		write(s[i]);
	}
}

void write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void expanderWrite(uint8_t _data){                                        
	// Wire.beginTransmission(_Addr);
	// printIIC((int)(_data) | _backlightval);
	// Wire.endTransmission();   
    uint8_t buffer[1];
    buffer[0] = _data | _backlightval;
    cyhal_i2c_master_write(&i2c_master_obj, 0x27, buffer, 1, 0, false);
}

void pulseEnable(uint8_t _data){
	expanderWrite(_data | En);	// En high
	cyhal_system_delay_us(1);		// enable pulse must be >450ns
	
	expanderWrite(_data & ~En);	// En low
	cyhal_system_delay_us(50);		// commands need > 37us to settle
} 


// Alias functions

void cursor_on(){
	cursor();
}

void cursor_off(){
	noCursor();
}

void blink_on(){
	blink();
}

void blink_off(){
	noBlink();
}

// void load_custom_character(uint8_t char_num, uint8_t *rows){
// 		createChar(char_num, rows);
// }

void setBacklight(uint8_t new_val){
	if(new_val){
		backlight();		// turn backlight on
	}else{
		noBacklight();		// turn backlight off
	}
}

// void printstr(const char c[]){
// 	//This function is not identical to the function used for "real" I2C displays
// 	//it's here so the user sketch doesn't have to be changed 
// 	print(c);
// }


// // unsupported API functions
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
// void off(){}
// void on(){}
// void setDelay (int cmdDelay,int charDelay) {}
// uint8_t status(){return 0;}
// uint8_t keypad (){return 0;}
// uint8_t init_bargraph(uint8_t graphtype){return 0;}
// void draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
// void draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
// void setContrast(uint8_t new_val){}
// #pragma GCC diagnostic pop
	