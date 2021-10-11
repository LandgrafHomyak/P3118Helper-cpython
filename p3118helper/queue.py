import asyncio
import re
from io import StringIO
from random import choice

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


class QueueMessage:
    __slots__ = "__final", "__name", "__tg_message", "__queues", "__mutex", "__key", "__first_slot"
    __global_pattern = re.compile(r"^(final|open) куеуе <\s*u\s*><\s*b\s*><\s*i\s*>(\w+)</\s*i\s*></\s*b\s*></\s*u\s*> {([\s\S]+)}$")
    __header_pattern = re.compile(r"^<\s*u\s*>(\w)</\s*u\s*>:$")
    __row_pattern = re.compile(r"^<\s*code\s*> {2}(\d+) ?</\s*code\s*>(?:<\s*a\s+href=['\"]?tg://user\?id=(\d+)['\"]?\s*>(.+)</\s*a\s*>|)$")

    class ParseError(ValueError):
        pass

    class MessageKey:
        __slots__ = "__chat", "__message"

        @property
        def chat(self):
            return self.__chat

        @property
        def message(self):
            return self.__message

        def __new__(cls, chat, message):
            self = super().__new__(cls)
            self.__chat = chat
            self.__message = message
            return self

        def __eq__(self, other):
            if not isinstance(other, type(self)):
                raise TypeError
            return self.__chat == other.__chat and self.__message == other.__message

        def __hash__(self):
            return hash((self.__chat, self.__message))

    class QueueSlot:
        __slots__ = "__uid", "__display_name", "__prev", "__next", "__master"

        @property
        def uid(self):
            return self.__uid

        @property
        def display_name(self):
            return self.__display_name

        @property
        def _next(self):
            return self.__next

        @_next.setter
        def _next(self, v):
            if v is None:
                if self.__next is not None:
                    self.__next._prev = None
                self.__next = None
                return

            if not isinstance(v, type(self)):
                raise TypeError

            if self.__next is not None:
                self.__next._prev = None
            self.__next = v
            if v.__prev is not self:
                v._prev = self

        @property
        def _prev(self):
            return self.__prev

        @_prev.setter
        def _prev(self, v):
            if v is None:
                if self.__prev is not None:
                    self.__prev.__next = None
                self.__prev = None
                return

            if not isinstance(v, type(self)):
                raise TypeError

            if self.__prev is not None:
                self.__prev._next = None
            self.__prev = v
            if v.__next is not self:
                v._next = self

        def __new__(cls, uid, display_name, master):
            self = super().__new__(cls)
            self.__uid = uid
            self.__display_name = display_name
            self.__master = master
            self.__next = None
            self.__prev = None
            return self

        @classmethod
        def empty(cls, master):
            return cls(None, None, master)

    class QueueData:
        __slots__ = "__first_slot", "__last_slot", "__name"

        @property
        def name(self):
            return self.__name

        def __new__(cls, name):
            self = super().__new__(cls)
            self.__name = name
            self.__first_slot = None
            self.__last_slot = None
            return self

        def _append(self, slot):
            if not isinstance(slot, QueueMessage.QueueSlot):
                raise TypeError

            if self.__first_slot is None:
                slot._prev = None
                slot._next = None
                self.__first_slot = self.__last_slot = slot
            else:
                slot._prev = self.__last_slot
                self.__last_slot = slot
                slot._next = None

        def __len__(self):
            p = self.__first_slot
            c = 1
            while p is not self.__last_slot and p._next is not None:
                c += 1
                p = p._next
            return c

        def __getitem__(self, i):
            if i >= 0:
                p = self.__first_slot
                while i > 0 and p is not self.__last_slot and p._next is not None:
                    i -= 1
                    p = p._next
            else:
                p = self.__last_slot
                i += 1
                while i < 0 and p is not self.__first_slot and p._prev is not None:
                    i += 1
                    p = p._prev
            if i != 0:
                raise IndexError("Queue index out of range")
            return p

        def __bool__(self):
            return self.__first_slot is not None

    @property
    def final(self):
        return self.__final

    # @final.setter
    # def final(self, v):
    #     if bool(v) and not self.__final:
    #         raise ValueError("Unexpected value for field final")
    #     self.__final = bool(v)

    @property
    def name(self):
        return self.__name

    @property
    def queue_count(self):
        return len(self.__queues)

    @property
    def queue_names(self):
        return tuple(map(self.QueueData.name.__get__, self.__queues))

    def __new__(cls, orig_message: Message):
        super_match = cls.__global_pattern.search(orig_message.html_text)
        if super_match is None:
            raise cls.ParseError("Message is not a queue")

        inheritance, name, data = super_match.group(1), super_match.group(2), super_match.group(3)
        last_queue = single_queue = cls.QueueData(None)
        queues = []
        for row in filter(lambda s: not s.isspace(), data.split("\n")):
            if (match := cls.__header_pattern.search(row)) is not None:
                queues.append(last_queue := cls.QueueData(match.group(1)))
            elif (match := cls.__row_pattern.search(row)) is not None:
                if not match.group(2):
                    last_queue._append(cls.QueueSlot.empty(last_queue))
                else:
                    last_queue._append(cls.QueueSlot(int(match.group(2)), match.group(3), last_queue))
            else:
                raise cls.ParseError("Invalid row")
        if last_queue is not single_queue and len(single_queue) > 0:
            raise cls.ParseError("Queue has unbound rows")

        last_slot = None
        for q in filter(bool, queues):
            q[0]._prev = last_slot
            last_slot = q[-1]
        del last_slot

        if not queues:
            queues.append(single_queue)
        self = super().__new__(cls)
        self.__name = name
        self.__final = inheritance == "final"
        self.__tg_message = orig_message
        self.__key = cls.MessageKey(orig_message.chat.id, orig_message.message_id)
        self.__queues = tuple(queues)
        self.__first_slot = queues[0][0]

        return self

    def finalize(self):
        self.__final = True


