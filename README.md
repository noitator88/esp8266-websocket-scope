
ref: https://github.com/esp8266/Arduino/issues/1634

from krzychb 

Here is a short manual wink

Prepare the following s/w components:

Arduino Sketch: WebSocket-Scope.ino
WebSocket-Scope.zip
WebSockets library by Links2004 - https://github.com/Links2004/arduinoWebSockets. The easiest way to go is installing WebSockets rev. 2.0.0 using Library Manager from Arduino IDE
HTML web page WebSocket-Scope.html that should be saved on a PC in any folder to be then opened in a web browser. No web server is required.
WebSocket-Scope.zip
CanvasJS JavaScript library v1.8.0 GA – downloaded free from http://canvasjs.com/. From the compressed package you need only one file canvasjs.min.js that should be placed in the same folder as WebSocket-Scope.html. This library is already included in the above zipped folder. I believe this is great, fast and really very well documented application for on-line data visualization in a web browser. I am not sure how I missed it before. Somehow I was not aware that this library can display hundreds of data points in a dozen or so milliseconds +1
To make it run:

Upload WebSocket-Scope.inoto your ESP module. I have been using Arduino IDE 1.6.7 together with ESP platform package 2.1.0
Check on Serial Monitor if module joined Wi-Fi network and check assigned IP address
Open WebSocket-Scope.html in a web browser. I have tested this application on Windows 7 using IE 11 and Google Chrome 48.0 as well as on Ubuntu 14.04 LTS with FireFox 44.0
Update IP address of your ESP under Host:
Press Connect button
Web browser should establish connection with ESP and start requesting A0 samples
If communication is working you can then adjust number of requested samples using the Sample Size slider
Using Threshold slider you can adjust A0 level to trigger sampling. Keep it below noise level if you like to see continuous sampling.
What you should see:

Samples of A0 are displayed in chart ESP8266 Analog Input Scope at the top
Transmission statistics is displayed on chart Web Socket Throughput at the bottom left
At bottom right you should see sliders to adjust Sample Size and Threshold. Below is Communication Diagnostics that is showing individual data being received from ESP8266, connection status and message size, etc.
Have fun making it run and let me know if it works for you smile

Krzysztof
