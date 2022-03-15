## **About**

Telegram bot on `aiogram` for group P3118 in [ITMO](https://itmo.ru/)

## **Installing**

`pip install https://github.com/LandgrafHomyak/P3118Helper-cpython/archive/refs/tags/v0.3.0rc0.tar.gz`

## **Using**

```python
import asyncio
from p3118helper import P3118HelperBot

b = P3118HelperBot("123:token", group_id=GROUPS_CHAT_ID_IN_TELEGRAM) # chat id can be obtained with command /cid
asyncio.new_event_loop().run_until_complete(b.run())
# or just (starts as daemon)
# b.start(loop=asyncio.get_event_loop())
```
