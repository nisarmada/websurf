#!/usr/bin/env python3

import cgi
import os
import sys
import time

<<<<<<< HEAD
=======
# --- ADDED CODE FOR DELAY ---
# This simulates a long-running CGI process
# print("Waiting for 10 seconds...")
# sys.stdout.flush() # Ensure this message is sent immediately
# time.sleep(10)
# print("Finished waiting!")
# # ----------------------------

>>>>>>> main
# Print the HTTP header
print("Content-Type: text/html\r\n", end='')
print("\r\n", end='')

# Start HTML document with embedded CSS for animation
print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head>")
print("    <meta charset='UTF-8'>")
print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
print("    <title>Dynamic CGI Visual</title>")
print("    <style>")
print("        body { font-family: 'Arial', sans-serif; background-color: #f4f7f6; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; margin: 0; color: #333; }")
print("        .container { text-align: center; padding: 2em; background: #ffffff; border-radius: 12px; box-shadow: 0 4px 20px rgba(0, 0, 0, 0.1); }")
print("        h1 { color: #5c677d; }")
print("        .box { width: 100px; height: 100px; background-color: #6a1b9a; margin: 2em auto; border-radius: 10px; animation: bounce 2s infinite ease-in-out; }")
print("        @keyframes bounce { 0%, 100% { transform: translateY(0); } 50% { transform: translateY(-30px); background-color: #ff6f61; } }")
print("        form { margin-top: 1.5em; }")
print("        input[type='text'], input[type='submit'] { padding: 10px; border-radius: 5px; border: 1px solid #ccc; font-size: 1em; }")
print("        input[type='submit'] { background-color: #007bff; color: white; border: none; cursor: pointer; transition: background-color 0.3s ease; }")
print("        input[type='submit']:hover { background-color: #0056b3; }")
print("        .message { margin-top: 1em; padding: 10px; border-radius: 5px; background-color: #e8f5e9; color: #388e3c; border: 1px solid #c8e6c9; display: none; }")
print("    </style>")
print("</head>")
print("<body>")
print("    <div class='container'>")
print("        <h1>Dynamic CGI Visual</h1>")

# The animated box
print("        <div class='box'></div>")

# CGI form processing
form = cgi.FieldStorage()
message = ""
if "name" in form:
    user_name = form.getvalue("name")
    if user_name:
        message = f"Hello, {user_name}! Thanks for visiting."

print("        <form method='post' action=''>")
print("            <label for='name'>Enter your name:</label>")
print("            <input type='text' id='name' name='name' placeholder='Your name...'>")
print("            <input type='submit' value='Submit'>")
print("        </form>")

# Display a message if POST data was received
if message:
    print(f"        <p class='message' style='display: block;'>{message}</p>")

print("    </div>")
print("</body>")
print("</html>")
