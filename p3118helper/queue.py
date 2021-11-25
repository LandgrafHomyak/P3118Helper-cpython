import asyncio
import io
import re
import warnings
from asyncio import Event
from io import StringIO
from random import choice

from aiogram.dispatcher.filters import Filter
from aiogram.types import Message, InlineKeyboardMarkup, InlineKeyboardButton, CallbackQuery
from aiogram.utils.exceptions import TelegramAPIError, MessageNotModified, RetryAfter

from ._base import BaseBot
from ._queue import QueueStruct


class GroupFilter(Filter):
    __slots__ = ("__group_id",)

    def __init__(self, group_id):
        self.__group_id = group_id

    async def check(self, event):
        if isinstance(event, CallbackQuery):
            return event.message.chat.id == self.__group_id
        elif isinstance(event, Message):
            return event.chat.id == self.__group_id


class PrefixCheckFilter(Filter):
    __slots__ = ("__prefix",)

    def __init__(self, prefix):
        self.__prefix = prefix

    async def check(self, cbq: CallbackQuery):
        return cbq.data.startswith(self.__prefix)


class QueueMessage(QueueStruct):
    # __slots__ = "__tg_message", "__mutex", "__key", "__processed_users"
    # todo i want to use slots !!1!

    KEYBOARD = InlineKeyboardMarkup(
        row_width=2,
        inline_keyboard=[
            [
                InlineKeyboardButton(text="push", callback_data=f"qpush"),
                InlineKeyboardButton(text="pop", callback_data=f"qpop"),
            ],
            [
                InlineKeyboardButton(text="up", callback_data=f"qup"),
                InlineKeyboardButton(text="down", callback_data=f"qdown")
            ],
            [
                InlineKeyboardButton(text="prev", callback_data=f"qprev"),
                InlineKeyboardButton(text="next", callback_data=f"qnext")
            ],
            # [
            #     InlineKeyboardButton(text=choice(("\U0001f31d", "\U0001f31a")), callback_data=" ")
            # ],
            # [
            #     InlineKeyboardButton(text="нажать после сдачи", callback_data=f"qpass")
            # ],
        ]
    )

    __global_pattern = re.compile(r"^<\s*b\s*>(final|open)</\s*b\s*> куеуе <\s*b\s*><\s*i\s*><\s*u\s*>(\w+)</\s*u\s*></\s*i\s*></\s*b\s*> {([\s\S]+)}$")
    __header_pattern = re.compile(r"^<\s*u\s*>(\w+)</\s*u\s*>:$")
    __row_pattern = re.compile(r"^<\s*code\s*> {2}(\d+) ?</\s*code\s*>(?:<\s*a\s+href=['\"]?tg://user\?id=(\d+)['\"]?\s*>(.+)</\s*a\s*>|)$")

    from ._queue import message_key

    class ParseError(ValueError):
        pass

    def __new__(cls, name, final, queues, message, mutex):
        self = super().__new__(cls, name, final, queues)
        self.__tg_message = message
        self.__mutex = mutex
        self.__key = cls.message_key.from_message(message)
        self.__processed_users = set()
        return self

    @property
    def key(self):
        return self.__key

    @classmethod
    def parse(cls, message, mutex):
        super_match = cls.__global_pattern.search(message.html_text)
        if super_match is None:
            raise cls.ParseError("message is not a queue")

        inheritance, name, data = super_match.group(1), super_match.group(2), super_match.group(3)
        last_queue = first_queue = []
        queues = dict()
        for row in filter(lambda s: not s.isspace() and s, data.split("\n")):
            if (match := cls.__header_pattern.search(row)) is not None:
                if match.group(1) in queues:
                    raise cls.ParseError("duplicating queue name")
                queues[match.group(1)] = (last_queue := [])
            elif (match := cls.__row_pattern.search(row)) is not None:
                if not match.group(2):
                    last_queue.append(None)
                else:
                    last_queue.append(cls.user(int(match.group(2)), match.group(3)))
            else:
                raise cls.ParseError("invalid row")

        if first_queue and queues:
            raise cls.ParseError("Queue has unbound rows")
        if first_queue or not queues:
            queues = first_queue

        return cls(name, inheritance == "final", queues, message, mutex)

    def dump(self):
        s = io.StringIO()
        s.write(f"<b>{'final' if self.final else 'open'}</b>")
        s.write(f" куеуе ")
        s.write(f"<u><b><i>{self.name}</i></b></u>")
        s.write(" {\n")
        for q in self:
            if q.name is not None:
                s.write(f"<u>{q.name}</u>:\n")
            for i, qs in enumerate(q):
                if qs:
                    s.write(f"<code>  {i} </code><a href='tg://user?id={qs.uid}'>{qs.display_name}</a>\n")
                else:
                    s.write(f"<code>  {i}</code>\n")
        s.write("}")
        s.seek(0)
        return s.read()

    async def __update(self):
        self.__processed_users.clear()
        try:
            new_message = await self.__tg_message.edit_text(self.dump(), parse_mode="html", reply_markup=(None if self.final else self.KEYBOARD))
            if isinstance(new_message, Message):
                self.__tg_message = new_message
        except MessageNotModified:
            pass
        except RetryAfter:
            await self.update()

    def user_synced(self, uid):
        return uid in self.__processed_users

    def sync_user(self, uid):
        r = uid in self.__processed_users
        self.__processed_users.add(uid)
        return r

    async def update(self):
        with warnings.catch_warnings():
            await self.__mutex.wait(self.key, self.__update())


