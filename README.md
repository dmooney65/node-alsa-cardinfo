# node-alsa-cardinfo

A native module using [N-API](https://nodejs.org/api/n-api.html) for determining the playback and/or capture capabilities of ALSA audio devices.

Useful when determining which parameters to pass to, for example, `arecord` or `aplay`.

This module depends on 'libasound-dev'.

## Usage

Install and use as follows:<br> 
`$ npm install node-alsa-cardinfo --save`<br>

Add require statement:<br>
`const cardInfo = require('node-alsa-cardinfo');`<br>

The addon has a single method which returns an object. e.g.

```javascript
let info = cardInfo.get('hw:0,0',cardInfo.PLAYBACK);
let info = cardInfo.get('default', 1);
let info = cardInfo.get();
```

It takes two optional parameters which correspond to the card name (e.g. `hw:0,0` or `default`) and the device direction ( whether the device is a playback or capture device).
The card name will default to `hw` if not specified and the device direction defaults to playback (`0` or `cardInfo.PLAYBACK`).

## Return values

If the device name requested is valid, an object will be returned of the form:

```json
{ deviceType: 'HW',
  accessTypes: [ 'MMAP_INTERLEAVED', 'RW_INTERLEAVED' ],
  sampleFormats: [ 'S16_LE', 'S32_LE' ],
  channels: [ 2, 4, 6, 8 ],
  sampleRates: [ 44100, 48000, 96000, 192000 ] }
```

For more information on the returned values, consult the [alsaaudio documentation](https://larsimmisch.github.io/pyalsaaudio/libalsaaudio.html).

If there is an error, an object with `error` and `errorDetails` properties will be returned, e.g.

```json
{ error: 'cannot open device: dmix - Invalid argument',
  errorDetails: 'pcm_dmix.c:968(snd_pcm_dmix_open) The dmix plugin supports only playback stream' }
```
