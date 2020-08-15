from player import Player


class Bear(Player):
    def __init__(self, name):
        self.__name = name
        self.__command = None

    def name(self):
        return self.__name

    def suggest(self, message_from_server):
        self.__command = {"COMMAND": "PASS", "DIRECTION": "NONE"}
        yield from iter(())

    def inform(self, message_from_server):
        yield "медведь {} сделал ход".format(self.__name), None

    def command(self):
        return self.__command


if __name__ == "__main__":
    pass
