Q3. Socket (6)

Assessment Goals

- Implement server
- Implement client
- Authenticate clients and ensure safe communication

Context

We are interested in developing a chat system in which we have a central server that manages 4 clients. Every client will login with the credential (username) before starts chatting with other online clients. Anytime the client authenticates with their username, they should be able to see all clients online and choose who to chat with, once the client choose the other client to chat, they can send a message which will be received on the other side as the message sender and the actual message. That is to say, that the clients can only communicate if they both connected and authenticated.

Tasks

- Implement the server
- Implement 4 clients
- Ensure that chats are bidirectional (either side can send but also can receive)
- Ensure that only connected and authenticated clients can communicate