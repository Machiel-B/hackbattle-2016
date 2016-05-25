import sys
import time
from wit import Wit


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
        return 'relaxing'
    if(intent == 'chill'):
        return 'relaxing'
    return

def order_uber():
    print('uber ordered')


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
    print(str(context))
    context['lighting'] = 'bright'
    return context

def change_music(session_id, context):
    context['song'] = 'i dunno an of the - cool new music'
    context['genre'] = 'country rock'
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
