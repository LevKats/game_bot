from player import Player
from collections import namedtuple


Message = namedtuple('Message', ['text', 'id'])


def pretty_print_status(recieved):
    ans = ""
    if recieved['STATUS'] == 'SUCCESS':
        ans += 'успех' + '\n'
    elif recieved['STATUS'] == 'NOTHING':
        ans += 'ничего не произошло' + '\n'
    elif recieved['STATUS'] == 'ERR':
        ans += 'неверная команда' + '\n'
    else:
        raise RuntimeError("strange status")
    if recieved['HOLL'] == '1':
        ans += 'вы переместились' + '\n'
    return ans


def pretty_print_state(recieved):
    ans = ""
    ans += "Игрок {}".format(recieved['STATE']['NAME']) + '\n'
    ans += "\tУ вас {} пастронов и {} бомб".format(recieved['STATE']['AMMOS'], recieved["STATE"]["BOMBS"]) + '\n'
    if recieved["STATE"]["ALIVE"] == '1':
        ans += "\tВы живы и у вас {}".format(
            "есть сокровище" if recieved['STATE']['TREASURE'] == '1' else 'нет сокровища'
        ) + '\n'
    else:
        ans += '\tСейчас вы призрак и не можете делать многого' + '\n'
    ans += "На вашей клетке:" + '\n'

    something_exist = False
    if recieved['CELLS'][0][0]['ARMORY'] == '1':
        something_exist = True
        ans += "\tесть арсенал" + '\n'
    if recieved['CELLS'][0][0]['EXIT'] == '1':
        something_exist = True
        ans += "\tесть выход" + '\n'
    if recieved['CELLS'][0][0]['HOSPITAL'] == '1':
        something_exist = True
        ans += "\tесть больница" + '\n'
    if recieved['CELLS'][0][0]['TREASURE'] == '1':
        something_exist = True
        ans += "\tесть сокровище" '\n'
    if recieved['CELLS'][0][0]['WORM_HOLL'] == '1':
        something_exist = True
        ans += "\tесть портал" + '\n'
    if not something_exist:
        ans += '\tничего интересного' + '\n'

    ans += 'Вокруг вас есть стены:' + '\n'

    something_exist = False
    if recieved['CELLS'][0][0]['LEFT_BORDER'] == '1':
        something_exist = True
        ans += "\tслева" + '\n'
    if recieved['CELLS'][0][0]['RIGHT_BORDER'] == '1':
        something_exist = True
        ans += "\tсправа" + '\n'
    if recieved['CELLS'][0][0]['UP_BORDER'] == '1':
        something_exist = True
        ans += "\tсверху" + '\n'
    if recieved['CELLS'][0][0]['DOWN_BORDER'] == '1':
        something_exist = True
        ans += "\tснизу" + '\n'
    if not something_exist:
        ans += '\tникаких' + '\n'
    return ans


class Human(Player):
    def __init__(self, name, id_):
        self.__name = name
        self.__command = None
        self.__id = id_

    def name(self):
        return self.__name

    def suggest(self, message_from_server):
        self.__command = None
        yield pretty_print_state(message_from_server), None
        require_dirrection_commands = {"GO", "SHOOT", "EXPLODE"}
        without_dirrection_commands = {"PASS", "TRY_EXIT", "GIVE_UP", "FINNISH_GAME"}
        while True:
            yield "команда", [
                ["GO", "SHOOT", "EXPLODE", "PASS"],
                ["TRY_EXIT", "GIVE_UP", "FINISH_GAME"]
            ]
            command = yield
            if command.id != self.__id:
                continue
            command = command.text.upper()
            if command in {'GIVE_UP', 'FINISH_GAME'}:
                sure = False
                while True:
                    yield "Вы уверены?", ["YES", "NO"]
                    ans = yield
                    if ans.id != self.__id:
                        continue
                    ans = ans.text.upper()
                    if ans in {"YES", "NO"}:
                        sure = ans == "YES"
                        break
                if not sure:
                    continue
            if command in require_dirrection_commands | without_dirrection_commands:
                break
        if command in require_dirrection_commands:
            while True:
                yield "направление", [
                    ["LEFT", "UP"],
                    ["DOWN", "RIGHT"]
                ]
                direction = yield
                if direction.id != self.__id:
                    continue
                direction = direction.text.upper()
                if direction in {"LEFT", "RIGHT", "UP", "DOWN"}:
                    break
        else:
            direction = "NONE"
        self.__command = {"COMMAND": command, "DIRECTION": direction}
        yield 'команда исполняется', None

    def inform(self, message_from_server):
        yield pretty_print_status(message_from_server), None

    def command(self):
        if self.__command is None:
            raise RuntimeError("no command yet")
        else:
            return self.__command
