#include "main.h"

int main(void)
{
	//Hardware Initializations
	init_board_leds();
	init_i2c();
	init_campsense();

	int8_t* btn;
	int32_t sld_status;
	int brightness = 400;	//initial brighness of the leds

    while(true)
    {
    	//checking the status of the buttons and the slider
    	btn = getBtnsStatus();
    	sld_status = getSliderStatus();
    	delay(5000);

    	// Check button 0
    	if(btn[0] == 1){            // Button 0 is pressed
    		if(brightness <= 20000)
    			brightness += 50;
    	}
    	// Check button 0
    	if(btn[1] == 1){            // Button 1 is pressed
    		if(brightness > 200)
    			brightness -= 50;
    	}

    	slideBlinker(sld_status,brightness);
    	//printf("btn1:%d , btn2:%d slider: %d\n",btn[0],btn[1],sld_status);
    }
    return 0;
}

/** Returnign the Buttons Status **/
int8_t* getBtnsStatus()
{
	//Reading The status of btns
	int32_t btn_status = I2C_Read_Single(I2C8_BASE,SLAVE_ADDR,0x88);

	static int8_t btn[2];

	    	//printf("%d\n",cs_status);
	    	//Doing some magic
	btn[0] = (btn_status & 0x08) >> 3;
	btn[1] = (btn_status & 0x10) >> 4;

	return btn;
}

/** Returning the Slider Status **/
int32_t getSliderStatus()
{
	int32_t sld_status = I2C_Read_Single(I2C8_BASE,SLAVE_ADDR,0x89);
	return sld_status;
}

/** Reading single byte packet from registers **/
int32_t I2C_Read_Single(uint32_t ui32Base, uint8_t ui8SlaveAddr ,uint8_t ui8SlaveRegAddr)
{
    // Setting The Slave Address for writing
    I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr, false);

   	//Setting the Slave Register
    I2CMasterDataPut(ui32Base, ui8SlaveRegAddr);
    I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_SEND);

    //wait for the bus
    wait_BUS(ui32Base);

    // Setting The Slave Address for reading
    I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr, true);
	// Initiate send of character from Master to Slave
    I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_RECEIVE);

    // Delay until transmission completes
    wait_BUS(ui32Base);

    //reading a single char
    int32_t int32_tData = I2CMasterDataGet(ui32Base);

    return int32_tData;
}

/** Reading consecutive from the registers on the board **/
int8_t I2C_Read_consecutive(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint8_t ui8SlaveRegAddr, uint32_t* data, unsigned int len)
{
    if (len < 2)	// Assume I2C Recieving will always return data
        return -1;

    // Setting The Slave Address for writing
    I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr, false);

	//Setting the Slave Register
	I2CMasterDataPut(ui32Base, ui8SlaveRegAddr);
	I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_SEND);

	// Delay until transmission completes
	wait_BUS(ui32Base);

	// Setting The Slave Address for reading
	I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr | 0x1, true);

	// Begin reading consecutive data
	I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_START);
	//wait for the tansmition to be complete
	wait_BUS(ui32Base);

	*data = I2CMasterDataGet(ui32Base);
	len--;
	data++;

	// Continue reading consecutive data
	while(len > 0){
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

		//wait for the tansmition to be complete
		wait_BUS(ui32Base);

		*data = I2CMasterDataGet(ui32Base);
		len--;
		data++;
	}

	// Read last character of data
    I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    //wait for the tansmition to be complete
	wait_BUS(ui32Base);

	*data = I2CMasterDataGet(ui32Base);

	return 0;
}

/** Writing data to the boadr registers **/
int32_t I2C_Write(uint32_t ui32Base, uint8_t ui8SlaveAddr , uint8_t ui8Data,uint8_t ui8SlaveRegAddr)
{
	// Specify slave address
	I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr, false);
	//Setting the Slave Register - Starting The transmition
	I2CMasterDataPut(ui32Base, ui8SlaveRegAddr);
    // Delay until transmission completes
    while(I2CMasterBusBusy(ui32Base)) { }

	if(sizeof(ui8Data) > sizeof(uint8_t)) {

		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_START);

        // Delay until transmission completes
		while(I2CMasterBusBusy(ui32Base)) { }

		//Checking For errors
		if(I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE) { return -1; }

		// Continue the transmition by sending the data
		I2CMasterDataPut(ui32Base, ui8Data);

		// Initiate send of character from Master to Slave
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_CONT);

		// Delay until transmission completes
		while(I2CMasterBusBusy(ui32Base)) { }

		//Checking For errors
		if(I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE) { return -1; }

		//Finishing the Transmition
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_FINISH);

		// Delay until transmission completes
		while(I2CMasterBusBusy(ui32Base)) { }

		//Checking For errors
		if(I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE) { return -1; }
	} else {

		//Send one byte
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_SEND);

		// Delay until transmission completes
		while(I2CMasterBusBusy(ui32Base)) { }

		//Checking For errors
		if(I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE) { return -1; }

		// Continue the transmition by sending the data
		I2CMasterDataPut(ui32Base, ui8Data);

		// Delay until transmission completes
		while(I2CMasterBusBusy(ui32Base)) { }

		//Checking For errors
		if(I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE) { return -1; }
	}
    return 0;
}
void wait_BUS(uint32_t ui32Base){
	while ( !(I2CMasterBusy(ui32Base)) );
	while (   I2CMasterBusy(ui32Base)  );

	// Check for I2C Errors by reading raw interrupt status
	uint32_t NAckState = (I2CMasterIntStatusEx(ui32Base, false) & I2C_MASTER_INT_NACK);
	I2CMasterIntClearEx(ui32Base, I2C_MASTER_INT_NACK);

    while( NAckState != 0) { }

    //Checking For errors
    if(I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE) { return ; }
}

