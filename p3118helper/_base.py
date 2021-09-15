import asyncio

from aiogram import Bot, Dispatcher

class BaseBot:
    def __init__(self, token, **kwargs):
        self._bot = Bot(token=token)
        self._dp = Dispatcher(self._bot)
        
    def start(self, loop=None):
        if loop is None:
            loop = asyncio.get_event_loop()
            
        return loop.create_task(self.run())
    
    async def run(self):
        await self._dp.start_polling()
    