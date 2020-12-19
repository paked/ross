import serial
import os
import argparse

MSG_ACK    = 0x12344321

MSG_HELLO  = 0x11111111
MSG_LEN    = 0x22222222
MSG_FILE   = 0x33333333

def write_uint(dev, n):
    dev.write(n.to_bytes(4, "little"))

def read_uint(dev):
    n = int.from_bytes(dev.read(4), "little")

    return n

def say_hello(dev):
    write_uint(dev, MSG_HELLO)

    ack = read_uint(dev)

    if ack != MSG_ACK:
        print(hex(ack))

        raise Exception("didn't receive acknowledgement for hello msg")

def say_len(dev, l):
    write_uint(dev, MSG_LEN)

    write_uint(dev, l)

    ack = read_uint(dev)

    if ack != MSG_ACK:
        print(hex(ack))
        raise Exception("didn't receive ack for len msg");

def say_file(dev, path, l):
    write_uint(dev, MSG_FILE)

    assert(l == read_uint(dev))

    bs = list(open(path, "rb").read())

    for i in range(len(bs)):
        b = bs[i]
        dev.write(bytes([b]))

        got = int.from_bytes(dev.read(1), "little")

        if got != b:
            print("wanted", b, "got", got)

            raise Exception("heck")

    dev.read(l)

    ack = read_uint(dev)

    if ack != MSG_ACK:
        print(hex(ack))
        raise Exception("didn't receive ack for file msg");

def cli():
    parser = argparse.ArgumentParser()

    parser.add_argument("bin", help="the binary file to upload to the rpi")
    parser.add_argument("dev", help="the device which serial is one")

    args = parser.parse_args()

    bin_path = args.bin
    dev_name = args.dev

    bin_len = os.path.getsize(bin_path)
    dev = serial.Serial(args.dev, 115200)

    say_hello(dev)

    say_len(dev, bin_len)

    say_file(dev, bin_path, bin_len)

if __name__ == '__main__':
    cli()
