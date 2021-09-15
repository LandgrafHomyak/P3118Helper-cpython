from typing import NoReturn

from ._base import BaseBot


class QueueBot(BaseBot):
    def __init__(self, token: str, *, group_id: int) -> NoReturn: ...
