import json

from human import Human
from bear import Bear


def messages(sock):
    res = ""
    while True:
        st = sock.recv(1024)
        res += st.decode('utf-8').replace('\x00', '')
        while res.find('\n') != -1:
            ind = res.find('\n')
            if ind != -1:
                yield res[:ind + 1:]
                res = res[ind + 1::]


class Game:
    def __init__(self, socket):
        self.__socket = socket
        self.__name_to_id = {}
        self.__id_to_name = {}
        self.__players = {}
        self.__field = {}
        self.__bear_count = 1
        self.__running = False

    def __del__(self):
        self.__socket.close()

    def __add_players_and_start(self):
        while True:
            yield "игра скоро начнется", [["добавить в игру", "начать игру"]]
            message = yield
            message = message
            if message.text == "добавить в игру":
                while True:
                    yield "введите имя", None
                    name = yield
                    if name.id != message.id:
                        continue

                    self.__name_to_id[name.text] = name.id
                    self.__id_to_name[name.id] = name.text

                    self.__players[name.text] = Human(self.__name_to_id[name.text], name.id)
                    break
            elif message.text == "начать игру":
                if len(self.__players) == 0:
                    yield "нельзя начать без игроков", None
                    continue
                else:
                    break
            else:
                continue

    def __field_properties(self):
        size = 4
        hollnum = 2
        while True:
            try:
                yield "введите размер поля", None
                size = yield
                size = int(size.text)
                if size < 4:
                    raise RuntimeError("too small")
            except (ValueError, RuntimeError):
                continue
            break
        while True:
            try:
                yield "введите количество дырок", None
                hollnum = yield
                hollnum = int(hollnum.text)
                if hollnum > size or hollnum < 2:
                    raise RuntimeError("holles")
            except (ValueError, RuntimeError):
                continue
            break
        self.__field = {
            "SIZE": "{}".format(size),
            "HOLLES": "{}".format(hollnum)
        }

    def __add_bears(self):
        for i in range(self.__bear_count):
            name = 'bear{}'.format(i)
            self.__players[name] = Bear(name)
        yield "Медведи добавлены", None

    def is_running(self):
        return self.__running

    def on_run(self):
        self.__running = True
        yield from self.__add_players_and_start()
        yield from self.__field_properties()
        yield from self.__add_bears()

        description = {
            "FIELD": self.__field,
            "PLAYERS": [
                {
                    "NAME": name,
                    "TYPE": "HUMAN" if isinstance(self.__players[name], Human) else "BEAR"
                }
                for name in self.__players
            ]
        }

        self.__socket.send((json.dumps(description) + '\n').encode('utf-8'))

        yield "Начинаем", None

        server_message_generator = messages(self.__socket)
        while True:
            server_message = next(server_message_generator)
            obj = json.loads(server_message)
            if "HAS_WINNER" in obj:
                break
            else:
                name = obj['STATE']['NAME']
                player = self.__players[name]
                yield from player.suggest(obj)
                self.__socket.send((json.dumps(player.command()) + '\n').encode('utf-8'))
                server_message = next(server_message_generator)
                obj = json.loads(server_message)
                yield from player.inform(obj)

        self.__running = False
        if obj["HAS_WINNER"] == '1':
            yield "Игра окончена, победил {}".format(obj['NAME']), None
        else:
            yield "Игра окончена", None


if __name__ == "__main__":
    pass
