import time
import json

from datetime import datetime
from flask import request, render_template, url_for

from service import app, commands


@app.route('/')
def hello():
    return render_template('home.html'), 200

@app.route('/favicon.ico')
def favicon():
    return b'', 204

@app.route('/api/v1/setalarm', methods=['GET'])
def set_alarm():
    # get the values from the query parameters. e.g. mysite.com/api/v1/set-alarm?h=6&m=30
    hour = request.args.get('h'),
    minute = request.args.get('m')
    key = request.args.get('key')

    if key == app.config['API_KEY']:

        # if hour and minute are numbers then save them to the cache
        if hour and minute:
            hour = hour[0]  # some strange tuple bug with flask
            if hour.isdigit() and minute.isdigit():
                hour = int(hour)
                minute = int(minute)
                if hour >= 0 and hour < 24 and minute >= 0 and minute < 60:
                    data_to_save = {
                        'hour': hour,
                        'minute': minute
                    }
                    commands.set('alarm', json.dumps(data_to_save))
                    return "SUCCESS: alarm set for %s:%s" % (hour, minute)

    return "error"


@app.route('/api/v1/getalarm')
def get_alarm():
    saved_time = commands.get('alarm')

    if saved_time:
        # decode and parse the json from the cache, if there's any available
        saved_time_dict = json.loads(saved_time.decode('utf-8'))
        alarm_start_hour = int(saved_time_dict['hour'])
        alarm_start_minute = int(saved_time_dict['minute'])

        # we want the alarm to be on for 30 minutes
        if alarm_start_minute >= 30:
            alarm_end_hour = alarm_start_hour + 1
            alarm_end_minute = (alarm_start_minute + 30) % 60
        else:
            alarm_end_hour = alarm_start_hour
            alarm_end_minute = alarm_start_minute + 30

        # get the current hour and minute
        current_hour = int(datetime.now().strftime('%H'))
        current_minute = int(datetime.now().strftime('%M'))

        current_time_in_minutes = current_hour * 60 + current_minute
        alarm_start_in_minutes = alarm_start_hour * 60 + alarm_start_minute
        alarm_end_in_minutes = alarm_end_hour * 60 + alarm_end_minute
        if alarm_start_in_minutes <= current_time_in_minutes <= alarm_end_in_minutes:
            return 'ON'

        return "time is %d:%d alarm starts at %d:%d and ends at %d:%d" % (
            current_hour, current_minute, alarm_start_hour, alarm_start_minute, alarm_end_hour, alarm_end_minute)

    return 'NOT SET'

