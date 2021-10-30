import wave
import array
import matplotlib.pyplot as plt
import numpy as np

wav = wave.open('recording.wav')
framerate = wav.getframerate() #int : 44100 (nb de valeurs par secondes)
bytes = wav.readframes(wav.getnframes())
signal = array.array('h', bytes).tolist() #liste

print(f"{framerate} \t type: {type(framerate)}")
#print(f"Bytes: {bytes}\t type: {type(bytes)}")
print(f"type: {type(signal)}")