from yolobit import *
from machine import Pin, SoftI2C, PWM
import music
button_a.on_pressed = None
button_b.on_pressed = None
button_a.on_pressed_ab = button_b.on_pressed_ab = -1

from yolobit import *

def in_danh_sach_gpio():
    # Danh sách các chân cắm mở rộng
    gpio = {
        "P0": pin0, "P1": pin1, "P2": pin2, "P3": pin3, "P4": pin4,
        "P5": pin5, "P6": pin6, "P7": pin7, "P8": pin8, "P9": pin9,
        "P10": pin10, "P11": pin11, "P12": pin12, "P13": pin13,
        "P14": pin14, "P15": pin15, "P16": pin16, "P19": pin19, "P20": pin20
    }
    
    # Danh sách các nút nhấn trên mạch
    special = {
        "Button A": button_a,
        "Button B": button_b,
        # "Buzzer": music,
        "LED": display.Led,
    }
    
    print()

    # print("-" * 30)
    print("| {:<12} | {:<15} |".format("Tên Pin", "GPIO Thực Tế"))
    print("|" + "-" * 15 + "|" + "-" * 15 + "|")

    # In thông tin các chân P
    for ten, doi_tuong in gpio.items():
        # Truy cập vào thuộc tính .pin bên trong Pins Object
        gpio_info = str(doi_tuong.pin) 
        print("| {:<12} | {:<15} |".format(ten, gpio_info))

    # print("-" * 30)

    # In thông tin các nút nhấn
    for ten, doi_tuong in special.items():
        # Đối với button, thuộc tính cũng là .pin
        gpio_info = str(doi_tuong.pin)
        print("| {:<12} | {:<15} |".format(ten, gpio_info))
    # print("-" * 30)

def test_loa():
  # Điều khiển trực tiếp GPIO 25 bằng PWM
  loa = PWM(Pin(25))
  
  # Phát một tiếng bíp ở tần số 1000Hz
  loa.freq(1000)
  loa.duty(512>>1)  # Âm lượng khoảng 25%
  time.sleep(0.5)
  
  # Tắt loa
  loa.deinit()
    
if True:
  in_danh_sach_gpio()


while True:
  time.sleep_ms(100)
