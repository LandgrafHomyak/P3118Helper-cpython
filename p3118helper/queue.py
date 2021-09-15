import asyncio
import re
from io import StringIO

from aiogram.dispatcher.filters import Filter
from aiogram.types import Message, InlineKeyboardMarkup, InlineKeyboardButton, CallbackQuery

from ._base import BaseBot


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


class Queue:
    __slots__ = ("final", "name", "__data")
    __global_pattern = re.compile(r"""^<\s*b\s*>(?:(final)\s+|)куеуе(?:\s+=\s+"</\s*b\s*><\s*i\s*>(.+)</\s*i\s*><\s*b\s*>"\s+|):</\s*b\s*>(?:\n\n([\s\S]+)|)$""")
    __row_pattern = re.compile(r"""(?:^|(?<=\n))<\s*code\s*>(\d+)\s+</\s*code\s*><\s*a\s+href=['"]?tg://user\?id=(\d+)['"]?\s*>(.+)</\s*a\s*>(?:$|(?=\n))""")

    class Record:
        __slots__ = ("pos", "uid", "name")

        def __init__(self, t):
            self.pos, self.uid, self.name = t
            self.pos = int(self.pos)
            self.uid = int(self.uid)

    def __new__(cls, s):
        g = cls.__global_pattern.search(s)
        if g is None:
            return None

        u = list(map(cls.Record, cls.__row_pattern.findall(g.group(3) or "")))
        fu = []
        for r in u:
            while r.pos > len(fu):
                fu.append(None)
            fu.append(r)

        self = super().__new__(cls)
        self.final = g.group(1) == "final"
        self.name = g.group(2)
        self.__data = fu
        return self

    def __getitem__(self, pos):
        return self.__data[pos]

    def __setitem__(self, pos, value):
        if not isinstance(value, self.Record):
            raise TypeError("Invalid queue data")
        self.__data[pos] = value

    def __delitem__(self, pos):
        self.__data[pos] = None
        while self.__data and self.__data[-1] is None:
            self.__data.pop()

    def __iter__(self):
        return iter(self.__data)

    def __len__(self):
        return len(self.__data)

    def dump(self):
        f = StringIO()
        f.write("<b>")
        if self.final:
            f.write("final ")
        f.write("куеуе")
        if self.name is not None:
            f.write(f" = \"</b><i>{self.name}</i><b>\" ")
        f.write(":</b>\n\n")
        for i, r in enumerate(self):
            if r is None:
                f.write(f"<code>{i}</code>\n")
            else:
                f.write(f"<code>{i} </code><a href='tg://user?id={r.uid}'>{r.name}</a>\n")

        f.seek(0)
        return f.read()

    def extend(self):
        self.__data.append(None)


class AFloodCache:
    __slots__ = ("__cache", "__mutex")

    class BoundAFlood:
        __slots__ = ("__message", "__cache", "__mutex")

        class BoundAFloodMessageWrapper:
            __slots__ = ("__message", "__cache")

            def __init__(self, m, c):
                self.__message = m
                self.__cache = c

            async def reply(self, *args, **kwargs):
                return await self.__message.reply(*args, **kwargs)


            async def edit_text(self, *args, **kwargs):
                r = self.__cache[self.__message.chat.id, self.__message.message_id] = await self.__message.edit_text(*args, **kwargs)
                return r

            @property
            def reply_markup(self):
                return self.__message.reply_markup

            @property
            def html_text(self):
                return self.__message.html_text

        def __init__(self, m, c, x):
            self.__message = m
            self.__cache = c
            self.__mutex = x

        async def __aenter__(self):
            await self.__mutex.wait()
            self.__mutex.clear()
            await asyncio.sleep(0.3)

            return self.BoundAFloodMessageWrapper(self.__cache.get((self.__message .chat.id, self.__message .message_id), self.__message ), self.__cache)

        async def __aexit__(self, exc_type, exc_val, exc_tb):
            self.__mutex.set()
            return False

    def __init__(self):
        self.__cache = dict()
        self.__mutex = None

    def __call__(self, message: Message):
        if self.__mutex is None:
            self.__mutex = asyncio.Event()  # todo bad creation
            self.__mutex.set()

        return self.BoundAFlood(message, self.__cache, self.__mutex)


