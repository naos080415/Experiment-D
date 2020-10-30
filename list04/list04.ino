#include <Servo.h>

#define SW 5    /* タクトスイッチのポート番号 */
#define LED 10

Servo mServo1;
Servo mServo2;
Servo mServo3;

long ad0,ad1,ad2;
/* ボリュームセンサからの角度調整用変数 */
int ad_theta1,ad_theta2,ad_theta3;

/* 事前に測定したデータを代入する配列 */
int data_theta1[10];
int data_theta2[10];
int data_theta3[10];

/* シリアルポートから角度を操作する変数 */
int serial_theta1,serial_theta2,serial_theta3;

void setup()
{
    /* サーボモータのI/Oポートを入力する */
    mServo1.attach(3);  /* 第1関節サーボモーター */
    mServo2.attach(6);  /* 第2関節サーボモーター */
    mServo3.attach(9);  /* 第3関節サーボモーター */
    pinMode(SW,INPUT);
    pinMode(LED,OUTPUT);
    Serial.begin(9600);
}

void loop(){
    Serial.println("Program Start");
    delay(2000);
    int i = 0,j = 0;
    int flag = 0;
    int mode = 0;
    int serial_num = 0;
    int weight = 100;
    char serial_string[12];
    serial_theta1 = 90;
    serial_theta2 = 90;
    serial_theta3 = 90;

    while(1){
        ad0 = analogRead(0);    /* ボリュームセンサ1 */
        ad1 = analogRead(1);    /* ボリュームセンサ2 */
        ad2 = analogRead(2);    /* ボリュームセンサ3 */

        ad_theta1 = ( ad0 * 180 / 1024);
        ad_theta2 = ( ad1 * 180 / 1024);
        ad_theta3 = ( ad2 * 180 / 1024);

        switch (mode)
        {
        case 0:
            Serial.println("init_servo(mode=0)");
            /* LEDの点灯及び,サーボーモータの初期値代入 */
            digitalWrite(LED,OUTPUT);
            mServo1.write(90);
            mServo2.write(90);
            mServo3.write(90);
            mode = 1;
            break;
        
        case 1:
            delay(100);
            Serial.println("Mode Setting");
            Serial.println("Input Mode(1-ad　　2-serial　　　3-data)");
            mode = 2;
            break;
        
        case 2:
            if( Serial.available() ){
                serial_num = Serial.read() - '0';
            }
            if( serial_num == 1 || serial_num == 2 || serial_num == 3 )
                mode = serial_num * 10;
            break;
        case 10:

            break;

        case 20:
            /* シリアルモニターに角度を送信すると,その角度まで移動するプログラム */
            Serial.println("Serial-Control");
            mode = 21;            
            break;
        
        case 21:
            /* 変数と配列を初期化する */
            for(i=0;i<12;i++){
                serial_string[i] = 0;
            } 
            i = 0;
            j = 0;
            mode = 22;
            break;

        case 22:
            if( Serial.available() ){
                serial_string[i] = char(Serial.read());
                if( serial_string[i] == ';' )
                    mode = 23;
                i++;
            }
            break;

        case 23:
            serial_theta1 = 0;
            serial_theta2 = 0;
            serial_theta3 = 0;
            mode = 24;            
            break;

        case 24:
            for(i=0;i<3;i++){
                serial_theta1 += (serial_string[i] - '0')*weight;
                weight /= 10;
            }
            for(i=4;i<7;i++){
                serial_theta2 += (serial_string[i] - '0')*weight;
                weight /= 10;
            }
            for(i=8;i<11;i++){
                serial_theta3 += (serial_string[i] - '0')*weight;
                weight /= 10;
            }

            mode = 25;
            break;

        case 25:                    
            mServo1.write(serial_theta1);
            mServo2.write(serial_theta2);
            mServo3.write(serial_theta3);
            break;

        case 30:


            break;        

        default:

            break;
        }
    }
    
}
