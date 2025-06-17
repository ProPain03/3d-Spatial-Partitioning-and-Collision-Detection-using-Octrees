# 3D Spatial Partitioning and Collision Detection using Octrees Web Application

## Project Overview
This project implements a 3D spatial partitioning and collision detection system using Octrees. It consists of a backend built with Python (using Flask or FastAPI) and a frontend utilizing Three.js for interactive 3D visualization. Users can perform operations such as inserting, deleting, and searching for points in the Octree, as well as detecting collisions in real-time.

## Project Structure
The project is organized into the following directories and files:

- **backend/**: Contains the server-side code, including models, routes, and utilities.
- **frontend/**: Contains the client-side code, including HTML, CSS, and JavaScript files for visualization.
- **config/**: Contains configuration settings for the application.
- **tests/**: Contains unit tests for various components of the application.
- **docker-compose.yml**: Defines services for easy setup and deployment using Docker.
- **Dockerfile**: Instructions for building the Docker image for the application.
- **.gitignore**: Specifies files and directories to be ignored by version control.

## Setup Instructions
1. **Compile C files** into shared libraries (.so or .dll) using appropriate commands for your OS.
2. Go to the octree-web-app directory
3. **Install Python dependencies** using pip:
   ```
   pip install -r requirements.txt
   ```
4. **Install npm dependencies** for the frontend (if using a package manager):
   ```
   npm install
   ```
5. **Run the backend server**:
   ```
   python backend/app.py
   ```
6. **Open the frontend** in a web browser to interact with the application.

## Dependencies
- Flask or FastAPI
- Flask-Cors (if using Flask)
- NumPy (for mathematical operations)
- Three.js (for frontend 3D visualization)
- WebSocket library (for real-time communication)
- Additional libraries for testing (e.g., pytest)

## Usage
- The backend provides API endpoints for Octree operations, which can be accessed via the frontend.
- Users can visualize the Octree structure and interact with points in 3D space.
- Real-time collision detection is implemented to provide feedback when points collide.

## Testing
- Unit tests are provided for the Octree functionality, collision detection logic, and API routes.
- To run the tests, use:
  ```
  pytest
  ```

## Contribution
Contributions to the project are welcome. Please submit a pull request or open an issue for discussion.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.
