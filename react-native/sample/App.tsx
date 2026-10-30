import React, {useState} from 'react';
import {Button, SafeAreaView, ScrollView, Text} from 'react-native';
import {
  InstantCamera,
  enumerateDevices,
  pylonVersion,
  version,
} from '../src';

export default function App(): React.JSX.Element {
  const [log, setLog] = useState('No devices listed yet (needs the native module).');
  const [grab, setGrab] = useState('No grab yet.');
  const onList = () => {
    const devs = enumerateDevices();
    const first = devs[0];
    setLog(
      `node-pylon ${version()} / ${pylonVersion()} devices=${devs.length}` +
        (first ? ` first=${first.modelName ?? '?'} s/n ${first.serialNumber ?? '?'}` : ''),
    );
  };
  const onGrab = () => {
    // InstantCamera grab against the first device (camera or CamEmu
    // emulator via PYLON_CAMEMU); shows image metadata + buffer size.
    const cam = new InstantCamera(0);
    try {
      cam.setEnum('AcquisitionMode', 'SingleFrame');
      const res = cam.grabOne(5000);
      setGrab(
        res.succeeded
          ? `grab ok: ${res.width}x${res.height} ${res.pixelType} bytes=${res.buffer.length} ts=${res.timestamp}`
          : 'grab returned no image (timeout)',
      );
    } finally {
      cam.close();
    }
  };
  return (
    <SafeAreaView>
      <ScrollView>
        <Text>node-pylon RN desktop sample (Windows/macOS/Linux, real pylon SDK)</Text>
        <Button title="List version + devices" onPress={onList} />
        <Text>{log}</Text>
        <Button title="Grab one frame (device 0)" onPress={onGrab} />
        <Text>{grab}</Text>
      </ScrollView>
    </SafeAreaView>
  );
}
