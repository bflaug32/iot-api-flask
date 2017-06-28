import os
import time
import json
import requests

from colormap import Color
from datetime import datetime
from bs4 import BeautifulSoup
from werkzeug.routing import BaseConverter

from flask import Flask, request, render_template, url_for
from pymemcache.client.base import Client as MemcachedClient


##########################################################
#   SETUP
#   setup the flask application, the cache, and set an API key
##########################################################

app = Flask(__name__)
cache = MemcachedClient(('localhost',11211))
api_key = os.environ['MY_API_KEY']
open_weather_api_key = os.environ['OPEN_WEATHER_API_KEY']


##########################################################
#   Helper Functions
##########################################################

# allows routes to match regular expressions
class RegexConverter(BaseConverter):
    def __init__(self, url_map, *items):
        super(RegexConverter, self).__init__(url_map)
        self.regex = items[0]
app.url_map.converters['regex'] = RegexConverter


##########################################################
#   Homepage
#   render a template site, really just sugar on top
#   see http://flask.pocoo.org/
#   also http://flask.pocoo.org/docs/0.12/templating/
##########################################################
@app.route('/')
@app.route('/<regex("[A-Za-z0-9-_/.]{1,40}"):req>')
def hello(req=""):
    if 'breadfactorystudios' in request.url:
        return render_template('breadfactory.html'), 200
    elif 'learn' in request.url:
        return render_template('learn.html'), 200
    elif 'fabbit' in request.url:
        return render_template('fabbit.html'), 200
    return render_template('bradflaugher.html'), 200


##########################################################
#   COLOR PICKER
#   see http://jscolor.com/examples/
#   also https://github.com/spik3r/simpleFlaskForm/blob/master/basic_example.py
##########################################################
@app.route('/api/v1/picker',methods=['GET','POST'])
@app.route('/picker',methods=['GET','POST'])
def picker():
    confirmation_text=""
    if request.method == 'POST':
        color = request.form.get("color")
        color_object = Color('#'+color)
        r,g,b = color_object.rgb
        r=int(round(r*255,0))
        g=int(round(g*255,0))
        b=int(round(b*255,0))
        cache.set('color',json.dumps({"r":r,"g":g,"b":b}))
        confirmation_text = "Success! Color was set as R:%s G:%s B:%s" % (r,g,b)
    return render_template('picker.html',confirmation_text=confirmation_text) 

@app.route('/api/v1/getcolor')
def color():
    color = cache.get('color')
    
    if color:
        return color.decode('utf-8')

    return 'NOT SET... go to /api/v1/picker to set'

##########################################################
#   WEATHER FORECAST
#   for getting the weather forecast for a city or a US zip code
##########################################################
@app.route('/api/v1/getforecastcity', methods=['GET'])
def get_forecast_city():
    # pull from cache first
    city = request.args.get('q','Philadelphia')
    saved_forecast = cache.get('forecastcity'+city)

    if saved_forecast:
        return saved_forecast.decode('utf-8')

    # get the values from the query parameters. e.g. mysite.com/api/v1/getforecast?q=19147
    response = requests.get('http://api.openweathermap.org/data/2.5/weather?q='+city+'&APPID='+open_weather_api_key, timeout=10)

    # get the information from the JSON that was returned to us, if there was any
    weather_dictionary = {}
    if response.json():
        weather_dictionary = response.json()

    # format the information so we can process it, convert the temp to F from K
    return_dictionary = {
        "des" : '' if len(weather_dictionary.get("weather",[{}])) < 1 else weather_dictionary.get("weather",[{}])[0].get("description",""),
        "temp" : '' if weather_dictionary.get("main",{}).get("temp") is None else int(int(weather_dictionary.get("main",{}).get("temp",0)) * 9/5 - 459.67)
    }

    # set cache for an hour
    cache.set('forecastcity'+city,json.dumps(return_dictionary),3600)

    return json.dumps(return_dictionary)

@app.route('/api/v1/getforecastzip', methods=['GET'])
def get_forecast_zip():
    # pull from cache first
    zip_code = request.args.get('q','19147')
    saved_forecast = cache.get('forecastzip'+zip_code)

    if saved_forecast:
        return saved_forecast.decode('utf-8')

    # get the values from the query parameters. e.g. mysite.com/api/v1/getforecastzip?q=19147
    response = requests.get('http://api.openweathermap.org/data/2.5/weather?zip='+zip_code+',us&APPID='+open_weather_api_key, timeout=10)

    # get the information from the JSON that was returned to us, if there was any
    weather_dictionary = {}
    if response.json():
        weather_dictionary = response.json()

    # format the information so we can process it, convert the temp to F from K
    return_dictionary = {
        "des" : '' if len(weather_dictionary.get("weather",[{}])) < 1 else weather_dictionary.get("weather",[{}])[0].get("description",""),
        "temp" : '' if weather_dictionary.get("main",{}).get("temp") is None else int(int(weather_dictionary.get("main",{}).get("temp",0)) * 9/5 - 459.67)
    }

    # set cache for an hour
    cache.set('forecastzip'+zip_code,json.dumps(return_dictionary),3600)

    return json.dumps(return_dictionary)


