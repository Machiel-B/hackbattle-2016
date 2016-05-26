from flask import Flask, render_template, request, jsonify, abort
from time import sleep
from HTMLParser import HTMLParser
import json

config = None

app = Flask(__name__, static_url_path='')

@app.route('/')
def index():
    return app.send_static_file('index.html')


@app.route('/api/settings/', methods=['GET', 'POST', 'PATCH', 'PUT', 'DELETE'])
def api():
    global config
    if request.method == 'PATCH':
        error = False
        for key in request.json.keys():
            if(not key in config.keys()):
                error = 'key %s not in settings' % (key)
                break
            config[key] = request.json[key]


    	if(error):
                response = jsonify({
                    'message': 'GENERIC_ERROR',
                    'errors': error
                })
                response.status_code = 400
                return response
        f = open('config.json', 'w')
        f.write(json.dumps(config))
        f.close()


    	response = jsonify(config)
    	response.status_code=200
    	return response


    if request.method == 'GET':
        return jsonify(config);



@app.after_request
def add_header(response):
    response.cache_control.max_age = 300
    return response

def start_server(cfg={}):
    global config
    config = cfg
    app.run(debug=True, threaded=True,)


if __name__ == '__main__':
    
    try:
        cfg = json.loads(open('config.json','r').read())
    except:
	cfg = {'test':'yolo'} 
    start_server(cfg)
