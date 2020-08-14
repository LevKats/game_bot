import socket
import json


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


def pretty_print_status(recieved):
    if recieved['STATUS'] == 'SUCCESS':
        print('успех')
    elif recieved['STATUS'] == 'NOTHING':
        print('ничего не произошло')
    if recieved['HOLL'] == '1':
        print('вы переместились')


def pretty_print_state(recieved):
    print("Игрок {}".format(recieved['STATE']['NAME']))
    print("\tУ вас {} пастронов и {} бомб".format(recieved['STATE']['AMMOS'], recieved["STATE"]["BOMBS"]))
    if recieved["STATE"]["ALIVE"] == '1':
        print("\tВы живы и у вас {}".format(
            "есть сокровище" if recieved['STATE']['TREASURE'] == '1' else 'нет сокровища'
        ))
    else:
        print('\tСейчас вы призрак и не можете делать многого')
    print("На вашей клетке:")

    something_exist = False
    if recieved['CELLS'][0][0]['ARMORY'] == '1':
        something_exist = True
        print("\tесть арсенал")
    if recieved['CELLS'][0][0]['EXIT'] == '1':
        something_exist = True
        print("\tесть выход")
    if recieved['CELLS'][0][0]['HOSPITAL'] == '1':
        something_exist = True
        print("\tесть больница")
    if recieved['CELLS'][0][0]['TREASURE'] == '1':
        something_exist = True
        print("\tесть сокровище")
    if recieved['CELLS'][0][0]['WORM_HOLL'] == '1':
        something_exist = True
        print("\tесть портал")
    if not something_exist:
        print('\tничего интересного')

    print('Вокруг вас есть стены:')

    something_exist = False
    if recieved['CELLS'][0][0]['LEFT_BORDER'] == '1':
        something_exist = True
        print("\tслева")
    if recieved['CELLS'][0][0]['RIGHT_BORDER'] == '1':
        something_exist = True
        print("\tсправа")
    if recieved['CELLS'][0][0]['UP_BORDER'] == '1':
        something_exist = True
        print("\tсверху")
    if recieved['CELLS'][0][0]['DOWN_BORDER'] == '1':
        something_exist = True
        print("\tснизу")
    if not something_exist:
        print('\tникаких')


def json_input():
    print('команда')
    words = input().split()
    command, direction = words[0].upper(), words[1].upper() if len(words) > 1 else 'NONE'
    return json.dumps({"COMMAND": command, "DIRECTION": direction})


def main():
    sock = socket.socket()
    sock.connect(('localhost', 8080))

    description = {
        "FIELD": {
            "SIZE": "10",
            "HOLLES": "3"
        },
        "PLAYERS": [
            {
                "NAME": "Lev",
                "TYPE": "HUMAN"
            }
        ]
    }

    sock.send((json.dumps(description) + '\n').encode('utf-8'))
    try:
        for i, message in enumerate(messages(sock)):
            print(message)
            if not i % 2:
                pretty_print_state(json.loads(message))
                sock.send((json_input() + '\n').encode('utf-8'))
            else:
                pretty_print_status(json.loads(message))
            i += 1

    except KeyboardInterrupt:
        sock.close()


if __name__ == "__main__":
    main()
