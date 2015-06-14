//Stijn Moreels  6EE Metronoom
//VTI Menen
#include<pic.h>
#include<Delays.h>
#include<Strings.h>

unsigned char teller1;
unsigned char teller2;
unsigned char teller3;
unsigned char Wachtwaarde;
char s[10];//variabele
unsigned int BPM;
unsigned char vlag;
unsigned int Resultaat;
float Tussenresultaat;
//Waarde die gestuurd wordt naar het LCD en de Speaker

void LCD_Write(char C_D, unsigned char Command)
{
	bRB4 = 0;
	bRB3 = 0; //Enable
	if (C_D == 'C') bRB5 = 0;
	if (C_D == 'D') bRB5 = 1;

	PORTA &= 0xF0;  //Om de minst beduidende bits op nul te plaatsen
	PORTA |= Command >> 4; //Om de meest beduidende bits 4 plaatsen op te schuiven zodat ze op de plaats staan van de minst beduidende

	bRB3 = 1; //Daarna wordt de Command ge-ORd met PORTB zodat de de minst(meest) beduidende van de Command   
	bRB3 = 0; //in de minst beduidende van PORTB geplaatst 
	bRB3 = 0;

	PORTA &= 0xF0;
	PORTA |= Command & 0x0F; //De minst beduidende bits van de Command doorgestuurd
	bRB3 = 1;
	bRB3 = 0;

	if (C_D == 'C')Wait(10);
	if (C_D == 'D')Wait(2);
}



void LCD_WriteString(char*String)
{
	while (*String)
	{
		LCD_Write('D', *(String++));
	}
}

void Interrupt()
{
	// debugging : led hoofdtoon aanleggen
	if (bINT0IF) //UP
	{
		BPM = BPM + 10;
		// debugging
		bRC2 = 1;
		if (BPM >= 250)BPM = 250;
		while (bRB0 == 0);
		bINT0IF = 0;
		vlag = 1;
	}

	if (bINT2IF) //DOWN
	{
		BPM = BPM - 10;
		if (BPM <= 30)BPM = 30;
		bRC4 = 1;
		while (bRB1 == 0);
		bINT2IF = 0;
		vlag = 1;
	}

	if (bINT1IF) //RESET
	{
		BPM = 120;
		while (bRB2 == 0);
		bINT1IF = 0;
		vlag = 1;
	}
}


void main()
{
	//120 bpm=Preset
	TRISC = 0x00;  //Voor de Speaker
	TRISB = 0b11000111;  //Voor de instellingen van de LCD en drukknoppen
	TRISA = 0x00;  //Voor de databits van de LCD
	ADCON1 = 0b00000111;
	BPM = 120;

	//INTERRUPTS
	bGIE = 1;
	bPEIE = 1;
	bINT0IF = 0;  //INTCON = 0b11011000;
	bINT0IE = 1;  //INTCON2 = 0xFF;
	bINT1IF = 0;  //INTCON3 = 0xFF;
	bINT1IE = 1;
	bINT2IF = 0;
	bINT2IE = 1;
	INTCON2 &= 0b10001111; //CONTROLEREN!!!

	//LCD
	Wait(50);
	for (Wachtwaarde = 1; Wachtwaarde > 2; Wachtwaarde++)
	{
		LCD_Write('C', 0b00100010);
	} //Stijn Funnction set
	LCD_Write('C', 0b00101000);//font lines Koen
	LCD_Write('C', 0b00001100);//display ON 
	LCD_Write('C', 0b00000001);//clear 
	LCD_Write('C', 0b00000110);//entry mode Stijn
	LCD_WriteString("Metronoom ");
	iPrtString(s, BPM);
	LCD_WriteString(s);
	while (1)
	{
		if (vlag == 1)
		{
			vlag = 0;
			LCD_Write('C', 0b00000001);//clear 
			LCD_WriteString("Metronoom ");
			iPrtString(s, BPM);
			LCD_WriteString(s);
		}
		Tussenresultaat = (float)BPM / 60.0;
		Resultaat = 1000 / Tussenresultaat;
		for (teller1 = 1; teller1 != 50; teller1++) // 100ms 
		{
			bRC7 = 1;
			bRC0 = 1;
			Wait(1);
			bRC7 = 0;
			bRC0 = 0;
			Wait(1);
		}
		Wait(Resultaat); //=500ms(120)
		LCD_Write('C', 0b00000001);//clear 
		LCD_WriteString("Metronoom ");
		iPrtString(s, BPM);
		LCD_WriteString(s);
		for (teller2 = 1; teller2 != 4; teller2++)
		{
			for (teller1 = 1; teller1 != 50; teller1++) //200ms
			{
				bRC7 = 1;
				bRC1 = 1;
				Wait(2);
				bRC7 = 0;
				bRC1 = 0;
				Wait(2);
			}
			Wait(Resultaat - 100);
			//1000/120/60 = 500ms => 500-100= 400ms
			//Er moet 100ms afgetrokken worden van de Waitwaarde  
			//omdat de lagere toon meer tijd in beslag neemt.
		}
		bRC2 = 0; bRC4 = 0; bRC0 = 0; bRB1 = 0;
	}
}
