import sys
import time
from wit import Wit
from deezer import deezer
import lightcontrol
from random import choice
import json
import ubercontrol
import wave
import pyaudio

SESSION = 'make_this_random'
main_context = {}
config = {
    'default_mood': 'angry'
}

if len(sys.argv) != 2:
    print("usage: python examples/quickstart.py <wit-token>")
    exit(1)
access_token = sys.argv[1]
listen_until = 0000

def first_entity_value(entities, entity):
    if entity not in entities:
        return None
    val = entities[entity][0]['value']
    if not val:
        return None
    return val['value'] if isinstance(val, dict) else val

def say(session_id, context, msg):
    print(msg)

def mood_from_intent(intent):
    if(intent == 'dance'):
        return 'happy'
    if(intent == 'cab' or intent == 'car' or intent == 'batmobile'):
        return 'bat_man'
    if(intent == 'chill'):
        return 'relaxing'
    return

def order_uber():
    ubers = ubercontrol.findRides()
    print(ubers)
    cars = len(ubers)
    many_car = cars > 1

    if cars:
        if many_car:
            say('there_are')
        else:
            say('there_is')

        if cars > 5:
            say('5_more')
        else:
            say(str(cars))

        if many_car:
            say('uber_cars')
        else:
            say('uber_car')

def say(name):
    wf = wave.open('speech/{name}.wav'.format(name=name), 'rb')
    # create an audio object
    p = pyaudio.PyAudio()

    # open stream based on the wave object which has been input.
    stream = p.open(format = p.get_format_from_width(wf.getsampwidth()),
                    channels = wf.getnchannels(),
                    rate = wf.getframerate(),
                    output = True)

    data = wf.readframes(1024)

    while data != '':
        stream.write(data)
        data = wf.readframes(1024)

    stream.close()
    p.terminate()

def merge(session_id, context, entities, msg):
    mood = first_entity_value(entities, 'mood')
    intent = first_entity_value(entities, 'intent')
    print(str(mood) + str(intent))

    if mood:
        context['mood'] = mood
    if intent:
        intended_mood = mood_from_intent(intent)
        if( not intended_mood == None):
            context['mood'] = intended_mood
        if(intent == 'cab' or intent == 'car' or intent == 'batmobile'):
            order_uber()

    if(not 'mood' in context.keys()):
	       context['mood'] = config['default_mood']

    return context

def error(session_id, context, e):
    print(str(e))

def change_lighting(session_id, context):
    moods = {
        "love":"deep_red",
        "horny":"deep_red",
        "excited":"normal",
        "happy": "normal",
        "relaxing":"sky_blue",
        "angry":"dark_green",
        "interested": "bright",
        "bat_man": "dark_blue"
    }
    colour={
        "deep_red": (1, 254, 125),
        "normal": (0, 0, 254),
        "sky_blue": (39000, 128, 50),
        "dark_green": (27000, 254, 50),
        "bright": (27000, 0, 254),
        "dark_blue": (45000, 254, 20)
    }
    context['lighting'] = moods.get((context["mood"] or context["intent"]),"normal")
    print(context["mood"])
    lightcontrol.setColor(*colour[context["lighting"]])
    return context

def change_music(session_id, context):
    moods = {
        "love":["love"],
        "horny":["porno"],
        "excited":["dance", "house"],
        "happy": ["dance", "house"],
        "relaxing":["chillout"],
        "angry":["metal"]
    }
    jdata = json.loads(open("config.json").read().decode())
    genres = jdata["playlists"]
    print(genres)
    print(context["mood"])
    context['genre'] = choice(moods.get(context["mood"], ["chillout"]))
    deezer.stop()
    print(genres[context["genre"]])
    t=deezer.Thread(target=deezer.playlist, args=[genres[context["genre"]]])
    t.start()
    print(str(context))
    return context


actions = {
    'merge': merge,
    'say': say,
    'error': error,
    'change_lighting': change_lighting,
    'change_music': change_music
}

client = Wit(access_token, actions)

def talk(message=''):
    global main_context
    global listen_until

    print (message, 'sophie' in message.lower())

    if('sophie' in message.lower()):
        print('heard my name')
        listen_until = time.time() + 20

    if(not message == '' and  time.time() < listen_until):
        main_context = client.run_actions(SESSION, message, main_context)

def bind_config(cfg={}):
    global config
    config = cfg
