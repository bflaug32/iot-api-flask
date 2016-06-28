# iot-api-flask
A simple API for Arduino-based devices using flask, created for and with the Internet Of Things Club at NextFab

##what's inside

* A simple API for sending and recieving data
* Code for some simple sensors that connects to the API

##Running the API project

* You'll need python installed (version 3.4+)

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
python app.py
```

##Running the Sensor/"Actor" Code
* Open the code from the _sensors folder in the Arduino IDE

* Upload to your MKR1000 and go!

* You'll need to change WIFI credentials and server data, it's commented in the code

* Please look to arduino IDE for libraries to install

## Goals

### Hub (Python/RaspberryPi)

* The hub does two things, it reads internet-connected sensors and controls internet-connected "actors"
* Sensors report their data to their assigned API endpoint (right now we only support "TEMP"), but in future they can register themselves giving a name, type, value type that they will be sending
* "Actors" report in a similar way, giving their names, IP addresses, and supported functions
* The hub acts as a quarterback for both the data and the interface for the connected devices

### Sensors

* Sensors are simple, you connect them and they report to the hub, for an example see the "temperature_sensor" code in this project

###"Actors"

* Actors can do a variety of functions (run a servo, turn on lights, feed the dog, electrocute an intruder)

## Roadmap

### Hub Roadmap

1. Improve the hub to allow registration of Sensors and "Actors" 
2. Add a proper database (e.g. sqlite3) to store sensor data and other goodies
3. Add security features (and figure out what those look like)
4. Options on what types of views for sensor data, guess what type is best
5. Alerts (browser, SMS, etc...)
6. Rules/scripts so you can control actors from sensor input

### Sensor/Actor Roadmap

1. Make them pretty
2. Make them useful
3. Put them in enclosures

## Contributors 

Marc Zucchelli
Anne Foster
Brad Flaugher
