import asyncio

from ._base import BaseBot
from .queue import QueueBot
from .random_queue import RandomQueueBot
from .cid import CIdBot


class P3118HelperBot(RandomQueueBot, QueueBot, CIdBot, BaseBot):
    def __init__(self, token, **kwargs):
        super().__init__(token, **kwargs)
