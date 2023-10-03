#!/usr/bin/env python
import os

print("Status: 200 OK\r\n\r\n")
print("Content-type: text/html\n")

print("<html><head><title>Environment Variables</title></head><body>")
print("<h1>Environment Variables:</h1>")
print("<ul>")
for key, value in os.environ.items():
    print(f"<li><strong>{key}:</strong> {value}</li>")
print("</ul>")
print("</body></html>")
