import serial
import os

to_load = "../kernel.bin"

to_load_len = os.path.getsize(to_load)
to_load_len_padded = to_load_len
if to_load_len_padded%4 != 0:
    to_load_len_padded += 4-(to_load_len_padded%4)


MSG_ACK    = 0x12344321

MSG_HELLO  = 0x11111111
MSG_LEN    = 0x22222222
MSG_FILE   = 0x33333333

dev = serial.Serial("/dev/ttyUSB0", 115200)

def write_uint(n):
    dev.write(n.to_bytes(4, "little"))

def read_uint():
    ack = int.from_bytes(dev.read(4), "little")

    return ack

def say_hello():
    write_uint(MSG_HELLO)

    ack = read_uint()

    if ack != MSG_ACK:
        print(hex(ack))
        raise Exception("didn't receive acknowledgement for hello msg")

def say_len():
    write_uint(MSG_LEN)

    write_uint(to_load_len_padded)

    ack = read_uint()

    if ack != MSG_ACK:
        print(hex(ack))
        raise Exception("didn't receive ack for len msg");

def say_file():
    write_uint(MSG_FILE)

    print("got len", read_uint()) # len

    bs = list(open("../kernel.bin", "rb").read())

    for i in range(len(bs)):
        b = bs[i]
        print("hello", b, i)
        dev.write(bytes([b]))

        got = int.from_bytes(dev.read(1), "little")

        if got != b:
            print("wanted", b, "got", got)

            raise Exception("heck")

    print('done')

    print(dev.read(to_load_len))

    print("hi")
    ack = read_uint()

    print("done")

    if ack != MSG_ACK:
        print(hex(ack))
        raise Exception("didn't receive ack for file msg");


print(to_load_len)
say_hello()

print("hello")

say_len()

print("len")

say_file()

print("sent file")

# while True:
#    print(dev.read())
#    # print(hex(int.from_bytes(dev.read(4), "little")))
