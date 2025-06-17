from flask import Blueprint, request, jsonify
from models.point import Point

game_bp = Blueprint('game_bp', __name__)

# Import the global octree from octree_routes
def get_octree():
    from routes.octree_routes import octree
    return octree

STEP = 50  # Movement step size

@game_bp.route('/move', methods=['PUT'])
def move_point():
    try:
        octree = get_octree()
        data = request.json
        old_point = Point(data['oldPoint']['x'], data['oldPoint']['y'], data['oldPoint']['z'])
        new_point = Point(data['newPoint']['x'], data['newPoint']['y'], data['newPoint']['z'])

        # Check for collision at new position
        if octree.detect_collision(new_point):
            return jsonify({
                'message': 'Collision detected, movement not allowed.',
                'success': False,
                'collision': True
            }), 400

        # Update point position
        success = octree.update_point(old_point, new_point)
        if success:
            return jsonify({
                'message': 'Point moved successfully.',
                'new_position': new_point.to_dict(),
                'old_position': old_point.to_dict(),
                'success': True,
                'collision': False
            }), 200
        else:
            return jsonify({
                'message': 'Failed to move point. Original point not found.',
                'success': False
            }), 404
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500

@game_bp.route('/move_direction', methods=['POST'])
def move_point_by_direction():
    try:
        octree = get_octree()
        data = request.json
        current_point = Point(data['point']['x'], data['point']['y'], data['point']['z'])
        direction = data['direction'].lower()

        # Calculate new position based on direction
        new_x, new_y, new_z = current_point.x, current_point.y, current_point.z
        
        if direction == 'w':  # Forward (positive Y)
            new_y += STEP
        elif direction == 's':  # Backward (negative Y)
            new_y -= STEP
        elif direction == 'a':  # Left (negative X)
            new_x -= STEP
        elif direction == 'd':  # Right (positive X)
            new_x += STEP
        elif direction == 'e':  # Up (positive Z)
            new_z += STEP
        elif direction == 'f':  # Down (negative Z)
            new_z -= STEP
        else:
            return jsonify({
                'message': 'Invalid direction. Use w/a/s/d/e/f',
                'success': False
            }), 400

        new_point = Point(new_x, new_y, new_z)

        # Check for collision at new position
        if octree.detect_collision(new_point):
            return jsonify({
                'message': 'Collision detected, movement not allowed.',
                'success': False,
                'collision': True,
                'current_position': current_point.to_dict()
            }), 400

        # Update point position
        success = octree.update_point(current_point, new_point)
        if success:
            return jsonify({
                'message': f'Point moved {direction.upper()} successfully.',
                'new_position': new_point.to_dict(),
                'old_position': current_point.to_dict(),
                'success': True,
                'collision': False
            }), 200
        else:
            return jsonify({
                'message': 'Failed to move point. Original point not found.',
                'success': False
            }), 404
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500

@game_bp.route('/collision', methods=['POST'])
def check_collision():
    try:
        octree = get_octree()
        data = request.json
        point = Point(data['x'], data['y'], data['z'])
        collision_status = octree.detect_collision(point)
        return jsonify({
            'collision': collision_status,
            'success': True
        }), 200
    except Exception as e:
        return jsonify({"message": f"Error: {str(e)}", "success": False}), 500