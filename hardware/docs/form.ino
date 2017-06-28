client.print(<html>\r\n);
client.print(<body>\r\n);
client.print(<form action='/submit' method='post'>\r\n);
client.print(SSID: <input name='ssid' type='text' value=''><br>\r\n);
client.print(Password: <input name='pw' type='password' value=''><br>\r\n);
client.print(<button type='submit'>Submit</button>\r\n);
client.print(</form>\r\n);
client.print(</body>\r\n);
client.print(</html>\r\n);
