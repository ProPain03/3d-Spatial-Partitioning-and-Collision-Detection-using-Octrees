class OctreeVisualizer {
    constructor(scene) {
        this.scene = scene;
        this.octreeNodes = [];
        this.pointMeshes = [];
        this.highlightedPoints = [];
        this.selectedPointMesh = null;
        this.rangeBox = null;
        this.nearestLines = [];
        this.axesHelper = null;
        
        this.colors = [
            0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 
            0xff00ff, 0x00ffff, 0xffffff, 0x888888
        ];
        
        this.initializeScene();
    }

    initializeScene() {
        // Add coordinate axes at bottom left
        this.createAxesHelper();
        
        // Add grid
        this.createGrid();
    }

    createAxesHelper() {
        // Create axes helper at bottom left
        this.axesHelper = new THREE.AxesHelper(100);
        this.axesHelper.position.set(-450, -450, -450);
        this.scene.add(this.axesHelper);
        
        // Add labels for axes
        this.addAxisLabels();
    }

    addAxisLabels() {
        const loader = new THREE.FontLoader();
        
        // Create text geometry for axis labels (simplified approach)
        const textMaterial = new THREE.MeshBasicMaterial({ color: 0xffffff });
        
        // X axis label
        const xGeometry = new THREE.TextGeometry || new THREE.SphereGeometry(5, 8, 8);
        const xLabel = new THREE.Mesh(xGeometry, new THREE.MeshBasicMaterial({ color: 0xff0000 }));
        xLabel.position.set(-350, -450, -450);
        this.scene.add(xLabel);
        
        // Y axis label
        const yLabel = new THREE.Mesh(new THREE.SphereGeometry(5, 8, 8), new THREE.MeshBasicMaterial({ color: 0x00ff00 }));
        yLabel.position.set(-450, -350, -450);
        this.scene.add(yLabel);
        
        // Z axis label
        const zLabel = new THREE.Mesh(new THREE.SphereGeometry(5, 8, 8), new THREE.MeshBasicMaterial({ color: 0x0000ff }));
        zLabel.position.set(-450, -450, -350);
        this.scene.add(zLabel);
    }

    createGrid() {
        const size = 2000;
        const divisions = 20;
        
        const gridHelper = new THREE.GridHelper(size, divisions, 0x444444, 0x222222);
        gridHelper.position.y = -500;
        this.scene.add(gridHelper);
    }

    clearVisualization() {
        // Remove octree nodes
        this.octreeNodes.forEach(node => {
            this.scene.remove(node);
            if (node.geometry) node.geometry.dispose();
            if (node.material) node.material.dispose();
        });
        this.octreeNodes = [];

        // Remove point meshes
        this.pointMeshes.forEach(mesh => {
            this.scene.remove(mesh);
            if (mesh.geometry) mesh.geometry.dispose();
            if (mesh.material) mesh.material.dispose();
        });
        this.pointMeshes = [];

        // Clear highlights
        this.clearHighlights();
    }

    clearHighlights() {
        // Remove highlighted points
        this.highlightedPoints.forEach(mesh => {
            this.scene.remove(mesh);
            if (mesh.geometry) mesh.geometry.dispose();
            if (mesh.material) mesh.material.dispose();
        });
        this.highlightedPoints = [];

        // Remove range box
        if (this.rangeBox) {
            this.scene.remove(this.rangeBox);
            if (this.rangeBox.geometry) this.rangeBox.geometry.dispose();
            if (this.rangeBox.material) this.rangeBox.material.dispose();
            this.rangeBox = null;
        }

        // Remove nearest neighbor lines
        this.nearestLines.forEach(line => {
            this.scene.remove(line);
            if (line.geometry) line.geometry.dispose();
            if (line.material) line.material.dispose();
        });
        this.nearestLines = [];

        // Remove selected point highlight
        if (this.selectedPointMesh) {
            this.scene.remove(this.selectedPointMesh);
            if (this.selectedPointMesh.geometry) this.selectedPointMesh.geometry.dispose();
            if (this.selectedPointMesh.material) this.selectedPointMesh.material.dispose();
            this.selectedPointMesh = null;
        }
    }

    createOctreeNode(nodeData) {
        const { center, size, depth, is_leaf, points } = nodeData;
        
        // Create wireframe box for octree node
        const geometry = new THREE.BoxGeometry(size, size, size);
        const material = new THREE.LineBasicMaterial({ 
            color: this.colors[depth % this.colors.length],
            opacity: Math.max(0.2, 0.8 - (depth * 0.1)),
            transparent: true
        });
        
        const edges = new THREE.EdgesGeometry(geometry);
        const wireframe = new THREE.LineSegments(edges, material);
        wireframe.position.set(center.x, center.y, center.z);
        
        this.scene.add(wireframe);
        this.octreeNodes.push(wireframe);

        // Create points if it's a leaf node
        if (is_leaf && points && points.length > 0) {
            points.forEach((point, index) => {
                this.createPoint(point, depth);
            });
        }
    }

    createPoint(pointData, depth = 0) {
        const geometry = new THREE.SphereGeometry(6, 16, 16);
        const material = new THREE.MeshLambertMaterial({ 
            color: 0x4488ff,
            emissive: 0x001122
        });
        
        const sphere = new THREE.Mesh(geometry, material);
        sphere.position.set(pointData.x, pointData.y, pointData.z);
        sphere.castShadow = true;
        sphere.receiveShadow = true;
        sphere.userData = { point: pointData };
        
        this.scene.add(sphere);
        this.pointMeshes.push(sphere);
    }

    highlightSelectedPoint(pointData) {
        // Remove previous selected point highlight
        if (this.selectedPointMesh) {
            this.scene.remove(this.selectedPointMesh);
            if (this.selectedPointMesh.geometry) this.selectedPointMesh.geometry.dispose();
            if (this.selectedPointMesh.material) this.selectedPointMesh.material.dispose();
        }

        // Create larger, glowing sphere for selected point
        const geometry = new THREE.SphereGeometry(15, 32, 32);
        const material = new THREE.MeshLambertMaterial({ 
            color: 0xff4444,
            emissive: 0x441111,
            transparent: true,
            opacity: 0.8
        });
        
        this.selectedPointMesh = new THREE.Mesh(geometry, material);
        this.selectedPointMesh.position.set(pointData.x, pointData.y, pointData.z);
        
        // Add pulsing animation
        this.selectedPointMesh.userData = { 
            originalScale: 1,
            pulseSpeed: 0.05,
            point: pointData
        };
        
        this.scene.add(this.selectedPointMesh);
    }

    highlightNearestNeighbors(targetPoint, nearestPoints) {
        // Clear previous highlights
        this.nearestLines.forEach(line => {
            this.scene.remove(line);
            if (line.geometry) line.geometry.dispose();
            if (line.material) line.material.dispose();
        });
        this.nearestLines = [];

        this.highlightedPoints.forEach(mesh => {
            this.scene.remove(mesh);
            if (mesh.geometry) mesh.geometry.dispose();
            if (mesh.material) mesh.material.dispose();
        });
        this.highlightedPoints = [];

        // Highlight target point
        const targetGeometry = new THREE.SphereGeometry(12, 16, 16);
        const targetMaterial = new THREE.MeshLambertMaterial({ 
            color: 0x00ff00,
            emissive: 0x002200
        });
        
        const targetMesh = new THREE.Mesh(targetGeometry, targetMaterial);
        targetMesh.position.set(targetPoint.x, targetPoint.y, targetPoint.z);
        this.scene.add(targetMesh);
        this.highlightedPoints.push(targetMesh);

        // Highlight nearest neighbors and draw lines
        nearestPoints.forEach((point, index) => {
            // Create highlighted point
            const geometry = new THREE.SphereGeometry(10, 16, 16);
            const material = new THREE.MeshLambertMaterial({ 
                color: 0xffff00,
                emissive: 0x222200
            });
            
            const mesh = new THREE.Mesh(geometry, material);
            mesh.position.set(point.x, point.y, point.z);
            this.scene.add(mesh);
            this.highlightedPoints.push(mesh);

            // Draw line from target to nearest neighbor
            const lineGeometry = new THREE.BufferGeometry().setFromPoints([
                new THREE.Vector3(targetPoint.x, targetPoint.y, targetPoint.z),
                new THREE.Vector3(point.x, point.y, point.z)
            ]);
            
            const lineMaterial = new THREE.LineBasicMaterial({ 
                color: 0xffff00,
                opacity: 0.7,
                transparent: true
            });
            
            const line = new THREE.Line(lineGeometry, lineMaterial);
            this.scene.add(line);
            this.nearestLines.push(line);
        });
    }

    showRangeQuery(minPoint, maxPoint, pointsInRange) {
        // Remove previous range box
        if (this.rangeBox) {
            this.scene.remove(this.rangeBox);
            if (this.rangeBox.geometry) this.rangeBox.geometry.dispose();
            if (this.rangeBox.material) this.rangeBox.material.dispose();
        }

        // Create range box
        const width = maxPoint.x - minPoint.x;
        const height = maxPoint.y - minPoint.y;
        const depth = maxPoint.z - minPoint.z;
        
        const centerX = (minPoint.x + maxPoint.x) / 2;
        const centerY = (minPoint.y + maxPoint.y) / 2;
        const centerZ = (minPoint.z + maxPoint.z) / 2;

        const geometry = new THREE.BoxGeometry(width, height, depth);
        const material = new THREE.MeshBasicMaterial({ 
            color: 0x00ffff,
            opacity: 0.2,
            transparent: true,
            wireframe: false
        });
        
        this.rangeBox = new THREE.Mesh(geometry, material);
        this.rangeBox.position.set(centerX, centerY, centerZ);
        
        // Add wireframe
        const wireframe = new THREE.LineSegments(
            new THREE.EdgesGeometry(geometry),
            new THREE.LineBasicMaterial({ color: 0x00ffff, opacity: 0.8, transparent: true })
        );
        wireframe.position.set(centerX, centerY, centerZ);
        
        this.scene.add(this.rangeBox);
        this.scene.add(wireframe);
        this.highlightedPoints.push(wireframe);

        // Highlight points in range
        pointsInRange.forEach(point => {
            const geometry = new THREE.SphereGeometry(8, 16, 16);
            const material = new THREE.MeshLambertMaterial({ 
                color: 0x00ffff,
                emissive: 0x002222
            });
            
            const mesh = new THREE.Mesh(geometry, material);
            mesh.position.set(point.x, point.y, point.z);
            this.scene.add(mesh);
            this.highlightedPoints.push(mesh);
        });
    }

    zoomToPoint(pointData, camera, controls) {
        if (!camera || !controls) return;

        // Calculate target position (offset from point)
        const targetPosition = new THREE.Vector3(
            pointData.x + 200,
            pointData.y + 200,
            pointData.z + 200
        );

        // Animate camera to point
        const startPosition = camera.position.clone();
        const startTime = Date.now();
        const duration = 2000; // 2 seconds

        const animateCamera = () => {
            const elapsed = Date.now() - startTime;
            const progress = Math.min(elapsed / duration, 1);
            
            // Smooth easing function
            const easeProgress = 1 - Math.pow(1 - progress, 3);
            
            camera.position.lerpVectors(startPosition, targetPosition, easeProgress);
            controls.target.set(pointData.x, pointData.y, pointData.z);
            controls.update();

            if (progress < 1) {
                requestAnimationFrame(animateCamera);
            }
        };

        animateCamera();
    }

    visualizeNodeRecursive(nodeData) {
        if (!nodeData) return;

        this.createOctreeNode(nodeData);

        // If not a leaf, visualize children
        if (!nodeData.is_leaf && nodeData.children) {
            nodeData.children.forEach(child => {
                if (child) {
                    this.visualizeNodeRecursive(child);
                }
            });
        }
    }

    updateVisualization(octreeData) {
        this.clearVisualization();
        
        if (octreeData) {
            this.visualizeNodeRecursive(octreeData);
        }
    }

    // Animation loop for pulsing selected point
    animate() {
        if (this.selectedPointMesh && this.selectedPointMesh.userData) {
            const userData = this.selectedPointMesh.userData;
            const time = Date.now() * userData.pulseSpeed;
            const scale = userData.originalScale + Math.sin(time) * 0.2;
            this.selectedPointMesh.scale.setScalar(scale);
        }
    }
}