/** Initializing the I2C of the board **/
void init_i2c()
{
		// Enable the Peripheral for I2C8
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		// Enable the Peripheral for MOSI/MISO
		SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C8);

		GPIOPinConfigure(GPIO_PA2_I2C8SCL);		//SCL -> MISO(12C)
		GPIOPinConfigure(GPIO_PA3_I2C8SDA);		//SDA -> MOSI(I2C)


		GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_2);	//Setting CSL
		GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_3);	//Setting SDA

		//Setting I2C Clock
		uint32_t getClockFreq = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, BORDFRQ); //120MHz
		I2CMasterInitExpClk(I2C8_BASE, getClockFreq, true);

		//Enabling I2C
		I2CMasterEnable(I2C8_BASE);
}

/** Initializing the campsense borad **/
void init_campsense(){

	// Enable pins on B0,B1 and slider as CapSense
	I2C_Write(I2C8_BASE,SLAVE_ADDR,COMMAND_REG,0x08);
	I2C_Write(I2C8_BASE,SLAVE_ADDR,CS_ENABL1,0x1F);	// Five pins will be used for Slider pads
	I2C_Write(I2C8_BASE,SLAVE_ADDR,CS_ENABL0,0x18);   	// Two pins will be used for Button pads
	I2C_Write(I2C8_BASE,SLAVE_ADDR,GPIO_ENABLE0,0x03);	// Three pins will be used as GPIO 2 for LED and 1 as GPIO2
	I2C_Write(I2C8_BASE,SLAVE_ADDR,DM_STRONG0,0x03);	// Enables strong drive mode for GPIOs

	   // Enable slider
	I2C_Write(I2C8_BASE,SLAVE_ADDR,CS_SLID_CONFIG,0x01);
	   /*
	      Configure slider resolution
	      Resolution = (SensorsInSlider - 1) * Multiplier
	      Resolution = 4 * 16.00 = 64
	      so the slider_val will be in range 0 to 64
	   */
	I2C_Write(I2C8_BASE,SLAVE_ADDR,CS_SLID_MULM,0x10);
	I2C_Write(I2C8_BASE,SLAVE_ADDR,CS_SLID_MULL,0x00);

	I2C_Write(I2C8_BASE,SLAVE_ADDR,COMMAND_REG,0x01);	// Store Current Configuration to NVM

	I2C_Write(I2C8_BASE,SLAVE_ADDR,COMMAND_REG,0x06);	// Reconfigure Device (POR)

	// Initial ON*OFF*ON LEDs (inverse logic 0-LED ON, 1-LED OFF)
	I2C_Write(I2C8_BASE,SLAVE_ADDR,OUTPUT_PORT0,0x00);
	I2C_Write(I2C8_BASE,SLAVE_ADDR,OUTPUT_PORT1,0x03);
	I2C_Write(I2C8_BASE,SLAVE_ADDR,OUTPUT_PORT0,0x00);
}

/** Initializing the leds **/
void init_board_leds(){
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);                         // LEDs (D1 & D2)
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);                         // LEDs (D2 & D3)

	    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);                    // LED 1
	    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);                    // LED 2
	    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);                    // LED 3
	    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);                    // LED 4

}

/** Blink leds based on the Slider Status **/
void slideBlinker(int status,int brightness)
{
	if (0 < status && status < 8 )
	{
		blink(1,brightness);
		//printf("1 - %d\n",status);
	}
	else if (7 < status && status < 16)
	{
		blink(2,brightness);
		//printf("2 - %d\n",status);
	}
	else if (15 < status && status < 26 )
	{
		blink(4,brightness);
		//printf("3 - %d\n",status);
	}
	else if (25 < status && status < 32 )
	{
		blink(3,brightness);
		//printf("4 - %d\n",status);
	}
}
/* Making LED blink by entering its sequel number on board*/
void blink(int ledNumber,int brightness)
{
	volatile int i =0;
	if(ledNumber == 1)
	{
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 2);        // LED 1
		delay(brightness);
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
	}
	else if(ledNumber == 2)
	{
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 1);        // LED 2
		delay(brightness);
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
	}
	else if(ledNumber == 3)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);       // LED 3
		delay(brightness);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
	}
	else if(ledNumber == 4)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 1);		 // LED 4
		delay(brightness);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);
	}
}
