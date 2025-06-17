// Global variables
let scene, camera, renderer, controls;
let octreeVisualizer;
let pointManager;
let selectedPoint = null;
let gameMode = false;

// Initialize Three.js scene
function initThree() {
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x001122);

    camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 10000);
    camera.position.set(600, 600, 600);

    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap;
    
    const container = document.getElementById('canvas-container');
    if (container) {
        container.appendChild(renderer.domElement);
    } else {
        document.body.appendChild(renderer.domElement);
    }

    // Add lights
    const ambientLight = new THREE.AmbientLight(0x404040, 0.6);
    scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 1.0);
    directionalLight.position.set(1000, 1000, 1000);
    directionalLight.castShadow = true;
    directionalLight.shadow.mapSize.width = 2048;
    directionalLight.shadow.mapSize.height = 2048;
    scene.add(directionalLight);

    // Add point light for better illumination
    const pointLight = new THREE.PointLight(0xffffff, 0.8, 0);
    pointLight.position.set(500, 500, 500);
    scene.add(pointLight);

    // Add orbit controls
    if (typeof THREE.OrbitControls !== 'undefined') {
        controls = new THREE.OrbitControls(camera, renderer.domElement);
        controls.enableDamping = true;
        controls.dampingFactor = 0.05;
        controls.maxDistance = 2000;
        controls.minDistance = 10;
    }
}

// Initialize managers
function initManagers() {
    octreeVisualizer = new OctreeVisualizer(scene);
    pointManager = new PointManager();
}

// Animation loop
function animate() {
    requestAnimationFrame(animate);
    
    if (controls) {
        controls.update();
    }
    
    // Update octree visualizer animations
    if (octreeVisualizer) {
        octreeVisualizer.animate();
    }
    
    renderer.render(scene, camera);
}

// Handle window resize
function handleResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
}

// Keyboard controls for game mode
function handleKeyPress(event) {
    if (!gameMode || !selectedPoint) return;

    const key = event.key.toLowerCase();
    if (['w', 'a', 's', 'd', 'e', 'f'].includes(key)) {
        event.preventDefault();
        moveSelectedPoint(key);
    }
}

// Move selected point
async function moveSelectedPoint(direction) {
    try {
        const result = await apiClient.movePointByDirection(selectedPoint, direction);
        
        if (result.success) {
            selectedPoint = result.new_position;
            updateStatus(`Point moved ${direction.toUpperCase()}: (${selectedPoint.x}, ${selectedPoint.y}, ${selectedPoint.z})`);
            updateSelectedPointDisplay();
            updateCollisionStatus('No collision');
            
            // Highlight the new position
            octreeVisualizer.highlightSelectedPoint(selectedPoint);
            
            await refreshVisualization();
        } else {
            updateStatus(`Movement failed: ${result.message}`);
            if (result.collision) {
                updateCollisionStatus('Collision detected! Movement blocked.');
                // Flash collision warning
                flashCollisionWarning();
            }
        }
    } catch (error) {
        console.error('Error moving point:', error);
        updateStatus('Error moving point');
    }
}

// Flash collision warning
function flashCollisionWarning() {
    const collisionElement = document.getElementById('collision-status');
    if (collisionElement) {
        collisionElement.style.color = '#ff4444';
        collisionElement.style.fontWeight = 'bold';
        setTimeout(() => {
            collisionElement.style.color = '';
            collisionElement.style.fontWeight = '';
        }, 2000);
    }
}

// Update status display
function updateStatus(message) {
    const statusElement = document.getElementById('status');
    if (statusElement) {
        statusElement.textContent = `Status: ${message}`;
    }
    console.log(message);
}

// Update collision status
function updateCollisionStatus(message) {
    const collisionElement = document.getElementById('collision-status');
    if (collisionElement) {
        collisionElement.textContent = `Collision Status: ${message}`;
    }
}

// Update selected point display
function updateSelectedPointDisplay() {
    const selectedElement = document.getElementById('selected-point');
    if (selectedElement && selectedPoint) {
        selectedElement.textContent = `Selected Point: (${selectedPoint.x}, ${selectedPoint.y}, ${selectedPoint.z})`;
    }
}

// Update point count display
function updatePointCountDisplay(count) {
    const countElement = document.getElementById('point-count');
    if (countElement) {
        countElement.textContent = `Total Points: ${count}`;
    }
}

// Refresh visualization
async function refreshVisualization() {
    try {
        const data = await apiClient.getOctreeStructure();
        
        if (data.success) {
            octreeVisualizer.updateVisualization(data.octree);
            
            // Re-highlight selected point if exists
            if (selectedPoint) {
                octreeVisualizer.highlightSelectedPoint(selectedPoint);
            }
        }
        
        // Update point count
        const pointsData = await apiClient.getAllPoints();
        if (pointsData.success) {
            updatePointCountDisplay(pointsData.count);
        }
    } catch (error) {
        console.error('Error refreshing visualization:', error);
    }
}

// Get point from inputs
function getPointFromInputs() {
    const x = parseFloat(document.getElementById('x-input').value) || 0;
    const y = parseFloat(document.getElementById('y-input').value) || 0;
    const z = parseFloat(document.getElementById('z-input').value) || 0;
    return { x, y, z };
}

// Clear inputs
function clearInputs() {
    document.getElementById('x-input').value = '';
    document.getElementById('y-input').value = '';
    document.getElementById('z-input').value = '';
}

