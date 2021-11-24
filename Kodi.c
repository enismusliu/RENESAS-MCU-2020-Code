#include"iodefine.h"
#define PWM_CYCLE       24575           // Motor PWM period (16ms)
#define SERVO_CENTER    2240           // Servo center value
#define HANDLE_STEP     13
#define MASK3_3        0xE7
#define MASK4_4        0xff
void Init_Clock();
void Init_PWM();
unsigned char sensor_leximi( unsigned char mask );
void LeftM_Velocity(signed short Percentage);
void RightM_Velocity(signed short Percentage);
void ServoAngle(int angle);
void timer( unsigned long timer_set );
int check_line( void );
unsigned long   cnt0;
unsigned long   cnt1;
int             pattern=0;
int flagBit20=0;
int flagBit04=0;
int LineDetect =0;
volatile unsigned int Time_Cnt = 0;
int flag=0;
int delay_disable=0;
int disable_cross=0;
int count_time=0;
int time_xc=0;
int k=0;
int main()
{
	Init_Clock();
	Init_PWM();

	while(1)
	{

		switch (pattern)
		{
						     case 0:     // normal trace
								      if((disable_cross==0) && (check_line()) ) {                // kontrollo per vije te bardhe horizontale
								                pattern = 10;                 // nese ka shko tek patterna 11 dhe dil nga case 0
								                break;
								      }
								switch( sensor_leximi(MASK3_3) ) {         // nese nuk ka vije te bardhe horizontale normal trace
								            case 0x00:
												ServoAngle(0);
												 LeftM_Velocity(75);
												 RightM_Velocity(75);
												break;
											case 0x04:
												ServoAngle(5);
												 LeftM_Velocity(70);
											     RightM_Velocity(70);
												break;

								            case 0x06:
								            	ServoAngle(18);
								            	LeftM_Velocity(50);
								                RightM_Velocity(28);
								               break;

								                case 0x07:
								                	ServoAngle(23);
									            	LeftM_Velocity(40);
									            	 RightM_Velocity(23);

								                    break;

								                case 0x03:

								                	ServoAngle(29);
									            	LeftM_Velocity(30);
									            	RightM_Velocity(17);

									            	pattern=1;

								                    break;

								                case 0x20:
								                	ServoAngle(-5);

									            	LeftM_Velocity(70);
									            	 RightM_Velocity(70);

								                    break;

								                case 0x60:
								                	ServoAngle(-18);

									            	LeftM_Velocity(28);
									             RightM_Velocity(50);

								                    break;

								                case 0xe0:

								                	ServoAngle(-23);

									            	LeftM_Velocity(23);
									            	RightM_Velocity(40);

								                    break;

								                case 0xc0:

								                	ServoAngle(-29);
									            	LeftM_Velocity(17);
									                RightM_Velocity(30);

								                    pattern = 2;
								                    break;

								                default:

									            	break;
								            }

						    case 1:
						    	if((disable_cross==0) && (check_line()) ) {

						    	                pattern = 10;
						    	                break;
						    	}

						    	if( sensor_leximi(MASK3_3) == 0x06 ) {         // nese nuk ka crossline- kthesa 30 shkalle
						    	                pattern = 0;                   // vazhdo ruaje gjendjen paraprake deri
						    	            }                                  // te arrihet rasti 0x06
						      break;
						    case 2:
						    	 if((disable_cross==0) && (check_line()) ) {

						    	                pattern = 10;
						    	                break;
						    	}

						    	 if( sensor_leximi(MASK3_3) == 0x60 ) {
						    	                pattern = 0;
						    	            }
						    	            break;
						    case 10:
						    	LineDetect |= sensor_leximi(MASK4_4);
						    	if(sensor_leximi(0x20)== 0x20){
						    		flagBit20=1;
						    	}
						    	if(sensor_leximi(0x04)== 0x04){
						    		flagBit04=1;
						    	}
						    	if(flagBit20==1 && sensor_leximi(0x20)==0){
						    		pattern =11;
						    	}
						    	if(flagBit04==1 && sensor_leximi(0x04)==0){
						    	pattern =11;
						    	}
						    	break;
						    case 11:
						    	if ( LineDetect > 0xfd)
						    	{
						    		ServoAngle(0);
						    		LeftM_Velocity(0);
						    		RightM_Velocity(0);
						    		pattern =21;
						    		cnt1=0;

						    	}
						    	else if (LineDetect==0xfc || LineDetect==0xf8 || LineDetect==0xf0 || LineDetect==0x78 || LineDetect==0xe0)  // Short Cross Left First Line Detected
						    					{
						    		                                ServoAngle(0);
						    								    		LeftM_Velocity(0);
						    								    		RightM_Velocity(0);
						    										cnt1=0;
						    										time_xc=0;
						    										pattern=52; //check leftline

						    					}

						    	else if (LineDetect==0x07 || LineDetect==0x0f || LineDetect==0x1f || LineDetect==0x1e || LineDetect==0x3f)  		// Short Cross Right First Line Detected
						    					{
						    		                                    ServoAngle(0);
						    								    		LeftM_Velocity(0);
						    								    		RightM_Velocity(0);
						    										cnt1=0;
						    										time_xc=0;
						    										pattern=42; // rightline

						    					}
						    	else
						    	{pattern=0;}
						    	break;

						    case 21 :
						    	if( cnt1 > 100 ){
                                pattern = 23;
                                cnt1 = 0;
                               }
                            break;

/*
						    case 21:
						    				if (sensor_leximi(MASK4_4)>251 || sensor_leximi(MASK4_4)==63 || sensor_leximi(MASK4_4)==126 || sensor_leximi(MASK4_4)==127) // Right/Left Turn Second Line Detected
						    				{pattern=22;}
						    				else																	// Right Turn Black Zone
						    				{LeftM_Velocity(10);
								    		RightM_Velocity(10);
						    					pattern=21;}
						    break;
						    case 22:
						    				if (sensor_leximi(MASK4_4)>251 || sensor_leximi(MASK4_4)==63 || sensor_leximi(MASK4_4)==126 || sensor_leximi(MASK4_4)==127) // Right/Left  Turn Second Line
						    				{pattern=22;}
						    				else																	// Right Turn Second Line Ended
						    				{pattern=23;}
						    break;          */
						    case 23:
						                // pasi kalojn 2 vijat duhet te detektojm kthesen e majt apo te djathte
						                if( sensor_leximi(MASK4_4)==240 || sensor_leximi(MASK4_4)==248 || sensor_leximi(MASK4_4)==224 || sensor_leximi(MASK4_4)==252 ) {
						                    pattern = 24;
						                    break;
						                }
						                if( sensor_leximi(MASK4_4)==15 || sensor_leximi(MASK4_4)==31 || sensor_leximi(MASK4_4)==7 || sensor_leximi(MASK4_4)==3) {
						                    // kthesa e djathte eshte detektuar me te gjithe sensoret apo 1f= 0001  1111
						                	pattern =30;
						                	break;

						                }       // nese ende nuk eshte detektuar kthesa te vazhdohet me ngadale
						                switch( sensor_leximi(MASK3_3) ) {
						                    case 0x00:
						                        // kit ndodhet ne qender vazhdohet drejt
						                    	ServoAngle(0);
						                    	LeftM_Velocity(35);
						                    	RightM_Velocity(35);
						                        break;
						                    case 0x04:                        // nese cilado prej ketyre case plotesohet
						                    case 0x06:                       // ekzekutohet kodi me posht
						                    case 0x07:
						                    case 0x03:
						                        // kit ndodhet pak ne te majt e kthejm ne qender
						                    	ServoAngle(3);
						                         LeftM_Velocity(30);
						                        RightM_Velocity(30);
						                        break;
						                    case 0x20:
						                    case 0x60:
						                    case 0xe0:
						                    case 0xc0:
						                        // pak ne te djathte
						                    	ServoAngle(-3);
						                    	LeftM_Velocity(30);
						                    	RightM_Velocity(30);
						                        break;
						                }
						                break;
						  case 24:
							  if(sensor_leximi(MASK4_4)==0)
							  {
								  pattern=25;
							  }
						                	break;
						  case 25:
							  ServoAngle(-40);
							  		LeftM_Velocity(0);
							  RightM_Velocity(35);




						  				if (sensor_leximi(MASK4_4)==192 || sensor_leximi(MASK4_4)==64 || sensor_leximi(MASK4_4)==128)
						  				{pattern=26;}
						  break;

						  case 26:
						  				if (sensor_leximi(MASK4_4)==0x20)
						  				{pattern=0;}
						  break;


						  case 30:
						  				if (sensor_leximi(MASK4_4)==0)
						  				{pattern=31;}
						  break;

						  case 31:	// Right
							  ServoAngle(47);
							  		LeftM_Velocity(51);
							  RightM_Velocity(0);
							  if (sensor_leximi(MASK4_4)==3 || sensor_leximi(MASK4_4)==2 || sensor_leximi(MASK4_4)==1)
							  						  				{pattern=32;}


						  break;

						  case 32:	// Right
						  				if (sensor_leximi(MASK4_4)==0x04 || sensor_leximi(MASK4_4)==0x06 )
						  				{pattern=0;}
						  break;

                        case 42:
                               // per 0.1 sekonda mendohet se kalohet vija e 2t
                             if( cnt1 > 100 ){
                                pattern = 43;
                                cnt1 = 0;
                               }
                            break;

                       case 43:
                             //
                            if( sensor_leximi(MASK4_4) == 0x00 ) {
                            ServoAngle( 20 );
                            LeftM_Velocity(40);
                            RightM_Velocity(31);
                            pattern = 44;
                           cnt1 = 0;
                           break;
                           }
                               switch( sensor_leximi(MASK3_3) ) {
                                                   case 0x00:
                                                            // qender- drejt
                                                            ServoAngle( 0 );
                                                            LeftM_Velocity( 30 );
                                                            RightM_Velocity( 30 );
                                                            break;
                                                  case 0x04:
                                                  case 0x06:
                                                  case 0x07:
                                                  case 0x03:
                                                          // pak ne te majt
                                                            ServoAngle( 8 );
                                                            LeftM_Velocity( 35 );
                                                            RightM_Velocity( 30 );
                                                            break;
                                                  case 0x20:
                                                  case 0x60:
                                                  case 0xe0:
                                                  case 0xc0:
                                                          // pak ne te djathte
                                                            ServoAngle( -8 );
                                                            LeftM_Velocity( 30 );
                                                            RightM_Velocity( 35 );
                                                            break;
                                                  default:
                                                    break;
                       }
                         break;

                    case 44:
                                 //
                            if( sensor_leximi(MASK4_4)==32 || sensor_leximi(MASK4_4)==64 || sensor_leximi(MASK4_4)==96 || sensor_leximi(MASK4_4)==112 || sensor_leximi(MASK4_4) == 192 ) {
                            	disable_cross=1;
                            	delay_disable=500;
                             pattern = 0;

                             }
                           break;

                   case 52:
                             // 0.1 s nuk lexojm
                            if( cnt1 > 100 ){
                               pattern = 53;
                           cnt1 = 0;
                             }
                           break;

                   case 53:
                           //
                           if( sensor_leximi(MASK4_4) == 0x00 ) {
                               ServoAngle( -15 );
                               LeftM_Velocity( 31 );
                               RightM_Velocity( 40 );
                               pattern = 54;
                               cnt1 = 0;
                               break;
                               }
                                  switch( sensor_leximi(MASK3_3) ) {
                                               case 0x00:
                                                        ServoAngle( 0 );
                                                        LeftM_Velocity( 30 );
                                                        RightM_Velocity( 30 );
                                                        break;
                                              case 0x04:
                                              case 0x06:
                                              case 0x07:
                                              case 0x03:
                                                        ServoAngle( 8 );
                                                        LeftM_Velocity( 30 );
                                                        RightM_Velocity( 25 );
                                                        break;
                                              case 0x20:
                                              case 0x60:
                                              case 0xe0:
                                              case 0xc0:
                                                        ServoAngle( -8 );
                                                        LeftM_Velocity( 25 );
                                                        RightM_Velocity( 30 );
                                                        break;
                                             default:
                                             break;
                     }
                      break;

                   case 54:

                	   if( sensor_leximi( MASK4_4 ) == 2 || sensor_leximi( MASK4_4 ) == 4 || sensor_leximi( MASK4_4 ) == 6 || sensor_leximi( MASK4_4 ) == 1 || sensor_leximi( MASK4_4 ) == 3 || sensor_leximi( MASK4_4 ) == 7) {
                		                              disable_cross=1; 					//  Disable Cross Detection for a delay, Please write the delay value on delay_disable [ms] variable
                		   					          delay_disable=500;
                		                              pattern = 0;

                	                                  }
                	                                  break;






                    default:
				    break;
			}



		}


	}


