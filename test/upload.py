import requests

url = 'http://localhost:8080/upload'
headers = {
    "Host": "localhost",
}
files = {'file': ('upload.txt', open('upload.txt', 'rb'))}

response = requests.post(url, headers=headers, files=files)

print(response.status_code)
