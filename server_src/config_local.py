from config import BaseConfig


class Config(BaseConfig):
    DEBUG = True
    # API Keys and associated services
    SECRET_KEY = '1aklsfjl1jk3b2t1'
    REDIS_PORT = 6379