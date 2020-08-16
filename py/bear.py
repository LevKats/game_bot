from player import Player

from enum import Enum
from math import tanh

from random import shuffle
from random import choices


class Direction(Enum):
    LEFT = 1
    RIGHT = 2
    UP = 3
    DOWN = 4

    @staticmethod
    def to_string(direction):
        if direction == Direction.LEFT:
            return "LEFT"
        elif direction == Direction.RIGHT:
            return "RIGHT"
        elif direction == Direction.UP:
            return "UP"
        else:
            return "DOWN"


class Bear(Player):
    def __init__(self, name, area=2.0):
        self.__name = name
        self.__command = None
        self.__stack = []
        self.__last = None
        self.__area = area

    @property
    def name(self):
        return self.__name

    def suggest(self, message_from_server):
        cells = message_from_server['CELLS']
        mid_point = (len(cells) // 2, len(cells) // 2)
        if cells[mid_point[0]][mid_point[1]]["CHARACTER"] == '1':
            self.__command = {"COMMAND": "BEAR_ATACK", "DIRECTION": "NONE"}
            self.__last = None
            yield "медведь {} атакует".format(self.__name), None
            return
        else:
            possible_directions = []

            last = None
            if len(self.__stack) != 0:
                last = self.__stack[-1]

            if cells[mid_point[0]][mid_point[1]]['LEFT_BORDER'] == '0':
                possible_directions.append((Direction.LEFT, last == Direction.RIGHT))
            if cells[mid_point[0]][mid_point[1]]['RIGHT_BORDER'] == '0':
                possible_directions.append((Direction.RIGHT, last == Direction.LEFT))
            if cells[mid_point[0]][mid_point[1]]['UP_BORDER'] == '0':
                possible_directions.append((Direction.UP, last == Direction.DOWN))
            if cells[mid_point[0]][mid_point[1]]['DOWN_BORDER'] == '0':
                possible_directions.append((Direction.DOWN, last == Direction.UP))

            shuffle(possible_directions)

            def find(predicate):
                def transform(direct, pt):
                    if direct == Direction.LEFT:
                        if cells[pt[0]][pt[1]]['LEFT_BORDER'] == '1':
                            return None
                        return pt[0], pt[1] - 1
                    elif direct == Direction.RIGHT:
                        if cells[pt[0]][pt[1]]['RIGHT_BORDER'] == '1':
                            return None
                        return pt[0], pt[1] + 1
                    elif direct == Direction.UP:
                        if cells[pt[0]][pt[1]]['UP_BORDER'] == '1':
                            return None
                        return pt[0] - 1, pt[1]
                    else:
                        if cells[pt[0]][pt[1]]['DOWN_BORDER'] == '1':
                            return None
                        return pt[0] + 1, pt[1]

                for direction, is_last in possible_directions:
                    point = mid_point

                    while 0 <= min(point) <= max(point) < len(cells):
                        if predicate(point) and point != mid_point:
                            return direction, is_last
                        else:
                            point = transform(direction, point)
                            if point is None:
                                break

            char = find(lambda point: cells[point[0]][point[1]]['CHARACTER'] == '1')

            if char is not None:
                self.__command = {"COMMAND": "GO", "DIRECTION": Direction.to_string(char[0])}
                self.__last = char

                print("{}".format(self.name))
                print('\t', *possible_directions)
                print('\t', 'character found at {}'.format(char[0]))

                yield "медведь {} рычит, увидев человека".format(self.__name), None
                return

            trace = find(lambda point: cells[point[0]][point[1]]['TRACE'] == '1')

            if trace is not None:
                self.__command = {"COMMAND": "GO", "DIRECTION": Direction.to_string(trace[0])}
                self.__last = trace

                print("{}".format(self.name))
                print('\t', *possible_directions)
                print('\t', 'trace found at {}'.format(trace[0]))

                yield "медведь {} видит след человека".format(self.__name), None
                return

            revert_last_prob = 0.0
            other_prob = 1.0 / len(possible_directions)
            if last is not None:
                if len(possible_directions) > 1:
                    revert_last_prob = tanh(len(possible_directions) / self.__area)
                    other_prob = (1.0 - revert_last_prob) / (len(possible_directions) - 1)
                else:
                    revert_last_prob = 1.0
                    other_prob = 0.0

            probs = []

            for direction_, is_last_ in possible_directions:
                if is_last_:
                    probs.append(revert_last_prob)
                else:
                    probs.append(other_prob)

            new_dir = choices(possible_directions, probs)[0]
            self.__command = {"COMMAND": "GO", "DIRECTION": Direction.to_string(new_dir[0])}
            self.__last = new_dir

            print("{}".format(self.name))
            print('\t', *zip(possible_directions, probs))
            print('\t', 'going at {}'.format(new_dir[0]))

            yield from iter(())
            return

    def inform(self, message_from_server):
        if message_from_server['STATUS'] == 'SUCCESS' and self.__last:
            if not self.__last[1]:
                self.__stack.append(self.__last[0])
                self.__last = None
            else:
                self.__stack.pop()
                self.__last = None
        yield "медведь {} сделал ход".format(self.__name), None
        if message_from_server['HOLL'] == '1':
            self.__stack = []
            self.__last = None
            yield "медведь {} переместился".format(self.__name), None

    @property
    def command(self):
        return self.__command


if __name__ == "__main__":
    pass
