

#include "hw_config.h"
#include <math.h>
#include "lcd_interface.h"
#include "format_print.h"
#include "calc_rc.h"
#include "dac.h"
#include "menu.h"
#include "vbat.h"
#include "corrector.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int printD = 0; //debug
float printGradus = -1000;
float Quality;
bool printError = false;
uint8_t type_screen = 0;
extern uint8_t SSD1306_on;
double Rre = 0;
double Rim = 0;

double RreS = 0;
double RimS = 0;

double RreP= 0;
double RimP= 0;

double RreS_vivod = 0;
double RimS_vivod = 0;

double RreP_vivod = 0;
double RimP_vivod = 0;

double Rre_vivod = 0;
double Rim_vivod = 0;

double valueL_vivod = 0;
double valueC_vivod = 0;

double valueL = 0;
double valueC = 0;

uint16_t countSumma = 0;
uint8_t usrednenie;

bool printRim = false; 
bool isSerial = true;
bool valueIsC = true;
bool calculatedValues = false;
extern uint8_t filtr_on;
bool auto_regim = true;
extern bool startFast;

void printLcdFrequency()
{	
		LcdGotoXYFont(16, 1);
	  if (auto_regim)  LcdStr(FONT_1X, "A");
    LcdGotoXYFont(17, 1);
    LcdStr(FONT_1X, isSerial?"S":"P");	
    printX2size(printError?FONT_1X:FONT_2X);	
	
	  LcdGotoXYFont(1,1);
	  LcdStr(FONT_1X, "uRLC ");
		
    float f = DacFrequency();
    if(f<999.5f)
    {
        printInt(round(f), FONT_1X);
        //LcdStr(FONT_1X, "Hz");
        //return;
    }
    else if(f<1e6f)
    {
        int32_t fi = round(f*1e-3f);
        if(fi*1000==f)
        {
            printInt(fi, FONT_1X);
        } else
        {
            //printIntFixed(round(f*1e-2f), FONT_1X, 2, 1);
					  printInt(round(f*1e-3f), FONT_1X);
        }

        LcdStr(FONT_1X, "k");
        //return;
    }
		
		LcdGotoXYFont(10,1);
		if (DacGetDelitel()==1) 	     LcdStr(FONT_1X, " 1.0V");
		else if (DacGetDelitel()==3) 	 LcdStr(FONT_1X, " 0.3V");	
		else 	                         LcdStr(FONT_1X, " 0.1V");

    //printF(f);
}


