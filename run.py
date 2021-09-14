import sys
import asyncio
from p3118helper import P3118HelperBot

b = P3118HelperBot(sys.argv[1], group_id=int(sys.argv[2]))
asyncio.new_event_loop().run_until_complete(b.run())
