import pytest
from backend.models.octree import Octree, OctreeNode
from backend.models.point import Point

def test_insert_and_search():
    octree = Octree(center=Point(0, 0, 0), size=1000)
    point = Point(10, 10, 10)
    octree.insert(point)
    assert octree.search(point) is not None

def test_delete():
    octree = Octree(center=Point(0, 0, 0), size=1000)
    point = Point(20, 20, 20)
    octree.insert(point)
    octree.delete(point)
    assert octree.search(point) is None

def test_range_query():
    octree = Octree(center=Point(0, 0, 0), size=1000)
    points = [Point(x, y, z) for x, y, z in [(10, 10, 10), (20, 20, 20), (30, 30, 30)]]
    for p in points:
        octree.insert(p)
    results = octree.range_query(Point(0, 0, 0), Point(25, 25, 25))
    assert len(results) == 2  # Should find two points within the range

def test_nearest_neighbor():
    octree = Octree(center=Point(0, 0, 0), size=1000)
    points = [Point(10, 10, 10), Point(20, 20, 20), Point(30, 30, 30)]
    for p in points:
        octree.insert(p)
    target = Point(15, 15, 15)
    nearest = octree.find_nearest_neighbor(target)
    assert nearest is not None
    assert nearest.x == 10 and nearest.y == 10 and nearest.z == 10  # Closest point

def test_collision_detection():
    octree = Octree(center=Point(0, 0, 0), size=1000)
    point1 = Point(10, 10, 10)
    point2 = Point(15, 15, 15)  # Close points for collision
    octree.insert(point1)
    octree.insert(point2)
    assert octree.detect_collision(point1) is True  # Should detect collision

def test_octree_initialization():
    octree = Octree(center=Point(0, 0, 0), size=1000)
    assert octree is not None
    assert octree.root.center.x == 0
    assert octree.root.center.y == 0
    assert octree.root.center.z == 0
    assert octree.root.size == 1000
    assert octree.root.points == []  # Should start with an empty list of points