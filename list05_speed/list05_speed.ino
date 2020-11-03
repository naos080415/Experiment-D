/* シリアルモニタに日本語を使っているため,メモリ使用量が非常に大きくなる */
#include <VarSpeedServo.h>

#define SW 5    /* タクトスイッチのポート番号 */
#define LED 10

/* サーボモータの回転限度角 */
#define Moter1_lim_min 
#define Moter1_lim_max 
#define Moter2_lim_min 
#define Moter2_lim_max 
#define Moter3_lim_min 
#define Moter3_lim_max 

VarSpeedServo mServo1;
VarSpeedServo mServo2;
VarSpeedServo mServo3;

unsigned long millis();
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


void Mother_anlge(int angle1,int angle2,int angle3){
    /* 回転限度角を超えないようにプログラムを追加する */
    mServo1.write(angle1);
    mServo2.write(angle2);
    mServo3.write(angle3);
}

void Mother_anlge_speed(int angle1,int speed1,int angle2,int speed2,int angle3,int speed3){
    mServo1.write(angle1,speed1,true);
    mServo2.write(angle2,speed2,true);
    mServo3.write(angle3,speed3,true);
}

int sw_flag(){
    int flag = 0;
    if( digitalRead(SW) == LOW ){
        delay(100);
        if( digitalRead(SW) == LOW )
            flag = 1;
    }
    return flag;
}

