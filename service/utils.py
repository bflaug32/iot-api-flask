from pytz import timezone
from datetime import datetime

from service import instructions


def populate_instructions(device_name, device_reading):
    """
    takes a device and a reading and populates the instructions with an appropriate action
    it can also take action based on readings from the web or from user input
    """

    now_time = datetime.now(timezone('US/Eastern'))
    if device_name == 'mic':
        if device_reading > 100:
            instructions['text'] = 'IT SURE IS LOUD!'
    if device_name == 'temp':
        # if device_reading > 100: send email to heating man
        # if device_reading > 80: AC
        # elif device_reading < 60: HEAT
        # else: NOTHING
        pass
    elif device_name == 'light':
        # Light based alarm
        if now_time.hour == 6:
            instructions['light'] = 'On'
        else:
            instructions['light'] = 'Off'
    elif device_name == 'speaker':
        # sound-based alarm
        if now_time.hour == 6 and now_time.minute > 30:
            instructions['speaker'] = 'Song'
    elif device_name == 'text':
        # a friendly greeting based on time of day
        if now_time < 6:
            instructions['text'] = 'sleeeeep'
        elif now_time.hour < 12:
            instructions['text'] = 'Good Morning!'
        elif now_time.hour == 12:
            instructions['text'] = 'Good Nooning!'
        elif now_time.hour < 17:
            instructions['text'] = 'Good Afternoon'
        elif now_time.hour <= 21:
            instructions['text'] = 'Good Evening'
        elif now_time.hour <= 24:
            instructions['text'] = 'Bedtime'
    elif device_name == 'multi_light':
        # use the multi-light as a weather station
        pass
