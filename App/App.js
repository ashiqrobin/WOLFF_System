import React from 'react';
import { StyleSheet, Text, View, TextInput, Button } from 'react-native';

export default class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {text: ""};
    this.onSend = this.onSend.bind(this);
  }

  onSend() {
    fetch('http://143.215.110.209/message', {
  method: 'POST',
  headers: {
           'Accept': 'application/json',
           'Content-Type': 'application/json'
           },
  body: JSON.stringify({message: this.state.text})
})
.then((response) => JSON.stringify(response.json()))
.then((responseData) => { console.log("response: " + responseData); })
.catch((err) => { console.log(err); });
  }

  render() {
    return (
      <View style={styles.container}>
        <TextInput
          style={{height: 40, width: 500, textAlign: "center"}}
          placeholder="Message to send to the node!"
          onChangeText={(text) => this.setState({text})}
        />
      <Button onPress={this.onSend} title="Send"/>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    justifyContent: 'center',
  },
});