void Init_PWM()
{
	//Konfigurohen portet apo pinat e PWM
	PORT7.DDR.BYTE=0x7e;
    // MTU3_3 MTU3_4 PWM mode synchronized by RESET
    MSTP_MTU            = 0;                //Release module stop state
    MTU.TSTRA.BYTE      = 0x00;             //MTU Stop counting

    // Compare match timer
       MSTP_CMT0 = 0;                          //CMT Release module stop state
       MSTP_CMT2 = 0;                          //CMT Release module stop state

       ICU.IPR[0x04].BYTE  = 0x0f;             //CMT0_CMI0 Priority of interrupts
       ICU.IER[0x03].BIT.IEN4 = 1;             //CMT0_CMI0 Permission for interrupt
       CMT.CMSTR0.WORD     = 0x0000;           //CMT0,CMT1 Stop counting
       CMT0.CMCR.WORD      = 0x00C3;           //PCLK/512
       CMT0.CMCNT          = 0;
       CMT0.CMCOR          = 96;               //1ms/(1/(49.152MHz/512))
       CMT.CMSTR0.WORD     = 0x0003;           //CMT0,CMT1 Start counting

    MTU3.TCR.BYTE   = 0x23;                 //ILCK/64(651.04ns)
    MTU3.TCNT = MTU4.TCNT = 0;              //MTU3,MTU4TCNT clear
    MTU3.TGRA = MTU3.TGRC = PWM_CYCLE;      //cycle(16ms)
    MTU3.TGRB = MTU3.TGRD = SERVO_CENTER;   //PWM(servo motor)
    MTU4.TGRA = MTU4.TGRC = 0;              //PWM(left motor)
    MTU4.TGRB = MTU4.TGRD = 0;              //PWM(right motor)
    MTU.TOCR1A.BYTE = 0x40;                 //Selection of output level
    MTU3.TMDR1.BYTE  = 0x38;                 //TGRC,TGRD buffer function
                                            //PWM mode synchronized by RESET
    MTU4.TMDR1.BYTE  = 0x00;                 //Set 0 to exclude MTU3 effects
    MTU.TOERA.BYTE  = 0xc7;                 //MTU3TGRB,MTU4TGRA,MTU4TGRB permission for output

    MTU.TSTRA.BYTE  = 0x40;                 //MTU0,MTU3 count function
}

