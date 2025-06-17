def detect_collision(points, moving_point, collision_size):
    collision_detected = False
    for point in points:
        if (abs(point.x - moving_point.x) < collision_size and
            abs(point.y - moving_point.y) < collision_size and
            abs(point.z - moving_point.z) < collision_size):
            collision_detected = True
            break
    return collision_detected