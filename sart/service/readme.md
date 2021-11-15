Autostart web server


1.create a file in /etc/systemd/system called sart.service (either cp or use nano)

2. sudo systemctl daemon-reload

3. sudo systemctl enable sart

You can manually start/ stop by:

sudo systemctl start(or stop) sart

use disable to disable on startup.