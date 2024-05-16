#!/usr/bin/env python3

import requests
url = 'http://0.0.0.0:8080/upload'  # Replace with the path to your CGI script
def chunked_generator():
    data = ['This', ' is', ' a', ' chunked', ' request!',
            'This', ' is', ' a', ' longer', ' chunked', ' request!',
            'This', ' is', ' a', ' very', ' long', ' chunked', ' request!',
            '   ', ' is', ' a', ' very', ' very', ' long', ' chunked', ' request!']
    for item in data:
        yield item.encode()  # Encode string to bytes
response = requests.post(url, data=chunked_generator(), headers={'Transfer-Encoding': 'chunked'})
print(response.status_code)
