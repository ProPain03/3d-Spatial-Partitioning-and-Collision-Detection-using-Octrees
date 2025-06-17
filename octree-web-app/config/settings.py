from pathlib import Path

# Base directory for the application
BASE_DIR = Path(__file__).resolve().parent.parent

# Flask or FastAPI configuration
APP_NAME = "Octree Web App"
DEBUG = True  # Set to False in production

# CORS settings (if using Flask)
CORS_ENABLED = True

# Database settings (if applicable)
DATABASE_URI = "sqlite:///db.sqlite3"  # Example for SQLite

# Octree settings
MAX_SIZE = 1000
MAX_DEPTH = 5
MAX_POINTS = 2
COLLISION_SIZE = 30

# File paths
INITIAL_POINTS_FILE = BASE_DIR / "backend/static/data/random1.txt"

# Logging settings
LOGGING_LEVEL = "DEBUG"  # Change to "INFO" or "ERROR" in production

# WebSocket settings (if applicable)
WEBSOCKET_URL = "ws://localhost:5000/ws"  # Example WebSocket URL

# Other constants can be added here as needed
