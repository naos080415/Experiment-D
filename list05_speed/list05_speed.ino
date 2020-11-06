/* シリアルモニタに日本語を使っているため,メモリ使用量が非常に大きくなる */
#include <VarSpeedServo.h>

#define SW 5    /* タクトスイッチのポート番号 */
#define LED 10

/* サーボモータの回転限度角 */
#define Moter1_lim_min 0
#define Moter1_lim_max 180
#define Moter2_lim_min 0
#define Moter2_lim_max 180
#define Moter3_lim_min 0
#define Moter3_lim_max 180


VarSpeedServo mServo1;
VarSpeedServo mServo2;
VarSpeedServo mServo3;

/* 
Moter = (148,155,172)
Moter = (148,155,150)
*/
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

void Mother_anlge_speed_all(int angle1,int speed1,int angle2,int speed2,int angle3,int speed3){
    if( angle1 >= Moter1_lim_min && angle1 <= Moter1_lim_max )
        mServo1.write(angle1,speed1,true);
    else
        Serial.println("Mother1-limit");

    if( angle2 >= Moter2_lim_min && angle2 <= Moter2_lim_max )
        mServo2.write(angle2,speed2,true);
    else
        Serial.println("Mother2-limit");

    if( angle3 >= Moter3_lim_min && angle3 <= Moter3_lim_max )
        mServo3.write(angle3,speed3,true);
    else
        Serial.println("Mother3-limit");
}

void Mother_anlge_speed_one(int angle,int speed,bool flag){
    if( angle >= Moter1_lim_min && angle <= Moter1_lim_max )
        mServo1.write(angle,speed,flag);
    else
        Serial.println("Mother1-limit");
}

void Mother_anlge_speed_two(int angle,int speed,bool flag){
    if( angle >= Moter2_lim_min && angle <= Moter2_lim_max )
        mServo2.write(angle,speed,flag);
    else
        Serial.println("Mother2-limit");
}