class QueueSynchronizer:
    DELAY = 0.5
    __slots__ = "__common_mutex", "__message_pool"

    class CommonMutex:
        __slots__ = "__queue", "__task"

        def __new__(cls):
            self = super().__new__(cls)
            self.__queue = dict()
            self.__task = None
            return self

        async def __tgenerator(self):
            while self.__queue:
                await asyncio.sleep(QueueSynchronizer.DELAY)
                for key in self.__queue.keys():
                    mutex, callback = self.__queue.pop(key)
                    mutex.set()
                    asyncio.get_running_loop().create_task(callback)
                    break  # Suppressing StopIteration without try-catch

        async def wait(self, key, callback):
            if key not in self.__queue:
                self.__queue[key] = (Event(), callback)

            if self.__task is None or self.__task.done():
                self.__task = asyncio.get_running_loop().create_task(self.__tgenerator())

            await self.__queue[key][0].wait()

    def __new__(cls):
        self = super().__new__(cls)
        self.__message_pool = dict()
        self.__common_mutex = cls.CommonMutex()
        return self

    def __call__(self, orig_message: Message):
        key = QueueMessage.message_key.from_message(orig_message)
        if key not in self.__message_pool:
            self.__message_pool[key] = QueueMessage.parse(orig_message, self.__common_mutex)
        return self.__message_pool[key]


