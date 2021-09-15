from ._base import BaseBot


class RandomQueueBot(BaseBot):
    def __init__(self, token, *, group_id, **kwargs):
        super().__init__(token, group_id=group_id, **kwargs)