// Setup UI controls
function setupUI() {
    // Game mode toggle
    const gameModeBtn = document.getElementById('game-mode-btn');
    if (gameModeBtn) {
        gameModeBtn.addEventListener('click', () => {
            gameMode = !gameMode;
            const modeText = gameMode ? 'Game mode ON - Use WASD/EF to move selected point' : 'Game mode OFF';
            updateStatus(modeText);
            gameModeBtn.textContent = gameMode ? 'Exit Game Mode' : 'Toggle Game Mode';
            gameModeBtn.style.backgroundColor = gameMode ? '#ff4444' : '';
            
            if (gameMode && selectedPoint) {
                octreeVisualizer.highlightSelectedPoint(selectedPoint);
            }
        });
    }

    // Insert point
    const insertBtn = document.getElementById('insert-btn');
    if (insertBtn) {
        insertBtn.addEventListener('click', async () => {
            const point = getPointFromInputs();
            const result = await apiClient.insertPoint(point);
            
            if (result.success) {
                updateStatus(`Point inserted: (${point.x}, ${point.y}, ${point.z})`);
                clearInputs();
                
                // Zoom to inserted point
                octreeVisualizer.zoomToPoint(point, camera, controls);
                
                await refreshVisualization();
            } else {
                updateStatus(`Insert failed: ${result.message}`);
            }
        });
    }

    // Delete point
    const deleteBtn = document.getElementById('delete-btn');
    if (deleteBtn) {
        deleteBtn.addEventListener('click', async () => {
            const point = getPointFromInputs();
            const result = await apiClient.deletePoint(point);
            
            if (result.success) {
                updateStatus(`Point deleted: (${point.x}, ${point.y}, ${point.z})`);
                clearInputs();
                
                // Clear highlights if deleted point was selected
                if (selectedPoint && selectedPoint.x === point.x && 
                    selectedPoint.y === point.y && selectedPoint.z === point.z) {
                    selectedPoint = null;
                    updateSelectedPointDisplay();
                }
                
                await refreshVisualization();
            } else {
                updateStatus(`Delete failed: ${result.message}`);
            }
        });
    }

    // Search point
    const searchBtn = document.getElementById('search-btn');
    if (searchBtn) {
        searchBtn.addEventListener('click', async () => {
            const point = getPointFromInputs();
            const result = await apiClient.searchPoint(point);
            
            if (result.found) {
                updateStatus(`Point found: (${result.point.x}, ${result.point.y}, ${result.point.z})`);
                selectedPoint = result.point;
                updateSelectedPointDisplay();
                
                // Highlight and zoom to found point
                octreeVisualizer.highlightSelectedPoint(selectedPoint);
                octreeVisualizer.zoomToPoint(selectedPoint, camera, controls);
            } else {
                updateStatus(`Point not found: (${point.x}, ${point.y}, ${point.z})`);
            }
        });
    }

    // Range query
    const rangeBtn = document.getElementById('range-btn');
    if (rangeBtn) {
        rangeBtn.addEventListener('click', async () => {
            const point = getPointFromInputs();
            const size = parseFloat(document.getElementById('range-size').value) || 100;
            const min = { x: point.x - size, y: point.y - size, z: point.z - size };
            const max = { x: point.x + size, y: point.y + size, z: point.z + size };
            
            const result = await apiClient.rangeQuery(min, max);
            
            if (result.success) {
                updateStatus(`Range query found ${result.count} points around (${point.x}, ${point.y}, ${point.z})`);
                
                // Visualize range query
                octreeVisualizer.showRangeQuery(min, max, result.points);
            } else {
                updateStatus(`Range query failed: ${result.message}`);
            }
        });
    }

    // Find nearest
    const nearestBtn = document.getElementById('nearest-btn');
    if (nearestBtn) {
        nearestBtn.addEventListener('click', async () => {
            const point = getPointFromInputs();
            const k = parseInt(document.getElementById('k-nearest').value) || 1;
            
            const result = await apiClient.nearestNeighbor({ ...point, k: k });
            
            if (result.success && result.nearest) {
                updateStatus(`Found ${result.nearest.length} nearest neighbor(s) for (${point.x}, ${point.y}, ${point.z})`);
                
                // Visualize nearest neighbors
                octreeVisualizer.highlightNearestNeighbors(point, result.nearest);
                
                if (result.nearest.length > 0) {
                    selectedPoint = result.nearest[0];
                    updateSelectedPointDisplay();
                }
            } else {
                updateStatus(`No nearest neighbor found for (${point.x}, ${point.y}, ${point.z})`);
            }
        });
    }
}

// Load initial octree data
async function loadInitialData() {
    try {
        updateStatus('Loading octree data...');
        await refreshVisualization();
        
        // Load all points
        const pointsData = await apiClient.getAllPoints();
        
        if (pointsData.success && pointsData.points.length > 0) {
            selectedPoint = pointsData.points[0]; // Select first point by default
            updateStatus(`Loaded ${pointsData.points.length} points. Selected: (${selectedPoint.x}, ${selectedPoint.y}, ${selectedPoint.z})`);
            updateSelectedPointDisplay();
            updatePointCountDisplay(pointsData.count);
            
            // Highlight selected point
            octreeVisualizer.highlightSelectedPoint(selectedPoint);
        }
    } catch (error) {
        console.error('Error loading initial data:', error);
        updateStatus('Error loading data');
    }
}

// Initialize everything
async function init() {
    updateStatus('Initializing...');
    
    initThree();
    initManagers();
    
    // Setup event listeners
    window.addEventListener('resize', handleResize);
    window.addEventListener('keydown', handleKeyPress);
    
    // Setup UI controls
    setupUI();
    
    // Load initial data
    await loadInitialData();
    
    // Start animation loop
    animate();
    
    updateStatus('Ready - Select a point and toggle Game Mode to start moving!');
}

// Start the application
document.addEventListener('DOMContentLoaded', init);