extern uint8_t grafic_u[110];
extern uint8_t grafic_i[110];
extern uint8_t i_grafic;
extern uint8_t Measure_ok;
extern complexf Zx;
complexf Zx_vivod;
extern uint16_t filter_lcd;
void LcdRepaint()
{
	if (SSD1306_on==0) return;
	
	if ((usrednenie==0)||(filter_lcd==0)){
		 valueL_vivod= valueL;
     valueC_vivod= valueC;			
	   Rre_vivod =Rre;
     Rim_vivod = Rim;
	   RreS_vivod = RreS;
     RimS_vivod = RimS;		
	   RreP_vivod = RreP;
     RimP_vivod = RimP;				
		 Zx_vivod=Zx;
     countSumma=0;		
	}	
	
	if (filter_lcd==1){
  if (calculatedValues)  {	
     usrednenie=1; 

		 if (countSumma<19) countSumma++;
		
	   Rre_vivod = 0.05*countSumma*Rre_vivod+0.05*(20-countSumma)*Rre;
     Rim_vivod = 0.05*countSumma*Rim_vivod+0.05*(20-countSumma)*Rim;
		 valueL_vivod= 0.05*countSumma*valueL_vivod+0.05*(20-countSumma)*valueL;
     valueC_vivod= 0.05*countSumma*valueC_vivod+0.05*(20-countSumma)*valueC;	 
		 Zx_vivod= 0.05*countSumma*Zx_vivod+0.05*(20-countSumma)*Zx;
		
	   RreS_vivod = 0.05*countSumma*RreS_vivod+0.05*(20-countSumma)*RreS;
     RimS_vivod = 0.05*countSumma*RimS_vivod+0.05*(20-countSumma)*RimS;		
	   RreP_vivod = 0.05*countSumma*RreP_vivod+0.05*(20-countSumma)*RreP;	
     RimP_vivod = 0.05*countSumma*RimP_vivod+0.05*(20-countSumma)*RimP;		 	 

 }
	else {
		 usrednenie=0; 
		 Zx_vivod=Zx;
		 valueL_vivod=valueL;
		 valueC_vivod=valueC;		
     Rre_vivod = Rre;
     Rim_vivod = Rim;	
	   RreS_vivod = RreS;
     RimS_vivod = RimS;		
	   RreP_vivod = RreP;
     RimP_vivod = RimP;			
		 countSumma=0;				
	}
	}
		
    LcdClear();	
    if(MenuIsOpen())
    {
        MenuRepaint();
        LcdUpdate();
        return;
    }			
    printLcdFrequency();		
    if(GetCorrector()->period==0)
    {
        LcdSingleBar( 0, 9, 9, 103, PIXEL_XOR );
    }
		
if (type_screen==0){
			
		printX2size(2);
	
	  float potolok;
		int F = (int)DacFrequency(); // они зависят от частоты
	
	  if (F>=187000){
      potolok=15e+6f;
	  }
	  else 	if (F>=93000){
      potolok=22e+6f;
	  }
	  else 	if (F>=10000){
      potolok=60e+6f;
	  }	
	  else 	if (F>=1000){
      potolok=150e+6f;
			
	  }		
	  else 	if (F>=100){
      potolok=120e+6f;
	  }	

				
		if (Rre>potolok) {
				}
		else if (Rre<-9.9e+6f) {			
				}
    else {	
			
				printRX2(Rre_vivod,11, 2);
					
				}
        if(printRim)
        {
            printRX2(Rim_vivod,11, 5);		
        } else
        {
            if(valueIsC)
            {
                printCX2(valueC_vivod,11, 5);
            } else
            {
                printLX2(valueL_vivod,11, 5);
            }
        }					

	}
//=============================================================
		else if (type_screen==1){ 

		  int vmin = g_data.ch_v.adc_min;
		  int vmax = g_data.ch_v.adc_max;
		  int imin = g_data.ch_i.adc_min;
		  int imax = g_data.ch_i.adc_max;
			uint8_t u_y=0;
			uint8_t i_y=0;
			uint8_t u_y_old=0;
			uint8_t i_y_old=0;
					
      for (uint8_t x=0; x<i_grafic-1; x++){
					u_y_old=46-((grafic_u[x]*36)>>8);
					i_y_old=46-((grafic_i[x]*36)>>8);
					u_y=46-((grafic_u[x+1]*36)>>8);
					i_y=46-((grafic_i[x+1]*36)>>8);		
							
					LcdLine(x, x+1, u_y_old, u_y, 1);
					LcdLine(x, x+1, i_y_old, i_y, 1);
			}

				
				printX2size(1);
        printRX2(Rre_vivod,11, 7);     
        if(printRim)
        {
            printRX2(Rim_vivod,11, 8);
        } else
        {
            if(valueIsC)
            {
                printCX2(valueC_vivod,11, 8);
            } else
            {
                printLX2(valueL_vivod,11, 8);
            }
        }					

            LcdGotoXYFont(1, 7);
            LcdStr(FONT_1X, "dU");
            LcdGotoXYFont(4, 7);
            printInt(ADC_STM32_Ref*1000*(vmax-vmin)/4096, FONT_1X);
            LcdStr(FONT_1X, "mV"); 

            LcdGotoXYFont(1, 8);
            LcdStr(FONT_1X, "dI");
            LcdGotoXYFont(4, 8);
            printInt(ADC_STM32_Ref*1000*(imax-imin)/4096, FONT_1X );
            LcdStr(FONT_1X, "mV");  

            LcdGotoXYFont(20,4);
						if (filtr_on){	
              LcdStr(FONT_1X, "F");
              printInt(GetLowPassFilter(), FONT_1X);					
						}
						else LcdStr(FONT_1X, " ");
			
            LcdGotoXYFont(20,5);
            LcdStr(FONT_1X, "R");
            printInt(resistorIdx, FONT_1X);
            LcdGotoXYFont(20,6);		
            LcdStr(FONT_1X, "V");
            printInt(gainVoltageIdx, FONT_1X); 
            LcdGotoXYFont(20,7);		 
            LcdStr(FONT_1X, "I");
            printInt(gainCurrentIdx, FONT_1X);
//		}
	}
//=================================================================
			else if (type_screen==2){ 
				char text[32];
				
				printX2size(1);
 	      LcdGotoXYFont(12, 3);
        LcdStr(FONT_1X, "Z");		
        double z;				 	
				z =	fabs(RreS_vivod)+fabs(RimS_vivod);       
				printRX2(z, 14, 3);
								
 	      LcdGotoXYFont(1, 3);
        LcdStr(FONT_1X, "Rs");	 				
        if (RreS_vivod<0)printRX2(RreS_vivod, 3, 3);  
				else printRX2(RreS_vivod, 4, 3);  
				
				
 	      LcdGotoXYFont(1, 4);
        LcdStr(FONT_1X, "Rp");	 				
        if (RreP_vivod<0)printRX2(RreP_vivod, 3, 4);  
				else printRX2(RreP_vivod, 4, 4);  
					
 	      LcdGotoXYFont(12, 4);
        LcdStr(FONT_1X, "X");	 				
        if (Rim_vivod<0){
					if ((printGradus>-90)&&(printGradus<=0))printRX2(-Rim_vivod, 14, 4);
					else printRX2(Rim_vivod, 13, 4);
				}					
				else printRX2(Rim_vivod, 14, 4);  				
				
        if(valueIsC)
        {
 	        LcdGotoXYFont(1, 5);
          LcdStr(FONT_1X, "C");						
          printCX2(valueC_vivod, 4, 5);
        } else
        {
 	        LcdGotoXYFont(1, 5);
          LcdStr(FONT_1X, "L");							
          printLX2(valueL_vivod, 4, 5);
        }
				extern double error_Re;
        extern double error_Rim;	 
			 
        LcdGotoXYFont(12, 6);
        LcdStr(FONT_1X, "Re%");
        LcdGotoXYFont(15, 6);
				if (error_Re<99) printIntFixed(round(error_Re*1000), FONT_1X, 4, 3);	
       	else printInt(round(error_Re), FONT_1X);	
				
 	      LcdGotoXYFont(12, 7);
        LcdStr(FONT_1X, "Im%");
        LcdGotoXYFont(15, 7);
        if (error_Rim<99) printIntFixed(round(error_Rim*1000), FONT_1X, 4, 3);
				else printInt(round(error_Rim), FONT_1X);	
				
				int vmin = g_data.ch_v.adc_min;
		    int vmax = g_data.ch_v.adc_max;
		    int imin = g_data.ch_i.adc_min;
		    int imax = g_data.ch_i.adc_max;
				
        LcdGotoXYFont(1, 6);
        LcdStr(FONT_1X, "dU");
        LcdGotoXYFont(4, 6);
        printInt(ADC_STM32_Ref*1000*(vmax-vmin)/4096, FONT_1X);
        LcdStr(FONT_1X, "mV"); 


        LcdGotoXYFont(1, 7);
        LcdStr(FONT_1X, "dI");
        LcdGotoXYFont(4, 7);
        printInt(ADC_STM32_Ref*1000*(imax-imin)/4096, FONT_1X );
        LcdStr(FONT_1X, "mV");  
				
        LcdGotoXYFont(10,8);
				if (filtr_on){	
              LcdStr(FONT_1X, "F");
              printInt(GetLowPassFilter(), FONT_1X);					
			  }
				else LcdStr(FONT_1X, " ");
			
        LcdGotoXYFont(13,8);
        LcdStr(FONT_1X, "R");
        printInt(resistorIdx, FONT_1X);
        LcdGotoXYFont(16,8);		
        LcdStr(FONT_1X, "V");
        printInt(gainVoltageIdx, FONT_1X); 
        LcdGotoXYFont(19,8);		 
        LcdStr(FONT_1X, "I");
        printInt(gainCurrentIdx, FONT_1X);	
				
				
				printX2size(1);
				LcdGotoXYFont(1,8);
				float printGradus_temp;
				if (printGradus<0)printGradus_temp=-printGradus;
				else printGradus_temp=printGradus;
				
				if (printGradus_temp<1)sprintf(text,"%0.4f{", printGradus); 
				else if (printGradus_temp<10)sprintf(text,"%0.3f{", printGradus); 
				else if (printGradus_temp<100)sprintf(text,"%0.2f{", printGradus);  
				else sprintf(text,"%0.1f{", printGradus); 					
	      LcdStr(FONT_1X, text);
								
				
        LcdGotoXYFont(12,5);	
				if(valueIsC)
        {
          LcdStr(FONT_1X, "D");		
				  Quality=  RreS_vivod/RimS_vivod ;	
					if (Quality<0)Quality=-Quality;
					 LcdGotoXYFont(13, 5);					 					
					 if (Quality<1)sprintf(text,"%0.4f", Quality); 
					 else if (Quality<10)sprintf(text,"%0.3f", Quality); 
					 else if (Quality<100)sprintf(text,"%0.2f", Quality);  
					 else sprintf(text,"%0.1f", Quality); 					
	         LcdStr(FONT_1X, text);
					
				}
				else 
        {
          LcdStr(FONT_1X, "Q");		
				  Quality=  RimS_vivod/RreS_vivod ;
					if (Quality<0)Quality=-Quality;

					 LcdGotoXYFont(13, 5);
					 if (Quality<1)sprintf(text,"%0.4f", Quality); 
					 else if (Quality<10)sprintf(text,"%0.3f", Quality); 
					 else if (Quality<100)sprintf(text,"%0.2f", Quality);  
					 else sprintf(text,"%0.1f", Quality); 
					 LcdStr(FONT_1X, text);

				}					
		  
			}
	
	
    VBatQuant();		
    LcdUpdate();
}


//=================================================================
void LcdRepaint_RVI(void)
{   if(MenuIsOpen())
    {
        return;
    }

if (type_screen==1){
            LcdGotoXYFont(20,4);
						if (filtr_on){	
              LcdStr(FONT_1X, "F");
              printInt(GetLowPassFilter(), FONT_1X);					
						}
						else LcdStr(FONT_1X, " ");
			
            LcdGotoXYFont(20,5);
            LcdStr(FONT_1X, "R");
            printInt(resistorIdx, FONT_1X);
            LcdGotoXYFont(20,6);		
            LcdStr(FONT_1X, "V");
            printInt(gainVoltageIdx_temp, FONT_1X); 
            LcdGotoXYFont(20,7);		 
            LcdStr(FONT_1X, "I");
            printInt(gainCurrentIdx_temp, FONT_1X);
						
					  LcdUpdate();
					}
						
}
//=================================================================
