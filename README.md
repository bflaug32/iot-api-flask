# iot-api-flask
A simple API for Arduino-based devices using flask, created for and with the Internet Of Things Club at NextFab

##what's inside
* A simple API for sending and recieving data
* Code for some simple sensors that connects to the API

##Running via docker (Recommended)
* Build and run in Docker
```
cd iot-api-flask 
docker build -t iot-api:v1 .
docker run -p 5000:5000 iot-api:v1
```
* Visit the API's website. at http://localhost:5000/
* if localhost does not work, get the IP of your docker machine via the following command
```
docker-machine ip
```

##Running locally using virtualenv/python3.4 
* You'll need python installed (version 3.4+ from https://www.python.org)
* Consider setting up a virtual environment (google "virtualenv python")
* Clone this project
```
git clone https://path.to.this.project.git
```
* Install the requirements (check that your install of python is in this path first. note that the second and third lines are optional)
```
cd iot-api-flask
virtualenv -p /usr/local/bin/python3.4 env
source env/bin/activate
pip install -r requirements.txt
```
* Run the app
```
python application.py
```

##Mini-API Guide
* `api/v1/display/<device-name>` will display a chart of reported data from a device
* `api/v1/report` accepts JSON POSTs from IoT devices of data to report, sends back any instructions for the device
* TODO `api/v1/command` allows you to set a command to send to a device the next time it calls `report`

##Running the Example 'devices' Code
* Open the code from the devices folder in the Arduino IDE
* Upload to your MKR1000 and go!
* You'll need to change WIFI credentials and server data, it's commented in the code
* Please look to arduino IDE for libraries to install

## Contributors 
* Marc Zucchelli
* Anne Foster
* Brad Flaugher
* Ed Polk
