import pytest
from backend.models.octree import Octree
from backend.models.point import Point

def test_collision_detection():
    octree = Octree()
    
    # Insert some points
    point1 = Point(0, 0, 0)
    point2 = Point(10, 10, 10)  # Far enough apart
    point3 = Point(5, 5, 5)     # Close to point1
    
    octree.insert(point1)
    octree.insert(point2)
    octree.insert(point3)
    
    # Test collision detection - point close to point1 and point3
    test_point = Point(2, 2, 2)  # Very close to point1
    collision = octree.detect_collision(test_point)
    assert collision == True
    
    # Test no collision
    far_point = Point(1000, 1000, 1000)
    no_collision = octree.detect_collision(far_point)
    assert no_collision == False

def test_point_distance_calculation():
    point1 = Point(0, 0, 0)
    point2 = Point(3, 4, 0)  # Distance should be 5
    distance = point1.distance_to(point2)
    assert abs(distance - 5.0) < 1e-6

def test_collision_with_itself():
    octree = Octree()
    point = Point(10, 10, 10)
    octree.insert(point)
    
    # When checking collision of the same point, it should not collide with itself
    collision = octree.detect_collision(point)
    assert collision == False  # Point should not collide with itself