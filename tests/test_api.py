import pytest
import json

@pytest.mark.userver_fixture
async def test_register_user(service_client, mongodb):
    response = await service_client.post('/api/v1/auth/register', json={
        'login': 'new_user',
        'first_name': 'Test',
        'last_name': 'User',
        'password': 'test123',
        'email': 'test@example.com'
    })
    assert response.status_code == 201
    data = response.json()
    assert data['login'] == 'new_user'
    assert 'id' in data


@pytest.mark.userver_fixture
async def test_register_duplicate_login(service_client, mongodb):
    await mongodb.users.insert_one({
        'id': 100, 'login': 'existing', 'first_name': 'X', 'last_name': 'Y',
        'password_hash': 'pw$test123', 'created_at': '2024-01-01T00:00:00Z'
    })
    response = await service_client.post('/api/v1/auth/register', json={
        'login': 'existing',
        'first_name': 'Test',
        'last_name': 'User',
        'password': 'test123'
    })
    assert response.status_code == 409

@pytest.mark.userver_fixture
async def test_login_success(service_client, mongodb):
    await mongodb.users.insert_one({
        'id': 1, 'login': 'testuser', 'first_name': 'Test', 'last_name': 'User',
        'password_hash': 'pw$test123', 'created_at': '2024-01-01T00:00:00Z'
    })
    response = await service_client.post('/api/v1/auth/login', json={
        'login': 'testuser',
        'password': 'test123'
    })
    assert response.status_code == 200
    data = response.json()
    assert 'token' in data
    assert data['user_id'] == 1

@pytest.mark.userver_fixture
async def test_login_invalid_password(service_client, mongodb):
    await mongodb.users.insert_one({
        'id': 1, 'login': 'testuser', 'first_name': 'Test', 'last_name': 'User',
        'password_hash': 'pw$correct', 'created_at': '2024-01-01T00:00:00Z'
    })
    response = await service_client.post('/api/v1/auth/login', json={
        'login': 'testuser',
        'password': 'wrong'
    })
    assert response.status_code == 401


#додел
@pytest.mark.userver_fixture
async def test_user_by_login(service_client, mongodb):
    await mongodb.users.insert_one({
        'id': 42, 'login': 'findme', 'first_name': 'Find', 'last_name': 'Me',
        'email': 'find@example.com', 'password_hash': 'pw$x', 'created_at': '2024-01-01T00:00:00Z'
    })
    response = await service_client.get('/api/v1/users/by-login', params={'login': 'findme'})
    assert response.status_code == 200
    data = response.json()
    assert data['id'] == 42
    assert data['first_name'] == 'Find'

@pytest.mark.userver_fixture
async def test_user_by_login_not_found(service_client, mongodb):
    response = await service_client.get('/api/v1/users/by-login', params={'login': 'nonexistent'})
    assert response.status_code == 404

@pytest.mark.userver_fixture
async def test_user_search_by_mask(service_client, mongodb):
    await mongodb.users.insert_many([
        {'id': 1, 'login': 'u1', 'first_name': 'Анна', 'last_name': 'Иванова', 'password_hash': 'pw$x', 'created_at': '2024-01-01T00:00:00Z'},
        {'id': 2, 'login': 'u2', 'first_name': 'Борис', 'last_name': 'Ананьев', 'password_hash': 'pw$x', 'created_at': '2024-01-01T00:00:00Z'},
        {'id': 3, 'login': 'u3', 'first_name': 'Виктор', 'last_name': 'Петров', 'password_hash': 'pw$x', 'created_at': '2024-01-01T00:00:00Z'},
    ])
    response = await service_client.get('/api/v1/users/search', params={'mask': 'ан'})
    assert response.status_code == 200
    data = response.json()
    assert 'users' in data
    assert len(data['users']) >= 2

@pytest.mark.userver_fixture
async def test_create_exercise(service_client, mongodb):
    response = await service_client.post('/api/v1/exercises', json={
        'name': 'Новое упражнение',
        'category': 'strength',
        'difficulty': 'beginner',
        'description': 'Описание',
        'muscle_groups': ['chest', 'triceps']
    })
    assert response.status_code == 201
    data = response.json()
    assert data['name'] == 'Новое упражнение'
    assert data['category'] == 'strength'

@pytest.mark.userver_fixture
async def test_list_exercises(service_client, mongodb):
    await mongodb.exercises.insert_many([
        {'id': 1, 'name': 'Ex1', 'category': 'strength', 'difficulty': 'beginner', 'is_public': True, 'created_by': 1, 'muscle_groups': [], 'created_at': '2024-01-01T00:00:00Z'},
        {'id': 2, 'name': 'Ex2', 'category': 'cardio', 'difficulty': 'intermediate', 'is_public': True, 'created_by': 1, 'muscle_groups': [], 'created_at': '2024-01-01T00:00:00Z'},
        {'id': 3, 'name': 'Ex3', 'category': 'strength', 'difficulty': 'advanced', 'is_public': False, 'created_by': 1, 'muscle_groups': [], 'created_at': '2024-01-01T00:00:00Z'},
    ])
    response = await service_client.get('/api/v1/exercises', params={'category': 'strength'})
    assert response.status_code == 200
    data = response.json()
    assert 'exercises' in data
    assert len(data['exercises']) >= 1
    assert all(ex['category'] == 'strength' for ex in data['exercises'])

 