void val_print(int val1,int val2,int val3,int flag){
    if( flag == 0 )
        Serial.print("Moter = (");
    else if( flag == 1 )
        Serial.print("Value = (");
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
    Serial.println("プログラムスタート");

    long ad0,ad1,ad2;

    /* ボリュームセンサからの角度調整用変数 */
    int ad_theta1,ad_theta2,ad_theta3;

    int speed1 = 255,speed2 = 255,speed3 = 255;    /* サーボモータのスピード制御用変数 */

    /* 事前に測定したデータを代入する配列(テスト用にわかりやすい値を代入しています。) */
    int data_theta1[13] = {180,160,140,120,100,80,60,40,20,0};
    int data_theta2[13];
    int data_theta3[13];

    /* 分割数を設定する変数 */
    int div = 1;

    /* シリアルポートから角度を操作する変数 */
    int serial_theta1,serial_theta2,serial_theta3;

    /* 実行時間計測用変数 */
    unsigned long start,end;

    /* モータ角度保存用のbuff */
    int buff1[13],buff2[13],buff3[13];
    /* buffの保存数カウント用変数 */
    int buff_cnt = 0;

    int i = 0;
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
            Serial.println("サーボモータを初期値に設定(90,90,90)");
            /* LEDの点灯及び,サーボーモータの初期値代入 */
            digitalWrite(LED,OUTPUT);
            mode = 1;
            break;
        
        case 1:
            Mother_anlge(90,90,90);
            delay(100);
            Serial.println("モード設定");
            Serial.println("(1-ボリュームで制御　　2-シリアルモニタで制御　　　3-配列からデータを読み出す　　4-センサ&モータ チェック)");
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
            Serial.println("ボリュームで制御");
            Serial.println("ボリュームを動かすと,その角度までモータを制御するプログラム");
            mode = 11;
            break;

        case 11:

            ad0 = analogRead(0);    /* ボリュームセンサ1 */
            ad1 = analogRead(1);    /* ボリュームセンサ2 */
            ad2 = analogRead(2);    /* ボリュームセンサ3 */

            ad_theta1 = ( ad0 * 180 / 1024);
            ad_theta2 = ( ad1 * 180 / 1024);
            ad_theta3 = ( ad2 * 180 / 1024);

            Mother_anlge(ad_theta1,ad_theta2,ad_theta3);
            val_print(ad_theta1,ad_theta2,ad_theta3,0);

            if( flag == 0 && digitalRead(SW) == LOW ){
                /* スイッチON */
                delay(100);
                if( flag == 0 && digitalRead(SW) == LOW ){
                    flag = 1;
                    mode = 1;
                }
            }else if( flag == 1 && digitalRead(SW) == HIGH ){
                delay(100);
                if( flag == 1 && digitalRead(SW) == HIGH )
                    flag = 0;
            }
            break;

        case 20:
            /* シリアルモニターに角度を送信すると,その角度まで移動するプログラム */
            Serial.println("シリアルモニタで制御");
            Serial.println("シリアルモニタに以下のように入力をすると,その角度まで移動するプログラム");
            Serial.println("例1： 090,090,090;");
            Serial.println("例1：すべてのモータを90度に設定する");
            Serial.println("例2： 180,000,075s;");
            Serial.println("例2：モータ1を180度,モータ2を0度,モータ3を75度に設定する.sをつけると,buffに保存する");
            Serial.println("例2： 100,090,000v;");
            Serial.println("例2：モータ1を100度,モータ2を90度,モータ3を0度に設定する.sをつけると,buffに保存し,buffを書き出す");

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
            mode = 22;
            break;

        case 22:
            if( Serial.available() ){
                serial_string[i] = char(Serial.read());
                if( serial_string[i] == ';' ){
                    serial_theta1 = 0;
                    serial_theta2 = 0;
                    serial_theta3 = 0;
                    mode = 23;
                }
                i++;
            }
            break;

        case 23:
            mode = 24;            
            break;

        case 24:
            weight = 100;
            for(i=0;i<3;i++){
                serial_theta1 += (serial_string[i] - '0')*weight;
                serial_theta2 += (serial_string[i+4] - '0')*weight;
                serial_theta3 += (serial_string[i+8] - '0')*weight;
                weight /= 10;
            }

            val_print(serial_theta1,serial_theta2,serial_theta3,0);
            mode = 25;

            break;

        case 25:                    
            Mother_anlge(serial_theta1,serial_theta2,serial_theta3);

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
            Serial.println("配列からデータを読み出す");
            mode = 31;
            break;

        case 31:
            /* 初期位置を設定する */
            /* 変数と配列を初期化する */
            for(i=0;i<13;i++){
                serial_string[i] = 0;
            }
            i = 0;
            mode = 32;
            break;

        case 32:
            if( Serial.available() ){
                serial_string[i] = char(Serial.read());
                if( serial_string[i] == ';' ){
                    serial_theta1 = 0;
                    serial_theta2 = 0;
                    serial_theta3 = 0;
                    mode = 33;
                }
                i++;
            }
            break;

        case 33:
            weight = 100;
            for(i=0;i<3;i++){
                serial_theta1 += (serial_string[i] - '0')*weight;
                serial_theta2 += (serial_string[i+4] - '0')*weight;
                serial_theta3 += (serial_string[i+8] - '0')*weight;
                weight /= 10;
            }
            Mother_anlge(serial_theta1,serial_theta2,serial_theta3);
            val_print(serial_theta1,serial_theta2,serial_theta3,0);
            mode = 34;
            break;
        
        case 34:
            Serial.print("初期状態を設定したら,スイッチを押してください。再度設定したい場合は,モニターに再び数値を入れてください");

            if( sw_flag() == 1 ){
                start = millis();
                mode = 35;
            }else if( Serial.available() ){
                mode = 31;
            }
            break;

        case 35:
            /* 第3関節サーボモーターを動かし,スイッチを押す動作を記述する */      
            mServo3.write(30,speed3,true);   //スイッチを押す動作
            mServo3.write(60,speed3,true);   //スイッチを離す動作(ここは,falseでもいいかも)

            mode = 36;
            break;

        case 36:
            /* 配列からのデータの読み出しを行い,スイッチ間を移動する */
            /* VarSpeedServoを使うことでなめらか制御が実現できる */
            for(i=0;i<10;i++){
                // Mother_anlge_speed(data_theta1[i],speed1,data_theta2[i],speed2,data_theta3[i],speed3);

                mServo1.write(data_theta1[i],speed1,true);            
                /* 実験の都合上,コメントしている. */
                // mServo2.write(data_theta2[i],speed2,true);
                // mServo3.write(data_theta3[i],speed3,true);
            }
            mode = 37;
            break;

        case 37:
            /* 第3関節サーボモーターを動かし,スイッチを押す動作を記述する */      
            mServo3.write(30,speed3,true);   //スイッチを押す動作
            end = millis();
            mServo3.write(60,speed3,true);   //スイッチを離す動作
            mode = 38;
            break;

        case 38:
            Serial.print("実行時間：");
            Serial.println(end-start);
            mode = 90;
            break;

        case 40:
            /* モータやセンサチェックプログラム */
            Serial.println("センサ&モータ チェック");
            mode = 41;
            break;

        case 41:
            Serial.println("モータチェック");
            Mother_anlge(120,120,120);
            delay(3000);
            Mother_anlge(90,90,90);
            delay(3000);
            Mother_anlge(60,60,60);
            delay(3000);
            mode = 42;
            break;

        case 42:
            Serial.println("ボリュームチェックプログラム");
            delay(3000);
            mode = 43;
            break;

        case 43:
            ad0 = analogRead(0);    /* ボリュームセンサ1 */
            ad1 = analogRead(1);    /* ボリュームセンサ2 */
            ad2 = analogRead(2);    /* ボリュームセンサ3 */
            
            val_print(ad0,ad1,ad2,1);
        
            if( sw_flag() == 1 )
                mode = 44;
        
            delay(3000);
            break;
        
        case 44:
            Serial.println("LED　チェック");
            mode = 45;
            break;
        
        case 45:
            for(i=0;i<10;i++){
                if( i % 2 == 0 )
                    digitalWrite(LED, HIGH);
                else
                    digitalWrite(LED, LOW);
                delay(750);
            }
            mode = 1;
            break;

        case 80:
            /* buff書き出し命令 */
            Serial.println("buffの値をシリアルモニターに書き出す");
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
            for(i=0;i<buff_cnt;i++){
                data_theta1[i] = buff1[i];
                data_theta2[i] = buff2[i];
                data_theta3[i] = buff3[i];
            }
            mode = 1;
            break;

        case 90:
            /* プログラム終了処理 */
            Serial.println("プログラム終了");
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
