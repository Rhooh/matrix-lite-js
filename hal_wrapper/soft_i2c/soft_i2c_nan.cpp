#include <nan.h>
#include "../matrix.h"
#include "soft_i2c_nan.h"
#include <v8.h>

#include <iostream>
#include "soft_i2c.h"
using namespace std;

i2c_t i2cPort;
// Paramters: int scl-pin (0-15), int sda-pin (0-15)
NAN_METHOD(Init){
    // if argument 1 & 2 are not a number, throw error
    if (!info[0]->IsNumber() && !info[1]->IsNumber()) {Nan::ThrowTypeError("soft_i2c_nan: Arguments 1 & 2 must be a number");return;}

    // get user arguments
    int scl = Nan::To<int>( info[0] ).FromJust();
    int sda = Nan::To<int>( info[1] ).FromJust();
    //cout << "scl = " << scl << endl;
    i2cPort = i2c_init( scl, sda );
    cout << "soft_i2c_nan: " << i2c_toString( i2cPort ) << "soft_i2c: Init done" << endl;
}

NAN_METHOD( Start ){
    cout << "soft_i2c_nan: Calling i2c_start on port( scl:" << i2cPort.scl << " , sda:" <<  i2cPort.sda << " )" << endl;
    i2c_start( i2cPort );
}
NAN_METHOD( Stop ){
    cout << "soft_i2c_nan: Calling i2c_stop on port( scl:" << i2cPort.scl << " , sda:" <<  i2cPort.sda << " )" << endl;
    i2c_stop( i2cPort );
}

// Paramters: uint8_t byte (0-255)
NAN_METHOD( SendByte ){
    // if argument 1 & 2 are not a number, throw error
    if ( !info[0]->IsNumber() ) {Nan::ThrowTypeError("Argument 1 must be a number");return;}

    // get user arguments
    uint8_t myByte = Nan::To<uint32_t>( info[0] ).FromJust();

    cout << "soft_i2c_nan: Calling i2c_send_byte ( byte = " << (int)myByte << " )" << endl;
    int ack = i2c_send_byte( i2cPort , myByte );
    cout << "soft_i2c_nan: Done writing " << (int)myByte << " to I2C-Bus" << endl;
    info.GetReturnValue().Set( ack==I2C_ACK );
}

// Paramters: uint8_t byte (0-255)
NAN_METHOD( SendBit ){
    // if argument 1 & 2 are not a number, throw error
    if ( !info[0]->IsNumber() ) {Nan::ThrowTypeError("Argument 1 must be a number");return;}

    // get user arguments
    uint8_t state = Nan::To<uint32_t>( info[0] ).FromJust();
    cout << "soft_i2c_nan: Calling i2c_send_bit ( bit = " << ( 1==(int)state ) << " )" << endl;
    i2c_send_bit( i2cPort , state );
    cout << "soft_i2c_nan: Done writing " << (int)state << " to I2C-Bus" << endl;
}


// Advanced functions
NAN_METHOD( ScanForDevices ){
    cout << "soft_i2c_nan: Calling scanForDevices" << endl;
    scanForDevices(i2cPort);
    cout << "soft_i2c_nan: Done scanning! " << endl;
}


NAN_METHOD( SendText ){
    // if argument 1 is not a number, throw error
    if ( !info[0]->IsNumber() ) {Nan::ThrowTypeError("soft_i2c_nan: Argument 1 must be a number");return;}
    // if argument 2 is not a string, throw error
    if ( !info[1]->IsString() ) {Nan::ThrowTypeError("soft_i2c_nan: Argument 2 must be a string");return;}

    // get user arguments
    uint8_t addr = Nan::To<uint32_t>( info[0] ).FromJust();
    string text = *Nan::Utf8String(info[1]);

    cout << "soft_i2c_nan: Calling sendText with addr = " << addr << "\n& text = " << text << endl;
    sendText( i2cPort , addr , text );
    cout << "soft_i2c_nan: Done sending! " << endl;
}



/////////////////////////////////
// ** EXPORTED SOFT_I2C_NAN OBJECT ** //
NAN_METHOD( soft_i2c_nan ) {
    v8::Local<v8::Object> obj = Nan::New<v8::Object>();

    // -------------- Set Object Properties --------------------
    // SOFT_I2C_NAN Init method
    Nan::Set( obj , Nan::New( "init" ).ToLocalChecked(),
    Nan::GetFunction( Nan::New<v8::FunctionTemplate>(Init) ).ToLocalChecked());

    // SOFT_I2C_NAN Start
    Nan::Set( obj , Nan::New( "start" ).ToLocalChecked(),
    Nan::GetFunction( Nan::New<v8::FunctionTemplate>(Start) ).ToLocalChecked());
    // SOFT_I2C_NAN Start
    Nan::Set( obj , Nan::New( "stop" ).ToLocalChecked(),
    Nan::GetFunction( Nan::New<v8::FunctionTemplate>(Stop) ).ToLocalChecked());

    // SOFT_I2C_NAN SendByte method
    Nan::Set( obj , Nan::New( "sendByte" ).ToLocalChecked(),
    Nan::GetFunction( Nan::New<v8::FunctionTemplate>(SendByte) ).ToLocalChecked());

    // SOFT_I2C_NAN SendBit method
    Nan::Set( obj , Nan::New( "sendBit" ).ToLocalChecked(),
    Nan::GetFunction( Nan::New<v8::FunctionTemplate>(SendBit) ).ToLocalChecked());

    // SOFT_I2C_NAN scanForDevices method
    Nan::Set( obj , Nan::New( "scanForDevices" ).ToLocalChecked(),
    Nan::GetFunction( Nan::New<v8::FunctionTemplate>(ScanForDevices) ).ToLocalChecked());

    // SOFT_I2C_NAN sendText method
    Nan::Set( obj , Nan::New( "sendText" ).ToLocalChecked(),
    Nan::GetFunction( Nan::New<v8::FunctionTemplate>(SendText) ).ToLocalChecked());


    // Return object
    info.GetReturnValue().Set( obj );
}
