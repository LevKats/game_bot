from player import Player
"""
from enum import Enum
from random import shuffle
class SearchResult(Enum):
    TRACE = 1
    CHARACTER = 2
    NONE = 3


def find_way(cells, point, used):
    if min(point[0], point[1]) < 0 or max(point[0], point[1]] > len(used):
        return SearchResult.NONE, 'NONE'
    if cells[]
    used[point[0]][point[1]] = True
    directions = ['LEFT', 'RIGHT', 'UP', 'DOWN']
"""


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
