const cardInfo = require('./main.js');

// Get params for first playback device
console.log(cardInfo.get('dsnoop', cardInfo.PLAYBACK));

console.log(cardInfo.get('dmix', cardInfo.PLAYBACK));

console.log(cardInfo.get('dmix', cardInfo.CAPTURE));

console.log(cardInfo.get('hw:0,0', cardInfo.CAPTURE));

console.log(cardInfo.get('hw:0,0', cardInfo.PLAYBACK));


