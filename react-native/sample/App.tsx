import React, {useState} from 'react';
import {Button, SafeAreaView, ScrollView, Text} from 'react-native';
import {enumerateDevices, grabMock, isMock, pylonVersion, version} from '../src';

export default function App(): React.JSX.Element {
  const [log, setLog] = useState('Noch kein Grab. (Mock, keine Kamera noetig)');
  const onList = () => {
    const devs = enumerateDevices();
    setLog(`node-pylon ${version()} / ${pylonVersion()} mock=${isMock()} Geraete=${devs.length}`);
  };
  const onGrab = () => {
    const b64 = grabMock(64, 48);
    setLog(`GrabMock 64x48 Mono8, base64-Laenge=${b64.length} (Mock, deterministisch)`);
  };
  return (
    <SafeAreaView>
      <ScrollView>
        <Text>node-pylon RN-Desktop-Sample (Windows/macOS/Linux, ohne Kamera)</Text>
        <Button title="Version + Geraete auflisten" onPress={onList} />
        <Button title="GrabMock (64x48)" onPress={onGrab} />
        <Text>{log}</Text>
      </ScrollView>
    </SafeAreaView>
  );
}
