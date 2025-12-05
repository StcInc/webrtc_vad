# WebRTC Voice Activity Detector

This repo contains build instructions and example of usage for voice activity detection.
For now only C++ is supported, though in future python bindings should be added.

This repo is inspired by https://github.com/wiseman/py-webrtcvad
Which, unfortunately, uses outdated version of WebRTC code base.

# Get started

In order to build you will need g++, cmake. Only linux supported for now.
```
git clone --recursive https://github.com/StcInc/webrtc_vad
cd webrtc_vad
./rebuild.sh
./run.sh
```
### Output should look like
```
...
Predictions:
[{"start": 0,"end": 0.68,"label": "background"},
{"start": 0.68,"end": 1.34,"label": "speech"},
{"start": 1.34,"end": 1.39,"label": "background"},
{"start": 1.39,"end": 1.74,"label": "speech"},
{"start": 1.74,"end": 1.78,"label": "background"},
{"start": 1.78,"end": 2.41,"label": "speech"},
{"start": 2.41,"end": 2.45,"label": "background"},
{"start": 2.45,"end": 2.75,"label": "speech"},
{"start": 2.75,"end": 2.8,"label": "background"},
{"start": 2.8,"end": 3.28,"label": "speech"},
{"start": 3.28,"end": 3.74,"label": "background"},
{"start": 3.74,"end": 5.07,"label": "speech"},
{"start": 5.07,"end": 5.12,"label": "background"},
{"start": 5.12,"end": 6.73,"label": "speech"},
{"start": 6.73,"end": 7,"label": "background"}]
```

# Dependencies
```
# https://abseil.io
git clone https://github.com/abseil/abseil-cpp.git

# webrtc https://webrtc.org
git clone https://webrtc.googlesource.com/src
```
