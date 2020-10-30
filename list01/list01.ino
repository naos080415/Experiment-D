#include <Servo.h>
#define SW 5    /* タクトスイッチのポート番号 */
#define LED 10

Servo mServo1;
Servo mServo2;
Servo mServo3;

long ad0,ad1,ad2;
int theta1,theta2,theta3;

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
        /* ボリュームのポート番号を設定する */     
        ad0 = analogRead(0);    /* ボリュームセンサ1 */
        ad1 = analogRead(1);    /* ボリュームセンサ2 */
        ad2 = analogRead(2);    /* ボリュームセンサ3 */

        theta1 = ( ad0 * 180 / 1024);
        theta2 = ( ad1 * 180 / 1024);
        theta3 = ( ad2 * 180 / 1024);

        mServo1.write(theta1);
        mServo2.write(theta2);
        mServo3.write(theta3);

        if( flag == 0 && digitalRead(SW) == LOW ){
            /* スイッチON */
            delay(100);
            if( flag == 0 && digitalRead(SW) == LOW ){
                flag = 1;
                Serial.print("Moter = (");
                Serial.print(theta1);
                Serial.print(",");
                Serial.print(theta2);
                Serial.print(",");
                Serial.print(theta3);
                Serial.println(")");
            }
        }else if( flag == 1 && digitalRead(SW) == HIGH ){
            delay(100);
            if( flag == 1 && digitalRead(SW) == HIGH )
                flag = 0;
        }
    }

    while(1){
        /* 無限ループ：処理終了 */
        digitalWrite(LED,LOW);
    }
    
}