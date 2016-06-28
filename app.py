import fakeredis
import time
import json
import pygal
import datetime

from werkzeug.wrappers import Response
from flask import Flask, request

# initialize app and our cache (it'll go away after the app stops running)
app = Flask(__name__)
app.config['DEBUG'] = True
temps_history_redis_store = fakeredis.FakeStrictRedis(0)
temps_instructions_redis_store = fakeredis.FakeStrictRedis(1)

# TODO: Add a form for entering instructions http://code.tutsplus.com/tutorials/intro-to-flask-adding-a-contact-page--net-28982
@app.route('/', methods=['GET'])
def get_temp():
    temps = []
    times = sorted([float(g.decode('utf-8')) for g in temps_history_redis_store.keys()])
    for k in times:
        temps.append(float(temps_history_redis_store.get(k).decode('utf-8')))
    title = "Temperature History"
    bar_chart = pygal.Bar(width=1200, height=600,
                          explicit_size=True, title=title)
    times = [datetime.datetime.fromtimestamp(g).strftime('%Y-%m-%d %H:%M:%S') for g in times]
    bar_chart.x_labels = times
    bar_chart.add('Temps in F', temps)
    html = """
        <html>
             <head>
                  <title>%s</title>
             </head>
              <body>
                 %s
             </body>
        </html>
        """ % (title, bar_chart.render())
    return html


# curl -H "Content-Type: application/json" -X POST -d '{"temp":72}' http://127.0.0.1:5000/api/v1/temp
@app.route('/api/v1/temp', methods=['POST'])
def post_temp():
    if not request.json:
        data = json.loads(request.data.decode('utf-8'))
    else:
        data = request.json
    if 'temp' in data.keys():
        temp = data['temp']
        temps_history_redis_store.set(time.time(), data['temp'])
        if temp > 75:
            return 'AC'
        elif temp < 65:
            return 'HEAT'
        else:
            return 'WINDOWS'
    return Response(status=400)


if __name__ == '__main__':
    app.run(host='0.0.0.0')
