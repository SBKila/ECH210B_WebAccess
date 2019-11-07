#include <Arduino.h>
const char main_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>ECH monitor</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css" />
    <script src="http://code.jquery.com/jquery-1.11.1.min.js"></script>
    <script src="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.js"></script>
</head>
<body>
    <h2>ECH monitor</h2>
    <div data-role="collapsibleset" data-theme="a" data-inset="false">
        <div data-role="collapsible">
            <h2>Mailbox</h2>
            <ul data-role="listview">
                <li>Outdoor Temp<span class="ui-li-count">12°c</span></a></li>
                <li>Outdoor Humidity<span class="ui-li-count">63%</span></a></li>
            </ul>
        </div>
        <div data-role="collapsible">
            <h2>Compressor</h2>
        </div>
        <div data-role="collapsible">
            <h2>Contacts</h2>
            <ul data-role="listview" data-autodividers="true" data-theme="a" data-divider-theme="b">
            <li>Adam Kinkaid</li>
            <li>Alex Wickerham</li>
            <li>Avery Johnson</li>
            <li>Bob Cabot</li>
            <li>Caleb Booth</li>
            <li>Christopher Adams</li>
            <li>Culver James</li>
            </ul>
        </div>
    </div>
</body>
</html>)rawliteral";