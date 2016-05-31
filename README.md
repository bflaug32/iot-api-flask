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

##Running the Sensor Code
* Open the code from the _sensors folder in the Arduino IDE

* Upload to your MKR1000 and go!

* You'll need to change WIFI credentials and server data, it's commented in the code