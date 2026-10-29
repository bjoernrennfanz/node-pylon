import React, {useState} from 'react';
import {Button, SafeAreaView, ScrollView, Text} from 'react-native';
import {enumerateDevices, pylonVersion, version} from '../src';

export default function App(): React.JSX.Element {
  const [log, setLog] = useState('No devices listed yet (needs the native module).');
  const onList = () => {
    const devs = enumerateDevices();
    setLog(`node-pylon ${version()} / ${pylonVersion()} devices=${devs.length}`);
  };
  return (
    <SafeAreaView>
      <ScrollView>
        <Text>node-pylon RN desktop sample (Windows/macOS/Linux, real pylon SDK)</Text>
        <Button title="List version + devices" onPress={onList} />
        <Text>{log}</Text>
      </ScrollView>
    </SafeAreaView>
  );
}