void Init_Clock()
{
	   // System Clock
	    SYSTEM.SCKCR.BIT.ICK = 0;               //12.288*8=98.304MHz
	    SYSTEM.SCKCR.BIT.PCK = 1;               //12.288*4=49.152MHz
}

void LeftM_Velocity(signed short Percentage )
{
	if(Percentage >=0){
		PORT7.DR.BYTE &= 0xef;
	   MTU4.TGRC =  ((PWM_CYCLE * Percentage)/100) ;
	}else
	{
		PORT7.DR.BYTE |= 0x10;
		MTU4.TGRC=  ((PWM_CYCLE * Percentage)/100) ;
	}
}

void RightM_Velocity(signed short Percentage)
{
	if(Percentage >=0){
			PORT7.DR.BYTE &= 0xdf;
		   MTU4.TGRD = ((PWM_CYCLE * Percentage)/100) ;
		}else
		{
			PORT7.DR.BYTE |= 0x20;
			MTU4.TGRD=  ((PWM_CYCLE * Percentage)/100) ;
		}
}




unsigned char sensor_leximi( unsigned char mask )
 {
	unsigned char sensor;

	 sensor = ~(PORT4.PORT.BYTE);

	 sensor &= mask;

	 return sensor;
 }
int check_line( void )
{
    unsigned char b;
    int ret;

    ret = 0;
    b = sensor_leximi(MASK4_4);
    if( b ==0xf8 || b==0x1f || b==0x0f || b==0x3f || b==0x7f || b==0xff) {
        ret = 1;
    }
    return ret;
}


void ServoAngle(int angle)
{
	 MTU3.TGRD = SERVO_CENTER - angle * HANDLE_STEP;
}
#pragma interrupt Excep_CMT0_CMI0(vect=28)
void Excep_CMT0_CMI0(void)
{
    cnt0++;
    cnt1++;
}
void timer( unsigned long timer_set )
{
    cnt0 = 0;
    while( cnt0 < timer_set );
}
#pragma interrupt (MTU1_Interrupt_1ms(vect=121))
void MTU1_Interrupt_1ms(void)
{
	MTU1.TSR.BIT.TGFA = 0;
	Time_Cnt++;
	if (disable_cross==1)
		{
			count_time++;
			if (count_time>delay_disable)
			{disable_cross=0;}
		}
	else
	{count_time=0;}



	time_xc++;

}