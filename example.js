const cardInfo = require('./main.js');

// Get params for first playback device
console.log(cardInfo.get('plughw:0,0', cardInfo.PLAYBACK));
