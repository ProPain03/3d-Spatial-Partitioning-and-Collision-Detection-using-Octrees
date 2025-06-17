def read_points_from_file(file_path):
    points = []
    try:
        with open(file_path, 'r') as file:
            for line in file:
                x, y, z = map(float, line.strip().split())
                points.append({'x': x, 'y': y, 'z': z})
    except FileNotFoundError:
        print(f"File not found: {file_path}")
    except Exception as e:
        print(f"An error occurred while reading the file: {e}")
    return points

def write_points_to_file(file_path, points):
    try:
        with open(file_path, 'w') as file:
            for point in points:
                file.write(f"{point['x']} {point['y']} {point['z']}\n")
    except Exception as e:
        print(f"An error occurred while writing to the file: {e}")