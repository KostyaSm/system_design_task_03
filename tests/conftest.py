import pytest

pytest_plugins = ['pytest_userver.plugins.mongo']

#додел 
MONGO_COLLECTIONS = {
    'users': {
        'settings': {
            'collection': 'users',
            'connection': 'fitness_tracker',
            'database': 'fitness_tracker',
        },
        'indexes': [
            {'keys': {'login': 1}, 'options': {'unique': True}},
            {'keys': {'first_name_lc': 1, 'last_name_lc': 1}},
        ],
    },
    'exercises': {
        'settings': {
            'collection': 'exercises',
            'connection': 'fitness_tracker',
            'database': 'fitness_tracker',
        },
        'indexes': [
            {'keys': {'name': 1}, 'options': {'unique': True}},
            {'keys': {'category': 1, 'difficulty': 1}},
        ],
    },
    'workouts': {
        'settings': {
            'collection': 'workouts',
            'connection': 'fitness_tracker',
            'database': 'fitness_tracker',
        },
        'indexes': [
            {'keys': {'user_id': 1, 'date': -1}},
        ],
    },
    'counters': {
        'settings': {
            'collection': 'counters',
            'connection': 'fitness_tracker',
            'database': 'fitness_tracker',
        },
        'indexes': [],
    },
}

@pytest.fixture(scope='session')
def mongodb_settings():
    return MONGO_COLLECTIONS

@pytest.fixture(autouse=True)
def clean_mongo(mongodb):
    mongodb.users.delete_many({})
    mongodb.exercises.delete_many({})
    mongodb.workouts.delete_many({})
    mongodb.counters.delete_many({})