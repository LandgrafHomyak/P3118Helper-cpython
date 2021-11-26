import asyncio
import queue
import sys
from asyncio import Lock
from getpass import getpass
from io import StringIO
from threading import Thread

import aiogram
import colorama
import pkg_resources
from aiogram import Dispatcher
from aiogram.types import Message
from aiogram.utils.exceptions import MessageNotModified

from p3118helper.queue import QueueMessage, QueueStruct


class main:
    __slots__ = "__bot", "__loop", "__last_command", "__editor_uid", "__output_mutex"

    def __new__(cls):
        self = super().__new__(cls)
        colorama.init()

        print(f"P3118Helper ({colorama.Fore.BLUE}{pkg_resources.get_distribution('p3118helper').version}{colorama.Fore.RESET}) editor tools")
        print(f"{colorama.Fore.MAGENTA}Bot Token:", end=" ")
        token = getpass("")
        print(f"Your UID:", end=" ")
        self.__editor_uid = input()
        print(colorama.Fore.RESET, end="")
        if not self.__editor_uid.isdigit():
            print(f"{colorama.Fore.RED}User ID must be int{colorama.Fore.RESET}")
            exit(1)
        self.__editor_uid = int(self.__editor_uid)

        print(f"{colorama.Fore.LIGHTBLACK_EX}Preparing environment...{colorama.Fore.RESET}")

        self.__loop = asyncio.new_event_loop()
        self.__bot = aiogram.Bot(token=token, loop=self.__loop)
        dp = Dispatcher(self.__bot)
        self.__last_command = ""
        Thread(target=self.__input_thread_proc, daemon=True).start()
        dp.register_message_handler(self.__edit_callback, commands=["edit"])
        self.__output_mutex = Lock(loop=self.__loop)

        try:
            self.__loop.run_until_complete(self.__check_profile())
            self.__loop.create_task(self.__gl_console())
            self.__loop.run_until_complete(dp.start_polling())

        finally:
            if dp.is_polling():
                dp.stop_polling()
            self.__loop.run_until_complete(self.__bot.close_bot())
            self.__loop.close()
            colorama.deinit()

    async def __check_profile(self):
        me = await self.__bot.get_me()
        print(
            f"bot-name={colorama.Fore.GREEN}{me.full_name!r}{colorama.Fore.RESET}",
            f"bot-uid={colorama.Fore.GREEN}{me.id}{colorama.Fore.RESET}",
        )
        print(f"{colorama.Fore.MAGENTA}Right bot? (y/N):", end=" ")
        answer = input()
        print(colorama.Fore.RESET, end="")
        if not "yes".startswith(answer.strip().lower()):
            print(f"{colorama.Fore.RED}Interrupted{colorama.Fore.RESET}")
            print(f"{colorama.Fore.LIGHTBLACK_EX}Stopping...{colorama.Fore.RESET}")
            exit(-2)
        else:
            print(f"Use command {colorama.Fore.BLUE}/edit{colorama.Fore.RESET} to attach to existing queue")

    async def __gl_console(self):
        while True:
            async with self.__output_mutex:
                command = self.__last_command
                if command is None:
                    await asyncio.sleep(0)
                    continue
                command = command.split()

                if len(command) == 0 or command == [""]:
                    pass
                elif command[0] == "help":
                    print(f" {colorama.Fore.BLUE}help{colorama.Fore.RESET} - show this text")
                    print(f" {colorama.Fore.BLUE}quit{colorama.Fore.RESET} - exit from editor")
                    print(f" {colorama.Fore.BLUE}exit{colorama.Fore.RESET} - exit from editor")
                    print(f" {colorama.Fore.BLUE}new{colorama.Fore.RESET} {colorama.Fore.YELLOW}name *subqueues{colorama.Fore.RESET} - create new queue message")
                elif command[0] == "exit" or command[0] == "quit":
                    print(f" {colorama.Fore.LIGHTBLACK_EX}Stopping...{colorama.Fore.RESET}")
                    exit(0)
                elif command[0] == "new":
                    print(f" {colorama.Fore.LIGHTBLACK_EX}Creating queue...{colorama.Fore.RESET}")
                    if len(command) >= 2:
                        try:
                            qs = QueueStruct(name=command[1], final=False, queues=[] if len(command) == 2 else {n: [] for n in command[2:]})
                        except Exception as exc:
                            print(f" {colorama.Fore.RED}{type(exc).__qualname__}: {exc!s}{colorama.Fore.RESET}")
                        else:
                            print(f" {colorama.Fore.LIGHTBLACK_EX}Starting queue editor...{colorama.Fore.RESET}")
                            self.__last_command = ""
                            await self.__e_console(qs)
                    else:
                        print(f" {colorama.Fore.RED}Queue name not specified{colorama.Fore.RESET}")
                else:
                    print(f" {colorama.Fore.RED}Unknown command{colorama.Fore.RESET}")
                print(f"{colorama.Fore.BLUE}>{colorama.Fore.RESET} {colorama.Fore.CYAN}", end="")
                self.__last_command = None

    def __input_thread_proc(self):
        while True:
            if self.__last_command is None:
                self.__last_command = input().strip()

    async def __edit_callback(self, message: Message):
        if message.from_user.id != self.__editor_uid:
            return
        pass

        message = message.reply_to_message
        try:
            qs = QueueMessage.parse(message, None)
        except QueueMessage.ParseError:
            return

        print()
        print(f" {colorama.Fore.LIGHTBLACK_EX}Command /edit invoked, attaching...{colorama.Fore.RESET}")
        print(f" {colorama.Fore.LIGHTBLACK_EX}Starting queue editor...{colorama.Fore.RESET}")
        self.__last_command = ""
        async with self.__output_mutex:
            await self.__e_console(qs, message)

    async def __e_console(self, qs, e: Message = None):
        while True:
            command = self.__last_command
            if command is None:
                await asyncio.sleep(0)
                continue
            print(colorama.Fore.RESET, end="")
            command = command.split()

            if len(command) == 0 or command == [""]:
                pass
            elif command[0] == "help":
                print(f" {colorama.Fore.BLUE}help{colorama.Fore.RESET} - show this text")
                print(f" {colorama.Fore.BLUE}view{colorama.Fore.RESET} - print current queue")
                print(f" {colorama.Fore.BLUE}quit{colorama.Fore.RESET} - exit from editor")
                print(f" {colorama.Fore.BLUE}exit{colorama.Fore.RESET} - exit from editor")
                print()
                print(f" {colorama.Fore.BLUE}push{colorama.Fore.RESET} {colorama.Fore.YELLOW}queue_no uid *user_display_name{colorama.Fore.RESET} - add user to queue")
                print(f" {colorama.Fore.BLUE}pop{colorama.Fore.RESET} {colorama.Fore.YELLOW}uid{colorama.Fore.RESET} - pop command")
                print(f" {colorama.Fore.BLUE}up{colorama.Fore.RESET} {colorama.Fore.YELLOW}uid{colorama.Fore.RESET} - up command")
                print(f" {colorama.Fore.BLUE}down{colorama.Fore.RESET} {colorama.Fore.YELLOW}uid{colorama.Fore.RESET} - down command")
                print(f" {colorama.Fore.BLUE}next{colorama.Fore.RESET} {colorama.Fore.YELLOW}uid{colorama.Fore.RESET} - next command")
                print(f" {colorama.Fore.BLUE}prev{colorama.Fore.RESET} {colorama.Fore.YELLOW}uid{colorama.Fore.RESET} - prev command")
                print()
                print(f" {colorama.Fore.BLUE}cancel{colorama.Fore.RESET} - cancel all uncommitted changed and detach")
                print(f" {colorama.Fore.RED}rollback{colorama.Fore.RESET} - not implemented")
                if e is not None:
                    print(f" {colorama.Fore.BLUE}commit{colorama.Fore.RESET} - send changes to attached message")
                print(f" {colorama.Fore.BLUE}send{colorama.Fore.RESET} {colorama.Fore.YELLOW}chat_id{colorama.Fore.RESET} - send changes to attached message")
                print(f" {colorama.Fore.RED}rollback{colorama.Fore.RESET} {colorama.Fore.YELLOW}chat_id{colorama.Fore.RESET} - not implemented")
            elif command[0] == "exit" or command[0] == "quit":
                print(f" {colorama.Fore.LIGHTBLACK_EX}Stopping...{colorama.Fore.RESET}")
                exit(0)
            elif command[0] == "cancel":
                return
            elif command[0] == "view":
                print(f" queue {colorama.Fore.GREEN}{qs.name!r}{colorama.Fore.RESET}", f"{colorama.Fore.YELLOW}(final){colorama.Fore.RESET}" if qs.final else "")
                for sq in qs:
                    if sq.name is not None:
                        print(f"   sub-queue {colorama.Fore.GREEN}{sq.name!r}{colorama.Fore.RESET}")
                    for i, s in enumerate(sq):
                        s: QueueStruct.QueueSlot
                        print(f"     {colorama.Fore.RED}{i}{colorama.Fore.RESET}:", end="")
                        if s.uid is not None:
                            print(f"\t {colorama.Fore.BLUE}{s.uid}{colorama.Fore.RESET}\t {colorama.Fore.LIGHTYELLOW_EX}{s.display_name!r}{colorama.Fore.RESET}")
                        else:
                            print()
            elif command[0] == "push":
                if len(command) < 4:
                    print(f" {colorama.Fore.RED}Not enough argument{colorama.Fore.RESET}")
                else:
                    if command[1].isdigit():
                        if command[2].isdigit():
                            try:
                                print(f" {colorama.Fore.LIGHTBLACK_EX}Pushing...{colorama.Fore.RESET}")
                                qs.push(qs.user(uid=int(command[2]), display_name=" ".join(command[3:])), int(command[1]))
                                print(f" {colorama.Fore.LIGHTGREEN_EX}Pushed{colorama.Fore.RESET}")
                            except IndexError:
                                print(f" {colorama.Fore.RED}Queue with this index doesn't exist{colorama.Fore.RESET}")
                            except KeyError:
                                print(f" {colorama.Fore.RED}User with this ID is already in queue{colorama.Fore.RESET}")
                        else:
                            print(f" {colorama.Fore.RED}User ID must be int{colorama.Fore.RESET}")
                    else:
                        print(f" {colorama.Fore.RED}Queue index must be int{colorama.Fore.RESET}")
            elif command[0] == "pop":
                if len(command) < 2:
                    print(f" {colorama.Fore.RED}Invalid arguments count{colorama.Fore.RESET}")
                else:
                    if command[1].isdigit():
                        print(f" {colorama.Fore.LIGHTBLACK_EX}Popping...{colorama.Fore.RESET}")
                        if (s := qs.find_user(int(command[1]))) is not None:
                            qs.pop(int(command[1]))
                            print(f" {colorama.Fore.LIGHTGREEN_EX}Popped{colorama.Fore.RESET}")
                        else:
                            print(f" {colorama.Fore.RED}No such user with this ID{colorama.Fore.RESET}")
                    else:
                        print(f" {colorama.Fore.RED}User ID must be int{colorama.Fore.RESET}")
            elif command[0] == "up":
                if len(command) < 2:
                    print(f" {colorama.Fore.RED}Invalid arguments count{colorama.Fore.RESET}")
                else:
                    if command[1].isdigit():
                        print(f" {colorama.Fore.LIGHTBLACK_EX}Upping...{colorama.Fore.RESET}")
                        if (s := qs.find_user(int(command[1]))) is not None:
                            qs.find_user(int(command[1])).up()
                            print(f" {colorama.Fore.LIGHTGREEN_EX}Time goes faster...{colorama.Fore.RESET}")
                        else:
                            print(f" {colorama.Fore.RED}No such user with this ID{colorama.Fore.RESET}")
                    else:
                        print(f" {colorama.Fore.RED}User ID must be int{colorama.Fore.RESET}")
            elif command[0] == "down":
                if len(command) < 2:
                    print(f" {colorama.Fore.RED}Invalid arguments count{colorama.Fore.RESET}")
                else:
                    if command[1].isdigit():
                        print(f" {colorama.Fore.LIGHTBLACK_EX}Downing...{colorama.Fore.RESET}")
                        if (s := qs.find_user(int(command[1]))) is not None:
                            s.down()
                            print(f" {colorama.Fore.LIGHTGREEN_EX}Time goes slower...{colorama.Fore.RESET}")
                        else:
                            print(f" {colorama.Fore.RED}No such user with this ID{colorama.Fore.RESET}")
                    else:
                        print(f" {colorama.Fore.RED}User ID must be int{colorama.Fore.RESET}")
            elif command[0] == "prev":
                if len(command) < 2:
                    print(f" {colorama.Fore.RED}Invalid arguments count{colorama.Fore.RESET}")
                else:
                    if command[1].isdigit():
                        try:
                            print(f" {colorama.Fore.LIGHTBLACK_EX}Moving next...{colorama.Fore.RESET}")
                            if (s := qs.find_user(int(command[1]))) is None:
                                print(f" {colorama.Fore.RED}No such user with this ID{colorama.Fore.RESET}")
                            else:
                                i = s.queue.index
                                if i > 0:
                                    d = QueueStruct.user(uid=s.uid, display_name=s.display_name)
                                    s.clear()
                                    qs.push(d, i - 1)
                                    print(f" {colorama.Fore.LIGHTGREEN_EX}Time goes slower...{colorama.Fore.RESET}")
                                else:
                                    print(f" {colorama.Fore.RED}User already at first sub-queue{colorama.Fore.RESET}")
                        except KeyError:
                            print(f" {colorama.Fore.RED}No such user with this ID{colorama.Fore.RESET}")
                    else:
                        print(f" {colorama.Fore.RED}User ID must be int{colorama.Fore.RESET}")
            elif command[0] == "next":
                if len(command) < 2:
                    print(f" {colorama.Fore.RED}Invalid arguments count{colorama.Fore.RESET}")
                else:
                    if command[1].isdigit():
                        try:
                            print(f" {colorama.Fore.LIGHTBLACK_EX}Moving next...{colorama.Fore.RESET}")
                            if (s := qs.find_user(int(command[1]))) is None:
                                print(f" {colorama.Fore.RED}No such user with this ID{colorama.Fore.RESET}")
                            else:
                                i = s.queue.index
                                if i < len(qs) - 1:
                                    d = QueueStruct.user(uid=s.uid, display_name=s.display_name)
                                    s.clear()
                                    qs.push(d, i + 1)
                                    print(f" {colorama.Fore.LIGHTGREEN_EX}Time goes slower...{colorama.Fore.RESET}")
                                else:
                                    print(f" {colorama.Fore.RED}User already at last sub-queue{colorama.Fore.RESET}")
                        except KeyError:
                            print(f" {colorama.Fore.RED}No such user with this ID{colorama.Fore.RESET}")
                    else:
                        print(f" {colorama.Fore.RED}User ID must be int{colorama.Fore.RESET}")
            elif command[0] == "commit":
                if e is not None:
                    if len(command) != 1:
                        print(f" {colorama.Fore.RED}Invalid arguments count{colorama.Fore.RESET}")
                    else:
                        try:
                            print(f" {colorama.Fore.LIGHTBLACK_EX}Updating...{colorama.Fore.RESET}")
                            await e.edit_text(QueueMessage.dump(qs), parse_mode="html", reply_markup=e.reply_markup)
                        except MessageNotModified:
                            print(f" {colorama.Fore.YELLOW}Nothing changed{colorama.Fore.RESET}")
                        except Exception as exc:
                            print(f" {colorama.Fore.RED}{type(exc).__name__}: {exc!s}{colorama.Fore.RESET}")
                else:
                    print(f" {colorama.Fore.RED}Queue is detached{colorama.Fore.RESET}")
            elif command[0] == "send":
                if len(command) != 2:
                    print(f" {colorama.Fore.RED}Invalid arguments count{colorama.Fore.RESET}")
                else:
                    if command[1].isdigit() or command[1].startswith("-") and command[1][1:].isdigit():
                        try:
                            print(f" {colorama.Fore.LIGHTBLACK_EX}Sending...{colorama.Fore.RESET}")
                            await self.__bot.send_message(int(command[1]), QueueMessage.dump(qs), parse_mode="html", reply_markup=QueueMessage.KEYBOARD)
                        except Exception as exc:
                            print(f" {colorama.Fore.RED}{type(exc).__name__}: {exc!s}{colorama.Fore.RESET}")
                    else:
                        print(f" {colorama.Fore.RED}Chat ID must be int{colorama.Fore.RESET}")
            else:
                print(f" {colorama.Fore.RED}Unknown command{colorama.Fore.RESET}")
            print(f"{colorama.Fore.BLUE}q@{colorama.Fore.RESET} {colorama.Fore.CYAN}", end="")
            self.__last_command = None


if __name__ == '__main__':
    main()