class Queue:
    __slots__ = ("final", "name", "__data")
    __global_pattern = re.compile(r"""^<\s*b\s*>(?:(final)\s+|)куеуе(?:\s+=\s+"(?:</\s*b\s*><\s*i\s*>(.*)</\s*i\s*><\s*b\s*>|)"\s+|):</\s*b\s*>(?:\n\n([\s\S]+)|)$""")
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

    def passed(self, n):
        self.__data.pop(n)


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

            return self.BoundAFloodMessageWrapper(self.__cache.get((self.__message.chat.id, self.__message.message_id), self.__message), self.__cache)

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
        self._dp.register_callback_query_handler(self.__pass, GroupFilter(group_id), PrefixCheckFilter("qpass"))
        self._dp.register_message_handler(self.__finalize_cmd, GroupFilter(group_id), commands=["qfinalize"])

    async def __create_queue(self, message: Message):
        qname = message.get_args()
        if not qname or qname.isidentifier():
            m = await message.reply("<b>Сдесь могла быть ваша куеуе</b>", parse_mode="html")
        else:
            await message.reply("<b>Сдесь могла быть ваша куеуе, но вы не умеете в названия переменных</b>", parse_mode="html")
            return

        await m.edit_text(
            "<b>куеуе" + (f" = \"</b><i>{qname}</i><b>\" " if qname else "") + ":</b>",
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
                    [
                        InlineKeyboardButton(text=choice(("\U0001f31d", "\U0001f31a")), callback_data=" ")
                    ],
                    [
                        InlineKeyboardButton(text="нажать после сдачи", callback_data=f"qpass {m.chat.id} {m.message_id}")
                    ],
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

    async def __pass(self, query: CallbackQuery):
        async with self.__afloodcache(query.message) as message:
            if (q := Queue(message.html_text)) is None:
                await query.answer("NullPointerException")
                return

            if q.final:
                await query.answer("AccessToFinalQueueException")
                return

            for r in q:
                if r is not None and r.uid == query.from_user.id:
                    q.passed(r.pos)
                    await message.edit_text(q.dump(), parse_mode="html", reply_markup=message.reply_markup)
                    await query.answer("Поздравляем с успешным отсислением из очереди")
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
            await qmessage.edit_text(q.dump(), parse_mode="html")
            await message.reply("Queue finalized")
