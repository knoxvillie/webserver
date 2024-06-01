#!/usr/bin/env python3
import os
import sys
import urllib.parse

# Read the standard input to get the POST data
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
post_data = sys.stdin.read(content_length)
post_params = urllib.parse.parse_qs(post_data)

# Get the value of the 'name' field from the POST data
name = post_params.get('name', [''])[0]

# HTML
html = """
<head>
<title>Python CGI Script</title>
</head>
<body>
"""

# Print the received name
if name:
    html += "<h2>Hello, {}!</h2>".format(name)
else:
    html += "<h2>Hello, Anonymous!</h2>"

html += """
</body>
</html>
"""

content_length = len(html.encode('utf-8'))

# Print HTTP header
print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print("Connection: keep-alive")
print("Content-Length: {}".format(content_length))
print("\r")

print(html)