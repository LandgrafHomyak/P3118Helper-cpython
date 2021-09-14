from aiogram.types import Message

from ._base import BaseBot

class CIdBot(BaseBot):
    def __init__(self, token, **kwargs):
        super().__init__(token, **kwargs)
        self._dp.register_message_handler(self.__command, commands=["cid"])
        
    async def __command(self, message:Message):
        await message.reply(f"<code>Chat ID: {message.chat.id}</code>", parse_mode="html")