import requests

# URL to which the POST request will be sent
url = "http://localhost:8080/upload"

# Data to be sent in the POST request
data = "This is the content of the request."

# Headers specifying the content type
headers = {
    "Host": "localhost",
    "Content-Type": "text/plain"
}

# Sending the POST request
response = requests.post(url, headers=headers, data=data)

# Handling the response
if response.status_code == 200:
    print("Request was successful.")
    print("Response content:")
    print(response.text)
else:
    print(f"Request failed with status code: {response.status_code}")
