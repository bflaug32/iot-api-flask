import time
import json
import pygal
import random

from datetime import datetime
from werkzeug.wrappers import Response
from flask import request, jsonify, render_template

from service import app, instructions, commands, history
from service.utils import populate_instructions


@app.route('/')
def hello():
    return "Welcome to the Internet of Things API"


@app.route('/api/v1/display/<requested_data_name>', methods=['GET'])
def display(requested_data_name):
    """
    Display data from a supported source
    :param requested_data_name: either 'temp' or 'light' etc...
    :return: a chart of datapoints from the requested sensor
    """

    # return 400 (bad request) error if the requested sensor is not supported
    if requested_data_name not in history.keys():
        return Response(status=400)

    # use a pipeline to get data from the redis cache
    recorded_times = sorted([float(g.decode('utf-8')) for g in history[requested_data_name].keys()])
    pipe = history[requested_data_name].pipeline()
    for k in recorded_times:
        pipe.get(k)
    returned_data = pipe.execute()

    # sort the returned data for display
    display_data = []
    for d in returned_data:
        display_data.append(float(d.decode('utf-8')))

    # return most recent data
    last_update_value = "N/A"
    last_update_time = "N/A"
    if len(returned_data) > 0:
        last_update_value = int(returned_data[-1])
        last_update_time = datetime.fromtimestamp(recorded_times[-1]).strftime('%Y-%m-%d %H:%M:%S')

    # format and return a line chart of the
    chart = pygal.Line(title=requested_data_name, width=800, height=400, explicit_size=True)
    chart.x_labels = [datetime.fromtimestamp(g).strftime('%Y-%m-%d %H:%M:%S') for g in recorded_times]
    chart.add(requested_data_name, display_data)
    return render_template('display.html', requested_data_name=requested_data_name, last_update_value=last_update_value,
                           last_update_time=last_update_time,chart_data=chart.render().decode('utf-8'))


# TODO for testing
@app.route('/populate')
def populate():
    for i in range(10):
        history['temp'].set(time.time(), random.randint(70, 85))
    return "Temps have been polulated"


@app.route('/api/v1/report', methods=['POST'])
def report():
    """
    Gets sensor data and returns instructions (if any) think 'reporting for duty'
    curl -H "Content-Type: application/json" -X POST -d '{"temp":72}' http://127.0.0.1:5000/api/v1/report
    :return: instructions for the device, if any
    """
    try:
        # get the data that was posted from the sensor
        posted_data = request.json if request.json else json.loads(request.data.decode('utf-8'))

        # post any and all sensor data to history
        instructions_to_send = 'None'
        for k in posted_data.keys():
            if k in history.keys():
                history[k].set(time.time(), posted_data[k])
                populate_instructions(k, posted_data[k])
            if k in instructions.keys():
                instructions_to_send = instructions[k]

        return jsonify(instructions=instructions_to_send)

    except ValueError:
        return Response(status=400)


@app.route('/api/v1/command')
def command():
    # The idea here is to be able to send commands on the fly and adjust the behavior of a specific device
    # TODO: Add a form for manually instructions http://code.tutsplus.com/tutorials/intro-to-flask-adding-a-contact-page--net-28982
    return jsonify(**commands)
