from phy_common import *
from time import sleep

write_pipe = '/tmp/phy_tx_in'

write_file = open(write_pipe, mode="w")

data = "helloworld"*6 + "hii"
#data = [chr(i) for i in a]

while True:
    write_file.write(bytearray(data))
    write_file.flush()
    print("Send packet")
    sleep(0.5)
