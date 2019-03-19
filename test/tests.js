var assert = require('assert');
var cardinfo = require('../main.js');

describe('cardinfo.get()', function () {
    it('should use default values', function (done) {
        
        var info = cardinfo.get();
        assert(info.deviceType, 'Should have valid response for devault values');
        assert.strictEqual(info.deviceType, 'HW', 'Should return value');
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

    it('should return correct deviceType for plughw', function (done) {
        
        var info = cardinfo.get('plughw');
        assert(info.deviceType, 'Should have valid response for devault values');
        assert.strictEqual(info.deviceType, 'PLUG', 'Should return value');
        done();
    });

    it('should return all values for valid playback device', function (done) {
        
        var info = cardinfo.get('hw',cardinfo.PLAYBACK);
        assert(info.deviceType, 'Should have deviceType');
        assert(info.accessTypes, 'Should have accessTypes');
        var accessTypesArray = info.accessTypes;
        assert.strictEqual(accessTypesArray.length >= 1, true, 'Arrays should contain at leas one value');
        assert(info.sampleFormats, 'Should have sampleFormats');
        assert(info.channels, 'Should have channels');
        assert(info.sampleRates, 'Should have sampleRates');
        assert(info.cardNum, 'Should have the card number');
        assert(info.devNum, 'Should have the device number');
        assert(info.subDevNum, 'Should have the subdevice number');
        assert(info.id, 'Should have the device id');
        assert(info.name, 'Should have the device name');
        assert(info.subName, 'Should have the subdevice name');
        done();
    });

    it('should populate arrays for valid playback device', function (done) {
        
        var info = cardinfo.get('hw',cardinfo.PLAYBACK);
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

    it('should return all values for valid capture device', function (done) {
        
        var info = cardinfo.get('hw',cardinfo.CAPTURE);
        assert(info.deviceType, 'Should have deviceType');
        assert(info.accessTypes, 'Should have accessTypes');
        var accessTypesArray = info.accessTypes;
        assert.strictEqual(accessTypesArray.length >= 1, true, 'Arrays should contain at leas one value');
        assert(info.sampleFormats, 'Should have sampleFormats');
        assert(info.channels, 'Should have channels');
        assert(info.sampleRates, 'Should have sampleRates');
        assert(info.cardNum, 'Should have the card number');
        assert(info.devNum, 'Should have the device number');
        assert(info.subDevNum, 'Should have the subdevice number');
        assert(info.id, 'Should have the device id');
        assert(info.name, 'Should have the device name');
        assert(info.subName, 'Should have the subdevice name');
        done();
    });

    it('should populate arrays for valid capture device', function (done) {
        
        var info = cardinfo.get('hw',cardinfo.CAPTURE);
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

describe('cardinfo.list()', function(){
    it('should have NULL device in list', function(done){

        var list = cardinfo.list();
        var found = false;
        for( var device in list ){
            if(device.id === 'null'){
                found = true;
                break; 
            }
        }
        assert(found, 'null device should appear in list');
        done();
    });

    it('Should return all values for all device', function(done){
        
        var list = cardinfo.list();
        for(var device in list){
            assert(device.name, 'Listed device should have name set');
            assert(device.desc, 'Listed device should have description set');
            assert(device.io, 'Listed device should have io direction set');
        }
        done();
    });
})