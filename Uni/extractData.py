from create import buildDatabase
import socket

dataServer = buildDatabase()
print("start server")
s = socket.socket()
s.bind(('0.0.0.0', 10000))
s.listen(0)

while True:
    client, addr = s.accept()
    while True:
        content = client.recv(32);
        if len(content) == 0:
            break
        else:
            userData = dataServer.get_account_baseOn_ID(content)
            print(userData)
            response = str(userData).encode()
            client.send(response)
    print('close connection')
    client.close()