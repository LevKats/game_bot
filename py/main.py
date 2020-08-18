from game import Game
from human import Message

from functools import partial
from os import environ
from sys import argv
import socket
from time import sleep

import telebot
from telebot import types
from telebot import logger


if len(argv) < 3:
    try:
        PORT = int(environ['PORT'])
    except KeyError:
        PORT = int(input('PORT '))
    try:
        TOKEN = environ['TOKEN']
    except KeyError:
        TOKEN = input('TOKEN ')
else:
    PORT = int(argv[1])
    TOKEN = argv[2]

# print(PORT)
# print(TOKEN)

bot = telebot.TeleBot(TOKEN, threaded=True)


def get_game_decorator():
    games = {}  # [chat_id] -> (obj, generator)

    def wrapper(func_to_wrap):
        return partial(func_to_wrap, games)

    return wrapper


DECORATOR_WITH_GAMES = get_game_decorator()


def _start(games, message):
    if message.chat.id not in games:
        sock = socket.socket()
        sock.settimeout(20.0)
        sock.connect(('localhost', PORT))

        game = Game(sock)
        gen = game.on_run()

        games[message.chat.id] = game, gen

        try:
            while True:
                mes = gen.send(None)
                if mes is None:
                    break
                text = mes[0]
                if mes[1] is not None:
                    keyboard = types.ReplyKeyboardMarkup()
                    for row in mes[1]:
                        keyboard.row(*row)
                    keyboard.one_time_keyboard = True
                    bot.send_message(message.chat.id, text, reply_markup=keyboard)
                else:
                    # markup = types.ReplyKeyboardRemove(selective=False)
                    # bot.send_message(message.chat.id, text, markup)
                    bot.send_message(message.chat.id, text)
        except StopIteration:
            del game
            games.pop(message.chat.id)
            # markup = types.ReplyKeyboardRemove(selective=False)
            # bot.send_message(message.chat.id, "в следующий раз используйте /start", markup)
            bot.send_message(message.chat.id, "в следующий раз используйте /start")
        except Exception as e:
            print(e)
            del game
            games.pop(message.chat.id)
            bot.send_message(message.chat.id, ("Вероятно, сессия была просрочена и игра завершена\n"
                                               "В следующий раз используйте /start"))
    else:
        bot.send_message(message.chat.id, "Игра уже началась")


@bot.message_handler(commands=['start'])
@DECORATOR_WITH_GAMES
def start_handler(games, message):
    _start(games, message)


@bot.message_handler(func=lambda message: True, content_types=['text'])
@DECORATOR_WITH_GAMES
def text_handler(games, message):
    if message.text.find('start') != -1:
        _start(games, message)
        return

    game = None
    try:
        game, gen = games[message.chat.id]

        mes = gen.send(Message(text=message.text, id=message.from_user.id))

        while True:
            text = mes[0]
            if mes[1] is not None:
                keyboard = types.ReplyKeyboardMarkup()
                for row in mes[1]:
                    keyboard.row(*row)
                keyboard.one_time_keyboard = True
                bot.send_message(message.chat.id, text, reply_markup=keyboard)
            else:
                # markup = types.ReplyKeyboardRemove(selective=False)
                # bot.send_message(message.chat.id, text, markup)
                bot.send_message(message.chat.id, text)

            mes = gen.send(None)
            if mes is None:
                break

        if not game.is_running():
            games.pop(message.chat.id)

    except KeyError:
        bot.send_message(message.chat.id, "use /start to start")
    except StopIteration:
        del game
        games.pop(message.chat.id)
        # markup = types.ReplyKeyboardRemove(selective=False)
        # bot.send_message(message.chat.id, "в следующий раз используйте /start", markup)
        bot.send_message(message.chat.id, "в следующий раз используйте /start")
    except Exception as e:
        print(e)
        del game
        games.pop(message.chat.id)
        bot.send_message(message.chat.id, ("Вероятно, сессия была просрочена и игра завершена\n"
                                           "В следующий раз используйте /start"))


def main():
    bot.infinity_polling(none_stop=True)


if __name__ == "__main__":
    main()
