from .point import Point
import math

# Constants from your original project
MAX_SIZE = 1000
MAX_DEPTH = 5
MAX_POINTS = 2
COLLISION_SIZE = 30

class OctreeNode:
    def __init__(self, center, size, depth=0):
        self.center = center
        self.size = size
        self.depth = depth
        self.is_leaf = True
        self.points = []
        self.children = [None] * 8
        self.pt_count = 0
        
        # Calculate min and max bounds
        half_size = size / 2
        self.min = Point(center.x - half_size, center.y - half_size, center.z - half_size)
        self.max = Point(center.x + half_size, center.y + half_size, center.z + half_size)

    def get_octant(self, point):
        """Determine which octant a point belongs to"""
        octant = 0
        if point.x >= self.center.x:
            octant |= 4
        if point.y >= self.center.y:
            octant |= 2
        if point.z >= self.center.z:
            octant |= 1
        return octant

    def subdivide(self):
        """Create 8 child nodes"""
        if not self.is_leaf:
            return
            
        half_size = self.size / 2
        quarter_size = half_size / 2
        
        # 8 octant centers
        octant_centers = [
            Point(self.center.x - quarter_size, self.center.y - quarter_size, self.center.z - quarter_size),  # 0
            Point(self.center.x - quarter_size, self.center.y - quarter_size, self.center.z + quarter_size),  # 1
            Point(self.center.x - quarter_size, self.center.y + quarter_size, self.center.z - quarter_size),  # 2
            Point(self.center.x - quarter_size, self.center.y + quarter_size, self.center.z + quarter_size),  # 3
            Point(self.center.x + quarter_size, self.center.y - quarter_size, self.center.z - quarter_size),  # 4
            Point(self.center.x + quarter_size, self.center.y - quarter_size, self.center.z + quarter_size),  # 5
            Point(self.center.x + quarter_size, self.center.y + quarter_size, self.center.z - quarter_size),  # 6
            Point(self.center.x + quarter_size, self.center.y + quarter_size, self.center.z + quarter_size),  # 7
        ]
        
        for i in range(8):
            self.children[i] = OctreeNode(octant_centers[i], half_size, self.depth + 1)
        
        self.is_leaf = False

    def insert(self, point):
        """Insert a point into the octree"""
        if self.is_leaf:
            if len(self.points) < MAX_POINTS or self.depth >= MAX_DEPTH:
                self.points.append(point)
                self.pt_count += 1
                return True
            else:
                # Need to subdivide
                self.subdivide()
                # Move existing points to children
                for p in self.points:
                    octant = self.get_octant(p)
                    self.children[octant].insert(p)
                self.points = []
                self.pt_count = 0
                # Insert the new point
                octant = self.get_octant(point)
                return self.children[octant].insert(point)
        else:
            octant = self.get_octant(point)
            success = self.children[octant].insert(point)
            if success:
                self.pt_count += 1
            return success

    def search(self, point):
        """Search for a point in the octree"""
        if self.is_leaf:
            for p in self.points:
                if p == point:
                    return p
            return None
        else:
            octant = self.get_octant(point)
            return self.children[octant].search(point)

    def delete(self, point):
        """Delete a point from the octree"""
        if self.is_leaf:
            for i, p in enumerate(self.points):
                if p == point:
                    self.points.pop(i)
                    self.pt_count -= 1
                    return True
            return False
        else:
            octant = self.get_octant(point)
            success = self.children[octant].delete(point)
            if success:
                self.pt_count -= 1
            return success

    def range_query(self, min_point, max_point, results=None):
        """Find all points within a range"""
        if results is None:
            results = []
            
        # Check if this node intersects with the query range
        if (self.max.x < min_point.x or self.min.x > max_point.x or
            self.max.y < min_point.y or self.min.y > max_point.y or
            self.max.z < min_point.z or self.min.z > max_point.z):
            return results
            
        if self.is_leaf:
            for point in self.points:
                if (min_point.x <= point.x <= max_point.x and
                    min_point.y <= point.y <= max_point.y and
                    min_point.z <= point.z <= max_point.z):
                    results.append(point)
        else:
            for child in self.children:
                if child:
                    child.range_query(min_point, max_point, results)
        
        return results

    def find_nearest_neighbor(self, target, best_point=None, best_distance=float('inf')):
        """Find the nearest neighbor to a target point"""
        if self.is_leaf:
            for point in self.points:
                distance = target.distance_to(point)
                if distance < best_distance:
                    best_distance = distance
                    best_point = point
        else:
            # Check children in order of distance to target
            for child in self.children:
                if child and child.pt_count > 0:
                    best_point, best_distance = child.find_nearest_neighbor(target, best_point, best_distance)
        
        return best_point, best_distance

    def get_all_points(self, points=None):
        """Get all points in the octree"""
        if points is None:
            points = []
            
        if self.is_leaf:
            points.extend(self.points)
        else:
            for child in self.children:
                if child:
                    child.get_all_points(points)
        
        return points

    def to_dict(self):
        """Convert node to dictionary for JSON serialization"""
        result = {
            "center": self.center.to_dict(),
            "size": self.size,
            "depth": self.depth,
            "is_leaf": self.is_leaf,
            "points": [p.to_dict() for p in self.points],
            "pt_count": self.pt_count,
            "min": self.min.to_dict(),
            "max": self.max.to_dict(),
            "children": []
        }
        
        # Add children if not a leaf
        if not self.is_leaf:
            for child in self.children:
                if child:
                    result["children"].append(child.to_dict())
                else:
                    result["children"].append(None)
        
        return result


class Octree:
    def __init__(self, center=None, size=MAX_SIZE):
        if center is None:
            center = Point(0, 0, 0)
        self.root = OctreeNode(center, size, 0)

    def insert(self, point):
        """Insert a point into the octree"""
        return self.root.insert(point)

    def search(self, point):
        """Search for a point in the octree"""
        return self.root.search(point)

    def delete(self, point):
        """Delete a point from the octree"""
        return self.root.delete(point)

    def range_query(self, min_point, max_point):
        """Find all points within a range"""
        return self.root.range_query(min_point, max_point)

    def find_nearest_neighbor(self, target):
        """Find the nearest neighbor to a target point"""
        best_point, best_distance = self.root.find_nearest_neighbor(target)
        return best_point

    def get_all_points(self):
        """Get all points in the octree"""
        return self.root.get_all_points()

    def detect_collision(self, point, collision_size=COLLISION_SIZE):
        """Detect collision with nearby points"""
        half_size = collision_size / 2
        min_point = Point(point.x - half_size, point.y - half_size, point.z - half_size)
        max_point = Point(point.x + half_size, point.y + half_size, point.z + half_size)
        
        nearby_points = self.range_query(min_point, max_point)
        
        for nearby_point in nearby_points:
            if nearby_point != point:
                distance = point.distance_to(nearby_point)
                if distance < collision_size:
                    return True
        return False

    def update_point(self, old_point, new_point):
        """Update a point's position"""
        if self.delete(old_point):
            return self.insert(new_point)
        return False

    def to_dict(self):
        """Convert octree to dictionary for JSON serialization"""
        return self.root.to_dict()