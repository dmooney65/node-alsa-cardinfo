# node-alsa-cardinfo

A native module using [N-API](https://nodejs.org/api/n-api.html) for determining the playback and/or capture capabilities of ALSA audio devices.

Useful when determining which parameters to pass to, for example, `arecord` or `aplay`.

This module depends on 'libasound-dev'.

## Usage
Install and use as follows:<br> 
`$ npm install node-alsa-cardinfo --save`<br>

Add require statement:<br>
`const cardInfo = require('node-alsa-cardinfo');`<br>

There are two methods, one for listing the available audio interfaces and one for getting the details about a specific device

## LIST

The `list` gets a list of available audio devices as and array of objects.
It does not take any parameters. 

### Return values

An array of objects will be returned like so:<br>
 If the device is input-only or output-only, the object will have a paramter `io` which will be set to either "Input", "Output" or "Bidirectional". <br>
```
[ { name: 'null',
    desc:
     'Discard all samples (playback) or generate zero samples (capture)',
     io: 'Bidirectional' },
  { name: 'default:CARD=Device',
    desc: 'USB PnP Sound Device, USB Audio\nDefault Audio Device',
     io: 'Bidirectional' },
  { name: 'sysdefault:CARD=Device',
    desc: 'USB PnP Sound Device, USB Audio\nDefault Audio Device',
     io: 'Bidirectional' },
...
  { name: 'hw:CARD=ALSA,DEV=1',
    desc:
     'bcm2835 ALSA, bcm2835 IEC958/HDMI\nDirect hardware device without any conversions',
    io: 'Output' },
  { name: 'plughw:CARD=ALSA,DEV=0',
    desc:
     'bcm2835 ALSA, bcm2835 ALSA\nHardware device with all software conversions',
    io: 'Output' },
  { name: 'plughw:CARD=ALSA,DEV=1',
    desc:
     'bcm2835 ALSA, bcm2835 IEC958/HDMI\nHardware device with all software conversions',
    io: 'Output' } ]
```

## GET

The `get` returns the details about a specific device as an object. e.g.<br>
`let info = cardInfo.get('hw:0,0',cardInfo.PLAYBACK);`<br>
`let info = cardInfo.get('default', 1);`<br>
`let info = cardInfo.get();`

It takes two optional parameters which correspond to the card name (e.g. `hw:0,0` or `default`) and the device direction ( whether the device is a playback or capture device).<br>
The card name will default to `hw` if not specified and the device direction defaults to playback (`0` or `cardInfo.PLAYBACK`).

### Return values

If the device name requested is valid, an object will be returned of the form:<br>
```
{ cardNum: 0,
  devNum: 0,
  subDevNum: 0,
  id: 'USB Audio',
  name: 'USB Audio',
  subName: 'subdevice #0',
  deviceType: 'HW',
  accessTypes: [ 'MMAP_INTERLEAVED', 'RW_INTERLEAVED' ],
  sampleFormats: [ 'S16_LE', 'S32_LE' ],
  channels: [ 2, 4, 6, 8 ],
  sampleRates: [ 44100, 48000, 96000, 192000 ] }
```

For more information on the returned values, consult the [alsaaudio documentation](https://larsimmisch.github.io/pyalsaaudio/libalsaaudio.html).

If there is an error, an object with `error` and `errorDetails` properties will be returned, e.g.<br>
`{ error: 'cannot open device: hw:0,0 - Device or resource busy',`<br>
&nbsp;&nbsp;&nbsp;`errorDetails: 'pcm_hw.c:1590(snd_pcm_hw_open) open \'hw:0,0\' failed (16)' }`

## KNOWN ISSUES

`list` cannot be called after a `get` call is made.
