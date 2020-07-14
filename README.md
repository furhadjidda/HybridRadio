 # Hybrid Radio using Radio DNS open standards
 
 Notes:
 For auto starting the Hybrid Radio app on startup I am using autostart method
 Reference: https://www.digikey.com/en/maker/projects/how-to-run-a-raspberry-pi-program-on-startup/cc16cb41a3d447b8aaacf1da14368b13
 If you need access to elements from the X Window System (e.g. you are making a graphical dashboard or game),
 then you will need to wait for the X server to finish initializing before running your code. One way to 
 accomplish this is to use the autostart system.
 After your desktop environment starts (LXDE-pi, in this case), it runs whatever commands it finds in the profile’s autostart script,
 which is located at /home/pi/.config/lxsession/LXDE-pi/autostart for our Raspberry Pi. Note that the directory pi might be different
 if you created a new user for your Raspberry Pi.
 If no user autostart script is found, Linux will run the global /etc/xdg/lxsession/LXDE-pi/autostart script instead.

In addition to running commands in autostart, Linux will also look for and execute .desktop scripts found in /home/pi/.config/autostart.
The easiest way to execute GUI programs on boot is to create one of these .desktop scripts.


`mkdir /home/pi/.config/autostart`

`nano /home/pi/.config/autostart/hybridRadio.dekstop`

Copy in the following text into the clock.desktop file

`[Desktop Entry]

Type=Application

Name=HybridRadio

Exec=/home/pi/qt-apps/HybridRadio/bin/HybridRadio`



`sudo reboot`


For Volume Controller using KY040 , I am using the systemd method of starting it automatically.
copy the volumeControl.service file in location /lib/systemd/system/

then run the following commands

`sudo systemctl daemon-reload`

`sudo systemctl enable volumeControl.service`

`sudo reboot`

To restart the service use:
`sudo systemctl restart volumeControl.service`

To stop the service use:
`sudo systemctl stop volumeControl.service`

To see the logs use
`journalctl -u volumeControl.service`

OR

`journalctl -fu volumeControl.service`