class QueueBot(BaseBot):
    __slots__ = ("__afloodcache",)

    def __init__(self, token, *, group_id, **kwargs):
        super().__init__(token, group_id=group_id, **kwargs)
        self.__afloodcache = AFloodCache()
        self._dp.register_message_handler(self.__create_queue, GroupFilter(group_id), commands=["queue"])
        self._dp.register_callback_query_handler(self.__push, GroupFilter(group_id), PrefixCheckFilter("qpush"))
        self._dp.register_callback_query_handler(self.__pop, GroupFilter(group_id), PrefixCheckFilter("qpop"))
        self._dp.register_callback_query_handler(self.__up, GroupFilter(group_id), PrefixCheckFilter("qup"))
        self._dp.register_callback_query_handler(self.__down, GroupFilter(group_id), PrefixCheckFilter("qdown"))
        # self._dp.register_callback_query_handler(self.__finalize_cbq, GroupFilter(group_id), PrefixCheckFilter("qfinalize"))
        self._dp.register_message_handler(self.__finalize_cmd, GroupFilter(group_id), commands=["qfinalize"])

    async def __create_queue(self, message: Message):
        qname = message.get_args()
        if not qname or qname.isidentifier():
            m = await message.reply("<b>Сдесь могла быть ваша куеуе</b>", parse_mode="html")
        else:
            await message.reply("<b>Сдесь могла быть ваша куеуе, но вы не умеете в названия переменных</b>", parse_mode="html")

        await m.edit_text(
            f"<b>куеуе = \"</b><i>{qname}</i><b>\" :</b>",
            parse_mode="html",
            reply_markup=InlineKeyboardMarkup(
                row_width=2,
                inline_keyboard=[
                    [
                        InlineKeyboardButton(text="push", callback_data=f"qpush {m.chat.id} {m.message_id}"),
                        InlineKeyboardButton(text="up", callback_data=f"qup {m.chat.id} {m.message_id}")
                    ],
                    [
                        InlineKeyboardButton(text="pop", callback_data=f"qpop {m.chat.id} {m.message_id}"),
                        InlineKeyboardButton(text="down", callback_data=f"qdown {m.chat.id} {m.message_id}")
                    ],
                    # [
                    #     InlineKeyboardButton(text=" ", callback_data=f" ")
                    # ],
                    # [
                    #     InlineKeyboardButton(text="finalize", callback_data=f"qfinalize {m.chat.id} {m.message_id}")
                    # ],
                ]
            )
        )

    async def __push(self, query: CallbackQuery):
        async with self.__afloodcache(query.message) as message:
            if (q := Queue(message.html_text)) is None:
                await query.answer("NullPointerException")
                return

            if q.final:
                await query.answer("AccessToFinalQueueException")
                return

            for r in q:
                if r is not None and r.uid == query.from_user.id:
                    if r.name != query.from_user.full_name:
                        r.name = query.from_user.full_name
                        await message.edit_text(q.dump(), parse_mode="html", reply_markup=message.reply_markup)
                        await query.answer("Synchronized")
                    else:
                        await query.answer("All up to dates!")
                    return

            i = -1
            for i, r in enumerate(q):
                if r is None:
                    break
            else:
                i += 1
                q.extend()
            q[i] = Queue.Record((i, query.from_user.id, query.from_user.full_name))

            await message.edit_text(q.dump(), parse_mode="html", reply_markup=message.reply_markup)
            await query.answer("Pushed")

    async def __pop(self, query: CallbackQuery):
        async with self.__afloodcache(query.message) as message:
            if (q := Queue(message.html_text)) is None:
                await query.answer("NullPointerException")
                return

            if q.final:
                await query.answer("AccessToFinalQueueException")
                return

            for r in q:
                if r is not None and r.uid == query.from_user.id:
                    del q[r.pos]
                    await message.edit_text(q.dump(), parse_mode="html", reply_markup=message.reply_markup)
                    await query.answer("Popped")
                    return
            await query.answer("UserNotFoundException")

    async def __up(self, query: CallbackQuery):
        async with self.__afloodcache(query.message) as message:
            if (q := Queue(message.html_text)) is None:
                await query.answer("NullPointerException")
                return

            if q.final:
                await query.answer("AccessToFinalQueueException")
                return

            for r in q:
                if r is not None and r.uid == query.from_user.id:
                    i = r.pos - 1
                    while i >= 0:
                        if q[i] is None:
                            q[i] = r
                            del q[r.pos]
                            await message.edit_text(q.dump(), parse_mode="html", reply_markup=message.reply_markup)
                            await query.answer("Time goes faster...")
                            return
                        i -= 1
                    else:
                        await query.answer("Can't accelerate")
                        return
            await query.answer("UserNotFoundException")

    async def __down(self, query: CallbackQuery):
        async with self.__afloodcache(query.message) as message:
            if (q := Queue(message.html_text)) is None:
                await query.answer("NullPointerException")
                return

            if q.final:
                await query.answer("AccessToFinalQueueException")
                return

            for r in q:
                if r is not None and r.uid == query.from_user.id:
                    i = r.pos + 1
                    while i < len(q):
                        if q[i] is None:
                            q[i] = r
                            del q[r.pos]
                            break
                        i += 1
                    else:
                        q.extend()
                        q[i] = r
                        del q[r.pos]
                    await message.edit_text(q.dump(), parse_mode="html", reply_markup=message.reply_markup)
                    await query.answer("Time goes slower...")
                    return
            await query.answer("UserNotFoundException")

    async def __finalize_cbq(self, query: CallbackQuery):
        async with self.__afloodcache(query.message) as message:

            if (q := Queue(message.html_text)) is None:
                await query.answer("NullPointerException")
                return

            if q.final:
                await query.answer("You too late, it's already final")
                return

            q.final = True
            await message.edit_text(q.dump(), parse_mode="html", reply_markup=message.reply_markup)
            await query.answer("Queue finalized")

    async def __finalize_cmd(self, message: Message):
        if message.reply_to_message is None:
            await message.reply("NullPointerException")

        async with self.__afloodcache(message.reply_to_message) as qmessage:
            if (q := Queue(qmessage.html_text)) is None:
                await message.reply("NullPointerException")
                return

            if q.final:
                await message.reply("You too late, it's already final")
                return

            q.final = True
            await qmessage.edit_text(q.dump(), parse_mode="html", reply_markup=qmessage.reply_markup)
            await message.reply("Queue finalized")

