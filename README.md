https://www.rfc-editor.org/rfc/rfc2812.html

# Subject:
# You must be able to authenticate, set a nickname, a username, join a channel,
# send and receive private messages using your reference client.

https://modern.ircdocs.horse/#connection-registration
The PASS command is not required for the connection to be registered, but if included it MUST precede the latter of the NICK and USER commands. i.e it mus tbe sent before you can set nick or username.



Join Command

	  Command: JOIN
   Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] )
			   / "0"

   The JOIN command is used by a user to request to start listening to
   the specific channel.  Servers MUST be able to parse arguments in the
   form of a list of target, but SHOULD NOT use lists when sending JOIN
   messages to clients.

   Once a user has joined a channel, <he receives information about
   <all commands his server receives affecting the channel.  This
   includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG / NOTICE>.
   This allows channel members to keep track of the other channel
   members, as well as channel modes.

   If a JOIN is <successful>, the user <receives a JOIN message> as
   confirmation and is <then sent the channel's topic (using RPL_TOPIC) and
   <the list of users who are on the channel (using RPL_NAMREPLY), which
   MUST include the user joining.

   Note that <this message accepts a special argument ("0"), which is
   <a special request to leave all channels the user is currently a member
   of.>  The server will process this message as if the user had sent
   a PART command (See Section 3.2.2) for each channel he is a member
   of.

   Numeric Replies:

		   ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
		   ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
		   ERR_CHANNELISFULL               ERR_BADCHANMASK
		   ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
		   ERR_TOOMANYTARGETS              ERR_UNAVAILRESOURCE
		   RPL_TOPIC

   Examples:

   JOIN #foobar                    ; Command to join channel #foobar.

   JOIN &foo fubar                 ; Command to join channel &foo using
								   key "fubar".






<PRIVMSG>

