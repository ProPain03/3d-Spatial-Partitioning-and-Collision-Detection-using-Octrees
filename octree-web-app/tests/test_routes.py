import pytest
import json
from backend.app import app
from backend.models.point import Point

@pytest.fixture
def client():
    app.config['TESTING'] = True
    with app.test_client() as client:
        yield client

def test_insert_point(client):
    response = client.post('/api/octree/insert', 
                          data=json.dumps({"x": 1, "y": 2, "z": 3}),
                          content_type='application/json')
    assert response.status_code == 201
    data = json.loads(response.data)
    assert data['success'] == True

def test_delete_point(client):
    # First insert a point
    client.post('/api/octree/insert', 
                data=json.dumps({"x": 1, "y": 2, "z": 3}),
                content_type='application/json')
    
    # Then delete it
    response = client.delete('/api/octree/delete', 
                           data=json.dumps({"x": 1, "y": 2, "z": 3}),
                           content_type='application/json')
    assert response.status_code == 200
    data = json.loads(response.data)
    assert data['success'] == True

def test_search_point(client):
    # First insert a point
    client.post('/api/octree/insert', 
                data=json.dumps({"x": 1, "y": 2, "z": 3}),
                content_type='application/json')
    
    # Then search for it
    response = client.post('/api/octree/search', 
                          data=json.dumps({"x": 1, "y": 2, "z": 3}),
                          content_type='application/json')
    assert response.status_code == 200
    data = json.loads(response.data)
    assert data['found'] == True

def test_range_query(client):
    # Insert some points
    points = [{"x": 0, "y": 0, "z": 0}, {"x": 5, "y": 5, "z": 5}, {"x": 15, "y": 15, "z": 15}]
    for point in points:
        client.post('/api/octree/insert', 
                   data=json.dumps(point),
                   content_type='application/json')
    
    # Perform range query
    response = client.post('/api/octree/range', 
                          data=json.dumps({
                              "min": {"x": -1, "y": -1, "z": -1},
                              "max": {"x": 10, "y": 10, "z": 10}
                          }),
                          content_type='application/json')
    assert response.status_code == 200
    data = json.loads(response.data)
    assert data['success'] == True
    assert len(data['points']) >= 2  # Should find at least 2 points

def test_nearest_neighbor(client):
    # Insert some points
    points = [{"x": 0, "y": 0, "z": 0}, {"x": 5, "y": 5, "z": 5}, {"x": 15, "y": 15, "z": 15}]
    for point in points:
        client.post('/api/octree/insert', 
                   data=json.dumps(point),
                   content_type='application/json')
    
    # Find nearest neighbor
    response = client.post('/api/octree/nearest', 
                          data=json.dumps({"x": 1, "y": 1, "z": 1}),
                          content_type='application/json')
    assert response.status_code == 200
    data = json.loads(response.data)
    assert data['success'] == True
    assert 'nearest' in data

def test_move_point(client):
    # Insert a point first
    client.post('/api/octree/insert', 
                data=json.dumps({"x": 0, "y": 0, "z": 0}),
                content_type='application/json')
    
    # Move the point
    response = client.post('/api/game/move_direction', 
                          data=json.dumps({
                              "point": {"x": 0, "y": 0, "z": 0},
                              "direction": "w"
                          }),
                          content_type='application/json')
    assert response.status_code == 200
    data = json.loads(response.data)
    assert data['success'] == True
