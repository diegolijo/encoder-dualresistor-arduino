#include <Arduino.h>
#include <Encoder.h>
#include <Control_Surface.h>
#include "resistor_encoder.h"

USBMIDI_Interface midiusb;

int midiVolValue = 0;

CCRotaryEncoder enc = {
    {2, 3},                         // pins
    MIDIAddress(MCU::CHANNEL_LEFT), // MIDI address (CC number + optional channel)
    1,
    4 // optional multiplier if the control isn't fast enough
};

CCButton btn = {4, MIDIAddress(MCU::CHANNEL_RIGHT)};

EncoderResistivo encRes;
MIDIAddress controller = {MIDI_CC::Channel_Volume_LSB, CHANNEL_1};

struct MyMIDI_Callbacks : MIDI_Callbacks
{
  // Callback for channel messages (notes, control change, pitch bend, etc.).
  void onChannelMessage(Parsing_MIDI_Interface &midi) override
  {
    ChannelMessage cm = midi.getChannelMessage();
    // Print the message
    Serial << F("Channel message: ") << hex << cm.header << ' ' << cm.data1
           << ' ' << cm.data2 << dec << F(" on cable ") << cm.CN << endl;

    if (cm.data2 == MIDI_CC::Channel_Volume_LSB)
    {
      // midiVolValue = cm.CN;
    }
  }
  // Callback for system exclusive messages
  void onSysExMessage(Parsing_MIDI_Interface &midi) override
  {
    SysExMessage se = midi.getSysExMessage();
    // Print the message
    Serial << F("System Exclusive message: ") << hex;
    for (size_t i = 0; i < se.length; ++i)
      Serial << se.data[i] << ' ';
    Serial << dec << F("on cable ") << se.CN << endl;
  }
  // Callback for real-time messages
  void onRealTimeMessage(Parsing_MIDI_Interface &midi) override
  {
    RealTimeMessage rt = midi.getRealTimeMessage();
    // Print the message
    Serial << F("Real-time message: ") << hex << rt.message << dec
           << F(" on cable ") << rt.CN << endl;
  }
} callbacks;

void setup()
{
  Serial.begin(115200);
  midiusb.begin();
  //  midiusb.setCallbacks(callbacks);
  RelativeCCSender::setMode(relativeCCmode::TWOS_COMPLEMENT); // cubase -> TWOS_COMPLEMENT
  Control_Surface.begin();
}

// main processing loop
void loop()
{
  midiusb.update();
  Control_Surface.loop();
  controller = {MIDI_CC::Channel_Volume_LSB, CHANNEL_1};
  encRes.updateEncoder(&midiusb, controller);
}