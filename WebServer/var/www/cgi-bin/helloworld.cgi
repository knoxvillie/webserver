#!/usr/bin/env python3

import time
import sys

time.sleep(5)
print("HTTP/1.1 200 OK\r")
print("Content-type: text/html\r")
print("Connection: keep-alive\r")
print("Content-length: 25\r")
print("\r")
print("Hello world! FROM PYTHON!")

sys.exit()