##########################################################
#   STOCK QUOTE
#   for getting a stock quote for a certain symbol e.g. "AAPL"
#########################################################
@app.route('/api/v1/getstockquote', methods=['GET'])
def get_stock():
    # get the values from the query parameters. e.g. mysite.com/api/v1/getstockquote?q=AAPL
    symbol = request.args.get('s','AAPL')

    # pull from cache first
    saved_quote = cache.get('stock'+symbol)

    if saved_quote:
        return saved_quote.decode('utf-8')

    return_dictionary = {
        "price":"",
        "dir": ""
    }

    user_agent = 'Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36'
    source_url = 'http://www.nasdaq.com/symbol/'+symbol+'/real-time'
    response = requests.get(source_url, timeout=10, headers={
        'User-agent': user_agent})

    doc = BeautifulSoup(response.text,'html.parser')

    prices = doc.select('div[id="qwidget_lastsale"]')
    arrows = doc.select('div[id="qwidget-arrow"]')

    if len(prices) > 0 and len(arrows) > 0:
        return_dictionary = {
            "price":prices[0].getText().strip(),
            "dir": 'down' if 'red' in str(arrows[0]) else 'up'
        }

    # set cache for an hour
    cache.set('stock'+symbol,json.dumps(return_dictionary),600)

    return json.dumps(return_dictionary)


##########################################################
#   BASEBALL STANDINGS
#   for getting a stock quote for a certain symbol e.g. "AAPL"
#########################################################
@app.route('/api/v1/getmlbstandings', methods=['GET'])
def get_mlb_standings():
    # get the values from the query parameters. e.g. mysite.com/api/v1/getmlbstandings?t=philadelphis
    team = request.args.get('t','philadelphia')

    # pull from cache first
    saved_stats = cache.get('mlb'+team)

    if saved_stats:
        return saved_stats.decode('utf-8')

    return_dictionary = {
        "STRK":"",
        "W":"",
        "L":"",
        "GB":""}

    user_agent = 'Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36'
    source_url = 'http://www.cbssports.com/mlb/standings'
    response = requests.get(source_url, timeout=10, headers={
        'User-agent': user_agent})

    doc = BeautifulSoup(response.text,'html.parser')

    trs = doc.select('tr')

    columns = ["TEAM","W","L","PCT","GB","RS","RA","DIFF","HOME","ROAD","EAST","CENT","WEST","L10","STRK"]

    for tr in trs:
        if team.lower() in str(tr).lower():
            tds = tr.select('td')
            w_index = columns.index('W')
            l_index = columns.index('L')
            gb_index = columns.index('GB')
            strk_index = columns.index('STRK')
            if len(tds) >= max([w_index,l_index,gb_index,strk_index]):
                return_dictionary = {
                    "W":tds[w_index].getText().strip(),
                    "L":tds[l_index].getText().strip(),
                    "GB":tds[gb_index].getText().strip(),
                    "STRK":tds[strk_index].getText().strip()
                    }

    # set cache for an hour
    cache.set('mlb'+team,json.dumps(return_dictionary),600)

    return json.dumps(return_dictionary)

##########################################################
#   TEMPERATURE SENSOR
#   old code, for deomonstration purposes
##########################################################
@app.route('/api/v1/settemp', methods=['GET'])
def set_temperature():
    # set the temperature (called from something like an arduino)
    # get the values from the query parameters. e.g. mysite.com/api/v1/settemp?t=21&h=30
    temp = request.args.get('t'),
    humidity = request.args.get('h')

    cache.set('temp', json.dumps({"temp":temp,"humidity":humidity}))

    return "TEMPOK"

# get the current temperature (can use to populate a dashboard or view directly in a browser)
@app.route('/api/v1/gettemp', methods=['GET'])
def get_temperature():
    # get the values from the query parameters. e.g. mysite.com/api/v1/set-alarm?h=6&m=30
    return cache.get('temp')


##########################################################
#   MAIN
#   don't edit this unless you know what you're doing :)
##########################################################

# The "main" function, to run the server
if __name__ == '__main__':
    app.run(host='0.0.0.0')
