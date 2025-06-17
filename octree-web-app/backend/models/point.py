class Point:
    def __init__(self, x, y, z):
        self.x = float(x)
        self.y = float(y)
        self.z = float(z)

    def __repr__(self):
        return f"Point({self.x}, {self.y}, {self.z})"
    
    def __eq__(self, other):
        if not isinstance(other, Point):
            return False
        return (abs(self.x - other.x) < 1e-6 and 
                abs(self.y - other.y) < 1e-6 and 
                abs(self.z - other.z) < 1e-6)
    
    def to_dict(self):
        return {"x": self.x, "y": self.y, "z": self.z}
    
    def distance_to(self, other):
        return ((self.x - other.x)**2 + (self.y - other.y)**2 + (self.z - other.z)**2)**0.5