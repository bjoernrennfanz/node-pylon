import React, {useState} from 'react';
import {Button, SafeAreaView, ScrollView, Text} from 'react-native';
import {enumerateDevices, grabMock, isMock, pylonVersion, version} from '../src';

export default function App(): React.JSX.Element {
  const [log, setLog] = useState('No grab yet. (Mock, no camera needed)');
  const onList = () => {
    const devs = enumerateDevices();
    setLog(`node-pylon ${version()} / ${pylonVersion()} mock=${isMock()} devices=${devs.length}`);
  };
  const onGrab = () => {
    const b64 = grabMock(64, 48);
    setLog(`GrabMock 64x48 Mono8, base64 length=${b64.length} (mock, deterministic)`);
  };
  return (
    <SafeAreaView>
      <ScrollView>
        <Text>node-pylon RN desktop sample (Windows/macOS/Linux, no camera)</Text>
        <Button title="List version + devices" onPress={onList} />
        <Button title="GrabMock (64x48)" onPress={onGrab} />
        <Text>{log}</Text>
      </ScrollView>
    </SafeAreaView>
  );
}
