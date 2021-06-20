#define Trig   2 //引脚Tring 连接 IO D2
#define Echo   3 //引脚Echo  连接 IO D3 
#define LED    4 //LED正极   连接 IO D4
#define Alarm  5 //闹钟正极   连接 IO D5
#define Button 6 //按键开关   连接 IO D6
 
float cm; //距离变量
float temp; // 
int state_alarm;//闹钟状态（0：关闭；1：计时；2：响）
int state_led;//LED状态（0：熄灭；1：亮；2：延时）
int count;//计数器
int remain;//夜灯延时计时器
 
void setup() {
  Serial.begin(9600);
  pinMode(Trig,  OUTPUT);
  pinMode(Echo,  INPUT);
  pinMode(LED,   OUTPUT);
  pinMode(Alarm, OUTPUT);
  pinMode(Button,   INPUT);
  state_alarm = 0;
  count = 0;
  remain = 5001;
}

boolean debounce(boolean last) //按键消抖函数
{
    boolean current = digitalRead(Button);
    if (last != current) {
        // 如果电压改变了，说明肯定按下按钮了，否则电压值是不会改变的。
        // 但是这个电压改变是不是抖动阶段的电压改变是不知道的，所以要等5ms（避开抖动
        // 阶段）再读取一次，第二次读取的就是稳定后的值可以直接返回了。
        delay(5);
        current = digitalRead(Button);
    }
    // 如果电压没有改变，不能判断是否按下按钮，直接返回，等待下一轮对引脚状态的查询
    return current;
}
 
void loop() {
  //给Trig发送一个低高低的短时间脉冲,触发测距
  digitalWrite(Trig, LOW); //给Trig发送一个低电平
  delayMicroseconds(2);    //等待 2微妙
  if(state_alarm == 1)//计时
  {
    count += 2;
  }
  remain += 2;
  digitalWrite(Trig,HIGH); //给Trig发送一个高电平
  delayMicroseconds(10);    //等待 10微妙
  if(state_alarm == 1)//计时
  {
    count += 10;
  }
  remain += 10;
  digitalWrite(Trig, LOW); //给Trig发送一个低电平
  
  temp = float(pulseIn(Echo, HIGH)); //存储回波等待时间,
  //pulseIn函数会等待引脚变为HIGH,开始计算时间,再等待变为LOW并停止计时
  //返回脉冲的长度
  
  //声速是:340m/1s 换算成 34000cm / 1000000μs => 34 / 1000
  //因为发送到接收,实际是相同距离走了2回,所以要除以2
  //距离(厘米)  =  (回波时间 * (34 / 1000)) / 2
  //简化后的计算公式为 (回波时间 * 17)/ 1000
  
  cm = (temp * 17 )/1000; //把回波时间换算成cm

  if(cm < 50)//开灯
  {
    analogWrite(LED, 255);    
    remain = 0;
  }
  else if(remain <= 5000)//延时
  {
    analogWrite(LED, 255);
  }
  else//熄灯
  {
    analogWrite(LED, 0);
  }

  int buttonState = debounce(digitalRead(Button));//设置闹钟
  if(!buttonState)
  {
    if(state_alarm == 0)//开始计时
    {
      state_alarm = 1; 
      count = 0;
    }
    else if(state_alarm == 2)//关闹钟
    {
      state_alarm = 0;
    }
  }

  if(count >= 5000 && state_alarm == 1)//计时满
  {
    state_alarm = 2;
  }  

  if(state_alarm == 2)//闹钟响
  {
    analogWrite(Alarm, 100);
  }
  else//关闹钟
  {
    analogWrite(Alarm, 0);
  }
 
  Serial.print("Echo =");
  Serial.print(temp);//串口输出等待时间的原始数据
  Serial.print(" | | Distance = ");
  Serial.print(cm);//串口输出距离换算成cm的结果
  Serial.println("cm");
  delay(100);
  if(state_alarm)//计时
  {
    count += 100;
  }
  remain += 100;
}
