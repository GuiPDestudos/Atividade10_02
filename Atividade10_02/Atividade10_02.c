#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h" 
#include "hardware/pwm.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
const uint display_high = 64;
const uint display_width = 128;
const uint display_pages = 8;
const uint display_address = 0x3C;
const uint centro_joystick = 2048;

const int VRX = 26;
const int VRY = 27;
const int ADC_CHANNEL_0 = 0;
const int ADC_CHANNEL_1 = 1;
const int SW = 22;

const int LED_B = 13;
const int LED_R = 12;
const float DIVIDER_PWM = 16.0;
const uint16_t PERIOD = 4096;
uint16_t led_b_level, led_r_level = 100;
uint slice_led_b, slice_led_r;

void setup_joystick()
{
    adc_init();
    adc_gpio_init(VRX);
    adc_gpio_init(VRY);
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);
}

void setup_pwm_led(uint led, uint *slice, uint16_t level)
{
    gpio_set_function(led, GPIO_FUNC_PWM);
    *slice = pwm_gpio_to_slice_num(led);
    pwm_set_clkdiv(*slice, DIVIDER_PWM);
    pwm_set_wrap(*slice, PERIOD);
    pwm_set_gpio_level(led, level);
    pwm_set_enabled(*slice, true);
}

void setup()
{
    stdio_init_all();
    setup_joystick();
    setup_pwm_led(LED_B, &slice_led_b, led_b_level);
    setup_pwm_led(LED_R, &slice_led_r, led_r_level);
}

void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
    adc_select_input(ADC_CHANNEL_0);
    sleep_us(2);
    *vrx_value = adc_read();
    adc_select_input(ADC_CHANNEL_1);
    sleep_us(2);
    *vry_value = adc_read();
}

uint16_t leitura_adc(uint canal){

    adc_select_input(canal);
    sleep_us(2);
    return adc_read();
}

int map_value(int valuem, int in_min, int in_max, int out_min, int out_max)
{
    return (valuem - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint16_t read_joystick(uint canal)
{
    uint16_t value = leitura_adc(canal);
    return map_value(value, 0, 4095, 0, 255);
}

void joystick_read_switch(uint16_t *sw_value)
{
    *sw_value = gpio_get(SW);
}

void joystick_read(uint16_t *vrx_value, uint16_t *vry_value, uint16_t *sw_value)
{
    joystick_read_axis(vrx_value, vry_value);
    joystick_read_switch(sw_value);
}

int main()
{
    uint16_t vrx_value, vry_value, sw_value;
    setup();
    printf("Joystick-PWM\n");

    while(1){
        joystick_read(&vrx_value, &vry_value, &sw_value);
        printf("VRX: %d VRY: %d SW: %d\n", vrx_value, vry_value, sw_value);
        pwm_set_gpio_level(LED_B, vrx_value);
        pwm_set_gpio_level(LED_R, vry_value);
        sleep_ms(100);
    }

    return 0;
}
