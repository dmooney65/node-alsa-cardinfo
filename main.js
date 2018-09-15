const cardinfo = require('bindings')('alsa_cardinfo');

module.exports.get = (card = 'hw', type) => {
    return cardinfo.get_card_info(card, type);
}

module.exports.PLAYBACK = 0;
module.exports.CAPTURE = 1;
