#!/usr/bin/env python3

import os
import sys

# Print the required HTTP header.
# This must be the first thing printed to stdout.
# Use '\r\n' for the line endings.
print("Content-Type: text/html\r\n", end='')
print("\r\n", end='') # This creates the blank line

# Start the HTML document
print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head>")
print("    <meta charset='UTF-8'>")
print("    <title>CGI Test Page</title>")
print("    <style>")
print("        body { font-family: sans-serif; background-color: #f0f0f0; margin: 2em; }")
print("        pre { background-color: #fff; padding: 1em; border: 1px solid #ccc; }")
print("        h1 { color: #333; }")
print("        table { width: 100%; border-collapse: collapse; margin-top: 1em; }")
print("        th, td { padding: 8px; border: 1px solid #ddd; text-align: left; }")
print("        th { background-color: #f2f2f2; }")
print("    </style>")
print("</head>")
print("<body>")

print("<h1>Webserv CGI Test</h1>")
print("<p>This page was generated dynamically by a Python CGI script.</p>")

print("<h2>Environment Variables</h2>")
print("<table>")
print("<thead><tr><th>Variable</th><th>Value</th></tr></thead>")
print("<tbody>")

# Loop through all environment variables and display them
for key, value in sorted(os.environ.items()):
    print(f"<tr><td><strong>{key}</strong></td><td><pre>{value}</pre></td></tr>")

print("</tbody>")
print("</table>")

# Check for POST data (sent to stdin)
if os.environ.get("REQUEST_METHOD") == "POST":
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    if content_length > 0:
        post_data = sys.stdin.read(content_length)
        print("<h2>POST Data</h2>")
        print(f"<pre>{post_data}</pre>")

print("</body>")
print("</html>")