hal = require('./build/Release/addon');
hal.info = hal.info();

var led = require('./lib/led.js');
var gpio = require('./lib/gpio.js');
var alsa = require('./lib/alsa.js');

// - final module exports
function exported(){
  var matrixLite = {
    'led':      led,
    'gpio':     gpio,
    'alsa':     alsa,
    'imu':      hal.imu(),
    'humidity': hal.humidity(),
    'pressure': hal.pressure(),
    'uv':       hal.uv(),
    'soft_i2c_nan': hal.soft_i2c_nan(),
    'info':     hal.info
  }

  return matrixLite;
}

module.exports = exported();
