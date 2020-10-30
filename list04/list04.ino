#include <Servo.h>

#define SW 5    /* タクトスイッチのポート番号 */
#define LED 10

Servo mServo1;
Servo mServo2;
Servo mServo3;

/* 2回目計測時
Moter = (135,62,179)1
Moter = (135,61,125)2
Moter = (136,79,179)3
Moter = (135,97,179)4
Moter = (135,120,179)5
Moter = (135,120,137)6
*/

/* 1回目計測時
int theta1[6] = {121,121,121,121,121,121};
int theta2[6] = {76,76,101,115,132,132};
int theta3[6] = {143,179,179,179,179,123};
*/

void motor_print(int val1,int val2,int val3){
    Serial.print("Moter = (");
    Serial.print(val1);
    Serial.print(",");
    Serial.print(val2);
    Serial.print(",");
    Serial.print(val3);
    Serial.println(")");
}

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
    long ad0,ad1,ad2;

    /* ボリュームセンサからの角度調整用変数 */
    int ad_theta1,ad_theta2,ad_theta3;

    /* 事前に測定したデータを代入する配列 */
    int data_theta1[10];
    int data_theta2[10];
    int data_theta3[10];

    /* シリアルポートから角度を操作する変数 */
    int serial_theta1,serial_theta2,serial_theta3;
    
    /* モータ角度保存用のbuff */
    int buff1[13],buff2[13],buff3[13];
    /* buffの保存数カウント用変数 */
    int buff_cnt = 0;

    int i = 0,j = 0;
    int flag = 0;
    int mode = 0;
    int serial_num = 0;
    int weight = 100;
    char serial_string[13];
    serial_theta1 = 90;
    serial_theta2 = 90;
    serial_theta3 = 90;

    delay(2000);
    while(1){


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
            Serial.println("Input Mode(1-ad　　2-serial　　　3-data  4-sensor_check)");
            mode = 2;
            break;
        
        case 2:
            if( Serial.available() ){
                serial_num = Serial.read() - '0';
            }
            if( serial_num == 1 || serial_num == 2 || serial_num == 3 || serial_num == 4)
                mode = serial_num * 10;
            break;

        case 10:
            /* ボリュームを回すと,その角度まで移動するプログラム */
            Serial.println("Volume-Control");
            mode = 11;
            break;

        case 11:
            /* AD変換は時間がかかるので,case 文に入れておく */
            ad0 = analogRead(0);    /* ボリュームセンサ1 */
            ad1 = analogRead(1);    /* ボリュームセンサ2 */
            ad2 = analogRead(2);    /* ボリュームセンサ3 */

            ad_theta1 = ( ad0 * 180 / 1024);
            ad_theta2 = ( ad1 * 180 / 1024);
            ad_theta3 = ( ad2 * 180 / 1024);
            mServo1.write(ad_theta1);
            mServo2.write(ad_theta2);
            mServo3.write(ad_theta3);

            if( flag == 0 && digitalRead(SW) == LOW ){
                /* スイッチON */
                delay(100);
                if( flag == 0 && digitalRead(SW) == LOW ){
                    flag = 1;
                    motor_print(ad_theta1,ad_theta2,ad_theta3);
                }
            }else if( flag == 1 && digitalRead(SW) == HIGH ){
                delay(100);
                if( flag == 1 && digitalRead(SW) == HIGH )
                    flag = 0;
            }
            break;

        case 20:
            /* シリアルモニターに角度を送信すると,その角度まで移動するプログラム */
            Serial.println("Serial-Control");
            for(i=0;i<13;i++){
                buff1[i] = 0;
                buff2[i] = 0;
                buff3[i] = 0;
            }
            mode = 21;            
            break;
        
        case 21:
            /* 変数と配列を初期化する */
            for(i=0;i<13;i++){
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
            weight = 100;
            for(i=0;i<3;i++){
                serial_theta1 += (serial_string[i] - '0')*weight;
                weight /= 10;
            }

            weight = 100;
            for(i=4;i<7;i++){
                serial_theta2 += (serial_string[i] - '0')*weight;
                weight /= 10;
            }

            weight = 100;
            for(i=8;i<11;i++){
                serial_theta3 += (serial_string[i] - '0')*weight;
                weight /= 10;
            }

            motor_print(serial_theta1,serial_theta2,serial_theta3);
            mode = 25;
            break;

        case 25:                    
            mServo1.write(serial_theta1);
            mServo2.write(serial_theta2);
            mServo3.write(serial_theta3);

            if( serial_string[11] == 's' || serial_string[11] == 'v' ){
                /* セーブモードになり,モータの角度をbuffに保存する. */
                buff1[buff_cnt] = serial_theta1;
                buff2[buff_cnt] = serial_theta2;
                buff3[buff_cnt] = serial_theta3;
                buff_cnt ++;
            }

            if( serial_string[11] == 'v' ){
                /* セーブモードになり,モータの角度をbuffに保存し,これまでのbuffの結果を出力. */
                mode = 80;
                break;
            }
            mode = 21;
            break;

        case 30:

            break;        

        case 40:
            
            break;

        case 80:
            /* buff書き出し命令 */
            Serial.println("buff Output");
            Serial.print("data_theta1[");
            Serial.print(buff_cnt);
            Serial.print("] = {");
            for(i=0;i<(buff_cnt-1);i++){
                Serial.print(buff1[i]);
                Serial.print(",");
            }
            Serial.print(buff1[buff_cnt-1]);
            Serial.println("};");

            Serial.print("data_theta2[");
            Serial.print(buff_cnt);
            Serial.print("] = {");
            for(i=0;i<(buff_cnt-1);i++){
                Serial.print(buff2[i]);
                Serial.print(",");
            }
            Serial.print(buff2[buff_cnt-1]);
            Serial.println("};");

            Serial.print("data_theta3[");
            Serial.print(buff_cnt);
            Serial.print("] = {");
            for(i=0;i<(buff_cnt-1);i++){
                Serial.print(buff3[i]);
                Serial.print(",");
            }
            Serial.print(buff3[buff_cnt-1]);
            Serial.println("};");
            mode = 90;
            break;
            
        case 90:
            /* プログラム終了処理 */
            Serial.println("Program exit");
            digitalWrite(LED,LOW);
            mode = 91;
            break;
        case 91:
            /* 何もしない */
            break;        

        default:

            break;
        }
    }
    
}
