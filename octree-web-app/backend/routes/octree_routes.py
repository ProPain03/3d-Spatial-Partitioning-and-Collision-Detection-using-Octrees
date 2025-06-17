from flask import Blueprint, request, jsonify
from models.octree import Octree
from models.point import Point
from utils.file_operations import read_points_from_file
import os

octree_bp = Blueprint('octree_bp', __name__)

# Initialize global octree
octree = Octree()

# Load initial points
initial_points_file = os.path.join(os.path.dirname(__file__), '..', 'static', 'data', 'random1.txt')
if os.path.exists(initial_points_file):
    points_data = read_points_from_file(initial_points_file)
    for point_data in points_data:
        point = Point(point_data['x'], point_data['y'], point_data['z'])
        octree.insert(point)

@octree_bp.route('/insert', methods=['POST'])
def insert_point():
    try:
        data = request.json
        point = Point(data['x'], data['y'], data['z'])
        success = octree.insert(point)
        if success:
            return jsonify({
                "message": "Point inserted successfully", 
                "success": True,
                "point": point.to_dict()
            }), 201
        else:
            return jsonify({"message": "Point insertion failed", "success": False}), 400
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500

@octree_bp.route('/delete', methods=['DELETE'])
def delete_point():
    try:
        data = request.json
        point = Point(data['x'], data['y'], data['z'])
        success = octree.delete(point)
        if success:
            return jsonify({
                "message": "Point deleted successfully", 
                "success": True,
                "point": point.to_dict()
            }), 200
        else:
            return jsonify({"message": "Point not found", "success": False}), 404
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500

@octree_bp.route('/search', methods=['POST'])
def search_point():
    try:
        data = request.json
        point = Point(data['x'], data['y'], data['z'])
        found_point = octree.search(point)
        if found_point:
            return jsonify({
                "found": True, 
                "point": found_point.to_dict(),
                "success": True
            }), 200
        else:
            return jsonify({
                "found": False, 
                "message": "Point not found",
                "success": False
            }), 404
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500

@octree_bp.route('/range', methods=['POST'])
def range_query():
    try:
        data = request.json
        min_point = Point(data['min']['x'], data['min']['y'], data['min']['z'])
        max_point = Point(data['max']['x'], data['max']['y'], data['max']['z'])
        points = octree.range_query(min_point, max_point)
        return jsonify({
            "points": [p.to_dict() for p in points],
            "count": len(points),
            "success": True,
            "range": {
                "min": min_point.to_dict(),
                "max": max_point.to_dict()
            }
        }), 200
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500

@octree_bp.route('/nearest', methods=['POST'])
def nearest_neighbor():
    try:
        data = request.json
        target_point = Point(data['x'], data['y'], data['z'])
        k = data.get('k', 1)  # Number of nearest neighbors
        
        if k == 1:
            nearest_point = octree.find_nearest_neighbor(target_point)
            if nearest_point:
                return jsonify({
                    "nearest": [nearest_point.to_dict()],
                    "target": target_point.to_dict(),
                    "success": True
                }), 200
        else:
            # For k > 1, we need to implement k-nearest neighbors
            all_points = octree.get_all_points()
            distances = [(p, target_point.distance_to(p)) for p in all_points if p != target_point]
            distances.sort(key=lambda x: x[1])
            nearest_points = [p[0].to_dict() for p in distances[:k]]
            
            return jsonify({
                "nearest": nearest_points,
                "target": target_point.to_dict(),
                "success": True
            }), 200
            
        return jsonify({
            "message": "No points found",
            "success": False
        }), 404
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500

@octree_bp.route('/points', methods=['GET'])
def get_all_points():
    try:
        points = octree.get_all_points()
        return jsonify({
            "points": [p.to_dict() for p in points],
            "count": len(points),
            "success": True
        }), 200
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500

@octree_bp.route('/structure', methods=['GET'])
def get_octree_structure():
    try:
        return jsonify({
            "octree": octree.to_dict(),
            "success": True
        }), 200
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500