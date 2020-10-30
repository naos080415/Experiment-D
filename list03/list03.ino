#include <Servo.h>
#define SW 5    /* タクトスイッチのポート番号 */
#define LED 10

Servo mServo1;
Servo mServo2;
Servo mServo3;

long ad0,ad1,ad2;
int theta1[],theta2[],theta3[];

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

    digitalWrite(LED,OUTPUT);
    while(1){
        if( digitalRead(SW) == LOW ){
            /* スイッチON */
            delay(100);
            if( digitalRead(SW) == LOW ){
                break;
    }

    for(int i = 0;i < sizeof(theta1);i++){
        mServo1.write(theta1[i]);
        mServo2.write(theta2[i]);
        mServo3.write(theta3[i]);
    }

    while(1){
        /* 無限ループ：処理終了 */
        digitalWrite(LED,LOW);
    }
    
}