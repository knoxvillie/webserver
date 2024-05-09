import requests

def send_chunked_request(url, data_chunks):
    session = requests.Session()
    headers = {'Transfer-Encoding': 'chunked'}

    try:
        with session.request("POST", url, headers=headers, stream=True) as response:
            for chunk in data_chunks:
                response.raw.write(chunk.encode('utf-8'))
            response.raw.flush()
            response.raw.close()

        if response.status_code == 200:
            print("Chunked request successful.")
        else:
            print(f"Chunked request failed with status code {response.status_code}.")
    except Exception as e:
        print(f"Error occurred: {e}")

# Example usage:
if __name__ == "__main__":
    url = "http://0.0.0.0:8080/image/"
    data_chunks = ["24\r\nThis is the first chunk.", "This is the second chunk.", "This is the last chunk."]
    send_chunked_request(url, data_chunks)
