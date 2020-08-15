import socket
from game import Game
from human import Message


def main3():
    sock = socket.socket()
    sock.connect(('localhost', 8080))

    g = Game(sock)
    gen = g.on_run()

    while True:
        a = gen.send(None)
        if a is None:
            print(gen.send(Message(text=input(), id=1)))
        else:
            print(a)


if __name__ == "__main__":
    main3()