3.3.1 Private messages

	  Command: PRIVMSG
   Parameters: <msgtarget> <text to be sent>

   PRIVMSG is used to send private messages between users, as well as to
   send messages to channels.  <msgtarget> is usually the nickname of
   the recipient of the message, or a channel name.

   The <msgtarget> parameter may also be a host mask (#<mask>) or server
   mask ($<mask>).  In both cases the server will only send the PRIVMSG
   to those who have a server or host matching the mask.  The mask MUST
   have at least 1 (one) "." in it and no wildcards following the last
   ".".  This requirement exists to prevent people sending messages to
   "#*" or "$*", which would broadcast to all users.  Wildcards are the
   '*' and '?'  characters.  This extension to the PRIVMSG command is
   only available to operators.

   Numeric Replies:

		   ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
		   ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
		   ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
		   ERR_NOSUCHNICK
		   RPL_AWAY


4. Channel Modes

   The various modes available for channels are as follows:

		O - give "channel creator" status;
#        o - give/take channel operator privilege;
		v - give/take the voice privilege;

		a - toggle the anonymous channel flag;
#        i - toggle the invite-only channel flag;
		m - toggle the moderated channel;
		n - toggle the no messages to channel from clients on the
			outside;
		q - toggle the quiet channel flag;
		p - toggle the private channel flag;
		s - toggle the secret channel flag;
		r - toggle the server reop channel flag;
#        t - toggle the topic settable by channel operator only flag;

#        k - set/remove the channel key (password);
#        l - set/remove the user limit to channel;

		b - set/remove ban mask to keep users out;
		e - set/remove an exception mask to override a ban mask;
		I - set/remove an invitation mask to automatically override
			the invite-only flag;



Testing the server

### **1. Review the Project Requirements**

- **Mandatory Features:**
  - Multi-client handling with non-blocking I/O using a single poll (or equivalent) loop
  - Authentication using the PASS command
  - Setting Nicknames and Usernames with NICK and USER commands
  - Channel management: creating/joining channels, message broadcasting
  - Channel operator commands: KICK, INVITE, TOPIC, and MODE (with modes i, t, k, o)
- **Makefile Requirements:**
  - Ensure the Makefile provides targets: `NAME`, `all`, `clean`, `fclean`, and `re`

---

### **2. Compilation and Build Testing**

- **Compile the Project:**
  - Run `make` and verify that the server compiles without errors.
- **Test Makefile Targets:**
  - Run `make clean` and `make fclean` to ensure proper removal of object files and binaries.
  - Run `make re` to confirm that the rebuild process works correctly.

---

### **3. Start the Server**

- **Run the Executable:**
  - Start your server using:
	```
	./ircserv <port> <password>
	```
  - Use a port between 1024 and 65535. For example:
	```
	./ircserv 6667 mypassword
	```
- **Monitor Server Output:**
  - Check that the server prints a “listening” message and is waiting for client connections.

---

### **4. Connect Clients to the Server**

- **Using Telnet or Netcat:**
  - Open multiple terminal windows and connect using:
	```
	telnet 127.0.0.1 <port>
	```
	or
	```
	nc 127.0.0.1 <port>
	```
- **Check the Connection:**
  - You should see the initial welcome message asking for authentication.

---

### **5. Test Authentication**

- **Send the PASS Command:**
  - Type:
	```
	PASS mypassword
	```
  - Verify that with the correct password you get a success message and your client is “registered.”
- **Invalid Password Test:**
  - Connect a new client and send an incorrect PASS command.
  - Verify that the server sends an error message and disconnects the client.

---

### **6. Test Nickname and Username Commands**

- **Set Nickname:**
  - After authentication, type:
	```
	NICK MyNick
	```
  - Verify the server confirms the nickname change.
- **Set Username:**
  - Type:
	```
	USER MyUser
	```
  - Confirm that the username is set with the appropriate success message.
- **Duplicate Checks:**
  - Try setting a nickname or username that is already taken and verify that you receive an error message.

---

### **7. Test Channel Management**

- **Join a Channel:**
  - Type:
	```
	JOIN #mychannel
	```
  - Verify that if the channel exists, you join it; if not, it is created.
- **Message Broadcasting:**
  - From one client in the channel, send a message (e.g., simply type text and press Enter) and confirm that other clients in the channel receive it.

---

### **8. Test Channel Operator Commands**

- **Become an Operator:**
  - Use the OPER command:
	```
	OPER MyNick mypassword
	```
  - Verify that you are granted operator status (server should confirm).
- **Test KICK:**
  - As an operator in a channel, type:
	```
	KICK #mychannel UserX
	```
  - Verify that the specified user is removed from the channel and receives a kick notification.

	// You are not an operator message is in main window
- **Test INVITE:**

  - // you'd see invite message in main irssi window
  - As an operator, type:
	```
	INVITE #mychannel UserY
	```
  - Verify that UserY (if connected) receives an invitation message.

- **Test TOPIC Command:**
-
  - //check topic when you are in the channel
  - **Viewing Topic:**
	- Type:
	  ```
	  TOPIC #mychannel
	  ```
	- If no topic is set, you should get an appropriate message.
  - **Setting Topic:**
	- Type:
	  ```
	  TOPIC #mychannel New channel topic here
	  ```
	- Verify that the topic is updated and a broadcast notification is sent.
  - **Restricted Topic (mode t):**
	- After setting mode `+t` (see next step), try changing the topic from a non-operator and verify that it’s denied.

---

### **9. Test Channel Modes with MODE Command**

- **Enable/Disable Modes:**
  - **Invite-Only Mode (i):**
    - //works perfectly
	- As an operator, type:
	  ```
	  MODE #mychannel +i
	  ```
	- Then, from a non-invited client, try joining the channel and confirm that you get an error.
  - **Topic Restriction Mode (t):**
    - //works perfectly
	- Set the mode:
	  ```
	  MODE #mychannel +t
	  ```
	- Have a non-operator try to change the topic and verify that it fails.
  - **Channel Key Mode (k):**
    - // works perfectly
	- Set a key by typing:
	  ```
	  MODE #mychannel +k secretkey
	  ```
	- Try to join the channel without providing the key and verify that access is denied; then join with:
	  ```
	  JOIN #mychannel secretkey
	  ```
	- Remove the key by:
	  ```
	  MODE #mychannel -k
	  ```
  - **Operator Privilege Mode (o):**
	- Grant operator status to another user using:
	  ```
	  MODE #mychannel +o OtherNick
	  ```
	- Remove operator status with:
	  ```
	  MODE #mychannel -o OtherNick
	  ```

---

### **10. Test Private Messaging**

- **Using PRIVMSG:**
  - From one client, type:
	```
	PRIVMSG OtherNick :Hello in private!
	```
  - Confirm that the target client receives the private message with the proper format.

---

### **11. Test Disconnection and Cleanup**

- **Client Disconnects:**
  - Close a client’s connection (e.g., by closing the terminal) and verify that:
	- The client is removed from the channel.
	- The server prints a disconnect message.
- **Resource Cleanup:**
  - Verify that the poll loop and client maps are updated accordingly.

---

### **12. Test Error Handling and Edge Cases**

- **Invalid Commands:**
  - Send malformed or incomplete commands (e.g., missing parameters) and verify that the server returns appropriate error messages.
- **Concurrent Clients:**
  - Connect multiple clients simultaneously and perform various commands concurrently. Check for message integrity and proper handling.

---

### **13. Monitor Server Logs and Console Outputs**

- **Check for Debug Logs:**
  - Review the server’s console output for logs on connections, disconnections, command processing, and errors.
- **Use Log Files (Optional):**
  - If you add logging to a file, verify that all critical events are logged.

---

### **14. Optional: Automated Testing**

- **Scripted Tests:**
  - Write simple shell scripts (using `nc` or `telnet`) to automate the sending of commands to simulate multiple client interactions.
- **Test Scenarios:**
  - Automate scenarios such as joining channels, sending messages, and using operator commands.








To test your IRC server implementation, you can use one of the following IRC clients. These clients are widely used, easy to set up, and compatible with most IRC servers, including your custom server.


******************************************

**/server add -auto -tls_pass 12345 localhost 9987*
**CONNECT localhost 9987 12345*
**MSG real Hello*
**JOIN #ch1*
**MSG User7 helloy*
**MSG #ch1 Message*
**quit (IRC client leaves)*

**ctrl + X to switch between networks if it glitches**

delete or stop reconns
**/rmreconns to abort timed-out connections**
**/server remove localhost 9985**


// to see private messages
**/query <nick>*
**/window close** //to close a particular chat or channel window

**//close the window and you would see the private message and the //client message in the**
**//main window.**

**//if you dont see the message in the main window, go to the /query <nick> window**

//I first messaged local client with <nick>

Then, local client oli received

NICK oli
Nickname changed to: oli
[PM from mac
USER mac mac localhost :mac]: :booni
PRIVMSG mac mowa
PRIVMSG mac sho sure
// all these shows //

// If you create a join the channel with #ch irssi would be able to
// message everyone in the ch with /MSG #ch1 Message
**Test sednign PRIVMSG to irrssi client**
**Modify channel creations so it must start with #**

---

### **1. `irssi` (Terminal-Based IRC Client)**
`irssi` is a popular, lightweight, and highly customizable terminal-based IRC client. It works well for testing IRC servers.

#### Installation:
- **Linux**:
  ```bash
  sudo apt install irssi
  ```
- **macOS** (using Homebrew):
  ```bash
  brew install irssi
  ```

#### Usage:
1. Start `irssi`:
   ```bash
   irssi
   ```
2. Connect to your server:
   ```
   /connect localhost 9990
   ```
3. Authenticate:
   ```
   /msg NickServ IDENTIFY <password>
   ```
4. Join a channel:
   ```
   /join #ch1
   ```
5. Send messages:
   ```
   /msg #ch1 Hello, world!
   ```

---

### **2. `HexChat` (Graphical IRC Client)**
`HexChat` is a user-friendly graphical IRC client available for Linux, Windows, and macOS.

#### Installation:
- **Linux**:
  ```bash
  sudo apt install hexchat
  ```
- **macOS** (using Homebrew):
  ```bash
  brew install --cask hexchat
  ```
- **Windows**: Download the installer from the [official website](https://hexchat.github.io/).

#### Usage:
1. Open `HexChat`.
2. Add your server:
   - Go to `Network List` > `Add`.
   - Enter a name for your server (e.g., `MyServer`).
   - Add a new server: `localhost/9990`.
3. Connect to your server:
   - Select your server and click `Connect`.
4. Authenticate:
   - If prompted, enter your password.
5. Join a channel:
   - Type `/join #ch1` in the input box.

---

### **3. `weechat` (Terminal-Based IRC Client)**
`weechat` is another lightweight and extensible terminal-based IRC client.

#### Installation:
- **Linux**:
  ```bash
  sudo apt install weechat
  ```
- **macOS** (using Homebrew):
  ```bash
  brew install weechat
  ```

#### Usage:
1. Start `weechat`:
   ```bash
   weechat
   ```
2. Connect to your server:
   ```
   /server add myserver localhost/9990
   /connect myserver
   ```
3. Authenticate:
   ```
   /msg NickServ IDENTIFY <password>
   ```
4. Join a channel:
   ```
   /join #ch1
   ```

---

### **4. `mIRC` (Windows IRC Client)**
`mIRC` is a popular IRC client for Windows. It has a graphical interface and is easy to use.

#### Installation:
- Download the installer from the [official website](https://www.mirc.com/).

#### Usage:
1. Open `mIRC`.
2. Add your server:
   - Go to `Options` > `Connect` > `Servers`.
   - Add a new server: `localhost` on port `9990`.
3. Connect to your server:
   - Select your server and click `Connect`.
4. Authenticate:
   - If prompted, enter your password.
5. Join a channel:
   - Type `/join #ch1` in the input box.

---

### **5. `Kiwi IRC` (Web-Based IRC Client)**
`Kiwi IRC` is a web-based IRC client that you can use directly in your browser.

#### Usage:
1. Open [Kiwi IRC](https://kiwiirc.com/).
2. Enter your server details:
   - Server: `localhost`
   - Port: `9990`
   - Nickname: Choose a nickname.
3. Click `Start`.
4. Authenticate:
   - If prompted, enter your password.
5. Join a channel:
   - Type `/join #ch1` in the input box.

---

### **6. `nc` (Netcat) or `telnet` (Basic Testing)**
For quick testing, you can use `nc` (Netcat) or `telnet` to connect to your server and send raw IRC commands.

#### Usage with `nc`:
1. Connect to your server:
   ```bash
   nc localhost 9990
   ```
2. Authenticate:
   ```
   PASS 12345
   NICK azeez
   USER azeez 0 * :Azeez User
   ```
3. Join a channel:
   ```
   JOIN #ch1
   ```
4. Send messages:
   ```
   PRIVMSG #ch1 Hello, world!
   ```

#### Usage with `telnet`:
1. Connect to your server:
   ```bash
   telnet localhost 9990
   ```
2. Authenticate:
   ```
   PASS 12345
   NICK azeez
   USER azeez 0 * :Azeez User
   ```
3. Join a channel:
   ```
   JOIN #ch1
   ```
4. Send messages:
   ```
   PRIVMSG #ch1 Hello, world!
   ```

---

### **Recommendation**
- For **quick testing**, use `nc` or `telnet`.
- For **interactive testing**, use `irssi` or `weechat` (terminal-based) or `HexChat` (graphical).
- For **web-based testing**, use `Kiwi IRC`.

Let me know if you need further assistance!





Abort Pending Reconnects:
Use the following command to cancel all pending reconnections:

/rmreconns
This will remove the reconnect attempts, so you won't see further reconnect messages.

Disconnect from a Server:
If you’re connected to a server that you don’t need, you can disconnect from it using:

/server disconnect <server_tag>
Replace <server_tag> with the identifier for the server (e.g., localhost6).