class QueueBot(BaseBot):
    __slots__ = ("__synchronizer", "__admins_list")

    def __init__(self, token, *, group_id, admins, **kwargs):
        super().__init__(token, group_id=group_id, admins=admins, **kwargs)
        self.__admins_list = admins
        self.__synchroonizer = QueueSynchronizer()
        self._dp.register_message_handler(self.__create_queue, GroupFilter(group_id), commands=["queue"])
        self._dp.register_callback_query_handler(self.__push, GroupFilter(group_id), PrefixCheckFilter("qpush"))
        self._dp.register_callback_query_handler(self.__pop, GroupFilter(group_id), PrefixCheckFilter("qpop"))
        self._dp.register_callback_query_handler(self.__up, GroupFilter(group_id), PrefixCheckFilter("qup"))
        self._dp.register_callback_query_handler(self.__down, GroupFilter(group_id), PrefixCheckFilter("qdown"))
        self._dp.register_callback_query_handler(self.__prev, GroupFilter(group_id), PrefixCheckFilter("qprev"))
        self._dp.register_callback_query_handler(self.__next, GroupFilter(group_id), PrefixCheckFilter("qnext"))
        # self._dp.register_callback_query_handler(self.__finalize_cbq, GroupFilter(group_id), PrefixCheckFilter("qfinalize"))
        # self._dp.register_callback_query_handler(self.__pass, GroupFilter(group_id), PrefixCheckFilter("qpass"))
        self._dp.register_message_handler(self.__finalize_cmd, GroupFilter(group_id), commands=["qfinalize"])
        self._dp.register_message_handler(self.__open_cmd, GroupFilter(group_id), commands=["qopen"])

    async def __create_queue(self, message: Message):
        qnames = message.get_args().split()
        if not qnames:
            return await message.reply("<b>Сдесь могла быть ваша куеуе, но вы не указали ее название</b>", parse_mode="html")
        if not all(map(str.isidentifier, qnames)):
            await message.reply("<b>Сдесь могла быть ваша куеуе, но вы не умеете в названия переменных</b>", parse_mode="html")
            return

        await message.reply(
            QueueMessage.dump(QueueStruct(qnames[0], False, [] if len(qnames) == 1 else {name: [] for name in qnames[1:]})),
            parse_mode="html",
            reply_markup=QueueMessage.KEYBOARD
        )

    async def __answer_final(self, cbq):
        return await cbq.answer("AccessToFinalQueueException")

    async def __answer_noqueue(self, cbq):
        return await cbq.answer("NullPointerException")

    async def __answer_nouser(self, cbq):
        return await cbq.answer("UserNotFoundException")

    async def __push(self, query: CallbackQuery):
        try:
            queue = self.__synchroonizer(query.message)
        except QueueMessage.ParseError:
            return await self.__answer_noqueue(query)

        slot = queue.find_user(query.from_user.id)
        if slot is None:
            if queue.final:
                return await self.__answer_final(query)

            if queue.sync_user(query.from_user.id):
                return await query.answer("")

            queue.push(QueueStruct.user(query.from_user.id, query.from_user.full_name))
            await query.answer("Pushed")
            await queue.update()
        else:
            if slot.display_name == query.from_user.full_name:
                return await query.answer("All up-to-date!")

            slot.display_name = query.from_user.full_name
            await query.answer("Synchronized")
            await queue.update()

    async def __pop(self, query: CallbackQuery):
        try:
            queue = self.__synchroonizer(query.message)
        except QueueMessage.ParseError:
            return await self.__answer_noqueue(query)

        slot = queue.find_user(query.from_user.id)
        if slot is None:
            return await self.__answer_nouser(query)
        else:
            if queue.final:
                return await self.__answer_final(query)

            if queue.sync_user(query.from_user.id):
                return await query.answer("")

            slot.clear()
            await query.answer("Popped")
            await queue.update()

    async def __up(self, query: CallbackQuery):
        try:
            queue = self.__synchroonizer(query.message)
        except QueueMessage.ParseError:
            return await self.__answer_noqueue(query)

        slot = queue.find_user(query.from_user.id)
        if slot is None:
            return await self.__answer_nouser(query)
        else:
            if queue.final:
                return await self.__answer_final(query)

            if queue.user_synced(query.from_user.id):
                return await query.answer("")

            new_slot = slot.up()
            if slot is new_slot:
                await query.answer("Can't accelerate")
            else:
                queue.sync_user(query.from_user.id)
                await query.answer("Time goes faster...")
                await queue.update()

    async def __prev(self, query: CallbackQuery):
        try:
            queue = self.__synchroonizer(query.message)
        except QueueMessage.ParseError:
            return await self.__answer_noqueue(query)
        slot = queue.find_user(query.from_user.id)
        if slot is None:
            return await self.__answer_nouser(query)
        else:
            if queue.final:
                return await self.__answer_final(query)

            if queue.user_synced(query.from_user.id):
                return await query.answer("")

            i = slot.queue.index
            if i == 0:
                return await query.answer("You already at start")
            data = QueueStruct.user(slot.uid, slot.display_name)
            slot.clear()
            queue.push(data, i - 1)

            await query.answer("Queue changed")
            await queue.update()

    async def __next(self, query: CallbackQuery):
        try:
            queue = self.__synchroonizer(query.message)
        except QueueMessage.ParseError:
            return await self.__answer_noqueue(query)

        slot = queue.find_user(query.from_user.id)
        if slot is None:
            return await self.__answer_nouser(query)
        else:
            if queue.final:
                return await self.__answer_final(query)

            if queue.user_synced(query.from_user.id):
                return await query.answer("")

            i = slot.queue.index
            if i == len(queue) - 1:
                return await query.answer("You already at end")
            data = QueueStruct.user(slot.uid, slot.display_name)
            slot.clear()
            queue.push(data, i + 1)

            await query.answer("Queue changed")
            await queue.update()

    async def __down(self, query: CallbackQuery):
        try:
            queue = self.__synchroonizer(query.message)
        except QueueMessage.ParseError:
            return await self.__answer_noqueue(query)

        slot = queue.find_user(query.from_user.id)
        if slot is None:
            return await self.__answer_nouser(query)
        else:
            if queue.final:
                return await self.__answer_final(query)

            if queue.sync_user(query.from_user.id):
                return await query.answer("")

            slot.down()

            await query.answer("Time goes slower...")
            await queue.update()

    # async def __pass(self, query: CallbackQuery):
    #     try:
    #         queue = self.__synchroonizer(query.message)
    #     except QueueMessage.ParseError:
    #         return await self.__answer_noqueue(query)
    #
    #     slot = queue.find_user(query.from_user.id)
    #     if slot is None:
    #         return await self.__answer_nouser(query)
    #     else:
    #         if queue.final:
    #             return await self.__answer_final(query)
    #
    #         if queue.user_synced(query.from_user.id):
    #             return await query.answer("")
    #
    #         slot.pop()
    #
    #         await query.answer("Поздравляем с успешным отсислением из очереди")
    #         await queue.update()

    # async def __finalize_cbq(self, query: CallbackQuery):
    #     try:
    #         queue = self.__synchroonizer(query.message)
    #     except QueueMessage.ParseError:
    #         return await self.__answer_noqueue(query)
    #
    #     if queue.final:
    #         return await query.answer("You are too late, it's already final")
    #
    #     queue.finalize()
    #     await query.answer("Queue finalized")
    #     await queue.update()
    #     await query.message.answer("Queue finalized")

    async def __finalize_cmd(self, message: Message):
        if message.reply_to_message is None or message.reply_to_message.from_user.id != (await self._bot.me).id:
            return await message.reply("NullPointerException")

        if message.from_user.id not in self.__admins_list:
            return await message.reply("AccessDeniedException")

        try:
            queue = self.__synchroonizer(message.reply_to_message)
        except QueueMessage.ParseError:
            return await message.reply("NullPointerException")

        if queue.final:
            return await message.reply("You are too late, it's already final")

        queue.finalize()
        await message.reply("Queue finalized")
        await queue.update()

    async def __open_cmd(self, message: Message):
        if message.reply_to_message is None or message.reply_to_message.from_user.id != (await self._bot.me).id:
            return await message.reply("NullPointerException")

        if message.from_user.id not in self.__admins_list:
            return await message.reply("AccessDeniedException")

        try:
            queue = self.__synchroonizer(message.reply_to_message)
        except QueueMessage.ParseError:
            return await message.reply("NullPointerException")

        if not queue.final:
            return await message.reply("You are so early, it's mutable yet")

        queue.open()
        await message.reply("Queue opened again")
        await queue.update()
