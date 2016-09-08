

from flask import Flask

app = Flask(__name__)
# you need a config_local.py in the root folder for this to work
app.config.from_object('config_local.Config')

# add caches to save data about the state of each object
if app.config['DEBUG'] is True:
    import fakeredis
    commands = fakeredis.FakeStrictRedis(0)
else:
    import redis
    _redis_host = app.config['REDIS_HOST']
    _redis_port = app.config['REDIS_PORT']
    commands = redis.StrictRedis(host=_redis_host, port=_redis_port, db=0)

from service import views