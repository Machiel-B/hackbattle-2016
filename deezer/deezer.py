import subprocess
from time import sleep
from threading import Thread
from threading import Lock

p = None
k = Lock()

def play(track_id):
    global p
    p = subprocess.Popen(["{}/NanoPlayer".format("/home/corrupted/HackBattle/deezer_wrapper"),str(track_id)])

def stop():
    global p
    p.kill()
    k.acquire()
    sleep(0.5)

def playlist(l_tracks):
    for i_track in l_tracks:
        if k.locked():
            break
        if p is not None:
            while p.poll() is None:
                if k.locked():
                    break
                sleep(0.01)
            play(i_track)
        else:
            play(i_track)


if __name__ == "__main__":
    t = Thread(target=playlist, args=[[108707936, 3135553]])
    t.start()
    print("waiting")
    sleep(10)
    p.terminate()
    sleep(10)
    p.terminate()