from phy_common import *
from time import sleep

write_pipe = '/tmp/phy_tx_in'

write_file = open(write_pipe, mode="w")

data = "helloworld"*5
#data = [chr(i) for i in a]
cnt = 0
while True:
    write_file.write(bytearray(data + format(cnt, '13d')))
    write_file.flush()
    cnt = cnt + 1
    print("Send packet")
    sleep(0.5)
