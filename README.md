# Small_Global_Chat_v.1.0

Serwer: Server.cpp

Client: Will be integrateded into the server.

Global chat, works on one process (other one is only for waiting for ESC key to terminate application). The technology called here is called asynchronous socket programming. What it basically does, is that it waits for activity on socket within the table ("select()" function), and if something is received, FD_ISSET checks if this is activity:

- on main socket, this means, that it's new Client and he is assign to clients sockets table 

- if activity is not on the main socket - that means, that this is incoming message, in that case, message is forwarded to all other connected clients

For entire time on server console there is displayed real-time log.

This is strictly global chat, to be used for small chats with very limited number of user, and as that, it is very fast and do not need thread pool - this solution will not be efficient, if you want to add many users, or possiblity of private conversations. But it is not intend for that. 

----------------------------------
Incoming

Next step will be to add possibility for saving log to a file, adding database with users and allowing them to login with unique handles and passwords. 

Of course only after cleaning code and combining client and server into one app, with possibility to choose IP of server by user (now it's hardcoded).   
