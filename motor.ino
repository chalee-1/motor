#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif


#include <ArduinoHardware.h>
#include <ros.h> 
#include <geometry_msgs/Twist.h> 
#include <std_msgs/Float32.h> 
#include <Encoder.h>


#define INA 8
#define INB 9
#define INA1 10
#define INB1 11


Encoder myEncL(2,3);
Encoder myEncR(4,5);

long oldPositionL  = 0;
long oldPositionR  = 0;

unsigned long g_prev_command_time = 0;

ros::NodeHandle nh;



geometry_msgs::Twist msg;
std_msgs::Float32 encL_msg;
std_msgs::Float32 encR_msg;


ros::Publisher EncL("Enc_L", &encL_msg);
ros::Publisher EncR("Enc_R", &encR_msg);

  long newPositionL;
  long newPositionR;
 
  
void roverCallBack(const geometry_msgs::Twist& cmd_vel)
{

  double x = cmd_vel.linear.x;
        double z = cmd_vel.angular.z;

  double moveL = x+(z/2);
  double moveR = x-(z/2);

  
if (moveL>0.0){
      analogWrite(INB,max(min(moveL*100,255),0));
      analogWrite(INA,0);
      analogWrite(INA1,max(min(moveL*100,255),0));
      analogWrite(INB1,0);
    }else if (moveL<0.0){
        analogWrite(INA,max(min(abs(moveL)*100,255),0));
        analogWrite(INB,0);
        analogWrite(INB1,max(min(abs(moveL)*100,255),0));
        analogWrite(INA1,0);
    }else{ 
  analogWrite(INA,0);
         analogWrite(INA,1);analogWrite(INB,1);
 analogWrite(INA1,0);
         analogWrite(INA1,1);analogWrite(INB1,1);
  }


  g_prev_command_time = millis();
 
}
ros::Subscriber <geometry_msgs::Twist> Motor("/cmd_vel", roverCallBack);

void setup()
{
  pinMode(INA,OUTPUT);  pinMode(INB,OUTPUT); 
  
  
  nh.initNode();
  nh.subscribe(Motor);
  nh.advertise(EncL);
  nh.advertise(EncR);
  
} 

void loop()
{
    newPositionL = myEncL.read();
    newPositionR = myEncR.read();

  if (newPositionL != oldPositionL) {
    oldPositionL = newPositionL;
     encL_msg.data = newPositionL;
  }
  if (newPositionR != oldPositionR) {
    oldPositionR = newPositionR;
     encR_msg.data = newPositionR;
  }
 
  
  if((newPositionL>10000)||(newPositionL<-10000)||(newPositionR>10000)||(newPositionR<-10000)){
  myEncL.write(0);
  myEncR.write(0);
 
  }
  EncL.publish( &encL_msg );
  EncR.publish( &encR_msg );

  if ((millis() - g_prev_command_time) >= 400)
    {
        analogWrite(INA,0);
         analogWrite(INA,1);analogWrite(INB,1);
        analogWrite(INA1,0);
         analogWrite(INA1,1);analogWrite(INB1,1);
    }
 
  nh.spinOnce();
  delay(100);
}
