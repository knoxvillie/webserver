import requests

def send_request(url):
    try:
        response = requests.get(url)
        if response.status_code == 200:
            print("Request successful!")
            print("Response content:")
            print(response.text)
        else:
            print(f"Request failed with status code: {response.status_code}")
    except requests.RequestException as e:
        print("Error sending request:", e)

# Example URL with file path
url = "0.0.0.0:8080/example.txt"
send_request(url)
