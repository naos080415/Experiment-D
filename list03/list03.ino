#include <Servo.h>
#define SW 5    /* タクトスイッチのポート番号 */
#define LED 10

Servo mServo1;
Servo mServo2;
Servo mServo3;

long ad0,ad1,ad2;
int init_val1,init_val2,init_val3;
int theta1[6] = {135,135,136,135,135,135};
int theta2[6] = {62,61,79,97,120,120};
int theta3[6] = {179,125,179,179,179,137};

/* 2回目計測時
Moter = (135,62,179)1
Moter = (135,61,125)2
Moter = (136,79,179)3
Moter = (135,97,179)4
Moter = (135,120,179)5
Moter = (135,120,137)6

*/
/*　1回目計測時
int theta1[6] = {121,121,121,121,121,121};
int theta2[6] = {76,76,101,115,132,132};
int theta3[6] = {143,179,179,179,179,123};

*/
/*
theta1[0] = 121;
theta2[0] = 76;
theta3[0] = 143;

theta1[1] = 121;
theta2[1] = 76;
theta3[1] = 179;

theta1[2] = 121;
theta2[2] = 101;
theta3[2] = 179;

theta1[3] = 121;
theta2[3] = 115;
theta3[3] = 179;

theta1[4] = 121;
theta2[4] = 132;
theta3[4] = 179;

theta1[5] = 121;
theta2[5] = 132;
theta3[5] = 123;
*/

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
    int flag = 0;
    delay(2000);
    digitalWrite(LED,OUTPUT);
    while(1){
        /* ボリュームのポート番号を設定する */     
        ad0 = analogRead(0);    /* ボリュームセンサ1 */
        ad1 = analogRead(1);    /* ボリュームセンサ2 */
        ad2 = analogRead(2);    /* ボリュームセンサ3 */

        init_val1 = ( ad0 * 180 / 1024);
        init_val2 = ( ad1 * 180 / 1024);
        init_val3 = ( ad2 * 180 / 1024);

/*
        mServo1.write(init_val1);
        mServo2.write(init_val2);
        mServo3.write(init_val3);
*/
        if( digitalRead(SW) == LOW ){
            /* スイッチON */
            delay(100);
            if( digitalRead(SW) == LOW )
                break;
        }
    }
    delay(5000);
    mServo1.write(theta1[0]);
    mServo2.write(theta2[0]);
    mServo3.write(theta3[0]);
    delay(3000);

    for(int i = 1;i < 6;i++){
        mServo1.write(theta1[i]);
        mServo2.write(theta2[i]);
        mServo3.write(theta3[i]);
        delay(1000);
    }

    while(1){
        /* 無限ループ：処理終了 */
        digitalWrite(LED,LOW);
    }
    
}