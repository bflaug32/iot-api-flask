import fakeredis
from flask import Flask

app = Flask(__name__)
app.config['DEBUG'] = True

# add caches to save data about the state of each object
history = {
    'temp': fakeredis.FakeStrictRedis(0),
    'light': fakeredis.FakeStrictRedis(1),
    'mic': fakeredis.FakeStrictRedis(2),
    'speaker': fakeredis.FakeStrictRedis(4),
    'text': fakeredis.FakeStrictRedis(5),
    'multi_light': fakeredis.FakeStrictRedis(5),
}

# instructions are the automated commands for each device
instructions = {
    'light': 'None',
    'speaker': 'None',
    'text': 'None',
    'multi_light': 'None'
}

# commands are overrides to instructions, and are manually set by the user, and cleared when performed
commands = {
    'light': 'None',
    'speaker': 'None',
    'text': 'None',
    'multi_light': 'None'
}

from service import views