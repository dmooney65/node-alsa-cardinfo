var assert = require('assert');
var cardinfo = require('../main.js');

describe('cardinfo.get CI', function () {
    it('should use default values', function (done) {
        
        var info = cardinfo.get('null');
        assert(info.deviceType, 'Should have valid response for devault values');
        assert.strictEqual(info.deviceType, 'NULL', 'Should return value');
        done();
    });

    it('should return error for invalid value', function (done) {
        var info = cardinfo.get('invalid_value');
        assert(info.error, 'Error should be propagated for invalid device');
        assert.strictEqual(info.error,'cannot open device: invalid_value - No such file or directory','Should return correct error value');
        done();
    });
    
    it('should return error for incorrect card value', function (done) {

        var info = cardinfo.get('hw:3,0');
        assert(info.error, 'Error should be propagated for invalid device');
        assert.strictEqual(info.error,'cannot open device: hw:3,0 - No such file or directory','Should return correct error value');
        done();
    });

    it('should return all values for NULL playback device', function (done) {
        
        var info = cardinfo.get('null',cardinfo.PLAYBACK);
        assert(info.deviceType, 'Should have deviceType');
        assert(info.accessTypes, 'Should have accessTypes');
        var accessTypesArray = info.accessTypes;
        assert.strictEqual(accessTypesArray.length >= 1, true, 'Arrays should contain at leas one value');
        assert(info.sampleFormats, 'Should have sampleFormats');
        assert(info.channels, 'Should have channels');
        assert(info.sampleRates, 'Should have sampleRates');
        done();
    });

    it('should populate arrays for NULL playback device', function (done) {
        
        var info = cardinfo.get('null',cardinfo.PLAYBACK);
        var accessTypesArray = info.accessTypes;
        assert.strictEqual(accessTypesArray.length >= 1, true, 'Access types array should contain at leas one value');
        var sampleFormatsArray = info.sampleFormats;
        assert.strictEqual(sampleFormatsArray.length >= 1, true, 'Sample formats array should contain at leas one value');
        var channelsArray = info.channels;
        assert.strictEqual(channelsArray.length >= 1, true, 'Channels array should contain at leas one value');
        var sampleRatesArray = info.sampleRates;
        assert.strictEqual(sampleRatesArray.length >= 1, true, 'Sample rates array should contain at leas one value');
        done();
    });

    it('should return all values for NULL capture device', function (done) {
        
        var info = cardinfo.get('null',cardinfo.CAPTURE);
        assert(info.deviceType, 'Should have deviceType');
        assert(info.accessTypes, 'Should have accessTypes');
        var accessTypesArray = info.accessTypes;
        assert.strictEqual(accessTypesArray.length >= 1, true, 'Arrays should contain at leas one value');
        assert(info.sampleFormats, 'Should have sampleFormats');
        assert(info.channels, 'Should have channels');
        assert(info.sampleRates, 'Should have sampleRates');
        done();
    });

    it('should populate arrays for NULL capture device', function (done) {
        
        var info = cardinfo.get('null',cardinfo.CAPTURE);
        var accessTypesArray = info.accessTypes;
        assert.strictEqual(accessTypesArray.length >= 1, true, 'Access types array should contain at leas one value');
        var sampleFormatsArray = info.sampleFormats;
        assert.strictEqual(sampleFormatsArray.length >= 1, true, 'Sample formats array should contain at leas one value');
        var channelsArray = info.channels;
        assert.strictEqual(channelsArray.length >= 1, true, 'Channels array should contain at leas one value');
        var sampleRatesArray = info.sampleRates;
        assert.strictEqual(sampleRatesArray.length >= 1, true, 'Sample rates array should contain at leas one value');
        done();
    });
});