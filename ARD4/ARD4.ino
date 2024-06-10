#include <Arduino.h>
/*Измерение АЦП 2 какнала по прерываниям
измерение частоты и фазы между каналами
v2.0 17/03/2020
*/
//*************************************************************************************************
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) //Биту sfr присваиваем 0
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))  //Биту sfr присваиваем 1
// чтение порта (проверка на 0-1)
#define PORTD_GET_2 (PIND & (1 << 2))
#define PORTD_GET_3 (PIND & (1 << 3))
//*************************************************************************************************
// Переменные Общие
volatile int mode, step;
// Переменные АЦП
uint8_t analog_ref = DEFAULT;//хранение  регистра
volatile int ADC_chanel, u_max,u_min;
// Переменные Таймера
volatile unsigned long time1_on,time1_off,time2_on;//отсечки времени фронт-старт, срез, фронт стоп
volatile bool is_start,is_front;
volatile unsigned long OVF_counter;//для переполнения таймера
//************************         АЦП                *********************************************
void ADC_init( )
{
    ADCSRA = 0; // Сбрасываем регистр ADCSRA
    ADCSRB = 0; // Сбрасываем регистр ADCSRB
    ADMUX |= (1 << REFS0)|(1 << REFS1); // Задаем внутренний ИОН
    cbi(ADMUX, ADLAR);
    //ADMUX |= (1 << ADLAR); // Меняем порядок записи бит, чтобы можно было читать только 8 бит регистра ADCH
    // Устанавливаем предделитель - 16  (ADPS[2:0]=100)
    //ADPS[2:0] 	Коэффициент деления
    //000	2           001	2       010	4
    //011	8           100	16      101	32
    //110	64          111	128
    //* 8
        cbi(ADCSRA, ADPS2); //Биту ADPS2 присваиваем единицу
        sbi(ADCSRA, ADPS1); //Биту ADPS1 присваиваем нули
        sbi(ADCSRA, ADPS0); //Биту ADPS0 присваиваем единицу

    /* 16
        sbi(ADCSRA, ADPS2); //Биту ADPS2 присваиваем единицу
        cbi(ADCSRA, ADPS1); //Биту ADPS1 присваиваем нули
        cbi(ADCSRA, ADPS0); //Биту ADPS0 присваиваем единицу
    */
    ADCSRA |= (1 << ADIE); // Разрешаем прерывания по завершении преобразования
}
//*****************************************************************************************
void ADC_start(int analogPin)// Выбор канала и старт АЦП
{
    // Запоминаем состояние регистра - оно будет использоваться при смене пина входящего сигнала
    analog_ref = ADMUX & 0xF8;
    ADMUX = analog_ref | (analogPin & 0x07);
    ADCSRA |= (1 << ADEN); // Включаем АЦП
    ADCSRA |= (1 << ADSC); // Запускаем преобразование
}

//*************************************************************************************************
ISR(ADC_vect){    //... Обработка прерывания от АЦП
    // Порядок записи результирующих битов был изменен из-за пренебрежения последними битами
    // ADLAR=1
    //int result = ADCH; // Считываем только значимые 8 бит - значение из диапазона 0...255
    // Если нужны все 10 бит (полная 10-битная точность), то если ADLAR=0:
    int result = ADCL | (ADCH << 8);
    //    int result = (ADCL>> 6) | (ADCH << 2);

    if (result>4)
    {
        if (step==0)
        {
            step=1;
        }
        if (step==1)
        {
            if (result>u_max)
            {
                u_max=result;
            }
        }
        if (step==2)
        {
            step=3;
        }

    }

    if (result<4)
    {
        if(step==1)
        {
            step=2;
        }
        if (step==2||step==0)
        {
          if (result<u_min)
            {
            u_min=result;
            }
        }

    }
    if (step<3)
    {
        ADC_start(ADC_chanel);
    }
}

int middle(int a, int b, int c) // нахождение среднего значения из 3-х
{
    int mid;// среднее значение
    if(a <= b && a <= c)
    {
        if(b <= c) mid = b;
        else mid = c;
    }
    else
    {
        if(b <= a && b <= c)
        {
            if(a <= c) mid = a;
            else mid = c;
        }
        else
        {
            if(a <= b) mid = a;
            else mid = b;
        }
    }
    return mid;
}

int filter(int n, int *m)
{
    int result = 0;
    if(n % 3 == 0)
    {
        int m2[n/3];
        for(int i = 0; i < n; i+=3)
        {
            m2[i/3] = middle(m[i],m[i+1],m[i+2]);
        }
        result = filter(n/3, m2);
    }
    else
    {
        result = m[0];
    }
    return result;
}

void setup()
{
	Serial.begin(9600);
	Serial.println("Hello world 25!");
	Serial.println("Hello world 25!");
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	ADC_init();
	mode=2;
	step=4;
    sei();
}

#define N 27

void loop()
{
    int mass_U_MAX[N] = {0};
    for(int i = 0; i < N;)
    {
        delay(10);
        if (step==4)
        {
            ADC_chanel=0;
            step=0;
            ADC_start(ADC_chanel);
        }
        if (step==3)
        {
            Serial.print("-- Umax = ");Serial.println(u_max);
       //     Serial.print("-****- Umin = ");Serial.println(u_min);
            mass_U_MAX[i] = u_max;
            //Serial.print("-- mass_U_MAX[i] = ");Serial.println(mass_U_MAX[i]);
            u_min=10;
            u_max=0;
            mode=3;
            step=4;
            i++;
        }
    }
    /*for(int i = 0; i < N; i++)
    {
        Serial.println(mass_U_MAX[i]);
    }*/
    Serial.print("U_MAX: ");
    Serial.println(filter(N, mass_U_MAX));
}


