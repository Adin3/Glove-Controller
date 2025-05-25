#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define MPU_ADDR 0x68
#define QMC5883L_ADDR 0x0D

void TWI_init(void) {
    TWSR = (0 << TWPS1) | (0 << TWPS0);
    TWBR = 0x48;
    TWCR = (1 << TWEN);
}

void TWI_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void TWI_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

void TWI_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

uint8_t TWI_read(uint8_t ack) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA);    
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

void USART_init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_send(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void USART_print(const char* str) {
    while (*str) USART_send(*str++);
}

void QMC5883L_init(void) {
    TWI_start();
    TWI_write(QMC5883L_ADDR << 1);
    TWI_write(0x09);
    TWI_write(0x1D);
    TWI_stop();
}

void QMC5883L_read(int16_t* mx, int16_t* my, int16_t* mz) {
    TWI_start();
    TWI_write(QMC5883L_ADDR << 1);
    TWI_write(0x00);
    TWI_start();
    TWI_write((QMC5883L_ADDR << 1) | 1);

    uint8_t xl = TWI_read(1);
    uint8_t xh = TWI_read(1);
    uint8_t yl = TWI_read(1);
    uint8_t yh = TWI_read(1);
    uint8_t zl = TWI_read(1);
    uint8_t zh = TWI_read(0);
    TWI_stop();

    *mx = (int16_t)((xh << 8) | xl);
    *my = (int16_t)((yh << 8) | yl);
    *mz = (int16_t)((zh << 8) | zl);
}

void MPU6500_init(void) {
    TWI_start();
    TWI_write(MPU_ADDR << 1);
    TWI_write(0x6B);
    TWI_write(0x00);
    TWI_stop();
}

void MPU6500_read(int16_t* ax, int16_t* ay, int16_t* az,
                      int16_t* gx, int16_t* gy, int16_t* gz) {
    TWI_start();
    TWI_write(MPU_ADDR << 1);
    TWI_write(0x3B);
    TWI_start();
    TWI_write((MPU_ADDR << 1) | 1);

    uint8_t axh = TWI_read(1);
    uint8_t axl = TWI_read(1);
    uint8_t ayh = TWI_read(1);
    uint8_t ayl = TWI_read(1);
    uint8_t azh = TWI_read(1);
    uint8_t azl = TWI_read(1);

    *ax = (int16_t)((axh << 8) | axl);         
    *ay = (int16_t)((ayh << 8) | ayl);
    *az = (int16_t)((azh << 8) | azl);

    TWI_read(1);
    TWI_read(1);

    uint8_t gxh = TWI_read(1);
    uint8_t gxl = TWI_read(1);
    uint8_t gyh = TWI_read(1);
    uint8_t gyl = TWI_read(1);
    uint8_t gzh = TWI_read(1);
    uint8_t gzl = TWI_read(0);

    *gx = (int16_t)((gxh << 8) | gxl);
    *gy = (int16_t)((gyh << 8) | gyl);
    *gz = (int16_t)((gzh << 8) | gzl);

    TWI_stop();
}

int main(void) {
    USART_init(103);
    TWI_init();
    QMC5883L_init();

    int16_t ax, ay, az, gx, gy, gz;
    int16_t mx, my, mz;
    char buffer[128];

    while (1) {
        MPU6500_read(&ax, &ay, &az, &gx, &gy, &gz);
        QMC5883L_read(&mx, &my, &mz);

        float axn = ax / 16384.0;
        float ayn = ay / 16384.0;
        float azn = az / 16384.0;

        float gxn = gx / 131.0;
        float gyn = gy / 131.0;
        float gzn = gz / 131.0;

        sprintf(buffer,
            "A: %.2f %.2f %.2f g | G: %.2f %.2f %.2f dps | M: %d %d %d\r\n",
            axn, ayn, azn, gxn, gyn, gzn, mx, my, mz);
        USART_print(buffer);
        _delay_ms(100);
    }
}
