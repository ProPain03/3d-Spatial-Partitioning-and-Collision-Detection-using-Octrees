from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import os
import sys

# Add the parent directory to Python path
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

# Import and register blueprints with fixed imports
from routes.octree_routes import octree_bp
from routes.game_routes import game_bp

app.register_blueprint(octree_bp, url_prefix='/api/octree')
app.register_blueprint(game_bp, url_prefix='/api/game')

@app.route('/')
def index():
    return jsonify({"message": "Welcome to the Octree Web App!"})

# Serve static files (frontend)
@app.route('/<path:path>')
def serve_frontend(path):
    frontend_dir = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'frontend')
    return send_from_directory(frontend_dir, path)

@app.route('/frontend')
def serve_index():
    frontend_dir = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'frontend')
    return send_from_directory(frontend_dir, 'index.html')

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
