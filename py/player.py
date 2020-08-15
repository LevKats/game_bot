from abc import ABC, abstractmethod


class Player(ABC):
    @abstractmethod
    def suggest(self, message_from_server):
        # message_from_server -- json
        # generator
        pass

    @abstractmethod
    def inform(self, message_from_server):
        # message_from_server -- json
        # also generator
        pass

    @property
    @abstractmethod
    def name(self):
        pass

    @property
    @abstractmethod
    def command(self):
        pass


if __name__ == "__main__":
    pass
