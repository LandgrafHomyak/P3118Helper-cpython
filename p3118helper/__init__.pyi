from typing import NoReturn

from .queue import QueueBot
from ._base import BaseBot
from .random_queue import RandomQueueBot
from .cid import CIdBot

class P3118HelperBot(RandomQueueBot, QueueBot, CIdBot, BaseBot):
    def __init__(self, token:str, *, group_id:int) -> NoReturn: ...
    
    