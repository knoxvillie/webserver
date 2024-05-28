#!/usr/bin/env python3

import time
import sys

try:
	time.sleep(5)
	print("HTTP/1.1 200 OK\r")
	print("Content-type: text/html\r")
	print("Connection: keep-alive\r")
	print("Content-length: 25\r")
	print("\r")
	print("Hello world! FROM PYTHON!")
except BrokenPipeError:
	print(f"CGI ERROR - Broken Pipe", file=sys.stderr)
except Exception as e:
	print(f"CGI ERROR - {e}", file=sys.stderr)
finally:
	sys.exit()