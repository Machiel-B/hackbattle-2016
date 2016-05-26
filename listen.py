import pyaudio
import wave
import audioop
from collections import deque
import os
import urllib2
import urllib
import time
import math
import sys
from base64 import b64encode
import witCommands
import cStringIO
import contextlib
import json

FLAC_CONV = 'flac -f'  # We need a WAV to FLAC converter. flac is available
                       # on Linux

WIT_SPEECH_API = 'https://api.wit.ai/speech?v=20160511'

token = sys.argv[1]

DEBUG = False
# Microphone stream config.
CHUNK = 1024  # CHUNKS of bytes to read each time from mic
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000
DEVICE_INDEX = pyaudio.PyAudio().get_default_input_device_info()['index']

THRESHOLD = 2500  # The threshold intensity that defines silence
                  # and noise signal (an int. lower than THRESHOLD is silence).

SILENCE_LIMIT = 2  # Silence limit in seconds. The max ammount of seconds where
                   # only silence is recorded. When this time passes the
                   # recording finishes and the file is delivered.

PREV_AUDIO = 0.9  # Previous audio (in seconds) to prepend. When noise
                  # is detected, how much of previously recorded audio is
                  # prepended. This helps to prevent chopping the beggining
                  # of the phrase.


def read_chunk_safe(stream,chunk=CHUNK):
    try:
        cur_data = stream.read(CHUNK, exception_on_overflow=False)
    except IOError, e:
        print(e.args[0], pyaudio.paInputOverflowed)
        if e.args[1] == pyaudio.paInputOverflowed:
            cur_data= '\x00' * CHUNK
        else:
            raise
    return cur_data

def audio_int(num_samples=50):
    """ Gets average audio intensity of your mic sound. You can use it to get
        average intensities while you're talking and/or silent. The average
        is the avg of the 20% largest intensities recorded.
    """

    print "Getting intensity values from mic."
    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    input_device_index= DEVICE_INDEX,
                    frames_per_buffer=CHUNK)

    values = [math.sqrt(abs(audioop.avg(read_chunk_safe(stream, CHUNK), 4)))
              for x in range(num_samples)]
    values = sorted(values, reverse=True)
    r = sum(values[:int(num_samples)*0.2]) / int(num_samples) *0.2
    print " Finished "
    print " Average audio intensity is ", r
    stream.close()
    p.terminate()
    return r


def listen_for_speech(threshold=THRESHOLD, num_phrases=-1):
    """
    Listens to Microphone, extracts phrases from it and sends it to
    Google's TTS service and returns response. a "phrase" is sound
    surrounded by silence (according to threshold). num_phrases controls
    how many phrases to process before finishing the listening process
    (-1 for infinite).
    """

    #Open stream
    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    input_device_index= DEVICE_INDEX,
                    frames_per_buffer=CHUNK)

    print('rate');
    print(p.get_device_info_by_index(DEVICE_INDEX)['defaultSampleRate'])
    print ("* Listening mic. ")
    audio2send = []
    cur_data = ''  # current chunk  of audio data
    rel = RATE/CHUNK
    slid_win = deque(maxlen=SILENCE_LIMIT * rel)
    #Prepend audio from 0.5 seconds before noise was detected
    prev_audio = deque(maxlen=PREV_AUDIO * rel)
    started = False
    n = num_phrases
    response = []

    while (num_phrases == -1 or n > 0):
        cur_data = read_chunk_safe(stream, CHUNK)
        #print(len(cur_data))
        slid_win.append(math.sqrt(abs(audioop.avg(cur_data, 4))))

        if(sum([x > THRESHOLD for x in slid_win]) > 0):
            if(not started):
                print( "Starting record of phrase")
                started = True
            audio2send.append(cur_data)
        elif (started is True):
            print( "Finished")
            # The limit was reached, finish capture and deliver.
            #filename = save_speech(list(prev_audio) + audio2send, p)
            with contextlib.closing(
                speech_to_wave(list(prev_audio) + audio2send, p)
            ) as wave_file:
                # Send file to Wit and get response
                r = wit_analyze(wave_file)
                if num_phrases == -1:
                    print(r)
                    witCommands.talk(r)
                else:
                    response.append(r)

                # Remove temp file. Comment line to review.
              #  os.remove(filename)
                # Reset all
                started = False
                slid_win = deque(maxlen=SILENCE_LIMIT * rel)
                prev_audio = deque(maxlen=0.5 * rel)
                audio2send = []
                n -= 1
                print "Listening ..."
        else:
            prev_audio.append(cur_data)

    print "* Done recording"
    stream.close()
    p.terminate()

    return response


def speech_to_wave(data, p):
    """ Saves mic data to temporary WAV file. Returns filename of saved
        file """

    buffer = cStringIO.StringIO()
    data = ''.join(data)
    with contextlib.closing(wave.open(buffer, 'wb')) as wf:
        wf.setnchannels(1)
        wf.setsampwidth(p.get_sample_size(pyaudio.paInt16))
        wf.setframerate(16000)  # TODO make this value a function parameter?
        wf.writeframes(data)

    buffer.flush()
    buffer.seek(0)

    if DEBUG:
        with open('{}.wav'.format(time.time()), 'wb') as f:
            f.write(buffer.read())

        buffer.seek(0)

    return buffer


def wit_analyze(audio_file):
    """ Sends audio file (audio_fname) to Google's text to speech
        service and returns service's response. We need a FLAC
        converter if audio is not FLAC (check FLAC_CONV). """

    print "Sending ", audio_file

    auth = 'Bearer %s' % (token)

    # Headers. A common Chromium (Linux) User-Agent
    hrs = {
        'User-Agent': "Mozilla/5.0 (X11; Linux i686) AppleWebKit/535.7 (KHTML, "
                      "like Gecko) Chrome/16.0.912.63 Safari/535.7",
        'Content-Type': 'audio/wav',
        'Authorization': auth,
        'Accept': '*/*'
    }

    req = urllib2.Request(WIT_SPEECH_API,  data=audio_file.read(), headers=hrs)
    print "Sending request to WIT"

    try:
        p = urllib2.urlopen(req)
        response = p.read()
        decoded = json.loads(response)
        res = decoded['_text']
        print(str(res))
    except urllib2.HTTPError, error:
        contents = error.read()
        print(contents)
        res = None

    return res


if(__name__ == '__main__'):
    listen_for_speech()  # listen to mic.
    #print stt_google_wav('hello.flac')  # translate audio file
    #audio_int()  # To measure your mic levels
