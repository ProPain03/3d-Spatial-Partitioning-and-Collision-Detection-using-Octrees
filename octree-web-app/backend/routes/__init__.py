# File: /octree-web-app/octree-web-app/backend/routes/__init__.py

from flask import Blueprint

# Initialize the routes blueprint
routes_bp = Blueprint('routes', __name__)

from .octree_routes import *
from .game_routes import *
