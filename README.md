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
./build.sh
```

The output should be in `build` folder - test app `build/vad_test`
```
cd build/
./vad_test
cd ..
cat out.txt
```
Test app processes `test_wav.wav` and outputs vad predictions to `out.txt`
Each line signifies prediction for 5 ms interval. These are raw predictions without any smoothing.

# Dependencies
```
# https://abseil.io
git clone https://github.com/abseil/abseil-cpp.git

# webrtc https://webrtc.org
git clone https://webrtc.googlesource.com/src
```
