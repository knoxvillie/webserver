#!/usr/bin/env python3
import os
import urllib.parse

# HTML
html = """
<head>
<title>Python CGI Script</title>
</head>
<body>
"""

# Parse form data
#form = cgi.FieldStorage()

# Get the value of the 'name' field from the form data
#name = form.getvalue('name')
query_string = os.environ.get("QUERY_STRING")
query_params = urllib.parse.parse_qs(query_string)

name = query_params.get('name', [''])[0]

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
print("Content-length: {}".format(content_length))
print("\r")

print(html)
