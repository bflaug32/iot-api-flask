

from flask import Flask

app = Flask(__name__)
# you need a config_local.py in the root folder for this to work
app.config.from_object('config_local.Config')

# add caches to save data about the state of each object
import fakeredis
commands = fakeredis.FakeStrictRedis(0)

from service import views