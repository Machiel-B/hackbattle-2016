import random
import requests
import threading
import json

randomThread = None

def apiCall(data):
    url = 'http://192.168.0.100/api/390432d92ee06f5729ccef1d19df725b/lights/3/state'
    requests.put(url, data=json.dumps(data))

def setState(state):
    data = {"on":state}
    apiCall(data)

def randomLoopStart(interval, sat = -1, bri = -1):
    global randomThread
    randomThread = threading.Timer(interval, randomLoopStart, args=[interval, sat, bri])
    randomThread.start()

    if sat < 0 :
        sat = random.randint(0,254)

    if bri < 0 :
        bri = random.randint(0,254)

    data = {"on":True, "sat":sat, "bri":bri, "hue":random.randint(0,65535)}
    apiCall(data)

def randomLoopStop():
    global randomThread
    randomThread.cancel()

def setHue(hue):
    if hue < 0:
        hue = 0
    elif hue > 65535:
        hue = 65535
    data = {"hue": hue}
    apiCall(data)

def setBrightness(bri):
    if bri < 0:
        bri = 0
    elif bri > 254:
        bri = 254
    data = {"bri": bri}
    apiCall(data)

def setSaturation(sat):
    if sat < 0:
        sat = 0
    elif sat > 254:
        sat = 254
    data = {"sat": sat}
    apiCall(data)

def setColor(hue, sat, bri):
    if hue < 0:
        hue = 0
    elif hue > 65535:
        hue = 65535

    if bri < 0:
        bri = 0
    elif bri > 254:
        bri = 254

    if sat < 0:
        sat = 0
    elif sat > 254:
        sat = 254

    data = {"hue": hue, "sat": sat, "bri": bri}
    apiCall(data)