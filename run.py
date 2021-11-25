import sys
import asyncio
from p3118helper import P3118HelperBot

b = P3118HelperBot(sys.argv[1], group_id=int(sys.argv[2]), admins=set(map(int, sys.argv[3].split(":"))))
asyncio.new_event_loop().run_until_complete(b.run())
