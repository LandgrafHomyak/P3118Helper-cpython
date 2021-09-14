import asyncio
from typing import NoReturn

from ._base import BaseBot
from .random_queue import RandomQueueBot
from .cid import CIdBot

class P3118HelperBot(RandomQueueBot, CIdBot, BaseBot):
    def __init__(self, token:str, *, group_id:int) -> NoReturn: ...
    
    