void Mother_anlge_speed_three(int angle,int speed,bool flag){
    if( angle >= Moter3_lim_min && angle <= Moter3_lim_max )
        mServo3.write(angle,speed,flag);
    else
        Serial.println("Mother3-limit");
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
    else if( flag == 2 )
        Serial.print("speed = (");
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
    Serial.println("Program start");

    long ad0,ad1,ad2;

    /* ボリュームセンサからの角度調整用変数 */
    int ad_theta1,ad_theta2,ad_theta3;

    int speed1 = 10,speed2 = 10,speed3 = 10;    /* サーボモータのスピード制御用変数 */
    bool wait_time1,wait_time2,wait_time3;      /* サーボモータ関数の待ち時間用変数 */

    wait_time1 = true;                          /* 初期値をtrueに設定 */
    wait_time2 = true;
    wait_time3 = true;
    
    /* 事前に測定したデータを代入する配列(テスト用にわかりやすい値を代入しています。) */
    // int data_theta1[13];
    // int data_theta2[13];
    // int data_theta3[13];
    int data_theta1[13] = {110,110,139,139,0,0,0,0,0,0,0,0,0};
    int data_theta2[13] = {80,126,132,82,0,0,0,0,0,0,0,0,0};
    int data_theta3[13] = {74,74,74,74,0,0,0,0,0,0,0,0,0};

    /* data_sizeを指定するプログラム */
    int data_size = 0;

    /* スピード制御用変数代入 */
    char serial_speed[4];

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
            Serial.println("Servo init(90,90,90)");
            /* LEDの点灯及び,サーボーモータの初期値代入 */
            digitalWrite(LED,OUTPUT);
            /* buff_cnt のカウントダウン */
            while( data_theta1[i] != 0 )
                i++;
            data_size = i;
            mode = 1;
            break;
        
        case 1:
            Mother_anlge(90,90,90);
            delay(1000);
            val_print(speed1,speed2,speed3,2);
            Serial.print("data_size = ");
            Serial.println(data_size);
            Serial.println("Mode Setting");
            Serial.println("(1-Valuse Control  2-Serial Control　　　3-index Control　　4-Mother&Sensor check　　5-Setting Mode)");
            serial_num = 0;
            mode = 2;
            break;
        
        case 2:
            if( Serial.available() ){
                serial_num = Serial.read() - '0';
            }
            if( serial_num == 1 || serial_num == 2 || serial_num == 3 || serial_num == 4 || serial_num == 5 )
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

            Mother_anlge_speed_all(ad_theta1,50,ad_theta2,50,ad_theta3,50);
            // Mother_anlge(ad_theta1,ad_theta2,ad_theta3);
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
            Serial.println("ex1： 090,090,090;");
            Serial.println("ex1：すべてのモータを90度に設定する");
            Serial.println("ex2： 180,000,075s;");
            Serial.println("ex2：Mother1:180[deg],Mother1:0[deg],Mother3:75[deg].s option save buff");
            Serial.println("ex2： 100,090,000v;");
            Serial.println("ex2：モータ1を100度,モータ2を90度,モータ3を0度に設定する.sをつけると,buffに保存し,buffを書き出す");

            for(i=0;i<13;i++){
                buff1[i] = 0;
                buff2[i] = 0;
                buff3[i] = 0;
            }
            buff_cnt = 0;
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
            /* 文字が入力されるまで待つ */
            if( Serial.available() ){
                serial_string[i] = char(Serial.read());
                if( serial_string[i] == ';' ){
                    serial_theta1 = 0;
                    serial_theta2 = 0;
                    serial_theta3 = 0;
                    mode = 23;
                }
                i++;
            }else if( sw_flag() == 1 )
                mode = 1;
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

            // Mother_anlge(serial_theta1,serial_theta2,serial_theta3);        /* テスト用関数 */
            Mother_anlge_speed_all(serial_theta1,50,serial_theta2,50,serial_theta3,50);

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
            Serial.println("index read");
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
                if( serial_string[i] == ';' || serial_string[i] == ':' ){
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
            
            Mother_anlge_speed_all(serial_theta1,10,serial_theta2,10,serial_theta3,10);
            // Mother_anlge(serial_theta1,serial_theta2,serial_theta3);
            val_print(serial_theta1,serial_theta2,serial_theta3,0);
            Serial.println("初期状態を設定したら,スイッチを押してください。再度設定したい場合は,モニターに再び数値を入れてください");
            mode = 34;
            break;
        
        case 34:
            Mother_anlge_speed_all(serial_theta1,10,serial_theta2,10,serial_theta3,10);
            if( sw_flag() == 1 ){
                start = millis();
                mode = 35;
            }else if( Serial.available() ){
                mode = 31;
            }
            break;

        case 35:
            /* 第3関節サーボモーターを動かし,スイッチを押す動作を記述する */
            mServo2.write(data_theta2[0],speed3,true);   //スイッチを押す動作
            mode = 36;
            break;

        case 36:
            /* 配列からのデータの読み出しを行い,スイッチ間を移動する */
            /* VarSpeedServoを使うことでなめらか制御が実現できる */
            for(i=1;i<data_size;i++){
                /* 前回のサーボ値と変化があれば,処理をする. */
                if( data_theta1[i-1] != data_theta1[i] )
                    Mother_anlge_speed_one(data_theta1[i],speed1,wait_time1);
    
                if( data_theta2[i-1] != data_theta2[i] )
                    Mother_anlge_speed_two(data_theta2[i],speed2,wait_time2);
    
                if( data_theta3[i-1] != data_theta3[i] )
                    Mother_anlge_speed_three(data_theta3[i],speed3,wait_time3);

                // Mother_anlge_speed_all(data_theta1[i],speed1,data_theta2[i],speed2,data_theta3[i],speed3);
                // mServo1.write(data_theta1[i],speed1,true);            
                // mServo2.write(data_theta2[i],speed2,true);
                // mServo3.write(data_theta3[i],speed3,true);
            }
            mode = 37;
            break;


        case 37:
            /* 第3関節サーボモーターを動かし,スイッチを押す動作を記述する */      
            // mServo3.write(30,speed3,true);   //スイッチを押す動作
            end = millis();
            // mServo3.write(60,speed3,true);   //スイッチを離す動作
            mode = 38;
            break;

        case 38:
            Serial.print("実行時間：");
            Serial.println(end-start);
            mode = 39;
            break;
        
        case 39:
            if( sw_flag() == 1 )
                mode = 1;
            break;

        case 40:
            /* モータやセンサチェックプログラム */
            Serial.println("センサ&モータ チェック");
            mode = 41;
            break;

        case 41:
            Serial.println("Mother check");
            Mother_anlge(120,120,120);
            delay(3000);
            Mother_anlge(90,90,90);
            delay(3000);
            Mother_anlge(60,60,60);
            delay(3000);
            mode = 42;
            break;

        case 42:
            Serial.println("Valume check");
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
            Serial.println("LED　check");
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

        case 50:
            /* スピード制御用変数の確認用プログラム */
            Serial.println("Setting Mode");
            Serial.println("Input Num");
            mode = 51;
            break;

        case 51:
            Serial.print("speed1 = ");
            /* 変数初期化 */
            for(i=0;i<4;i++)
                serial_speed[i] = 0;
            i = 0;
            mode =52;
            break;

        case 52:
            /* シリアルモニタからの入力待ち変数 */
            if( Serial.available() ){
                serial_speed[i] = char(Serial.read());
                if( serial_speed[i] == ';' ){
                    speed1 = 0;
                    mode = 53;
                }
                i++;
            }
            break;

        case 53:
            weight = 100;
            for(i=0;i<3;i++){
                speed1 += (serial_speed[i] - '0')*weight;
                weight /= 10;
            }
            Serial.println(speed1);
            mode = 54;
            break;


        case 54:
            Serial.print("speed2 = ");
            /* 変数初期化 */
            for(i=0;i<4;i++)
                serial_speed[i] = 0;
            i = 0;
            mode =55;
            break;

        case 55:
            /* シリアルモニタからの入力待ち変数 */

            if( Serial.available() ){
                serial_speed[i] = char(Serial.read());
                if( serial_speed[i] == ';' ){
                    speed2 = 0;
                    mode = 56;
                }
                i++;
            }
            break;

        case 56:
            weight = 100;
            for(i=0;i<3;i++){
                speed2 += (serial_speed[i] - '0')*weight;
                weight /= 10;
            }
            Serial.println(speed2);
            mode = 57;
            break;

        case 57:
            Serial.print("speed3 = ");
            /* 変数初期化 */
            for(i=0;i<4;i++)
                serial_speed[i] = 0;
            i = 0;
            mode =58;
            break;

        case 58:
            /* シリアルモニタからの入力待ち変数 */
            if( Serial.available() ){
                serial_speed[i] = char(Serial.read());
                if( serial_speed[i] == ';' ){
                    speed3 = 0;
                    mode = 59;
                }
                i++;
            }
            break;

        case 59:
            weight = 100;
            for(i=0;i<3;i++){
                speed3 += (serial_speed[i] - '0')*weight;
                weight /= 10;
            }
            Serial.println(speed3);
            mode = 1;
            break;


        case 80:
            /* buff書き出し命令 */
            Serial.println("buff write");
            Serial.print("int data_theta1[13] = {");
            for(i=0;i<(buff_cnt);i++){
                Serial.print(buff1[i]);
                Serial.print(",");
            }
            
            for(;i<13;i++){
                Serial.print(0);
                if( i != 12 )
                    Serial.print(",");
            }

            Serial.println("};");

            Serial.print("int data_theta2[13] = {");
            for(i=0;i<(buff_cnt);i++){
                Serial.print(buff2[i]);
                Serial.print(",");
            }
            for(;i<13;i++){
                Serial.print(0);
                if( i != 12 )
                    Serial.print(",");
            }
            Serial.println("};");

            Serial.print("int data_theta3[13] = {");
            for(i=0;i<(buff_cnt);i++){
                Serial.print(buff3[i]);
                Serial.print(",");
            }
            for(;i<13;i++){
                Serial.print(0);
                if( i != 12 )
                    Serial.print(",");
            }

            Serial.println("};");

            /* data_thetaの初期化 */
            for(i=0;i<13;i++){
                data_theta1[i] = 0;
                data_theta2[i] = 0;
                data_theta3[i] = 0;
            }

            for(i=0;i<buff_cnt;i++){
                data_theta1[i] = buff1[i];
                data_theta2[i] = buff2[i];
                data_theta3[i] = buff3[i];
            }
            data_size = buff_cnt;
            mode = 1;
            break;

        case 90:
            /* プログラム終了処理 */
            Serial.println("exit");
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