@pytest.mark.userver_fixture
async def test_create_workout(service_client, mongodb):
    await mongodb.users.insert_one({'id': 1, 'login': 'wuser', 'first_name': 'W', 'last_name': 'U', 'password_hash': 'pw$x', 'created_at': '2024-01-01T00:00:00Z'})
    await mongodb.exercises.insert_one({'id': 10, 'name': 'Pushup', 'category': 'strength', 'difficulty': 'beginner', 'is_public': True, 'created_by': 1, 'muscle_groups': [], 'created_at': '2024-01-01T00:00:00Z'})
    
    response = await service_client.post('/api/v1/workouts', json={
        'title': 'Morning workout',
        'date': '2024-06-01T08:00:00Z',
        'duration_seconds': 1800,
        'notes': 'Felt great',
        'exercises': [
            {'exercise_id': 10, 'exercise_name': 'Pushup', 'sets': 3, 'reps': 15, 'order': 0}
        ]
    }, params={'user_id': 1})
    assert response.status_code == 201
    data = response.json()
    assert data['title'] == 'Morning workout'
    assert 'id' in data
 
@pytest.mark.userver_fixture
async def test_add_exercise_to_workout(service_client, mongodb):
    await mongodb.users.insert_one({'id': 1, 'login': 'wuser', 'first_name': 'W', 'last_name': 'U', 'password_hash': 'pw$x', 'created_at': '2024-01-01T00:00:00Z'})
    workout_id = 999
    await mongodb.workouts.insert_one({
        'id': workout_id, 'user_id': 1, 'title': 'Test', 'date': '2024-06-01T08:00:00Z',
        'exercises': [], 'created_at': '2024-01-01T00:00:00Z'
    })
    
    response = await service_client.post(f'/api/v1/workouts/{workout_id}/exercises', json={
        'exercise_id': 5,
        'exercise_name': 'Squat',
        'sets': 4,
        'reps': 10,
        'weight_kg': 80,
        'order': 0
    })
    assert response.status_code == 200
    updated = await mongodb.workouts.find_one({'id': workout_id})
    assert len(updated['exercises']) == 1
    assert updated['exercises'][0]['exercise_name'] == 'Squat'

@pytest.mark.userver_fixture
async def test_get_user_workouts(service_client, mongodb):
    await mongodb.users.insert_one({'id': 5, 'login': 'wuser5', 'first_name': 'W5', 'last_name': 'U5', 'password_hash': 'pw$x', 'created_at': '2024-01-01T00:00:00Z'})
    await mongodb.workouts.insert_many([
        {'id': 101, 'user_id': 5, 'title': 'Workout A', 'date': '2024-05-01T08:00:00Z', 'exercises': [], 'created_at': '2024-01-01T00:00:00Z'},
        {'id': 102, 'user_id': 5, 'title': 'Workout B', 'date': '2024-05-15T08:00:00Z', 'exercises': [], 'created_at': '2024-01-01T00:00:00Z'},
        {'id': 103, 'user_id': 99, 'title': 'Other user', 'date': '2024-05-10T08:00:00Z', 'exercises': [], 'created_at': '2024-01-01T00:00:00Z'},
    ])
    
    response = await service_client.get('/api/v1/users/me/workouts', params={'user_id': 5, 'limit': 10})
    assert response.status_code == 200
    data = response.json()
    assert 'workouts' in data
    assert len(data['workouts']) == 2
    assert all(w['user_id'] == 5 for w in data['workouts'])

@pytest.mark.userver_fixture
async def test_workout_statistics(service_client, mongodb):
    await mongodb.users.insert_one({'id': 7, 'login': 'wuser7', 'first_name': 'W7', 'last_name': 'U7', 'password_hash': 'pw$x', 'created_at': '2024-01-01T00:00:00Z'})
    await mongodb.workouts.insert_many([
        {'id': 201, 'user_id': 7, 'title': 'W1', 'date': '2024-03-01T08:00:00Z', 'duration_seconds': 3600,
         'exercises': [{'exercise_id': 1, 'exercise_name': 'Bench', 'sets': 3, 'reps': 10, 'weight_kg': 60, 'order': 0}],
         'created_at': '2024-01-01T00:00:00Z'},
        {'id': 202, 'user_id': 7, 'title': 'W2', 'date': '2024-04-01T08:00:00Z', 'duration_seconds': 2400,
         'exercises': [{'exercise_id': 2, 'exercise_name': 'Squat', 'sets': 4, 'reps': 8, 'weight_kg': 100, 'order': 0}],
         'created_at': '2024-01-01T00:00:00Z'},
    ])
    
    response = await service_client.get('/api/v1/workouts/statistics', params={
        'user_id': 7, 'from': '2024-01-01', 'to': '2024-12-31'
    })
    assert response.status_code == 200
    data = response.json()
    assert data['total_workouts'] >= 2
    assert 'total_volume_kg' in data