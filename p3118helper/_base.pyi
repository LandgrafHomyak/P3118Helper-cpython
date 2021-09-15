import asyncio
from typing import NoReturn

from aiogram import Bot, Dispatcher

class BaseBot:
    _bot: Bot
    _dp: Dispatcher
    
    def __init__(self, token:str, **kwargs) -> NoReturn: ...
        
    async def run(self) -> NoReturn: ...
    
    def start(self, loop:asyncio.AbstractEventLoop=None) -> asyncio.Task: ...