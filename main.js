const cardinfo = require('bindings')('alsa_cardinfo');

module.exports.get = (card = 'hw', type) => {
    return cardinfo.get_card_info(card, type);
}

module.exports.list = () => {
    return cardinfo.enumerate_cards();
}

module.exports.PLAYBACK = 0;
module.exports.CAPTURE = 1;

module.exports.INPUT_DEVICE = "Input";
module.exports.OUTPUT_DEVICE = "Output";
module.exports.BIDIRECTIONAL_DEVICE = "Bidirectional";

