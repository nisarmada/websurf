<?php
    // Set the HTTP header
    header("Content-type: text/html");
    echo "<h1>PHP CGI Test Successful</h1>";
    echo "<h2>Request Details:</h2>";

    // --- 1. Check Request Method ---
    $method = $_SERVER['REQUEST_METHOD'];
    echo "<p><strong>Method Used:</strong> " . $method . "</p>";

    // --- 2. Process Input ---
    $name = "";
    if ($method === 'POST') {
        // For POST, data comes from the request body
        $name = isset($_POST['user_name']) ? htmlspecialchars($_POST['user_name']) : 'No POST data received.';
    } elseif ($method === 'GET') {
        // For GET, data comes from the query string
        $name = isset($_GET['user_name']) ? htmlspecialchars($_GET['user_name']) : 'No GET data received.';
    }

    // --- 3. Display Result ---
    if ($name && $name !== 'No POST data received.' && $name !== 'No GET data received.') {
        echo "<p style='color: green; font-weight: bold;'>Hello, $name! Your data was successfully processed by PHP.</p>";
    } else {
        echo "<p style='color: orange;'>No 'user_name' parameter found in the request.</p>";
    }

    // --- 4. HTML Form for Testing POST ---
    echo "<h2>Test POST Submission:</h2>";
    echo "<form method='POST' action='test.php'>";
    echo "<label for='user_name'>Enter Name:</label>";
    echo "<input type='text' id='user_name' name='user_name' required>";
    echo "<input type='submit' value='Submit (POST)'>";
    echo "</form>";

    echo "<h2>Server Environment Variables:</h2>";
    echo "<pre>";
    print_r($_SERVER);
    echo "</pre>";
?>