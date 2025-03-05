https://www.rfc-editor.org/rfc/rfc2812.html

# Subject:
# You must be able to authenticate, set a nickname, a username, join a channel,
# send and receive private messages using your reference client.


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
